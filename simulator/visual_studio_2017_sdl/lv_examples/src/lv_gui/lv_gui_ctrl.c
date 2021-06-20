/**
 * @file lv_gui_ctrl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gui.h"
#include "lv_gui_login.h"
#include "lv_gui_ctrl.h"

#ifndef SIMULATOR
#include "ws_client.h"
#include "esp_timer.h"
#endif
#include LV_THEME_DEFAULT_INCLUDE

#include <stdio.h>
LV_FONT_DECLARE(symbol_36);
LV_FONT_DECLARE(dinpro_144);
/*********************
 *      DEFINES
 *********************/
#define LV_SYMBOL_FANHIGH   "\xEE\xA4\x80"
#define LV_SYMBOL_FANMID    "\xEE\xA4\x81"
#define LV_SYMBOL_FANLOW    "\xEE\xA4\x82"

#define LV_SYMBOL_SUN       "\xEF\x86\x85"
#define LV_SYMBOL_SNOW      "\xEF\x8B\x9C"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void inc_btn_event_cb(lv_obj_t* btn, lv_event_t e);
static void dec_btn_event_cb(lv_obj_t* btn, lv_event_t e);
static void power_btn_event_cb(lv_obj_t* btn, lv_event_t e);
static void back_btn_event_cb(lv_obj_t* btn, lv_event_t e);
#ifndef SIMULATOR
static void tartemp_timer_cb(void* arg);
static void update_timer_cb(void *arg);
static void disconn_task_cb(lv_task_t *t);
#else
static void tartemp_task_cb(lv_task_t* t);
static void update_task_cb(lv_task_t* t);
#endif

static void air_slave_disp_tartemp(air_slave_t* slave);
static void air_slave_disp_curtemp(air_slave_t* slave);
static void air_slave_disp_speed(air_slave_t* slave);
static void air_slave_disp_mode(air_slave_t* slave);
static void air_slave_disp_cost(air_slave_t* slave);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *ctrl_page;

static lv_obj_t* img_conn;
static lv_obj_t* led_speed[3];
static lv_obj_t* label_mode;
static lv_obj_t* label_temp;
static lv_obj_t* label_temp_sec;
static lv_obj_t* label_room;
static lv_obj_t* label_set;
static lv_obj_t* label_cost;

static int value_tar_temp = 22;

static int value_temp;
static int value_temp_sec;
static int value_mode = -1;
static int value_speed;
static double value_cost;


