#ifndef WS_CLIENT_H
#define WS_CLIENT_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#define WS_SERVER_URI_BASE CONFIG_MAGA_WS_SERVER_URI_BASE
// #define WS_SERVER_URI_BASE   "ws://123.60.215.79:8000/ws/"
// #define WS_SERVER_URI_BASE   "ws://10.128.238.197:8080/ws/"
// #define WS_SERVER_URI_BASE  "ws://139.9.141.88:6789"

void ws_client_start(const char* room_id, const char* user_id);
int ws_client_is_connected();
void ws_client_stop();

int ws_client_send_report(int cur_temp, int tar_temp, int mode, int speed);

extern EventGroupHandle_t ws_client_event;
#define WS_CONNECT_EVENT_BIT        BIT0
#define WS_DISCONNECT_EVENT_BIT     BIT1
#define WS_DATA_MASTER_EVENT_BIT    BIT2

#endif