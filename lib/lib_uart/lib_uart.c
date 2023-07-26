#include <stdio.h>
#include "lib_uart.h"

struct uart_handle_t
{
    SemaphoreHandle_t xMutexUART;
    QueueHandle_t uartQueue;
    struct uart_handle_t *next_uart_handle;
} __attribute__((packed));

struct uart_handle_t *first_uart_handle = NULL;

struct uart_handle_t *lib_uart_init(uart_port_t uart_num,
                                    uint16_t baud_rate,
                                    uart_word_length_t data_bits,
                                    uart_parity_t parity,
                                    uart_stop_bits_t stop_bits,
                                    uart_hw_flowcontrol_t flow_ctrl,
                                    uint8_t txPort,
                                    uint8_t rxPort,
                                    uint8_t rtsPort,
                                    uint8_t ctsPort)
{
    static struct uart_handle_t *last_uart_handle_pointer = NULL;
    struct uart_handle_t *uart_handle = (struct uart_handle_t *)malloc(sizeof(struct uart_handle_t));

    if (last_uart_handle_pointer != NULL)
    {
        last_uart_handle_pointer->next_uart_handle = uart_handle;
    }
    else
    {
        first_uart_handle = uart_handle;
    }

    // create mutex
    uart_handle->xMutexUART = xSemaphoreCreateMutex();
    if (uart_handle->xMutexUART == NULL)
    {
        printf("Mutex can't allocated\n");
        while (1)
        {
        }
    }

    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = data_bits,
        .parity = parity,
        .stop_bits = stop_bits,
        .flow_ctrl = flow_ctrl,
        .rx_flow_ctrl_thresh = 122,
    };

    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart_handle->uartQueue, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, txPort, rxPort, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    last_uart_handle_pointer = uart_handle;

    return uart_handle;
}