#ifndef SIMULATOR
const static esp_timer_create_args_t temp_timer_args = {
        .callback = &tartemp_timer_cb,
        .name = "temp_timer"
};
static esp_timer_handle_t temp_timer = NULL;
const static esp_timer_create_args_t update_timer_args = {
        .callback = &update_timer_cb,
        .name = "update_timer"
};
static esp_timer_handle_t update_timer = NULL;
static lv_task_t* disconn_task = NULL;
#else
static lv_task_t* temp_task = NULL;
static lv_task_t* update_task = NULL;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ctrl_open(uint32_t delay)
{
    value_tar_temp = air_get_tartemp(air_slave);

    ctrl_page = lv_win_create(lv_scr_act(), NULL);
    lv_obj_set_size(ctrl_page, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_focus_parent(ctrl_page, true);
    lv_win_set_layout(ctrl_page, LV_LAYOUT_OFF);
    lv_win_set_title(ctrl_page, "Control Panel");
    lv_obj_t* btn_back = lv_win_add_btn_left(ctrl_page, LV_SYMBOL_LEFT);
    lv_obj_set_event_cb(btn_back, back_btn_event_cb);
    lv_obj_t* btn_conn = lv_win_add_btn_right(ctrl_page, LV_SYMBOL_LOOP);
    img_conn = lv_obj_get_child(btn_conn, NULL);

    /* status area */

    //lv_obj_t* stat_cont = lv_cont_create(ctrl_page, NULL);
    //lv_cont_set_layout(stat_cont, LV_LAYOUT_COLUMN_MID);
    //lv_cont_set_fit2(stat_cont, LV_FIT_TIGHT, LV_FIT_TIGHT);
    ////lv_obj_set_size(stat_cont, 120, 240);
    ////lv_obj_set_style_local_pad_all(stat_cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);
    //lv_obj_set_style_local_pad_inner(stat_cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 2);
    //lv_obj_set_style_local_border_width(stat_cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0); // TODO: 修改container默认样式为无边框

    const char* text_speed[3] = { LV_SYMBOL_FANLOW, LV_SYMBOL_FANMID, LV_SYMBOL_FANHIGH };
    lv_obj_t* cont_speed = lv_cont_create(ctrl_page, NULL);//lv_obj_t* cont_speed = lv_cont_create(stat_cont, NULL);
    lv_cont_set_fit2(cont_speed, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(cont_speed, 100);
    lv_obj_set_style_local_pad_inner(cont_speed, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_pad_bottom(cont_speed, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);
    lv_obj_set_style_local_border_width(cont_speed, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0); // TODO: 修改container默认样式为无边框
    lv_obj_set_style_local_bg_opa(cont_speed, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
    lv_cont_set_layout(cont_speed, LV_LAYOUT_GRID);

    for (int i = 2; i >= 0; i--) {
        led_speed[i] = lv_led_create(cont_speed, NULL);
        lv_obj_set_style_local_radius(led_speed[i], LV_LED_PART_MAIN, LV_STATE_DEFAULT, 5);
        lv_obj_set_size(led_speed[i], 30, 30);
        lv_led_set_bright(led_speed[i], LV_LED_BRIGHT_MIN);
        lv_obj_t* label_speed = lv_label_create(cont_speed, NULL);
        lv_obj_set_style_local_text_font(label_speed, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &symbol_36);
        lv_label_set_text_static(label_speed, text_speed[i]);
        //lv_obj_align(label_speed, led_speed[i], LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    }
    value_speed = -1;
    air_slave_disp_speed(air_slave);
    lv_obj_align(cont_speed, NULL, LV_ALIGN_IN_LEFT_MID, 15, -27); // test

    label_mode = lv_label_create(ctrl_page, NULL);//label_mode = lv_label_create(stat_cont, NULL);
    lv_obj_set_style_local_text_font(label_mode, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &symbol_36);
    air_slave_disp_mode(air_slave);
    lv_label_set_align(label_mode, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(label_mode, cont_speed, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    //lv_obj_align(stat_cont, ctrl_page, LV_ALIGN_IN_LEFT_MID, 8, 0);

    /* button area */

    lv_obj_t* btn_cont = lv_cont_create(ctrl_page, NULL);
    //lv_obj_set_focus_parent(btn_cont, true);
    lv_cont_set_layout(btn_cont, LV_LAYOUT_COLUMN_MID);
    lv_cont_set_fit2(btn_cont, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_obj_set_style_local_border_width(btn_cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0); // TODO: 修改container默认样式为无边框
    lv_obj_set_style_local_bg_opa(btn_cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);

    lv_obj_t* power_btn = lv_btn_create(btn_cont, NULL);
    lv_obj_set_event_cb(power_btn, back_btn_event_cb);
    lv_obj_set_size(power_btn, 50, 50);
    lv_obj_t* power_label = lv_label_create(power_btn, NULL);
    lv_label_set_text_static(power_label, LV_SYMBOL_POWER);

    //lv_obj_t* sepa_line = lv_line_create(btn_cont, NULL);
    //const lv_point_t sepa_points[] = { {.x = 0, .y = 0}, {.x = 50, .y = 0} };
    ////lv_line_set_auto_size(sepa_line, true);
    //lv_line_set_points(sepa_line, sepa_points, 2);
    /*lv_obj_set_style_local_border_width(sepa_line, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 5);*/
    /*lv_obj_set_style_local_border_color(sepa_line, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);*/

    lv_obj_t* inc_btn = lv_btn_create(btn_cont, power_btn);
    lv_obj_set_event_cb(inc_btn, inc_btn_event_cb);
    lv_obj_t* inc_label = lv_label_create(inc_btn, NULL);
    lv_label_set_text_static(inc_label, LV_SYMBOL_PLUS);

    lv_obj_t* dec_btn = lv_btn_create(btn_cont, inc_btn);
    lv_obj_set_event_cb(dec_btn, dec_btn_event_cb);
    lv_obj_t* dec_label = lv_label_create(dec_btn, inc_label);
    lv_label_set_text_static(dec_label, LV_SYMBOL_MINUS);

    lv_btn_toggle(power_btn);

    lv_obj_align(btn_cont, NULL, LV_ALIGN_IN_RIGHT_MID, -15, 0);

    /* temperature display area */

    lv_obj_t* label_centi = lv_label_create(ctrl_page, NULL);
    lv_label_set_text_static(label_centi, "C");
    lv_obj_set_style_local_text_font(label_centi, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_48);
    lv_obj_align(label_centi, inc_btn, LV_ALIGN_OUT_LEFT_MID, -25, 0);
    label_temp = lv_label_create(ctrl_page, NULL);
    lv_obj_set_style_local_text_font(label_temp, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &dinpro_144);
    lv_label_set_text(label_temp, "00");
    lv_obj_align(label_temp, label_centi, LV_ALIGN_OUT_LEFT_BOTTOM, 0, 0);
    value_temp = 0;
    label_temp_sec = lv_label_create(ctrl_page, NULL);
    lv_obj_set_style_local_text_font(label_temp_sec, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_36);
    lv_label_set_text(label_temp_sec, "00");
    lv_obj_align(label_temp_sec, label_temp, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    lv_obj_set_style_local_text_color(label_temp_sec, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_lighten(LV_COLOR_BLACK, LV_OPA_80));
    value_temp_sec = 0;
    label_room = lv_label_create(ctrl_page, NULL);
    lv_obj_set_style_local_text_font(label_room, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_label_set_text_static(label_room, "Room");
    lv_obj_align(label_room, label_temp, LV_ALIGN_OUT_LEFT_BOTTOM, 0, 0);
    label_set = lv_label_create(ctrl_page, label_room);
    lv_label_set_text_static(label_set, "Set");
    lv_obj_align(label_set, label_temp, LV_ALIGN_OUT_LEFT_TOP, 0, 0);
    air_slave_disp_curtemp(air_slave);

    label_cost = lv_label_create(ctrl_page, NULL);
    lv_obj_set_style_local_text_font(label_cost, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
    lv_label_set_text(label_cost, "cost: 0.0");
    lv_obj_align(label_cost, label_temp, LV_ALIGN_OUT_BOTTOM_MID, 0, 25);  
    value_cost = 0.0;  
    air_slave_disp_cost(air_slave);

#ifndef SIMULATOR
    if (update_timer == NULL) {
        esp_timer_create(&update_timer_args, &update_timer);
    }
    esp_timer_start_periodic(update_timer, 1000 * 500);
    if (disconn_task == NULL) {
        disconn_task = lv_task_create(disconn_task_cb, 100, LV_TASK_PRIO_HIGHEST, NULL);
    }
#else
    update_task = lv_task_create(update_task_cb, 500, LV_TASK_PRIO_MID, NULL);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void inc_btn_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
#ifndef SIMULATOR
        if (temp_timer == NULL) {
            esp_timer_create(&temp_timer_args, &temp_timer);
        }
        else {
            if (value_tar_temp < (air_get_mode(air_slave) == AIR_MODE_HEAT ? air_get_airtemp(air_slave) : 30)) {
                value_tar_temp += 1;
            }
            esp_timer_stop(temp_timer);
        }
        esp_timer_start_once(temp_timer, 1000 * 1000);
        air_slave_disp_tartemp(air_slave);
#else
        if (temp_task == NULL) {
            temp_task = lv_task_create(tartemp_task_cb, 1000, LV_TASK_PRIO_MID, NULL);
            lv_task_once(temp_task);
        }
        else {
            if (value_tar_temp < 30) {
                value_tar_temp += 1;
            }
            lv_task_reset(temp_task);
        }
        air_slave_disp_tartemp(air_slave);
#endif
    }
}

static void dec_btn_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
#ifndef SIMULATOR
        if (temp_timer == NULL) {
            esp_timer_create(&temp_timer_args, &temp_timer);
        }
        else {
            if (value_tar_temp > (air_get_mode(air_slave) == AIR_MODE_COOL ? air_get_airtemp(air_slave) : 18)) {
                value_tar_temp -= 1;
            }
            esp_timer_stop(temp_timer);
        }
        esp_timer_start_once(temp_timer, 1000 * 1000);
        air_slave_disp_tartemp(air_slave);
#else
        if (temp_task == NULL) {
            temp_task = lv_task_create(tartemp_task_cb, 1000, LV_TASK_PRIO_MID, NULL);
            lv_task_once(temp_task);
        }
        else {
            if (value_tar_temp > 18) {
                value_tar_temp -= 1;
            }
            lv_task_reset(temp_task);
        }
        air_slave_disp_tartemp(air_slave);
#endif
    }
}

static void power_btn_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        printf("now disp (%d,%d)\n", LV_HOR_RES, LV_VER_RES);
    }
}

static void back_btn_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
#ifndef SIMULATOR
        if (temp_timer) {
            esp_timer_stop(temp_timer);
            esp_timer_delete(temp_timer);
            temp_timer = NULL;
        }
        if (update_timer) {
            esp_timer_stop(update_timer);
            esp_timer_delete(update_timer);
            update_timer = NULL;
        }
        if (disconn_task) {
            lv_task_del(disconn_task);
            disconn_task = NULL;
        }
        air_destroy(air_slave);
        ws_client_stop();
#else
        if (temp_task)
            lv_task_del(temp_task);
        temp_task = NULL;
        if (update_task)
            lv_task_del(update_task);
        update_task = NULL;
#endif
        lv_gui_anim_out_all(lv_scr_act(), 0);
        login_open(0);
    }
}

#ifndef SIMULATOR
static void tartemp_timer_cb(void* arg)
{
   air_slave_disp_curtemp(air_slave);
   air_set_tartemp(air_slave, value_tar_temp);
   air_update_speed(air_slave);
   temp_timer = NULL;
}

static void update_timer_cb(void *arg)
{
    air_update_curtemp_env(air_slave);
    air_update_speed(air_slave);
    int value_cur_temp = air_get_curtemp(air_slave);
    if (temp_timer == NULL) {
        if (value_temp != value_cur_temp) {
            value_temp = value_cur_temp;
            lv_label_set_text_fmt(label_temp, "%2d", value_temp);
        }
    }
    else {
        if (value_temp_sec != value_cur_temp) {
            value_temp_sec = value_cur_temp;
            lv_label_set_text_fmt(label_temp_sec, "%2d", value_temp_sec);
        }
    }
    air_slave_disp_mode(air_slave);
    air_slave_disp_speed(air_slave);
    air_slave_disp_cost(air_slave);
}

static void disconn_task_cb(lv_task_t *t) 
{
    EventBits_t bits;
    bits = xEventGroupClearBits(ws_client_event, WS_DISCONNECT_EVENT_BIT|WS_CONNECT_EVENT_BIT);
    if ((bits & WS_DISCONNECT_EVENT_BIT) == WS_DISCONNECT_EVENT_BIT) {
        lv_img_set_src(img_conn, LV_SYMBOL_WARNING);
        // air_destroy(air_slave);
        // lv_task_del(t);
    }
    if ((bits & WS_CONNECT_EVENT_BIT) == WS_CONNECT_EVENT_BIT) {
        lv_img_set_src(img_conn, LV_SYMBOL_LOOP);
    }
}
#else
static void tartemp_task_cb(lv_task_t* t)
{
    air_slave_disp_curtemp(air_slave);
    air_set_tartemp(air_slave, value_tar_temp);
    air_update_speed(air_slave);
    temp_task = NULL;
}

static void update_task_cb(lv_task_t* t)
{
    air_update_curtemp_env(air_slave);
    air_update_speed(air_slave);
    int value_cur_temp = air_get_curtemp(air_slave);
    if (temp_task == NULL) {
        if (value_temp != value_cur_temp) {
            value_temp = value_cur_temp;
            lv_label_set_text_fmt(label_temp, "%2d", value_temp);
        }
    }
    else {
        if (value_temp_sec != value_cur_temp) {
            value_temp_sec = value_cur_temp;
            lv_label_set_text_fmt(label_temp_sec, "%2d", value_temp_sec);
        }
    }
    air_slave_disp_speed(air_slave);
    air_slave_disp_cost(air_slave);
}
#endif // !SIMULATOR

static void air_slave_disp_tartemp(air_slave_t* slave)
{
     int value_cur_temp = air_get_curtemp(air_slave);
    //int value_cur_temp = value_temp;
    if (value_temp != value_tar_temp) {
        value_temp = value_tar_temp;
        lv_label_set_text_fmt(label_temp, "%2d", value_temp);
    }
    if (value_temp_sec != value_cur_temp) {
        value_temp_sec = value_cur_temp;
        lv_label_set_text_fmt(label_temp_sec, "%2d", value_temp_sec);
    }
    lv_obj_set_style_local_text_color(label_set, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(label_room, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_lighten(LV_COLOR_BLACK, LV_OPA_80));
}


static void air_slave_disp_curtemp(air_slave_t* slave)
{
     int value_cur_temp = air_get_curtemp(air_slave);
    //int value_cur_temp = value_temp_sec;
    if (value_temp != value_cur_temp) {
        value_temp = value_cur_temp;
        lv_label_set_text_fmt(label_temp, "%2d", value_temp);
    }
    if (value_temp_sec != value_tar_temp) {
        value_temp_sec = value_tar_temp;
        lv_label_set_text_fmt(label_temp_sec, "%2d", value_temp_sec);
    }
    lv_obj_set_style_local_text_color(label_room, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(label_set, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_lighten(LV_COLOR_BLACK, LV_OPA_80));
}


static void air_slave_disp_speed(air_slave_t* slave)
{
    int speed = air_get_speed(slave);
    if (value_speed != speed) {
        for (int i = 0; i < 3; i++) {
            if (speed >= i && lv_led_get_bright(led_speed[i]) != LV_LED_BRIGHT_MAX) {
                lv_led_set_bright(led_speed[i], LV_LED_BRIGHT_MAX);
            }
            else if (speed < i && lv_led_get_bright(led_speed[i]) != LV_LED_BRIGHT_MIN) {
                lv_led_set_bright(led_speed[i], LV_LED_BRIGHT_MIN);
            }
        }
        value_speed = speed;
    }
}

static void air_slave_disp_mode(air_slave_t* slave)
{
    int mode = air_get_mode(air_slave);
    if (mode != value_mode) {
        if (mode == AIR_MODE_HEAT) {
            lv_label_set_text(label_mode, LV_SYMBOL_SUN);
            lv_obj_set_style_local_text_color(label_mode, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
        }
        else {
            lv_label_set_text(label_mode, LV_SYMBOL_SNOW);
            lv_obj_set_style_local_text_color(label_mode, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);
        }
        value_mode = mode;
    }
}

static void air_slave_disp_cost(air_slave_t* slave)
{
    double cost = air_get_cost(slave);
    if (cost - value_cost >= 0.1) {
        lv_label_set_text_fmt(label_cost, "cost: %.1lf", air_get_cost(air_slave));
        value_cost = cost;
    }
}
