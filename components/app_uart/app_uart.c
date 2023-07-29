#include <stdio.h>
#include "app_uart.h"

#define RX_BUF_SIZE 1024

struct uart_handle_t *uart2_handle;

void app_uart_init(void)
{
    uart2_handle = lib_uart_init(UART_NUM_2,
                                 57600,
                                 UART_DATA_8_BITS,
                                 UART_PARITY_DISABLE,
                                 UART_STOP_BITS_1,
                                 UART_HW_FLOWCTRL_DISABLE,
                                 GPIO_NUM_17,
                                 GPIO_NUM_16,
                                 UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE);
}

void app_uart(void *arg)
{
    uint8_t *string_recv = (uint8_t *)malloc(RX_BUF_SIZE + 1 * sizeof(uint8_t));

    while (1)
    {
        uint16_t length = recv_uart(UART_NUM_2, pdMS_TO_TICKS(1000), string_recv, RX_BUF_SIZE);
        if (length > 0)
        {
            printf("Data get: %s\n", string_recv);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    free(string_recv);
    vTaskDelete(NULL);
}
