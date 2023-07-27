#include <stdio.h>
#include "lib_uart.h"

struct uart_handle_t
{
    uart_port_t uart_num;
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
                                    int8_t txPort,
                                    int8_t rxPort,
                                    int8_t rtsPort,
                                    int8_t ctsPort)
{
    static struct uart_handle_t *last_uart_handle_pointer = NULL;
    struct uart_handle_t *uart_handle = (struct uart_handle_t *)malloc(sizeof(struct uart_handle_t));

    // init uart handle
    uart_handle->next_uart_handle = NULL;
    uart_handle->uart_num = uart_num;

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
    ESP_ERROR_CHECK(uart_set_pin(uart_num, txPort, rxPort, rtsPort, ctsPort));

    last_uart_handle_pointer = uart_handle;
    return uart_handle;
}

void send_uart(uart_port_t uart_num, char *send_data)
{
    struct uart_handle_t *uart_handle = first_uart_handle;
    do
    {
        if (uart_handle->uart_num == uart_num)
        {
            if (xSemaphoreTake(uart_handle->xMutexUART, portMAX_DELAY))
            {
                uart_write_bytes(uart_num, (const char *)send_data, strlen(send_data));
                xSemaphoreGive(uart_handle->xMutexUART);
            }
            break;
        }
        uart_handle = uart_handle->next_uart_handle;
    } while (uart_handle->next_uart_handle != NULL);
}

uint8_t recv_uart(uart_port_t uart_num, uint8_t timeout)
{
    uint8_t data[BUF_SIZE];
    int rxBytes = 0;

    memset(data, 0, BUF_SIZE);

    if (timeout > 10)
    {
        rxBytes = uart_read_bytes(uart_num, data, BUF_SIZE, pdMS_TO_TICKS(timeout));
    }
    else
    {
        rxBytes = uart_read_bytes(uart_num, data, BUF_SIZE, portMAX_DELAY);
    }

    if (rxBytes > 0)
    {
    }
}