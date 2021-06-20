
/*********************
 *      INCLUDES
 *********************/
#include "air_slave.h"
#include <stdio.h>
#include <math.h>

#ifndef SIMULATOR
#include "esp_timer.h"
#include "ws_client.h"
#endif

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/
struct _air_slave_t {
    /* public */
    float tar_temp;
    float cur_temp;
    float air_temp;
    air_speed_t air_speed;  /* air_speed_t */
    air_mode_t air_mode;   /* air_mode_t */
    int interval;
    double cost;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
#ifndef SIMULATOR
static void report_timer_cb(void *arg);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static const float speed_effect[4] = { 0.0, 1.0, 2.0, 3.0 };

static air_slave_t _air_slave = {
    .tar_temp = 22,
    .cur_temp = 25,
    .air_temp = 19,
    .air_mode = AIR_MODE_COOL,
    .air_speed = AIR_SPEED_MID,
    .cost = 0.0,
    .interval = 1000,
};
air_slave_t *air_slave = &_air_slave;
#ifndef SIMULATOR
const static esp_timer_create_args_t report_timer_args = {
        .callback = &report_timer_cb,
        .name = "report_timer"
    };
static esp_timer_handle_t report_timer = NULL;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


int air_get_tartemp(air_slave_t *slave)
{
    return (int)round(slave->tar_temp);
}

int air_get_curtemp(air_slave_t *slave)
{
    return (int)round(slave->cur_temp);
}

int air_get_airtemp(air_slave_t *slave)
{
    return slave->air_temp;
}

int air_get_speed(air_slave_t *slave)
{
    return slave->air_speed;
}

int air_get_mode(air_slave_t *slave)
{
    return slave->air_mode;
}

double air_get_cost(air_slave_t *slave)
{
    return slave->cost;
}




void air_init(air_slave_t *slave, air_mode_t mode, int air_temp)
{
    slave->air_mode = mode;
    slave->air_temp = (float)air_temp;
#ifndef SIMULATOR
    if (report_timer == NULL) {
        esp_timer_create(&report_timer_args, &report_timer);
    }
    esp_timer_start_periodic(report_timer, 1000 * slave->interval);
#endif
}

void air_set_tartemp(air_slave_t *slave, int tar_temp)
{
    slave->tar_temp = tar_temp;
}

void air_update_curtemp(air_slave_t *slave, int air_temp, air_speed_t air_speed)
{
    slave->cur_temp += (air_temp - slave->cur_temp) * 0.1 * speed_effect[air_speed + 1];
    if (slave->air_temp != air_temp) {
        slave->air_temp = air_temp;
    }
    printf("cur_temp:%.2lf\n", slave->cur_temp);
}

void air_update_curtemp_env(air_slave_t *slave)
{
    float env_temp;
    if (slave->air_mode == AIR_MODE_COOL)
        env_temp = 30.0;
    else if (slave->air_mode == AIR_MODE_HEAT)
        env_temp = 18.0;
    else
        env_temp = slave->cur_temp;
    slave->cur_temp += (env_temp - slave->cur_temp) * 0.1 * 0.03;
    // printf("cur_temp: %.1lf\n", slave->cur_temp);
}

void air_update_speed(air_slave_t *slave)
{
    float diff;
    if (slave->air_mode == AIR_MODE_COOL)
        diff = slave->cur_temp - slave->tar_temp;
    else if (slave->air_mode == AIR_MODE_HEAT)
        diff = slave->tar_temp - slave->cur_temp;
    else
        diff = 0;
    
    if (diff < 0.5) {
        slave->air_speed = AIR_SPEED_NONE;
    }
    else if (diff <= 3) {
        slave->air_speed = AIR_SPEED_LOW;
    }
    else if (diff <= 6) {
        slave->air_speed = AIR_SPEED_MID;
    }
    else {
        slave->air_speed = AIR_SPEED_HIGH;
    }
}

/* TODO: 修改mode可能有问题 */
void air_set_mode(air_slave_t *slave, air_mode_t mode)
{
    slave->air_mode = mode;
}

void air_set_cost(air_slave_t *slave, double cost)
{
    slave->cost = cost;
}

void air_set_interval(air_slave_t *slave, int interval)
{
    slave->interval = interval;
    /* 定时器更新 */
#ifndef SIMULATOR
    esp_timer_stop(report_timer);
    esp_timer_start_periodic(report_timer, 1000 * slave->interval);
#endif
}

void air_destroy(air_slave_t *slave)
{
#ifndef SIMULATOR
    if (report_timer != NULL) {
        esp_timer_stop(report_timer);
        esp_timer_delete(report_timer);
        report_timer = NULL;
    }
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#ifndef SIMULATOR
static void report_timer_cb(void *arg)
{
    if (air_slave->air_speed == AIR_SPEED_NONE) {
        ws_client_send_report((int)round(air_slave->cur_temp), (int)round(air_slave->tar_temp), AIR_MODE_NONE, AIR_SPEED_LOW);
    }
    else {
        ws_client_send_report((int)round(air_slave->cur_temp), (int)round(air_slave->tar_temp), air_slave->air_mode, air_slave->air_speed);
    }
}
#endif



/*********************
 *      INCLUDES
 *********************/
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
/**********************
 *   STATIC FUNCTIONS
 **********************/