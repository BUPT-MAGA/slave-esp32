#ifndef AIR_SLAVE_H
#define AIR_SLAVE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
/*********************
 *      DEFINES
 *********************/
// #define SIMULATOR
/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    AIR_SPEED_LOW   = 0,
    AIR_SPEED_MID   = 1,
    AIR_SPEED_HIGH  = 2,
    AIR_SPEED_NONE  = -1,
} air_speed_t;

typedef enum {
    AIR_MODE_COOL   = 0,
    AIR_MODE_HEAT   = 1,
    AIR_MODE_NONE   = 2,
} air_mode_t;

typedef struct _air_slave_t air_slave_t;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern air_slave_t *air_slave;

int air_get_tartemp(air_slave_t *slave);
int air_get_curtemp(air_slave_t *slave);
int air_get_airtemp(air_slave_t *slave);
int air_get_speed(air_slave_t *slave);
int air_get_mode(air_slave_t *slave);
double air_get_cost(air_slave_t *slave);

void air_init(air_slave_t *slave, air_mode_t mode, int air_temp);
void air_set_tartemp(air_slave_t *slave, int tar_temp);
void air_update_curtemp(air_slave_t *slave, int air_temp, air_speed_t air_speed);
void air_update_curtemp_env(air_slave_t *slave);
void air_update_speed(air_slave_t *slave);
/* TODO: 设置mode可能有问题 */
void air_set_mode(air_slave_t *slave, air_mode_t mode);
void air_set_cost(air_slave_t *slave, double cost);
void air_set_interval(air_slave_t *slave, int interval);

void air_destroy(air_slave_t *slave);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AIR_SLAVE_H */