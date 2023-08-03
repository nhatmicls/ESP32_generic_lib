#include <stdio.h>
#include "app_button.h"

void test(void)
{
    char test_str[] = "This is a test string.\r\n";
    send_uart(UART_NUM_2, test_str);
}

struct timer_handle_t *ts = NULL;

void press()
{
    printf("press\n");
    lib_timer_start(ts);
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

    ts = lib_timer_init(SOC_MOD_CLK_APB, GPTIMER_COUNT_UP, 1000000, 0, 1000000, false, true, test);

    lib_add_press_callback(gpio_25_handle, press);
    lib_add_shortpress_callback(gpio_25_handle, short_press);
    lib_add_longpress_callback(gpio_25_handle, long_press);

    lib_add_press_callback(gpio_26_handle, press);
    lib_add_shortpress_callback(gpio_26_handle, short_press);
    lib_add_longpress_callback(gpio_26_handle, long_press);

    xTaskCreate(lib_button_ISR_task_handler, "Button_Task", 2048, NULL, ISR_TASK_PRIORITY, NULL);
    xTaskCreate(lib_button_debouncing_task_handler, "Button_Debouncing_Task", 2048, NULL, ISR_TASK_DEBOUND, NULL);

    while (1)
    {
        vTaskSuspend(NULL);
    }

    vTaskDelete(NULL);
}
