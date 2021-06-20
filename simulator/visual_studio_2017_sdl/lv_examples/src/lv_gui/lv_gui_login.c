/**
 * @file lv_gui_login.c
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
#endif

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
static void login_btn_event_cb(lv_obj_t* btn, lv_event_t e);
static void reset_btn_event_cb(lv_obj_t* btn, lv_event_t e);
static void ta_event_cb(lv_obj_t* ta, lv_event_t e);
static void kb_event_cb(lv_obj_t* kb, lv_event_t e);

static void loader_task_cb(lv_task_t* t);
static void btn_cancel_event_cb(lv_obj_t* btn, lv_event_t e);
static void lv_gui_add_loader(lv_task_cb_t task_cb);

#ifndef SIMULATOR
static void btn_timer_cb(void *arg);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t* login_page;
static lv_obj_t* kb_spec;
static lv_obj_t* ta_roomid;
static lv_obj_t* ta_userid;

static const char* kb_map[] = { LV_SYMBOL_OK, "\n",
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

static lv_obj_t *page_loader;
static lv_obj_t* spinner_loader;
static lv_obj_t* label_note;
static lv_obj_t* btn_cancel;
static lv_task_t *task_loader;
#ifndef SIMULATOR
static const esp_timer_create_args_t btn_timer_args = {
    .callback = btn_timer_cb,
    .name = "btn_timer"
};
static esp_timer_handle_t btn_timer = NULL;
#endif
/**********************
 *      MACROS
 **********************/

 /**********************
  *   GLOBAL FUNCTIONS
  **********************/

void login_open(uint32_t delay)
{
    login_page = lv_win_create(lv_scr_act(), NULL);
    lv_obj_set_size(login_page, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_focus_parent(login_page, true);
    lv_win_set_layout(login_page, LV_LAYOUT_CENTER);
    lv_win_set_title(login_page, "Login");
    //lv_obj_set_style_local_border_width(login_page, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 0); //TODO: 更改默认形式为无边框

    ta_roomid = lv_textarea_create(login_page, NULL);
    lv_textarea_set_text(ta_roomid, "");
    lv_textarea_set_placeholder_text(ta_roomid, "Room ID");
    lv_textarea_set_one_line(ta_roomid, true);
    lv_textarea_set_cursor_hidden(ta_roomid, true);
    lv_obj_set_event_cb(ta_roomid, ta_event_cb);
    /*lv_obj_t* label_user = lv_label_create(ta_roomid, NULL);
    lv_label_set_text(label_user, LV_SYMBOL_AUDIO);*/

    ta_userid = lv_textarea_create(login_page, ta_roomid);
    lv_textarea_set_placeholder_text(ta_userid, "User ID");

    lv_obj_t* cont_ta = lv_cont_create(login_page, NULL);
    lv_cont_set_layout(cont_ta, LV_LAYOUT_ROW_MID);
    lv_cont_set_fit2(cont_ta, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_obj_set_style_local_border_width(cont_ta, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_bg_opa(cont_ta, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);

    lv_obj_t* btn_reset = lv_btn_create(cont_ta, NULL);
    lv_obj_set_event_cb(btn_reset, reset_btn_event_cb);
    lv_obj_t* label_reset = lv_label_create(btn_reset, NULL);
    lv_label_set_text(label_reset, "Reset");

    lv_obj_t* btn_login = lv_btn_create(cont_ta, NULL);
    lv_obj_set_event_cb(btn_login, login_btn_event_cb);
    lv_obj_t* label_login = lv_label_create(btn_login, NULL);
    lv_label_set_text(label_login, "Login");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

 /* login page */
static void login_btn_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        if (kb_spec) {
            lv_obj_set_height(login_page, LV_VER_RES);
            lv_obj_del(kb_spec);
            kb_spec = NULL;
        }
        printf("roomID = %s, userID = %s\n", lv_textarea_get_text(ta_roomid), lv_textarea_get_text(ta_userid));
        lv_gui_add_loader(loader_task_cb);
         lv_label_set_text(label_note, "Connecting...");
        lv_obj_set_hidden(btn_cancel, true);
#ifndef SIMULATOR
        if (btn_timer == NULL) {
            esp_timer_create(&btn_timer_args, &btn_timer);
            esp_timer_start_once(btn_timer, 1000 * 4000);
        }
        ws_client_start(lv_textarea_get_text(ta_roomid), lv_textarea_get_text(ta_userid));
#endif
    }
}

static void reset_btn_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        lv_textarea_set_text(ta_roomid, "");
        lv_textarea_set_text(ta_userid, "");
    }
}

