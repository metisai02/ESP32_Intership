#include "uart_is.h"
#include "driver/uart.h"
#include <stdio.h>
#include <string.h>
static const int RX_BUF_SIZE = 1024;
extern QueueHandle_t xQueue_gpio;
TaskHandle_t xtask_uart_handle = NULL;
static void UartTaskHanlder(void *para);
void uart_init() {
  const uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };
  // We won't use a buffer for sending data.
  uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(UART_NUM_1, &uart_config);
  uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE,
               UART_PIN_NO_CHANGE);
  xTaskCreate(UartTaskHanlder, "task uart handler", 2094, NULL, 10,
              &xtask_uart_handle);
}
static void UartTaskHanlder(void *para) {
  uint32_t gpio_pin = 0;
  static const char *RX_TASK_TAG = "RX_TASK";
  esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
  char *data = (char *)malloc(RX_BUF_SIZE + 1);
  while (1) {
    const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE,
                                        500 / portTICK_PERIOD_MS);
    if (rxBytes > 0) {
      data[rxBytes] = '\0';
      ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
      if (strcmp(data, "RELAY_1") == 1) {
        gpio_pin = BUTTON_1;
        xQueueSend(xQueue_gpio, &gpio_pin, 10);
      } else if (strcmp(data, "RELAY_2") == 1) {
        gpio_pin = BUTTON_2;
        xQueueSend(xQueue_gpio, &gpio_pin, 10);

      } else if (strcmp(data, "RELAY_3") == 1) {
        gpio_pin = BUTTON_3;
        xQueueSend(xQueue_gpio, &gpio_pin, 10);
      }
    }
    free(data);
  }
}
