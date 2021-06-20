#ifndef __MY_AIR_H
#define __MY_AIR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define SIMULATOR

/**********************
 *      TYPEDEFS
 **********************/
#ifdef SIMULATOR
typedef struct {
    uint8_t ssid[33];
} wifi_ap_record_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void my_air(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

