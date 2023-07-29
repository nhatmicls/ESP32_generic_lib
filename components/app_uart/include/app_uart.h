/**
 * @file app_uart.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef APP_UART_H
#define APP_UART_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"

#include "./../../../lib/lib_uart/include/lib_uart.h"

void app_uart_init(void);
void app_uart(void *arg);

#endif
