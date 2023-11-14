#include "http_server.h"
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_netif.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "main.h"
#include <esp_http_server.h>
#include <esp_log.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
extern EventGroupHandle_t event_group;
QueueHandle_t xQueue_web;
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
httpd_handle_t server = NULL;
TaskHandle_t xTaskControlSW = NULL;
static void Task_SW_Handler(void *para);
/* Our URI handler function to be called during GET /uri request */
esp_err_t get_web_handler(httpd_req_t *req)
{
    /* Send a simple response */
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start,
                    index_html_end - index_html_start);
    return ESP_OK;
}
esp_err_t sw1_post_handler(httpd_req_t *req)
{
    char buf[100];
    sw_data_t swData1;
    httpd_req_recv(req, buf, req->content_len);
    swData1.swID = SWITCH_1;
    swData1.swData = buf[0];
    xQueueSend(xQueue_web, &swData1, 0);
    httpd_resp_send_chunk(req, NULL, 0);
    printf("SWITCH: %d\n", swData1.swID);
    printf("DATA: %d\n", swData1.swData);
    if (swData1.swData == '1')
    {
        gpio_set_level(GPIO_NUM_27, 1);
    }
    else
    {
        gpio_set_level(GPIO_NUM_27, 0);
    }
    return ESP_OK;
}
esp_err_t sw2_post_handler(httpd_req_t *req)
{
    char buf[100];
    sw_data_t swData2;
    httpd_req_recv(req, buf, req->content_len);
    swData2.swID = SWITCH_2;
    swData2.swData = buf[0];
    xQueueSend(xQueue_web, &swData2, 10);
    httpd_resp_send_chunk(req, NULL, 0);
    printf("SWITCH: %d\n", swData2.swID);
    if (swData2.swData == '1')
    {
        gpio_set_level(GPIO_NUM_14, 1);
    }
    else
    {
        gpio_set_level(GPIO_NUM_14, 0);
    }
    return ESP_OK;
}
esp_err_t sw3_post_handler(httpd_req_t *req)
{
    char buf[100];
    sw_data_t swData3;
    httpd_req_recv(req, buf, req->content_len);
    swData3.swID = SWITCH_3;
    swData3.swData = buf[0];
    xQueueSend(xQueue_web, &swData3, 0);
    httpd_resp_send_chunk(req, NULL, 0);
    printf("SWITCH: %d\n", swData3.swID);
    printf("DATA: %d\n", swData3.swData);
    if (swData3.swData == '1')
    {
        gpio_set_level(GPIO_NUM_12, 1);
    }
    else
    {
        gpio_set_level(GPIO_NUM_12, 0);
    }
    return ESP_OK;
}

/* Our URI handler function to be called during POST /uri request */
esp_err_t get_data_handler(httpd_req_t *req)
{
    const char *resp = (const char *)"{\"temperature\": \"30.4\"}";
    httpd_resp_send(req, resp, strlen(resp));
    return ESP_OK;
}

/* URI handler structure for GET /uri */
httpd_uri_t uri_get_web = {.uri = "/",
                           .method = HTTP_GET,
                           .handler = get_web_handler,
                           .user_ctx = NULL};
httpd_uri_t uri_get_data = {.uri = "/getdata",
                            .method = HTTP_GET,
                            .handler = get_data_handler,
                            .user_ctx = NULL};

/* URI handler structure for POST /uri */
httpd_uri_t uri_post_sw1 = {.uri = "/sw1",
                            .method = HTTP_POST,
                            .handler = sw1_post_handler,
                            .user_ctx = NULL};
httpd_uri_t uri_post_sw2 = {.uri = "/sw2",
                            .method = HTTP_POST,
                            .handler = sw2_post_handler,
                            .user_ctx = NULL};
httpd_uri_t uri_post_sw3 = {.uri = "/sw3",
                            .method = HTTP_POST,
                            .handler = sw3_post_handler,
                            .user_ctx = NULL};
/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Empty handle to esp_http_server */

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK)
    {

        /* Register URI handlers */
        printf("server start\n");
        xTaskCreate(Task_SW_Handler, "handler SW", 1028, NULL, 11, &xTaskControlSW);
        httpd_register_uri_handler(server, &uri_get_web);
        httpd_register_uri_handler(server, &uri_get_data);
        httpd_register_uri_handler(server, &uri_post_sw1);
        httpd_register_uri_handler(server, &uri_post_sw2);
        httpd_register_uri_handler(server, &uri_post_sw3);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server)
    {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}
static void Task_SW_Handler(void *para)
{
    sw_data_t swgetData;
    while (1)
    {
        if (xQueueReceive(xQueue_web, &swgetData, 1000) == pdTRUE)
        {
            switch (swgetData.swID)
            {
            case SWITCH_1:
                /* code */
                printf("Get SWITCH_1\n");
                break;
            case SWITCH_2:
                /* code */
                printf("Get SWITCH_2\n");
                break;
            case SWITCH_3:
                /* code */
                printf("Get SWITCH_3\n");
                break;
            default:
                printf("ERROR WHEN RECEIVING DATA\n");
                break;
            }
        }
        else
        {
        }
        vTaskDelay(10);
    }
}
