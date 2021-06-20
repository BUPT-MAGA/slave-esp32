/**
 * @file lv_demo_widgets.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_examples.h"
#include "my_demo.h"

#include LV_THEME_DEFAULT_INCLUDE

#include <stdio.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void login_page_open(uint32_t delay);
static void ctrl_page_open(uint32_t delay);

// static void btn_event_handler(lv_obj_t* btn, lv_event_t evt);
static void login_btn_event_cb(lv_obj_t *btn, lv_event_t e);
static void reset_btn_event_cb(lv_obj_t* btn, lv_event_t e);
static void ta_event_cb(lv_obj_t *ta, lv_event_t e);
static void kb_event_cb(lv_obj_t *_kb, lv_event_t e);

static void inc_btn_event_cb(lv_obj_t* btn, lv_event_t e);
static void dec_btn_event_cb(lv_obj_t* btn, lv_event_t e);
static void ctrl_page_event_cb(lv_obj_t* page, lv_event_t e);

static void lv_gui_add_loader(lv_task_cb_t task_cb);
static void loader_task_cb_example(lv_task_t* t);   //TODO: 范例函数, 记得删除

static void lv_gui_anim_out_all(lv_obj_t* obj, uint32_t delay);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *login_page;
static lv_obj_t *kb;
static lv_obj_t *ta_roomid;
static lv_obj_t *ta_userid;

static lv_obj_t* loader_page;

static lv_obj_t* ctrl_page;
static lv_obj_t* nowtemp_label;
static lv_obj_t* tartemp_label;
static int nowtemp_value = 25;
static int tartemp_value = 22;
static lv_obj_t* speed_roller;
static lv_obj_t* mode_roller;
static int speed_value = 0;
static int mode_value = 0;

static const char* kb_map[] = {   LV_SYMBOL_OK, "\n",
                                        "1", "2", "3", "\n",
                                        "4", "5", "6", "\n",
                                        "7", "8", "9", "\n",
                                        "X", "0", LV_SYMBOL_BACKSPACE, ""
                                    };
static const lv_btnmatrix_ctrl_t kb_ctrl_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 3,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void my_demo(void)
{
    //login_page_open(0);
    ctrl_page_open(0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void login_page_open(uint32_t delay)
{
    //login_page = lv_page_create(lv_scr_act(), NULL);
    login_page = lv_win_create(lv_scr_act(), NULL);
    lv_obj_set_size(login_page, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_focus_parent(login_page, true);
    //lv_page_set_scrl_layout(login_page, LV_LAYOUT_CENTER);
    lv_win_set_layout(login_page, LV_LAYOUT_CENTER);

    ta_roomid = lv_textarea_create(login_page, NULL);
    lv_textarea_set_text(ta_roomid, "");
    lv_textarea_set_placeholder_text(ta_roomid, "Room ID");
    lv_textarea_set_one_line(ta_roomid, true);
    lv_textarea_set_cursor_hidden(ta_roomid, true);
    lv_obj_set_event_cb(ta_roomid, ta_event_cb);

    ta_userid = lv_textarea_create(login_page, ta_roomid);
    lv_textarea_set_placeholder_text(ta_userid, "User ID");

    lv_obj_t* hor = lv_cont_create(login_page, NULL);
    lv_cont_set_layout(hor, LV_LAYOUT_ROW_MID);
    lv_cont_set_fit2(hor, LV_FIT_TIGHT, LV_FIT_TIGHT);

    lv_obj_t* btn_reset = lv_btn_create(hor, NULL);
    lv_obj_set_event_cb(btn_reset, reset_btn_event_cb);
    lv_obj_t* label_reset = lv_label_create(btn_reset, NULL);
    lv_label_set_text(label_reset, "Reset");

    lv_obj_t* btn_login = lv_btn_create(hor, NULL);
    lv_obj_set_event_cb(btn_login, login_btn_event_cb);
    lv_obj_t* label_login = lv_label_create(btn_login, NULL);
    lv_label_set_text(label_login, "Login");
}

//static void ctrl_page_open(uint32_t delay)
//{
//    ctrl_page = lv_page_create(lv_scr_act(), NULL);
//    lv_obj_set_size(ctrl_page, LV_HOR_RES, LV_VER_RES);
//    lv_obj_set_focus_parent(ctrl_page, true);
//    lv_page_set_scrl_layout(ctrl_page, LV_LAYOUT_OFF);
//    lv_obj_set_event_cb(lv_scr_act(), ctrl_page_event_cb);
//
//    lv_obj_t* label = lv_label_create(ctrl_page, nowtemp_label);
//    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_48);
//    lv_label_set_text(label, "|");
//    lv_obj_align(label, NULL, LV_ALIGN_CENTER, -30, 0);
//
//    nowtemp_label = lv_label_create(ctrl_page, label);
//    lv_label_set_text_fmt(nowtemp_label, "%2d"LV_SYMBOL_SETTINGS, nowtemp_value);
//    lv_obj_align(nowtemp_label, label, LV_ALIGN_OUT_LEFT_MID, -10, 0);
//
//    tartemp_label = lv_label_create(ctrl_page, label);
//    lv_obj_set_style_local_text_color(tartemp_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
//    lv_label_set_align(tartemp_label, LV_LABEL_ALIGN_RIGHT);
//    lv_label_set_text_fmt(tartemp_label, "%2d"LV_SYMBOL_SETTINGS, tartemp_value);
//    lv_obj_align(tartemp_label, label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
//
//
//    lv_obj_t* btn_cont = lv_cont_create(ctrl_page, NULL);
//    lv_obj_set_focus_parent(btn_cont, true);
//    lv_cont_set_layout(btn_cont, LV_LAYOUT_COLUMN_MID);
//    lv_cont_set_fit2(btn_cont, LV_FIT_TIGHT, LV_FIT_TIGHT);
//    lv_obj_set_style_local_border_width(btn_cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0); // TODO: 修改container默认样式为无边框
//    /*lv_obj_set_width(btn_cont, LV_HOR_RES / 2);
//    lv_cont_set_fit2(btn_cont, LV_FIT_NONE, LV_FIT_TIGHT);*/
//
//    lv_obj_t* inc_btn = lv_btn_create(btn_cont, NULL);
//    lv_obj_set_style_local_radius(inc_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
//    lv_obj_set_size(inc_btn, 30, 30);
//    lv_obj_set_event_cb(inc_btn, inc_btn_event_cb);
//    lv_obj_t* inc_label = lv_label_create(inc_btn, NULL);
//    lv_label_set_text(inc_label, LV_SYMBOL_UP);
//
//    lv_obj_t* dec_btn = lv_btn_create(btn_cont, inc_btn);
//    lv_obj_set_event_cb(dec_btn, dec_btn_event_cb);
//    lv_obj_t* dec_label = lv_label_create(dec_btn, inc_label);
//    lv_label_set_text(dec_label, LV_SYMBOL_DOWN);
//
//    lv_obj_align(btn_cont, tartemp_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
//
//    speed_roller = lv_roller_create(ctrl_page, NULL);
//    lv_obj_align(speed_roller, NULL, LV_ALIGN_IN_TOP_RIGHT, 50, 0);
//    lv_obj_set_click(lv_page_get_scrl(speed_roller), false); //TODO: 禁止拖动, 不知道会不会有问题
//    lv_roller_set_visible_row_count(speed_roller, 1);
//    lv_roller_set_options(speed_roller, LV_SYMBOL_AUDIO"\n"LV_SYMBOL_BACKSPACE"\n"LV_SYMBOL_BATTERY_1, LV_ROLLER_MODE_INFINITE);
//    lv_roller_set_selected(speed_roller, speed_value, LV_ANIM_OFF);
//
//    mode_roller = lv_roller_create(ctrl_page, speed_roller);
//    lv_obj_align(mode_roller, speed_roller, LV_ALIGN_OUT_LEFT_MID, -10, 0);
//    lv_roller_set_options(mode_roller, LV_SYMBOL_BLUETOOTH"\n"LV_SYMBOL_WIFI, LV_ROLLER_MODE_INFINITE);
//    lv_roller_set_selected(mode_roller, mode_value, LV_ANIM_OFF);
//
//    /*lv_obj_t* shut_btn = lv_btn_create(ctrl_page, NULL);
//    lv_obj_set_size(shut_btn, 70, 70);
//    lv_obj_set_style_local_radius(shut_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
//    lv_obj_align(shut_btn, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
//    lv_obj_t* shut_label = lv_label_create(shut_btn, NULL);
//    lv_label_set_text(shut_label, LV_SYMBOL_POWER);*/
//}

