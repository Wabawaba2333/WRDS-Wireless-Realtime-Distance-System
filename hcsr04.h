#ifndef HCSR04_H
#define HCSR04_H

#include <stdint.h> 

/**
 * @file    hcsr04.h
 * @brief   HC-SR04 ultrasonic sensor FSM driver header
 * @version 1.0
 * @date    2026-05-11
 */

void hcsr04_init(void);
void hcsr04_tick(void);
int32_t hcsr04_get_distance(void);

#endif /* HCSR04_H */