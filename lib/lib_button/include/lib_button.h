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

#ifndef LIB_BUTTON_H
#define LIB_BUTTON_H

#include <stdint.h>

#include <inttypes.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_timer.h"

#include "./../../lib_common_func/include/lib_common_func.h"

// #define DEBUG_BUTTON

#define BUTTON_MAX_QUEUE 20
#define BUTTON_SHORT_HOLDING_TIME 1000 // ms
#define BUTTON_LONG_HOLDING_TIME 2000  // ms
#define BUTTON_PRESS_TIME 100          // ms
#define BUTTON_ESP_INTR_FLAG_DEFAULT 0

typedef enum
{
    PRESSING,
    RELEASING,
    PRESS,
    SHORT_HOLD,
    LONG_HOLD,
    INVALID
} button_event_t;

typedef enum
{
    INTERNAL,
    EXTERNAL,
} button_pull_loc_t;

typedef void (*BUTTON_CALLBACK_FUNCTION_t)(void);

struct button_callback_list;

struct button_callback_list *lib_button_init(gpio_num_t gpio_pin, button_pull_loc_t pull_loc, gpio_pull_mode_t pull_mode, gpio_int_type_t intr_mode);
void lib_button_deinit(struct button_callback_list **self);

void lib_add_press_callback(struct button_callback_list *self, BUTTON_CALLBACK_FUNCTION_t callback);
void lib_add_shortpress_callback(struct button_callback_list *self, BUTTON_CALLBACK_FUNCTION_t callback);
void lib_add_longpress_callback(struct button_callback_list *self, BUTTON_CALLBACK_FUNCTION_t callback);

void lib_button_ISR_task_handler(void *arg);
void lib_button_debouncing_task_handler(void *arg);

#endif