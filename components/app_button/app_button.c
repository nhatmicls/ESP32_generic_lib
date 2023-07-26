#include <stdio.h>
#include "app_button.h"

void press()
{
    printf("press\n");
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

    gpio_num_t gpio_pin;

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
