#include <stdio.h>
#include "app_button.h"

void press()
{
    printf("press\n");
    char test_str[] = "This is a test string.\r\n";
    send_uart(UART_NUM_2, test_str);
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
        vTaskSuspend(NULL);
    }

    vTaskDelete(NULL);
}
