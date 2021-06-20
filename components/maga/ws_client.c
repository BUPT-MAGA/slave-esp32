
/*********************
 *      INCLUDES
 *********************/
#include "ws_client.h"
#include "ws_client_json.h"
#include "air_slave.h"

#include "esp_websocket_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_log.h"

EventGroupHandle_t ws_client_event;
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ws_event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/**********************
 *  STATIC VARIABLES
 **********************/

static const char *TAG = "WS_CLIENT";

static esp_websocket_client_config_t ws_cfg = { .uri = WS_SERVER_URI_BASE };
static char ws_server_uri[128];
// static TimerHandle_t shutdown_signal_timer;
static esp_websocket_client_handle_t ws_client;
static char ws_client_tbuf[128];
static ws_msg ws_client_rbuf;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

// void ws_client_init()
// {
//     /* TODO: 超时定时器 */
//     // shutdown_signal_timer = xTimerCreate("Websocket shutdown timer", NO_DATA_TIMEOUT_SEC * 1000 / portTICK_PERIOD_MS,
//     //                                      pdFALSE, NULL, shutdown_signaler);
//     ws_client = esp_websocket_client_init(&ws_cfg);
//     esp_websocket_register_events(ws_client, WEBSOCKET_EVENT_ANY, ws_event_handler, (void *)ws_client);
//     ws_client_event = xEventGroupCreate();
// }

void ws_client_start(const char* room_id, const char* user_id)
{
    sprintf(ws_server_uri, WS_SERVER_URI_BASE"?room_id=%s&user_id=%s", room_id, user_id);
    // if (ESP_OK != (esp_websocket_client_set_uri(ws_client, ws_server_uri)))
    // {
    //     ESP_LOGE(TAG, "Invalid URI");
    // }
    ws_cfg.uri = ws_server_uri;
    ESP_LOGI(TAG, "Connecting to %s...", ws_cfg.uri);
    // ESP_LOGI(TAG, "room_id=%s, len=%d", room_id, strlen(room_id));
    // ESP_LOGI(TAG, "user_id=%s, len=%d", user_id, strlen(user_id));
    if (ws_client == NULL) {
        ws_client = esp_websocket_client_init(&ws_cfg);
        esp_websocket_register_events(ws_client, WEBSOCKET_EVENT_ANY, ws_event_handler, (void *)ws_client);
    }
    if (ws_client_event == NULL) {
        ws_client_event = xEventGroupCreate();
    }
    esp_websocket_client_start(ws_client);
    // xTimerStart(shutdown_signal_timer, portMAX_DELAY);
}

int ws_client_is_connected()
{
    return esp_websocket_client_is_connected(ws_client);
}

void ws_client_stop()
{
    if (ws_client != NULL) {
        esp_websocket_client_stop(ws_client);
        esp_websocket_client_destroy(ws_client);
        ws_client = NULL;
    }
    if (ws_client_event != NULL) {
        vEventGroupDelete(ws_client_event);
        ws_client_event = NULL;
    }
}

int ws_client_send_report(int cur_temp, int tar_temp, int mode, int speed)
{
    ESP_LOGI(TAG, "Report: cur_temp=%d, tar_temp=%d, mode=%d, speed=%d", cur_temp, tar_temp, mode, speed);
    ws_client_json_report(ws_client_tbuf, cur_temp, tar_temp, mode, speed);
    return esp_websocket_client_send_text(ws_client, ws_client_tbuf, strlen(ws_client_tbuf), 1);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ws_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        xEventGroupSetBits(ws_client_event, WS_CONNECT_EVENT_BIT);
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        xEventGroupSetBits(ws_client_event, WS_DISCONNECT_EVENT_BIT);
        break;
    case WEBSOCKET_EVENT_DATA:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
        ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
        // ESP_LOGW(TAG, "Received=%.*s", data->data_len, (char *)data->data_ptr);
        // ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);
        // xTimerReset(shutdown_signal_timer, portMAX_DELAY);
        if (data->op_code == 0x01) {
            ws_client_json_parse(data->data_ptr, &ws_client_rbuf);
            switch (ws_client_rbuf.event_id) {
            case JSON_D_MASTER_ID:
                air_init(air_slave, ws_client_rbuf.data.data_master.mode, ws_client_rbuf.data.data_master.temp);
                ESP_LOGW(TAG, "Master: mode=%d, temp=%d", ws_client_rbuf.data.data_master.mode, ws_client_rbuf.data.data_master.temp);
                xEventGroupSetBits(ws_client_event, WS_DATA_MASTER_EVENT_BIT);
                break;
            case JSON_D_SUPAIR_ID:
                air_update_curtemp(air_slave, ws_client_rbuf.data.data_supair.temp, ws_client_rbuf.data.data_supair.speed);
                air_set_cost(air_slave, ws_client_rbuf.data.data_supair.cost);
                //TODO: 处理主从控mode不同的情况
                air_set_mode(air_slave, ws_client_rbuf.data.data_supair.mode);
                air_update_speed(air_slave);
                ESP_LOGW(TAG, "SupplyAir: mode=%d, temp=%d, speed=%d, cost=%.1lf", 
                    ws_client_rbuf.data.data_supair.mode, 
                    ws_client_rbuf.data.data_supair.temp, 
                    ws_client_rbuf.data.data_supair.speed, 
                    ws_client_rbuf.data.data_supair.cost);
                break;
            case JSON_D_SETITV_ID:
                air_set_interval(air_slave, ws_client_rbuf.data.data_setitv.interval);
                ESP_LOGW(TAG, "SetInterval: interval=%d", ws_client_rbuf.data.data_setitv.interval);
                break;
            }
        }
        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    }
}