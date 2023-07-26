#include <stdio.h>
#include "app_gpio.h"

#define CONFIG_DELAY_MAX 250

uint8_t app_gpio_state = 0;
uint8_t app_led_loop = 0;
uint8_t app_led_data[7] = {LED_OFF};
uint8_t app_led_delay = CONFIG_DELAY_MAX;

void state_change(void);
void all_black(void);
void data_change(void);

void app_led_rbg(void *)
{
    uint64_t bit_mask = 0UL;

    bit_mask |= 1ULL << LED_RED;
    bit_mask |= 1ULL << LED_BLUE;
    bit_mask |= 1ULL << LED_GREEN;

    lib_gpio_init(bit_mask, GPIO_MODE_OUTPUT, GPIO_FLOATING, GPIO_INTR_DISABLE);
    while (1)
    {
        lib_gpio_set_value(LED_RED, 0);
        lib_gpio_set_value(LED_BLUE, 1);
        lib_gpio_set_value(LED_GREEN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        lib_gpio_set_value(LED_RED, 1);
        lib_gpio_set_value(LED_BLUE, 0);
        lib_gpio_set_value(LED_GREEN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        lib_gpio_set_value(LED_RED, 1);
        lib_gpio_set_value(LED_BLUE, 1);
        lib_gpio_set_value(LED_GREEN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        lib_gpio_set_value(LED_RED, 1);
        lib_gpio_set_value(LED_BLUE, 0);
        lib_gpio_set_value(LED_GREEN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_led_7_seg(void *)
{
    uint64_t bit_mask = 0UL;

    bit_mask |= 1UL << LED_A;
    bit_mask |= 1UL << LED_B;
    bit_mask |= 1UL << LED_C;
    bit_mask |= 1UL << LED_D;
    bit_mask |= 1UL << LED_E;
    bit_mask |= 1UL << LED_F;
    bit_mask |= 1UL << LED_G;
    bit_mask |= 1UL << LED_DP;

    lib_gpio_init(bit_mask, GPIO_MODE_OUTPUT, GPIO_FLOATING, GPIO_INTR_DISABLE);
    all_black();

    while (1)
    {
        lib_gpio_set_value(LED_A, app_led_data[0]);
        lib_gpio_set_value(LED_B, app_led_data[1]);
        lib_gpio_set_value(LED_C, app_led_data[2]);
        lib_gpio_set_value(LED_D, app_led_data[3]);
        lib_gpio_set_value(LED_E, app_led_data[4]);
        lib_gpio_set_value(LED_F, app_led_data[5]);
        lib_gpio_set_value(LED_G, app_led_data[6]);
        ++app_led_loop;
        vTaskDelay(app_led_delay / portTICK_PERIOD_MS);
        data_change();
    }
}

void app_gpio()
{
    // xTaskCreate(app_led_7_seg, "Led_7_seg_Task", 2048, NULL, 10, NULL);
    xTaskCreate(app_led_rbg, "Led_rbg_Task", 2048, NULL, 10, NULL);
}

void data_change(void)
{
    static uint8_t led_state = 0;

    all_black();

    switch (led_state)
    {
    case 1:
        app_led_data[0] = LED_ON;
        led_state = 2;
        break;
    case 2:
        app_led_data[1] = LED_ON;
        led_state = 3;
        break;
    case 3:
        app_led_data[2] = LED_ON;
        led_state = 4;
        break;
    case 4:
        app_led_data[3] = LED_ON;
        led_state = 5;
        break;
    case 5:
        app_led_data[4] = LED_ON;
        led_state = 6;
        break;
    case 6:
        app_led_data[5] = LED_ON;
        state_change();
        led_state = 1;
        break;
    default:
        led_state = 1;
        break;
    }
}

void state_change(void)
{
    static uint8_t app_state = 0;
    switch (app_state)
    {
    case 1:
        app_led_delay = CONFIG_DELAY_MAX;
        app_state = 2;
        break;
    case 2:
        app_led_delay -= 10;
        if (app_led_delay == 10)
        {
            app_state = 3;
            app_led_loop = 0;
        }
        break;
    case 3:
        if (app_led_loop > 100)
        {
            app_state = 4;
            app_led_loop = 0;
        }
        break;
    case 4:
        app_led_delay += 10;
        if (app_led_delay == CONFIG_DELAY_MAX)
        {
            app_state = 2;
            app_led_loop = 0;
        }
        break;
    default:
        app_state = 1;
        break;
    }
}

void all_black(void)
{
    for (int x = 0; x < sizeof(app_led_data) / sizeof(uint8_t); x++)
    {
        app_led_data[x] = LED_OFF;
    }
}