#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "ssd1306.h"
#include "string.h"
#include <gpio.h>
#include <stdio.h>
#include <stdlib.h>

#define BUTTON_1 17
#define BUTTON_2 16
#define BUTTON_3 4

#define RELAY_1 12
#define RELAY_2 14
#define RELAY_3 27

#define LED_1 2
#define LED_2 15
#define LED_3 13
SSD1306_t dev;
extern QueueHandle_t xQueue_gpio;
TaskHandle_t TaskMenuHandle = NULL;
static void TaskMenuHandler(void *para);
void MenuOptionLCD(uint8_t index, bool state);
void app_main(void) {
  /*Init flash to stored */
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  /* Init GPIO on Board*/
  GpioInit(BUTTON_1, GPIO_MODE_INPUT);
  GpioInit(BUTTON_2, GPIO_MODE_INPUT);
  GpioInit(BUTTON_3, GPIO_MODE_INPUT);

  GpioInit(RELAY_1, GPIO_MODE_OUTPUT);
  GpioInit(RELAY_2, GPIO_MODE_OUTPUT);
  GpioInit(RELAY_3, GPIO_MODE_OUTPUT);

  GpioInit(LED_1, GPIO_MODE_OUTPUT);
  GpioInit(LED_2, GPIO_MODE_OUTPUT);
  GpioInit(LED_3, GPIO_MODE_OUTPUT);

  /*Init Oled LCD*/
  i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
  ssd1306_init(&dev, 128, 32);
  ssd1306_clear_screen(&dev, false);
  ssd1306_contrast(&dev, 0xff);

  /*Create Task to test*/
  xTaskCreate(TaskMenuHandler, "task", 2094, NULL, 10, &TaskMenuHandle);
}
static void TaskMenuHandler(void *para) {
  uint32_t u32_number_pin;
  uint32_t u32_count_nu1 = 0;
  uint32_t u32_count_nu2 = 0;
  uint32_t u32_count_nu3 = 0;
  bool b_state_1 = false;
  bool b_state_2 = false;
  bool b_state_3 = false;
  while (1) {
    if (xQueueReceive(xQueue_gpio, &u32_number_pin, 1000) == pdTRUE) {
      switch (u32_number_pin) {
      case BUTTON_1:
        /* Go down */
        MenuOptionLCD(1, b_state_1);
        gpio_set_level(RELAY_1, b_state_1);
        gpio_set_level(LED_1, b_state_1);
        b_state_1 = !b_state_1;
        break;
      case BUTTON_2:
        /*Go up*/
        MenuOptionLCD(2, b_state_2);
        gpio_set_level(RELAY_2, b_state_2);
        gpio_set_level(LED_2, b_state_2);
        b_state_2 = !b_state_2;
        break;
      case BUTTON_3:
        /*Selecting*/
        MenuOptionLCD(3, b_state_3);
        gpio_set_level(RELAY_3, b_state_3);
        gpio_set_level(LED_3, b_state_3);
        b_state_3 = !b_state_3;
        break;
      default:
        break;
      }
    }
  }
}

void MenuOptionLCD(uint8_t index, bool state) {

  char *string_data = malloc(17);
  char *s_data = "ON";
  if (!state) {
    s_data = "OFF";
  }
  sprintf(string_data, "RELAY_%u: %s", index, s_data);
  ssd1306_clear_line(&dev, index, false);
  ssd1306_display_text(&dev, index, string_data, strlen(string_data), true);
  free(string_data);
}