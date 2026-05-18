#include "hcsr04.h"
#include "gd32vf103.h"
#include <string.h>

#define MTIME_LO (*(volatile uint32_t*)0xD1000000) /* MISRA Rule 11.4 deviation: hardware register access */

#define TICK_PER_MS   (27000U)
#define TIME_OUT_30MS (810000U)   /* 30ms — waiting for echo to start (STATE_ECHO_HIGH) */
#define TIME_OUT_40MS (1080000U)  /* 40ms — waiting for echo to end; covers HC-SR04's 38ms no-object hold */
#define WINDOW_SIZE 5

/**
 * @file    hcsr04.c
 * @brief   HC-SR04 ultrasonic sensor FSM driver
 * @version 1.0
 * @date    2026-05-14
 */

typedef enum status
{
    STATE_IDLE,
    STATE_TRIG_START,
    STATE_TRIG_WAIT,
    STATE_TRIG_GUARD,
    STATE_ECHO_HIGH,
    STATE_ECHO_LOW,
    STATE_ERROR
} hcsr04state;

static hcsr04state current_state = STATE_IDLE;
static uint32_t start_time = 0;
static uint32_t end_time = 0;
static int32_t distance= 0;
static uint32_t error_count = 0;

static uint32_t last_trig_time = 0;
static int32_t filter_buffer[WINDOW_SIZE] = {0};
static uint8_t filter_idx = 0;

static int32_t apply_median_filter(int32_t raw_val) 
{
    filter_buffer[filter_idx] = raw_val;
    filter_idx = (filter_idx + 1) % WINDOW_SIZE;
    
    int32_t sorted[WINDOW_SIZE];
    memcpy(sorted, filter_buffer, sizeof(sorted));
    
    for (int i = 0; i < WINDOW_SIZE - 1; i++) 
    {
        for (int j = 0; j < WINDOW_SIZE - 1 - i; j++) 
        {
            if (sorted[j] > sorted[j+1]) 
            {
                int32_t tmp = sorted[j];
                sorted[j] = sorted[j+1];
                sorted[j+1] = tmp;
            }
        }
    }
    return sorted[WINDOW_SIZE / 2];
}

void hcsr04_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
}

int32_t hcsr04_get_distance(void)  {return distance;}

void hcsr04_tick(void)
{
    switch(current_state)
    {
        case STATE_IDLE:
        if(MTIME_LO - last_trig_time >= 100U * TICK_PER_MS) //wait for 100ms
        {current_state = STATE_TRIG_START;}
        break;

        case STATE_TRIG_START:
        gpio_bit_set(GPIOA, GPIO_PIN_0);
        last_trig_time = MTIME_LO;
        current_state = STATE_TRIG_WAIT;
        break;

        case STATE_TRIG_WAIT:
        if(MTIME_LO - last_trig_time >= 270U) //10µs
        {
            gpio_bit_reset(GPIOA, GPIO_PIN_0);
            last_trig_time = MTIME_LO;
            current_state = STATE_TRIG_GUARD;
        }
        break;

        case STATE_TRIG_GUARD: /* wait 300µs after TRIG falls — ignores capacitive coupling glitch on ECHO */
        if(MTIME_LO - last_trig_time >= 8100U) /* 300µs × 27 ticks/µs */
        { current_state = STATE_ECHO_HIGH; }
        break;

        case STATE_ECHO_HIGH:
        if(gpio_input_bit_get(GPIOA,GPIO_PIN_1) == SET)
        {
            start_time = MTIME_LO;
            current_state = STATE_ECHO_LOW;
        }

        else if(MTIME_LO - last_trig_time > TIME_OUT_30MS) //1s = 2700 0000 tck, 1ms = 27000 tck
        {current_state = STATE_ERROR;}
        break;

        case STATE_ECHO_LOW:
        if(gpio_input_bit_get(GPIOA,GPIO_PIN_1) == RESET)
        {
            end_time = MTIME_LO;
            
            int32_t raw_distance = (int32_t)((end_time - start_time) / 157U); /* cm: 27MHz × 58µs/cm = 1566, /10 ≈ 157 */
            distance = apply_median_filter(raw_distance); 
            
            error_count = 0;
            current_state = STATE_IDLE;
        }

        else if(MTIME_LO - start_time > TIME_OUT_40MS)
        {current_state = STATE_ERROR;}
        break;
        
        case STATE_ERROR:
        if(error_count < 250U)
        {error_count++;}

        if(error_count > 3U)
        {distance = -1;}

        current_state = STATE_IDLE;
        last_trig_time = MTIME_LO;
        break;

        default:
        current_state = STATE_IDLE;
        break;

    }
}