static void ctrl_page_open()
{
    ctrl_page = lv_win_create(lv_scr_act(), NULL);
    lv_obj_set_size(ctrl_page, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_focus_parent(ctrl_page, true);
    //lv_page_set_scrl_layout(ctrl_page, LV_LAYOUT_OFF);
    lv_win_set_layout(ctrl_page, LV_LAYOUT_OFF);
    lv_win_add_btn_left(ctrl_page, LV_SYMBOL_LEFT);

    /*nowtemp_label = lv_label_create(ctrl_page, NULL);
    lv_obj_set_style_local_text_font(nowtemp_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_48);
    lv_label_set_text_fmt(nowtemp_label, "%2d"LV_SYMBOL_SETTINGS, nowtemp_value);
    lv_obj_align(nowtemp_label, NULL, LV_ALIGN_CENTER, 50, 0);*/
 
    lv_obj_t* btn_cont = lv_cont_create(ctrl_page, NULL);
    lv_obj_set_focus_parent(btn_cont, true);
    lv_cont_set_layout(btn_cont, LV_LAYOUT_COLUMN_MID);
    lv_cont_set_fit2(btn_cont, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_obj_set_style_local_border_width(btn_cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0); // TODO: 修改container默认样式为无边框
    /*lv_obj_set_width(btn_cont, LV_HOR_RES / 2);
    lv_cont_set_fit2(btn_cont, LV_FIT_NONE, LV_FIT_TIGHT);*/

    lv_obj_t* inc_btn = lv_btn_create(btn_cont, NULL);
    lv_obj_set_style_local_radius(inc_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_size(inc_btn, 30, 30);
    lv_obj_set_event_cb(inc_btn, inc_btn_event_cb);
    lv_obj_t* inc_label = lv_label_create(inc_btn, NULL);
    lv_label_set_text(inc_label, LV_SYMBOL_UP);

    lv_obj_t* dec_btn = lv_btn_create(btn_cont, inc_btn);
    lv_obj_set_event_cb(dec_btn, dec_btn_event_cb);
    lv_obj_t* dec_label = lv_label_create(dec_btn, inc_label);
    lv_label_set_text(dec_label, LV_SYMBOL_DOWN);
}

/* login page */
static void login_btn_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        if (kb) {
            lv_obj_set_height(login_page, LV_VER_RES);
            lv_obj_del(kb);
            kb = NULL;
        }
        printf("roomID = %s, userID = %s\n", lv_textarea_get_text(ta_roomid), lv_textarea_get_text(ta_userid));
        lv_gui_add_loader(loader_task_cb_example);
    }
}

static void reset_btn_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        lv_textarea_set_text(ta_roomid, "");
        lv_textarea_set_text(ta_userid, "");
    }
}

