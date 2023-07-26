/**
 * @file gpio.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef APP_GPIO_H
#define APP_GPIO_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "./../../../lib/lib_gpio/include/lib_gpio.h"

#define LED_A 22
#define LED_B 23
#define LED_C 19
#define LED_D 18
#define LED_E 5
#define LED_F 12
#define LED_G 13
#define LED_DP 21

#define LED_RED 15
#define LED_GREEN 4
#define LED_BLUE 16

#define LED_OFF 0
#define LED_ON 1

void app_gpio();

#endif