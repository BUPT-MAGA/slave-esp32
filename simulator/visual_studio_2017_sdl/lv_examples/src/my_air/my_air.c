
/*********************
 *      INCLUDES
 *********************/
#include "my_air.h"

#ifndef SIMULATOR
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#endif

#include "../lv_examples.h"

//#include LV_THEME_DEFAULT_INCLUDE

/*********************
 *      DEFINES
 *********************/
#define DEFAULT_SCAN_LIST_SIZE 10

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void login_open(uint32_t delay);
static void wifi_open(uint32_t delay);
static void scan_list_btn_event_handler(lv_obj_t *btn, lv_event_t evt);
static void scan_list_add_item(lv_obj_t *list, wifi_ap_record_t *ap_info, uint16_t ap_count);
static void login_wifi_btn_event_handler(lv_obj_t *btn, lv_event_t evt);
static void my_anim_out_all(lv_obj_t *obj, uint32_t delay);


/**********************
 *  STATIC VARIABLES
 **********************/
#ifndef SIMULATOR
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_SCAN_DONE_BIT  BIT0
#define WIFI_CONNECT_BIT    BIT1
#define WIFI_FAIL_BIT       BIT2
#endif
lv_obj_t *scan_list;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void my_air(void)
{
    lv_obj_t *scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    login_open(0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void login_open(uint32_t delay)
{
    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btn, login_wifi_btn_event_handler);
}

static void wifi_open(uint32_t delay) 
{
    /* 开始扫描 */
    #ifndef SIMULATOR
    static wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = DEFAULT_SCAN_LIST_SIZE;
    esp_wifi_scan_start(NULL, true);
    /* 开始播放loader动画, 等待扫描结束 */
    xEventGroupWaitBits(s_wifi_event_group, WIFI_SCAN_DONE_BIT, 1, 0, portMAX_DELAY); //FIXME: 这里不应该阻塞

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_info));
    // ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    #else
    static wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE] = {
        {"BUPT-portal"},
        {"BUPT-mobile"},
        {"BUPT-iot"},
        {"kys iphone"},
        {"kys iphone"},
        {"BUPT-iot"},
        {"BUPT-iot"},
        {"BUPT-iot"},
        {"kys iphone"},
        {"kys iphone"},
    };
    uint16_t ap_count = DEFAULT_SCAN_LIST_SIZE;
    #endif
    /* 生成列表 */
    scan_list = lv_list_create(lv_scr_act(), NULL);
    for (int i = 0; i < ap_count; i++) {
        scan_list_add_item(scan_list, &ap_info[i], ap_count);
    }
    // #ifndef SIMULATOR

    // #endif
}

static void scan_list_add_item(lv_obj_t *list, wifi_ap_record_t *ap_info, uint16_t ap_count)
{
    lv_obj_t* btn = lv_list_add_btn(list, NULL, ap_info->ssid);
    lv_obj_set_event_cb(btn, scan_list_btn_event_handler);
}

static void login_wifi_btn_event_handler(lv_obj_t *btn, lv_event_t evt) 
{
    if (evt == LV_EVENT_CLICKED) {
        my_anim_out_all(lv_scr_act(), 0);
        wifi_open(0);
    }
}

static void scan_list_btn_event_handler(lv_obj_t *btn, lv_event_t evt)
{
    if (evt == LV_EVENT_CLICKED) {
        printf("The index of Clicked-Button is %d\n", lv_list_get_btn_index(scan_list, btn));
        // TODO: 按下按键之后的处理
    }
}

static void my_anim_out_all(lv_obj_t *obj, uint32_t delay)
{
    lv_obj_t *child = lv_obj_get_child_back(obj, NULL);
    while (child) {
        lv_obj_del(child);
        child = lv_obj_get_child_back(obj, child);
    }
}