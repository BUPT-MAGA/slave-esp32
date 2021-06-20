#include "ws_client_json.h"
#include "string.h"
#include "cJSON.h"
#include <stdio.h>

#ifdef USE_DEPRECATED
/* @Deprecated */
char *ws_client_json_request_air(char *buf, int speed, int mode)
{
    cJSON *event_id = cJSON_CreateNumber(JSON_U_REQAIR_ID);

    cJSON *data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "speed", speed);
    cJSON_AddNumberToObject(data, "mode", mode);

    cJSON *json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "event_id", event_id);
    cJSON_AddItemToObject(json, "data", data);

    char *tbuf = cJSON_Print(json);
    strcpy(buf, tbuf);
    cJSON_free(tbuf);

    cJSON_Delete(json);

    return buf;
}

/* @Deprecated */
char *ws_client_json_request_stop(char *buf) 
{
    cJSON *event_id = cJSON_CreateNumber(JSON_U_REQSTP_ID);

    cJSON *data = cJSON_CreateObject();

    cJSON *json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "event_id", event_id);
    cJSON_AddItemToObject(json, "data", data);

    char *tbuf = cJSON_Print(json);
    strcpy(buf, tbuf);
    cJSON_free(tbuf);

    cJSON_Delete(json);

    return buf;
}
#endif

char *ws_client_json_report(char *buf, int cur_temp, int tar_temp, int mode, int speed)
{
    cJSON *event_id = cJSON_CreateNumber(JSON_U_REPORT_ID);

    cJSON *data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "cur_temp", cur_temp);
    cJSON_AddNumberToObject(data, "tar_temp", tar_temp);
    cJSON_AddNumberToObject(data, "mode", mode);
    cJSON_AddNumberToObject(data, "speed", speed);

    cJSON *json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "event_id", event_id);
    cJSON_AddItemToObject(json, "data", data);

    char *tbuf = cJSON_Print(json);
    strcpy(buf, tbuf);
    cJSON_free(tbuf);

    cJSON_Delete(json);

    return buf;
}

int ws_client_json_parse(const char *buf, ws_msg *msg)
{
    cJSON *json;
    cJSON *event_id, *data;
    int ret = -1;

    if (NULL == (json = cJSON_Parse(buf)))
        return ret;

    if (NULL == (event_id = cJSON_GetObjectItem(json, "event_id")) || !cJSON_IsNumber(event_id)) { 
        goto cleanup;
    }
    if (NULL == (data = cJSON_GetObjectItem(json, "data")) || !cJSON_IsObject(data)) {
        goto cleanup;
    }

    msg->event_id = event_id->valueint;
    
    if (event_id->valueint == JSON_D_MASTER_ID)
    {
        cJSON *mode, *temp;
        if (NULL == (mode = cJSON_GetObjectItem(data, "mode")) || !cJSON_IsNumber(mode)) {
            goto cleanup;
        }
        if (NULL == (temp = cJSON_GetObjectItem(data, "temp")) || !cJSON_IsNumber(temp)) {
            goto cleanup;
        }
        msg->data.data_master.mode = mode->valueint;
        msg->data.data_master.temp = mode->valueint;
    }
    else if (event_id->valueint == JSON_D_SUPAIR_ID)
    {
        cJSON *temp, *speed, *mode, *cost;
        if (NULL == (temp = cJSON_GetObjectItem(data, "temp")) || !cJSON_IsNumber(temp)) {
            goto cleanup;
        }
        if (NULL == (speed = cJSON_GetObjectItem(data, "speed")) || !cJSON_IsNumber(speed)) {
            goto cleanup;
        }
        if (NULL == (mode = cJSON_GetObjectItem(data, "mode")) || !cJSON_IsNumber(mode)) {
            goto cleanup;
        }
        if (NULL == (cost = cJSON_GetObjectItem(data, "cost")) || !cJSON_IsNumber(cost)) {
            goto cleanup;
        }
        msg->data.data_supair.temp = temp->valueint;
        msg->data.data_supair.speed = speed->valueint;
        msg->data.data_supair.mode = mode->valueint;
        msg->data.data_supair.cost = cost->valuedouble;
    }
#ifdef USE_DEPRECATED
    else if (event_id->valueint == JSON_D_STPAIR_ID)
    {
        cJSON *cost;
        if (NULL == (cost = cJSON_GetObjectItem(data, "cost")) || !cJSON_IsNumber(cost)) {
            goto cleanup;
        }
        /* 处理 */
    }
#endif
    else if (event_id->valueint == JSON_D_SETITV_ID)
    {
        cJSON *interval;
        if (NULL == (interval = cJSON_GetObjectItem(data, "interval")) || !cJSON_IsNumber(interval)) {
            goto cleanup;
        }
        msg->data.data_setitv.interval = interval->valueint;
    }
    else {
        ret = -2;
        goto cleanup;
    }
    ret = 0;

cleanup:
    cJSON_Delete(json);
    return ret;
}