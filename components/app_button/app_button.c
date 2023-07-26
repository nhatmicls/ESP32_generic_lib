#include <stdio.h>
#include "app_button.h"

void press()
{
    printf("press\n");
    char *test_str = "This is a test string.\n";
    uart_write_bytes(UART_NUM_2, (const char *)test_str, strlen(test_str));
}

void short_press()
{
    printf("short press\n");
}

void long_press()
{
    printf("long press\n");
}

void app_button(void *)
{
    struct button_callback_list *gpio_25_handle = lib_button_init(GPIO_NUM_25, EXTERNAL, GPIO_PULLUP_ONLY, GPIO_INTR_ANYEDGE);
    struct button_callback_list *gpio_26_handle = lib_button_init(GPIO_NUM_26, EXTERNAL, GPIO_PULLUP_ONLY, GPIO_INTR_ANYEDGE);
    struct uart_handle_t *uart2_handle = lib_uart_init(UART_NUM_2,
                                                       57600,
                                                       UART_DATA_8_BITS,
                                                       UART_PARITY_DISABLE,
                                                       UART_STOP_BITS_1,
                                                       UART_HW_FLOWCTRL_DISABLE,
                                                       GPIO_NUM_17,
                                                       GPIO_NUM_16,
                                                       UART_PIN_NO_CHANGE,
                                                       UART_PIN_NO_CHANGE);

    lib_add_press_callback(gpio_25_handle, press);
    lib_add_shortpress_callback(gpio_25_handle, short_press);
    lib_add_longpress_callback(gpio_25_handle, long_press);

    lib_add_press_callback(gpio_26_handle, press);
    lib_add_shortpress_callback(gpio_26_handle, short_press);
    lib_add_longpress_callback(gpio_26_handle, long_press);

    xTaskCreate(lib_button_ISR_task_handler, "Button_Task", 2048, NULL, 1, NULL);
    xTaskCreate(lib_button_debouncing_task_handler, "Button_Debouncing_Task", 2048, NULL, 5, NULL);

    while (1)
    {
        printf("total ram: %d\n", heap_caps_get_total_size(MALLOC_CAP_DEFAULT));
        printf("free ram: %d\n", heap_caps_get_free_size(MALLOC_CAP_32BIT));
        vTaskSuspend(NULL);
    }

    vTaskDelete(NULL);
}
