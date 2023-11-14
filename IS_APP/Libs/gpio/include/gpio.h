
#include <stdio.h>
#include "driver/gpio.h"
#include "main.h"
#define BUTTON_1 35
#define BUTTON_2 34
#define BUTTON_3 4

#define RELAY_1 12
#define RELAY_2 14
#define RELAY_3 27

#define LED_1 2
#define LED_2 15
#define LED_3 13

void GpioInit(gpio_num_t number, gpio_mode_t mode);
void gpio_init();