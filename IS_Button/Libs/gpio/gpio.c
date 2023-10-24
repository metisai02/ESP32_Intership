#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "gpio.h"
#include <stdio.h>
#include <time.h>
#define BUTTON_DEBOUNCING 50
static time_t time_old_isr = 0;
QueueHandle_t xQueue_gpio;
static void IRAM_ATTR GpioIsrHandler(void *arg)
{
    time_t time_now_isr = time(NULL);
    time_t time_isr = (time_now_isr - time_old_isr) * 1000L;
    if (time_isr >= BUTTON_DEBOUNCING)
    {
        uint32_t gpio_pin = (uint32_t)arg;
        xQueueSendFromISR(xQueue_gpio, &gpio_pin, NULL);
    }
    time_old_isr = time_now_isr;
}

void GpioInit(gpio_num_t number, gpio_mode_t mode)
{
    gpio_config_t base_gpio = {.mode = mode,
                               .pin_bit_mask = 1 << number,
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
            xQueueCreate(10, sizeof(uint32_t)); // Increased queue size to 10
    }
    else
    {
        gpio_config(&base_gpio);
    }
}
