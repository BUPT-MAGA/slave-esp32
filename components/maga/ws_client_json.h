#ifndef WS_CLIENT_JSON_H
#define WS_CLIENT_JSON_H

// #define USE_DEPRECATED

typedef enum {
    JSON_D_MASTER_ID = 1,
#ifdef USE_DEPRECATED
    JSON_U_REQAIR_ID = 2,   // @Deprecated
#endif
    JSON_D_SUPAIR_ID = 3,
#ifdef USE_DEPRECATED
    JSON_U_REQSTP_ID = 4,   // @Deprecated
    JSON_D_STPAIR_ID = 5,   // @Deprecated
#endif
    JSON_D_SETITV_ID = 6,
    JSON_U_REPORT_ID = 7,
} json_event_id;

typedef struct {
    int event_id;
    union {
        struct { int mode, temp; }                    data_master;
        struct { int temp, speed, mode; double cost; } data_supair;
        struct { int interval; }                      data_setitv;
    } data;
} ws_msg;

#ifdef USE_DEPRECATED
char *ws_client_json_request_air(char *buf, int speed, int mode);
char *ws_client_json_request_stop(char *buf); 
#endif
char *ws_client_json_report(char *buf, int cur_temp, int tar_temp, int mode, int speed);
int ws_client_json_parse(const char *buf, ws_msg *msg);

#endif