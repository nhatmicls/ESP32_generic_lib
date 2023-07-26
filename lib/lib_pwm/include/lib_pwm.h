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
#include "driver/gptimer.h"

typedef struct
{
} timer_handle_struct;

void timer_init(void);

#endif
