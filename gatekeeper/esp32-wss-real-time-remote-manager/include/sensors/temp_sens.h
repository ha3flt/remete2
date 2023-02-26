/**************************************************************/
/* Temperature sensors by HA3FLT, HA2OS @ 2022-2023           */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_system.h>
//#include <inttypes.h>

// Temperature sensors
typedef enum {
    eGTempSensorRoom,
    eGTempSensorOutside
} __attribute__((__packed__)) Global_Temp_Sensor_t;

/***********************/
/* Function prototypes */
/***********************/
extern esp_err_t tsens_init(void);

extern int tsens_get_celsius(Global_Temp_Sensor_t sensor_nr);
