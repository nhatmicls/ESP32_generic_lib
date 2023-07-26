#include <stdio.h>
#include "lib_button.h"

struct button_callback_list
{
    BUTTON_CALLBACK_FUNCTION_t press_callback;
    BUTTON_CALLBACK_FUNCTION_t short_press_callback;
    BUTTON_CALLBACK_FUNCTION_t long_press_callback;
    struct button_callback_list *next_button_callback;
    gpio_num_t gpio_pin;
    uint8_t last_value;
    button_event_t last_button_event;
    uint64_t last_button_event_time;
} __attribute__((packed));

QueueHandle_t button_event_queue = NULL;
QueueHandle_t button_event_debouncing_queue = NULL;
QueueHandle_t button_event_time_queue = NULL;
TimerHandle_t button_soft_timer = NULL;

struct button_callback_list *first_button_handle = NULL;
static uint8_t button_count = 0;

void IRAM_ATTR lib_button_isr_handler(struct button_callback_list *self);
void lib_softtime_isr_handler(void);

/**
 * @brief initialize button
 *
 * @param gpio_pin
 * @param pull_mode
 * @param intr_mode
 */
struct button_callback_list *lib_button_init(gpio_num_t gpio_pin, button_pull_loc_t pull_loc, gpio_pull_mode_t pull_mode, gpio_int_type_t intr_mode)
{
    static struct button_callback_list *last_button_callback_pointer = NULL;
    uint8_t state_releasing = 0;
    struct button_callback_list *button_callback = (struct button_callback_list *)malloc(sizeof(struct button_callback_list));

    // set init data for callback struct
    button_callback->press_callback = NULL;
    button_callback->short_press_callback = NULL;
    button_callback->long_press_callback = NULL;
    button_callback->next_button_callback = NULL;

    // set linked list for button
    if (last_button_callback_pointer != NULL)
    {
        last_button_callback_pointer->next_button_callback = button_callback;
    }
    else
    {
        first_button_handle = button_callback;

        gpio_install_isr_service(BUTTON_ESP_INTR_FLAG_DEFAULT);

        button_event_queue = xQueueCreate(BUTTON_MAX_QUEUE, sizeof(gpio_num_t));
        button_event_debouncing_queue = xQueueCreate(BUTTON_MAX_QUEUE, sizeof(gpio_num_t));
        button_event_time_queue = xQueueCreate(BUTTON_MAX_QUEUE, sizeof(uint64_t));

        button_soft_timer = xTimerCreate("button_soft_timer",
                                         pdMS_TO_TICKS(100),
                                         pdFALSE,
                                         (void *)0,
                                         (TimerCallbackFunction_t)lib_softtime_isr_handler);
        // xTimerStart(button_soft_timer, 1);

        // create queue for ISR
        if (!button_event_queue)
        {
            printf("Creating queue for button failed\n");
            while (1)
                ;
        }
    }

    button_callback->gpio_pin = gpio_pin;

    // button init
    gpio_config_t gpio_config_data = {};

    // set bit mask
    gpio_config_data.pin_bit_mask = 1UL << gpio_pin;

    // set gpio mode
    gpio_config_data.mode = GPIO_MODE_INPUT;

    // set pulling mode
    if (pull_mode == GPIO_PULLUP_ONLY)
    {
        state_releasing = 1;
        gpio_config_data.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config_data.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }
    else if (pull_mode == GPIO_PULLDOWN_ONLY)
    {
        state_releasing = 0;
        gpio_config_data.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config_data.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }
    else if (pull_mode == GPIO_PULLUP_PULLDOWN)
    {
        state_releasing = 0;
        gpio_config_data.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config_data.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }
    else if (pull_mode == GPIO_FLOATING)
    {
        state_releasing = 0;
        gpio_config_data.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config_data.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }

    // disable pull mode in mcu if it set for external
    if (pull_loc == EXTERNAL)
    {
        gpio_config_data.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config_data.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }

    // set intr_mode
    gpio_config_data.intr_type = intr_mode;
    if (intr_mode != GPIO_INTR_DISABLE)
    {
        ESP_ERROR_CHECK(gpio_isr_handler_add(gpio_pin, (gpio_isr_t)lib_button_isr_handler, button_callback));
    }

    // config button gpio
    gpio_config(&gpio_config_data);

    // init state button
    if (gpio_get_level(button_callback->gpio_pin) == state_releasing)
    {
        button_callback->last_button_event = RELEASING;
    }
    else
    {
        button_callback->last_button_event = PRESSING;
    }

    button_callback->last_button_event_time = 0;

    // set next item linked list
    last_button_callback_pointer = button_callback;
    ++button_count;

    return button_callback;
}

