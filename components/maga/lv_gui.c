/**
 * @file lv_gui.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gui.h"
#include "lv_gui_login.h"
#include "lv_gui_ctrl.h"

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

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_gui(void)
{
    //LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(), lv_theme_get_color_secondary(), 
    //                      LV_THEME_MATERIAL_FLAG_DARK, 
    //                      lv_theme_get_font_small(), lv_theme_get_font_normal(), lv_theme_get_font_subtitle(), lv_theme_get_font_title());
    login_open(0);
    //ctrl_open(0);
}

void lv_gui_anim_out_all(lv_obj_t* obj, uint32_t delay)
{
    lv_obj_t* child = lv_obj_get_child_back(obj, NULL);
    while (child) {
        lv_obj_del(child);
        child = lv_obj_get_child_back(obj, child);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
