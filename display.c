/**
 * @file    display.c
 * @brief   HC-SR04 LCD display module
 * @version 1.5
 * @date    2026-05-14
 */

#include "display.h"
#include "lcd.h"

#define HCSR04_DIST_ERROR  (-1)
#define NUM_X  (80U) 
#define NUM_Y  (40U)
#define DOT_X  (104U)
#define DEC_X  (112U)

/* last_distance = -2: forces first display_update() to actually draw,
   even though hcsr04.c initialises distance = 0 */
static int32_t last_distance = -2;

void display_init(void)
{
    BACK_COLOR = BLACK;
    LCD_Clear(BLACK);
    LCD_ShowStr(0U,    0U,   "Hcsr-04",   WHITE, TRANSPARENT);
    LCD_ShowStr(0U,   30U,   "Distance:", WHITE, TRANSPARENT);
    LCD_ShowStr(130U, NUM_Y, "cm",         WHITE, TRANSPARENT);
    LCD_Wait_On_Queue();
}

void display_update(int32_t distance)
{
    if (distance == last_distance) { return; }
    last_distance = distance;

    if (distance == HCSR04_DIST_ERROR)
    {
        /* " ERR " = 5 chars, same width as "NNN.N" — overwrites number area */
        LCD_ShowStr(NUM_X, NUM_Y, " ERR ", GREEN, OPAQUE);
    }
    else
    {
        /* distance unit: 0.1 cm  →  NNN.N cm */
        LCD_ShowNum(NUM_X, NUM_Y, (uint16_t)((uint32_t)distance / 10U), 3U, WHITE);
        LCD_ShowStr(DOT_X, NUM_Y, ".",                                   WHITE, OPAQUE);
        LCD_ShowNum(DEC_X, NUM_Y, (uint16_t)((uint32_t)distance % 10U), 1U, WHITE);
    }
}
