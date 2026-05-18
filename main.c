#include "gd32vf103.h"
#include "drivers.h"
#include "lcd.h"
#include "hcsr04.h"
#include "display.h"

/**
 * @file    main.c
 * @brief   HC-SR04 Main file
 * @version 1.0
 * @date    2026-05-14
 */

int main(void)
{
    t5omsi();
    colinit();
    l88init();
    Lcd_SetType(LCD_NORMAL);
    Lcd_Init();
    hcsr04_init();
    display_init();
    eclic_global_interrupt_enable();

    for(;;)
    {
        LCD_WR_Queue();
        hcsr04_tick();

        if (t5expq())
        {
            int32_t dist = hcsr04_get_distance();
            l88row(colset());
            l88mem(4U, (uint8_t)((uint32_t)dist >> 8));
            l88mem(5U, (uint8_t)((uint32_t)dist));
            
            display_update(dist);
        }
    }
}