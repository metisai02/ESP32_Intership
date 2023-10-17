#include <stdio.h>
#include "driver/gpio.h"
#include "gpio.h"

void GpioInit(gpio_num_t number, gpio_mode_t mode)
{
    gpio_config_t base_gpio = {
        .mode = mode,
        .pin_bit_mask = 1 << number,
        .pull_up_en = GPIO_PULLDOWN_ENABLE,
    };
    gpio_config(&base_gpio);
}
