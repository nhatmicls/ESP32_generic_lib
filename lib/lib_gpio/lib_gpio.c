#include <stdio.h>
#include "lib_gpio.h"

void lib_gpio_init(uint64_t gpio_bit_mask, gpio_mode_t mode, gpio_pull_mode_t pull_mode, gpio_int_type_t intr_mode)
{
    gpio_config_t gpio_config_data = {};

    // set bit mask
    gpio_config_data.pin_bit_mask = gpio_bit_mask;

    // set gpio mode
    gpio_config_data.mode = mode;

    // set pulling mode
    if (pull_mode == GPIO_PULLUP_ONLY)
    {
        gpio_config_data.pull_up_en = GPIO_PULLUP_ENABLE;
    }
    else if (pull_mode == GPIO_PULLDOWN_ONLY)
    {
        gpio_config_data.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }
    else if (pull_mode == GPIO_PULLUP_PULLDOWN)
    {
        gpio_config_data.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config_data.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }
    else if (pull_mode == GPIO_FLOATING)
    {
        gpio_config_data.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config_data.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }

    // set intr_mode
    gpio_config_data.intr_type = intr_mode;

    gpio_config(&gpio_config_data);
}

void lib_gpio_output_init(gpio_num_t gpio_pin)
{
    ESP_ERROR_CHECK(gpio_reset_pin(gpio_pin));
    ESP_ERROR_CHECK(gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT));
}

void lib_gpio_input_init(gpio_num_t gpio_pin)
{
    ESP_ERROR_CHECK(gpio_reset_pin(gpio_pin));
    ESP_ERROR_CHECK(gpio_set_direction(gpio_pin, GPIO_MODE_INPUT));
}

void lib_gpio_set_value(gpio_num_t gpio_pin, uint8_t value)
{
    ESP_ERROR_CHECK(gpio_set_level(gpio_pin, value));
}

uint8_t lib_gpio_get_value(gpio_num_t gpio_pin)
{
    return gpio_get_level(gpio_pin);
}
