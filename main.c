/**
 * @file    main.c
 * @brief   HC-SR04 Main file
 * @version 1.0
 * @date    2026-05-19
 */

#include "gd32vf103.h"
#include "drivers.h"
#include "lcd.h"
#include "hcsr04.h"
#include "display.h"

int main(void)
{
    t5omsi();
    Lcd_SetType(LCD_NORMAL);
    Lcd_Init();
    hcsr04_init();
    display_init();

    for(;;)
    {
        LCD_WR_Queue();
        hcsr04_tick();

        if (t5expq())
        {
            int32_t dist = hcsr04_get_distance();
            display_update(dist);
        }
    }
}