/**
 * @file app_button.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef APP_BUTTON_H
#define APP_BUTTON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"

#include "./../../include/priority.h"

#include "./../../../lib/lib_button/include/lib_button.h"
#include "./../../../lib/lib_uart/include/lib_uart.h"
#include "./../../../lib/lib_timer/include/lib_timer.h"

void app_button(void *);

#endif
