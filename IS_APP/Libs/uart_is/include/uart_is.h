#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#define BUTTON_1 17
#define BUTTON_2 16
#define BUTTON_3 4

#define RELAY_1 12
#define RELAY_2 14
#define RELAY_3 27

#define LED_1 2
#define LED_2 15
#define LED_3 13

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
#define TXD_PIN (GPIO_NUM_34)
#define RXD_PIN (GPIO_NUM_35)

void uart_init();