void lib_button_deinit(struct button_callback_list **self)
{
    // remove obj from linked list
    if (*self == first_button_handle)
    {
        first_button_handle = first_button_handle->next_button_callback;
    }
    else
    {
        struct button_callback_list *current_button = first_button_handle;
        for (uint8_t i = 0; i < button_count; i++)
        {
            if (current_button->next_button_callback == *self)
            {
                current_button->next_button_callback = (*self)->next_button_callback;
            }
        }
    }

    // free memory
    free(*self);

    *self = NULL;
}

void lib_add_press_callback(struct button_callback_list *self, BUTTON_CALLBACK_FUNCTION_t callback)
{
    self->press_callback = callback;
}

void lib_add_shortpress_callback(struct button_callback_list *self, BUTTON_CALLBACK_FUNCTION_t callback)
{
    self->short_press_callback = callback;
}

void lib_add_longpress_callback(struct button_callback_list *self, BUTTON_CALLBACK_FUNCTION_t callback)
{
    self->long_press_callback = callback;
}

void debug_isr(char *from)
{
    printf("[%s] Event queue:%d, Event debounding queue:%d, Event time queue:%d \n",
           from,
           uxQueueMessagesWaiting(button_event_queue),
           uxQueueMessagesWaiting(button_event_debouncing_queue),
           uxQueueMessagesWaiting(button_event_time_queue));
}

void lib_button_isr_handler(struct button_callback_list *self)
{
    gpio_intr_disable(self->gpio_pin);

    BaseType_t high_task_awoken = pdFALSE;
    uint64_t system_tick = (uint64_t)esp_timer_get_time() / 1000;

    xQueueSendFromISR(button_event_queue, &(self->gpio_pin), &high_task_awoken);
    xQueueSendFromISR(button_event_debouncing_queue, &(self->gpio_pin), &high_task_awoken);
    xQueueSendFromISR(button_event_time_queue, &(system_tick), &high_task_awoken);

    xTimerStartFromISR(button_soft_timer, &high_task_awoken);
}

void lib_softtime_isr_handler(void)
{
    uint8_t value;
    if (xQueueReceive(button_event_queue, &value, portMAX_DELAY))
    {
        gpio_intr_enable(value);
        xTimerStart(button_soft_timer, 1);
    }
}

void lib_button_ISR_task_handler(void *arg)
{
    while (1)
    {
        struct button_callback_list *current_button = first_button_handle;

        for (uint8_t i = 0; i < button_count; i++)
        {
            if (current_button->last_button_event == PRESS ||
                current_button->last_button_event == SHORT_HOLD ||
                current_button->last_button_event == LONG_HOLD)
            {
                if (current_button->last_button_event == PRESS && current_button->press_callback != NULL)
                {
                    current_button->press_callback();
                }
                else if (current_button->last_button_event == SHORT_HOLD && current_button->short_press_callback != NULL)
                {
                    current_button->short_press_callback();
                }
                else if (current_button->last_button_event == LONG_HOLD && current_button->long_press_callback != NULL)
                {
                    current_button->long_press_callback();
                }
                current_button->last_button_event = RELEASING;
            }
            current_button = current_button->next_button_callback;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void lib_button_debouncing_task_handler(void *arg)
{
    uint8_t buttonPin;
    uint64_t systemTickTimer;

    while (1)
    {
        if (xQueueReceive(button_event_debouncing_queue, &buttonPin, portMAX_DELAY) == pdTRUE &&
            xQueueReceive(button_event_time_queue, &systemTickTimer, portMAX_DELAY) == pdTRUE)
        {
            printf("GPIO: %d, time: %lld\n", (int)buttonPin, systemTickTimer);

            struct button_callback_list *current_button = first_button_handle;

            for (uint8_t i = 0; i < button_count; i++)
            {
                uint64_t deltaTime = systemTickTimer - current_button->last_button_event_time;
                if (current_button->gpio_pin == buttonPin)
                {
                    printf("delta time: %lld, stage: %d\n", deltaTime, current_button->last_button_event);
                    if (current_button->last_button_event == PRESSING && deltaTime > BUTTON_PRESS_TIME)
                    {
                        printf("PRESSING\n");
                        if (deltaTime > BUTTON_LONG_HOLDING_TIME)
                        {
                            current_button->last_button_event = LONG_HOLD;
                        }
                        else if (deltaTime > BUTTON_SHORT_HOLDING_TIME)
                        {
                            current_button->last_button_event = SHORT_HOLD;
                        }
                        else if (deltaTime > BUTTON_PRESS_TIME)
                        {
                            current_button->last_button_event = PRESS;
                        }
                    }
                    else if (current_button->last_button_event == RELEASING && deltaTime > BUTTON_PRESS_TIME)
                    {
                        printf("RELEASING\n");
                        current_button->last_button_event = PRESSING;
                    }
                    current_button->last_button_event_time = systemTickTimer;
                    break;
                }
                else
                {
                    current_button = current_button->next_button_callback;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}