/**
 * @file    display.c
 * @brief   HC-SR04 LCD display module
 * @version 1.4
 * @date    2026-05-14
 */

#include "display.h"
#include "lcd.h"

#define HCSR04_DIST_ERROR  (-1)
#define NUM_X              (20U)
#define NUM_Y              (20U)

static int32_t last_distance = -2;

void display_init(void)
{
    BACK_COLOR = BLACK;
    LCD_Clear(BLACK);
    //LCD_ShowStr(10U, LBL_Y, "Dist(cm):", WHITE, TRANSPARENT);
    LCD_Wait_On_Queue();
}

void display_update(int32_t distance)
{
    if (distance == last_distance) { return; }
    last_distance = distance;

    if (distance == HCSR04_DIST_ERROR)
    {
        LCD_ShowNum(0, 0, 0U, 4U, BLACK);                  /* clear stale number */
        LCD_ShowStr(NUM_Y, NUM_Y, " ERR ", GREEN, OPAQUE);
    }
    else
    {
        LCD_ShowStr(NUM_Y, NUM_Y, "     ", GREEN, OPAQUE); /* clear stale ERR */
        LCD_ShowNum(0, 0, (uint16_t)distance, 4U, WHITE);
    }
}
