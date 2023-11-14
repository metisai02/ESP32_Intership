#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "gpio.h"
#include <stdio.h>
#include "esp_timer.h"
#define BUTTON_DEBOUNCING 175
static volatile int64_t time_old_isr = 0;
QueueHandle_t xQueue_gpio;
int64_t get_timer_ms()
{
    return esp_timer_get_time() / 1000L;
}
static void IRAM_ATTR GpioIsrHandler(void *arg)
{
    int64_t time_now_isr = get_timer_ms();
    int64_t time_isr = (time_now_isr - time_old_isr);
    if (time_isr >= BUTTON_DEBOUNCING)
    {
        uint32_t relay = 0;
        if ((uint32_t)arg == BUTTON_1)
        {
            relay = RELAY_1;
        }
        else if ((uint32_t)arg == BUTTON_2)
        {
            relay = RELAY_2;
        }
        else if ((uint32_t)arg == BUTTON_3)
        {
            relay = RELAY_3;
        }
        control_t temp = {.relay = relay, .is_uart = false};
        xQueueSendFromISR(xQueue_gpio, &temp, NULL);
    }
    time_old_isr = time_now_isr;
}

void GpioInit(gpio_num_t number, gpio_mode_t mode)
{
    gpio_config_t base_gpio = {.mode = mode,
                               .pin_bit_mask = 1ULL << number,
                               .pull_up_en = GPIO_PULLUP_ENABLE,
                               .pull_down_en = GPIO_PULLDOWN_DISABLE};
    if (mode == GPIO_MODE_INPUT)
    {
        gpio_intr_disable(number);
        base_gpio.intr_type = GPIO_INTR_NEGEDGE;
        gpio_config(&base_gpio);
        // Install the ISR service and add the ISR handler
        gpio_install_isr_service(0);
        gpio_isr_handler_add(number, GpioIsrHandler, (void *)number);

        // Create a queue to hold GPIO pin values
        xQueue_gpio =
            xQueueCreate(10, sizeof(control_t)); // Increased queue size to 15
    }
    else
    {

        gpio_config(&base_gpio);
        gpio_set_direction(number, GPIO_MODE_INPUT_OUTPUT );
    }
}

void gpio_init()
{
    GpioInit(34, GPIO_MODE_INPUT);
    GpioInit(35, GPIO_MODE_INPUT);
    GpioInit(4, GPIO_MODE_INPUT);

    GpioInit(RELAY_1, GPIO_MODE_OUTPUT);
    GpioInit(RELAY_2, GPIO_MODE_OUTPUT);
    GpioInit(RELAY_3, GPIO_MODE_OUTPUT);

    GpioInit(LED_1, GPIO_MODE_OUTPUT);
    GpioInit(LED_2, GPIO_MODE_OUTPUT);
    GpioInit(LED_3, GPIO_MODE_OUTPUT);
}