/**************************************************************/
/* Status storage by HA3FLT, HA2OS @ 2022-2023                */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_system.h>
#include <inttypes.h>

// Status database in
typedef struct {
} __attribute__((__packed__)) Status_Database_t;

//static Status_Database_t s_StatusDB;

/***********************/
/* Function prototypes */
/***********************/
extern esp_err_t status_init(void);