static void ta_event_cb(lv_obj_t * ta, lv_event_t e)
{
    if(e == LV_EVENT_RELEASED) {
        if(kb == NULL) {
             lv_obj_set_height(login_page, LV_VER_RES / 2);
            kb = lv_keyboard_create(lv_scr_act(), NULL);
            lv_keyboard_set_ctrl_map(kb, LV_KEYBOARD_MODE_NUM, kb_ctrl_map);
            lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_NUM, kb_map);
            lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUM);
            lv_obj_set_event_cb(kb, kb_event_cb);

            lv_indev_wait_release(lv_indev_get_act());
        }
        lv_textarea_set_cursor_hidden(ta, false);
        lv_page_focus(login_page, lv_textarea_get_label(ta), LV_ANIM_ON);
        lv_keyboard_set_textarea(kb, ta);
    } else if(e == LV_EVENT_DEFOCUSED) {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

static void kb_event_cb(lv_obj_t * _kb, lv_event_t e)
{
    lv_keyboard_def_event_cb(kb, e);

    if(e == LV_EVENT_CANCEL || e == LV_EVENT_APPLY) {
        if(kb) {
             lv_obj_set_height(login_page, LV_VER_RES);
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}

/* loader page */

static void lv_gui_add_loader(lv_task_cb_t task_cb)
{
    /* 覆盖整个屏幕 */
    loader_page = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(loader_page, LV_HOR_RES, LV_VER_RES); // TODO: 修改大小适配屏幕
    lv_page_set_scrl_layout(loader_page, LV_LAYOUT_CENTER);
    (loader_page, LV_LAYOUT_CENTER);
    /* spinner */
    lv_obj_t* spinner = lv_spinner_create(loader_page, NULL);
    // lv_obj_t *btn = lv_btn_create(btn, NULL);
    // lv_obj_set_event_cb(btn, cancel_cb);
    lv_task_create(task_cb, 10, LV_TASK_PRIO_HIGH, NULL);
}

static void loader_task_cb_example(lv_task_t* t)   //TODO: 范例函数, 记得删除
{
    static int count = 0;
    count++;
    if (count < 500) {
        return;
    }
    lv_obj_del(loader_page);
    lv_gui_anim_out_all(lv_scr_act(), 0);
    ctrl_page_open(0);
    lv_task_del(t);
}

/* ctrl page */
static void inc_btn_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        if (tartemp_value < 30) {
            tartemp_value += 1;
            lv_label_set_text_fmt(tartemp_label, "%2d"LV_SYMBOL_SETTINGS, tartemp_value);
        }
    }
}

static void dec_btn_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        if (tartemp_value > 18) {
            tartemp_value -= 1;
            lv_label_set_text_fmt(tartemp_label, "%2d"LV_SYMBOL_SETTINGS, tartemp_value);
        }
    }
}

static void ctrl_page_event_cb(lv_obj_t* page, lv_event_t e)
{
    if (e == LV_EVENT_GESTURE) {
        printf("Page Event Gesture: %d\n", lv_indev_get_gesture_dir(lv_indev_get_act()));
    }
}

/* common utils */
static void lv_gui_anim_out_all(lv_obj_t* obj, uint32_t delay)
{
    lv_obj_t* child = lv_obj_get_child_back(obj, NULL);
    while (child) {
        lv_obj_del(child);
        child = lv_obj_get_child_back(obj, child);
    }
}