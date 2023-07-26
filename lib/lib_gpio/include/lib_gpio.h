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

#ifndef LIB_GPIO_H
#define LIB_GPIO_H

#include <stdint.h>

#include "esp_err.h"
#include "driver/gpio.h"
#include "esp_system.h"

struct lib_gpio_init_struct
{
    uint64_t gpio_bit_mask
};

void lib_gpio_init(uint64_t gpio_bit_mask, gpio_mode_t mode, gpio_pull_mode_t pull_mode, gpio_int_type_t intr_mode);

void lib_gpio_output_init(gpio_num_t gpio_pin);
void lib_gpio_input_init(gpio_num_t gpio_pin);

void lib_gpio_set_value(gpio_num_t gpio_pin, uint8_t value);
uint8_t lib_gpio_get_value(gpio_num_t gpio_pin);

#endif