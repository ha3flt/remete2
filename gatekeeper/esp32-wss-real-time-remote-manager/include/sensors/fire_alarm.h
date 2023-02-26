/**************************************************************/
/* Fire alarm unit by HA3FLT, HA2OS @ 2022-2023               */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_system.h>
//#include <inttypes.h>

/***********************/
/* Function prototypes */
/***********************/
extern esp_err_t fire_alarm_init(void);

extern bool fire_alarm_is_fire(void);
