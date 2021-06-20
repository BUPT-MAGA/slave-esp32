/**
 * @file lv_gui.h
 *
 */

#ifndef LV_GUI_H
#define LV_GUI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "air_slave.h"
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
// #include "lv_examples/lv_examples.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_gui(void);
void lv_gui_anim_out_all(lv_obj_t* obj, uint32_t delay);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*!LV_GUI_H*/
