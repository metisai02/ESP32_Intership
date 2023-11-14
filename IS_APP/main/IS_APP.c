
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
#include "main.h"
#define WIFI_CONNECTED_BIT (1 << 0)
/************************************/
SSD1306_t dev;
extern QueueHandle_t xQueue_gpio;
TaskHandle_t TaskMenuHandle = NULL;
TaskHandle_t TaskMenuHandle1 = NULL;
EventGroupHandle_t event_group;
/************************************/
static void TaskMenuHandler(void *para);
static void TaskMenuHandler1(void *para);
void MenuOptionLCD(uint32_t relay, bool state);
void app_main(void)
{
    /*Init flash to stored */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
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
    uart_init();
    wifi_init();
    /******************************/
    event_group = xEventGroupCreate();
    xTaskCreate(TaskMenuHandler, "task", 2094, NULL, 11, &TaskMenuHandle);
    //  xTaskCreate(TaskMenuHandler1, "task", 2094, NULL, 11, &TaskMenuHandle1);
    xEventGroupWaitBits(event_group, WIFI_CONNECTED_BIT, true, true,
                        portMAX_DELAY);
    start_webserver();
}

static void TaskMenuHandler(void *para)
{
    control_t check_button;
    // uint32_t u32_count_nu1 = 0;
    // uint32_t u32_count_nu2 = 0;
    // uint32_t u32_count_nu3 = 0;
    bool b_state_1 = false;
    bool b_state_2 = false;
    bool b_state_3 = false;
    while (1)
    {
        if (xQueueReceive(xQueue_gpio, &check_button, portMAX_DELAY) == pdTRUE)
        {
            if (check_button.relay != 0)
            {
                if (!check_button.is_uart)
                {
                    char *data = malloc(3);
                    check_button.state = !gpio_get_level(check_button.relay);
                    char temp_1 = 'O';
                    if (check_button.state == 0)
                    {
                        temp_1 = 'F';
                    }
                    char temp_2 = '1';
                    if (check_button.relay == RELAY_2)
                        temp_2 = '2';
                    if (check_button.relay == RELAY_3)
                        temp_2 = '3';
                    /*Notify to GUI*/
                    sprintf(data, "%c%c", temp_2, temp_1);
                    int u8_Check = uart_write_bytes(UART_NUM_2, data, 2);
                    if (u8_Check < 0)
                    {
                        printf("Fail to transmission\n");
                    }
                    else
                    {
                        printf("suscess to transmission\n");
                    }
                    free(data);
                }
                if (check_button.relay == RELAY_1)
                {
                    gpio_set_level(LED_1, check_button.state);
                }
                else if (check_button.relay == RELAY_2)
                {
                    gpio_set_level(LED_2, check_button.state);
                }
                else
                {
                    gpio_set_level(LED_3, check_button.state);
                }

                gpio_set_level(check_button.relay, check_button.state);
                MenuOptionLCD(check_button.relay, check_button.state);
            }
        }
    }
}
void MenuOptionLCD(uint32_t relay, bool state)
{
    char *string_data = malloc(17);
    char *s_data = "ON";
    uint8_t index = 0;
    if (!state)
    {
        s_data = "OFF";
    }
    if (relay == RELAY_1)
    {
        index = 1;
    }
    else if (relay == RELAY_2)
    {
        index = 2;
    }
    else if (relay == RELAY_3)
    {
        index = 3;
    }
    sprintf(string_data, "RELAY_%u: %s", index, s_data);
    ssd1306_clear_line(&dev, index, false);
    ssd1306_display_text(&dev, index, string_data, strlen(string_data), true);
    free(string_data);
}