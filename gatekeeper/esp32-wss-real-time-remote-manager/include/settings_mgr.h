/**************************************************************/
/* Settings manager by HA3FLT, HA2OS @ 2022-2023              */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_log.h>
#include <esp_system.h>

#include <inttypes.h>

#include "nvs_flash.h"

/*typedef struct {
} __attribute__((__packed__)) xxx_t;*/

#define SETTINGS_MAX_STRING_PARAM_LEN   33

/***********************/
/* Function prototypes */
/***********************/
extern esp_err_t settings_init(void);

extern esp_err_t settings_init_partition(void);
extern esp_err_t settings_open(void);
extern esp_err_t settings_close(void);
extern esp_err_t settings_commit(void);
extern esp_err_t settings_destroy_all(void);

extern esp_err_t settings_get_echo(bool* echo);
extern esp_err_t settings_set_echo(bool echo);

extern esp_err_t settings_get_loglevel(esp_log_level_t* level);
extern esp_err_t settings_set_loglevel(esp_log_level_t level);

extern esp_err_t settings_getWifiCreds(char* ssid);
extern esp_err_t settings_setWifiCreds(char* ssid, char* password);
