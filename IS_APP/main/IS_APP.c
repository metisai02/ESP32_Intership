
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "gpio.h"
#include "http_server.h"
#include "nvs_flash.h"
#include "ssd1306.h"
#include "string.h"
#include "wifi.h"
#include <stdio.h>
#include <stdlib.h>
#include "uart_is.h"
#define WIFI_CONNECTED_BIT (1 << 0)
/************************************/
SSD1306_t dev;
extern QueueHandle_t xQueue_gpio;
TaskHandle_t TaskMenuHandle = NULL;
EventGroupHandle_t event_group;
/************************************/
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
  /*Init Oled LCD*/
  i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
  ssd1306_init(&dev, 128, 32);
  ssd1306_clear_screen(&dev, false);
  ssd1306_contrast(&dev, 0xff);
  /* Init Periperal on Board*/
  gpio_init();

  /**/
  event_group = xEventGroupCreate();
  xTaskCreate(TaskMenuHandler, "task", 2094, NULL, 11, &TaskMenuHandle);
  xEventGroupWaitBits(event_group, WIFI_CONNECTED_BIT, true, true,
                      portMAX_DELAY);
  start_webserver();
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
        /*Toggle Relay1 and Led1*/
        MenuOptionLCD(1, b_state_1);
        gpio_set_level(RELAY_1, b_state_1);
        gpio_set_level(LED_1, b_state_1);
        b_state_1 = !b_state_1;
        break;
      case BUTTON_2:
        /*Toggle Relay2 and Led2*/
        MenuOptionLCD(2, b_state_2);
        gpio_set_level(RELAY_2, b_state_2);
        gpio_set_level(LED_2, b_state_2);
        b_state_2 = !b_state_2;
        break;
      case BUTTON_3:
        /*Toggle Relay3 and Led3*/
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