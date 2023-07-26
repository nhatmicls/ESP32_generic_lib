#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "nvs_flash.h"

#include "./../components/app_gpio/include/app_gpio.h"
#include "./../components/app_button/include/app_button.h"

#define BLINK_GPIO 2
#define CONFIG_BLINK_PERIOD 1000

uint8_t s_led_state = 0;
QueueHandle_t queueTest = NULL;

void blink_led(void *)
{
    while (1)
    {
        gpio_set_level(BLINK_GPIO, s_led_state);
        s_led_state = !s_led_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT));

    TaskHandle_t ledHandle = NULL;

    TaskHandle_t buttonHandle = NULL;

    xTaskCreate(blink_led, "Led_Task", 2048, NULL, 10, &ledHandle);
    app_gpio();

    xTaskCreate(app_button, "Button_Task", 2048, NULL, tskIDLE_PRIORITY, &buttonHandle);
    while (1)
    {
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
