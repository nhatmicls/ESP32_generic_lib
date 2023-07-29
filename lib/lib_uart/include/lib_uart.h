/**
 * @file lib_uart.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef LIB_UART_H
#define LIB_UART_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include "sdkconfig.h"
#include "esp_log.h"

#include "string.h"

#define BUF_SIZE 1024

struct uart_handle_t;

struct uart_handle_t *lib_uart_init(uart_port_t uart_num,
                                    uint16_t baud_rate,
                                    uart_word_length_t data_bits,
                                    uart_parity_t parity,
                                    uart_stop_bits_t stop_bits,
                                    uart_hw_flowcontrol_t flow_ctrl,
                                    int8_t txPort,
                                    int8_t rxPort,
                                    int8_t rtsPort,
                                    int8_t ctsPort);

void send_uart(uart_port_t uart_num, char *send_data);
uint16_t recv_uart(uart_port_t uart_num, uint8_t timeout, uint8_t *data, uint16_t size);

#endif
