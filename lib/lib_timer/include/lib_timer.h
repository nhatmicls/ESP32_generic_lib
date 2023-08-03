/**
 * @file lib_timer.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef LIB_TIMER_H
#define LIB_TIMER_H

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "driver/gptimer.h"

#include "sdkconfig.h"
#include "esp_log.h"

#include <stdbool.h>

#include "./../../lib_common_func/include/lib_common_func.h"

typedef void (*TIMER_CALLBACK_FUNCTION_t)(void);

struct timer_handle_t;

struct timer_handle_t *lib_timer_init(gptimer_clock_source_t clk_src,
                                      gptimer_count_direction_t direction,
                                      uint64_t resolution,
                                      uint64_t reload_count,
                                      uint64_t alarm_count,
                                      bool auto_reload_on_alarm,
                                      bool isr_enable,
                                      TIMER_CALLBACK_FUNCTION_t callback);

void timer_init(void);

void lib_timer_start(struct timer_handle_t *self);

void lib_timer_ISR_task_handler(void *arg);

#endif
