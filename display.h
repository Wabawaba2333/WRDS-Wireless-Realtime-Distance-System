#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

/**
 * @file    display.h
 * @brief   HC-SR04 lcd display header
 * @version 1.0
 * @date    2026-05-11
 */

void display_init(void);
void display_update(int32_t distance);

#endif 