static void ta_event_cb(lv_obj_t* ta, lv_event_t e)
{
    if (e == LV_EVENT_RELEASED) {
        if (kb_spec == NULL) {
            lv_obj_set_height(login_page, LV_VER_RES / 2);
            kb_spec = lv_keyboard_create(lv_scr_act(), NULL);
            lv_keyboard_set_ctrl_map(kb_spec, LV_KEYBOARD_MODE_NUM, kb_ctrl_map);
            lv_keyboard_set_map(kb_spec, LV_KEYBOARD_MODE_NUM, kb_map);
            lv_keyboard_set_mode(kb_spec, LV_KEYBOARD_MODE_NUM);
            lv_obj_set_event_cb(kb_spec, kb_event_cb);

            lv_indev_wait_release(lv_indev_get_act());
        }
        lv_textarea_set_cursor_hidden(ta, false);
        lv_win_focus(login_page, lv_textarea_get_label(ta), LV_ANIM_ON);
        lv_keyboard_set_textarea(kb_spec, ta);
    }
    else if (e == LV_EVENT_DEFOCUSED) {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

static void kb_event_cb(lv_obj_t* kb, lv_event_t e)
{
    lv_keyboard_def_event_cb(kb, e);

    if (e == LV_EVENT_CANCEL || e == LV_EVENT_APPLY) {
        if (kb_spec) {
            lv_obj_set_height(login_page, LV_VER_RES);
            lv_obj_del(kb_spec);
            kb_spec = NULL;
        }
    }
}

#ifndef SIMULATOR
static void loader_task_cb(lv_task_t* t)
{
    // static int loader_state = 0;
    // if (!ws_client_is_connected())
    //     return;
    // lv_gui_anim_out_all(lv_scr_act(), 0);
    // ctrl_open(0);
    // lv_task_del(t);
    EventBits_t bits;
    bits = xEventGroupClearBits(ws_client_event, WS_CONNECT_EVENT_BIT|WS_DISCONNECT_EVENT_BIT);
    if ((bits & WS_CONNECT_EVENT_BIT) == WS_CONNECT_EVENT_BIT) {
        lv_label_set_text(label_note, "Initializing...");
        /* TODO: Remove This */
        // lv_gui_anim_out_all(lv_scr_act(), 0);
        // ctrl_open(0);
        // lv_task_del(t);
        // air_init(air_slave, 0, 22);  //FIXME: Remove This
    }
    else if ((bits & WS_DISCONNECT_EVENT_BIT) == WS_DISCONNECT_EVENT_BIT) {
        lv_label_set_text(label_note, "#ff0000 Error: Disconnected#");
        lv_obj_set_hidden(btn_cancel, false);
    }
    else if ((bits & WS_DATA_MASTER_EVENT_BIT) == WS_DATA_MASTER_EVENT_BIT) {
        if (btn_timer != NULL) {
            esp_timer_stop(btn_timer);
            esp_timer_delete(btn_timer);
            btn_timer = NULL;
        }
        lv_gui_anim_out_all(lv_scr_act(), 0);
        ctrl_open(0);
        lv_task_del(t);
    }
}

static void btn_timer_cb(void *arg)
{
    lv_obj_set_hidden(btn_cancel, false);   
    btn_timer = NULL;
}

#else
static void loader_task_cb(lv_task_t* t)   //TODO: 范例函数
{
    static int count = 0;
    count++;
    if (count == 200) {
        lv_label_set_text(label_note, "Initializing...");
    }
    else if (count == 300) {
        lv_obj_set_hidden(btn_cancel, false);
    }
    else if (count == 500) {
        lv_label_set_text(label_note, "#ff0000 Error: Disconnected!#");
        //lv_gui_set_loader_spinner_stop();
    }
    if (count < 800) {
        return;
    }
    lv_gui_anim_out_all(lv_scr_act(), 0);
    ctrl_open(0);
    lv_task_del(t);
}
#endif

static void lv_gui_add_loader(lv_task_cb_t task_cb)
{
    /* 覆盖整个屏幕 */
    page_loader = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(page_loader, LV_HOR_RES, LV_VER_RES); // TODO: 修改大小适配屏幕
    lv_obj_set_focus_parent(page_loader, true);
    lv_page_set_scrl_layout(page_loader, LV_LAYOUT_COLUMN_MID);
    lv_obj_set_style_local_pad_top(page_loader, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 50);

    /* spinner */
    spinner_loader = lv_spinner_create(page_loader, NULL);
    lv_obj_set_size(spinner_loader, 100, 100);

    label_note = lv_label_create(page_loader, NULL);
    lv_label_set_recolor(label_note, true);
    lv_obj_align(label_note, NULL, LV_ALIGN_CENTER, 0, 0);

    btn_cancel = lv_btn_create(page_loader, NULL);
    lv_obj_set_size(btn_cancel, 100, 50);
    lv_obj_set_event_cb(btn_cancel, btn_cancel_event_cb);
    lv_btn_toggle(btn_cancel);
    lv_obj_t* label_cancel = lv_label_create(btn_cancel, NULL);
    lv_label_set_text(label_cancel, LV_SYMBOL_CLOSE);

    task_loader = lv_task_create(task_cb, 10, LV_TASK_PRIO_HIGH, NULL);
}

static void btn_cancel_event_cb(lv_obj_t* btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
#ifndef SIMULATOR
        ws_client_stop();
        if (btn_timer != NULL) {
            esp_timer_stop(btn_timer);
            esp_timer_delete(btn_timer);
            btn_timer = NULL;
        }
#endif
        lv_task_del(task_loader);
        lv_obj_del(page_loader);
    }
}