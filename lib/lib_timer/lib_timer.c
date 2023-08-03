#include <stdio.h>
#include "lib_timer.h"

struct timer_handle_t
{
    gptimer_handle_t gptimer;
    QueueHandle_t timer_isr_queue;
    TIMER_CALLBACK_FUNCTION_t callback;
    uint8_t last_state;
    struct timer_handle_t *next_timer_handle;
} __attribute__((packed));

typedef enum
{
    FREE,
    TRIGGER
} timer_state_t;

SemaphoreHandle_t timer_event_count = NULL;
struct timer_handle_t *first_timer_handle = NULL;

static const char *TAG = "TIMER";

void timer_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *data);

// timer_event_count =

struct timer_handle_t *lib_timer_init(gptimer_clock_source_t clk_src,
                                      gptimer_count_direction_t direction,
                                      uint64_t resolution,
                                      uint64_t reload_count,
                                      uint64_t alarm_count,
                                      bool auto_reload_on_alarm,
                                      bool isr_enable,
                                      TIMER_CALLBACK_FUNCTION_t callback)
{
    static struct timer_handle_t *last_timer_handle_pointer = NULL;
    struct timer_handle_t *timer_handle = (struct timer_handle_t *)malloc(sizeof(struct timer_handle_t));

    // init timer handle
    timer_handle->next_timer_handle = NULL;
    timer_handle->timer_isr_queue = xQueueCreate(10, sizeof(uint64_t));
    timer_handle->callback = NOP;
    timer_handle->last_state = FREE;

    if (timer_handle->timer_isr_queue == NULL)
    {
        ESP_LOGE(TAG, "Creating queue failed");
        while (1)
            ;
    }

    if (last_timer_handle_pointer != NULL)
    {
        last_timer_handle_pointer->next_timer_handle = timer_handle;
    }
    else
    {
        first_timer_handle = timer_handle;
    }

    gptimer_config_t timer_config = {
        .clk_src = clk_src,
        .direction = direction,
        .resolution_hz = resolution,
    };

    gptimer_alarm_config_t alarm_config = {
        .reload_count = reload_count,
        .alarm_count = alarm_count,
        .flags.auto_reload_on_alarm = auto_reload_on_alarm,
    };

    // enable timer
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &(timer_handle->gptimer)));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_handle->gptimer, &alarm_config));

    if (isr_enable == true)
    {
        gptimer_event_callbacks_t cbs = {
            .on_alarm = (void *)timer_isr,
        };
        timer_handle->callback = callback;
        gptimer_register_event_callbacks(timer_handle->gptimer, &cbs, (void *)timer_handle);
    }

    // ESP_ERROR_CHECK(gptimer_enable(timer_handle->gptimer));
    last_timer_handle_pointer = timer_handle;
    return timer_handle;
}

void timer_init(void)
{
    timer_event_count = xSemaphoreCreateCounting(100, 0);
}

void lib_timer_start(struct timer_handle_t *self)
{
    ESP_ERROR_CHECK(gptimer_enable(self->gptimer));
    ESP_ERROR_CHECK(gptimer_start(self->gptimer));
}

void lib_timer_deinit(struct timer_handle_t **self)
{
    // remove obj from linked list
    if (*self == first_timer_handle)
    {
        first_timer_handle = first_timer_handle->next_timer_handle;
    }
    else
    {
        struct timer_handle_t *timer_handle = first_timer_handle;
        do
        {
            if (timer_handle->next_timer_handle == *self)
            {
                timer_handle->next_timer_handle = (*self)->next_timer_handle;
                break;
            }
            timer_handle = timer_handle->next_timer_handle;
        } while (timer_handle->next_timer_handle != NULL);
    }

    // free memory
    free(*self);

    *self = NULL;
}

void timer_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *data)
{
    BaseType_t high_task_awoken = pdFALSE;
    struct timer_handle_t *self = (struct timer_handle_t *)data;

    self->last_state = TRIGGER;

    xSemaphoreGiveFromISR(timer_event_count, &high_task_awoken);
}

void lib_timer_ISR_task_handler(void *arg)
{
    while (1)
    {
        if (xSemaphoreTake(timer_event_count, portMAX_DELAY))
        {
            struct timer_handle_t *timer_handle = first_timer_handle;
            do
            {
                if (timer_handle->last_state == TRIGGER)
                {
                    timer_handle->callback();
                    timer_handle->last_state = FREE;
                    break;
                }
                timer_handle = timer_handle->next_timer_handle;
            } while (timer_handle != NULL);
        }
        taskYIELD();
        printf("Semaphore queue: %d\n", uxSemaphoreGetCount(timer_event_count));
        vTaskDelay(10);
    }

    vTaskDelete(NULL);
}
