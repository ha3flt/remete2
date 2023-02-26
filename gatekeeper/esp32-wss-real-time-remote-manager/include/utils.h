/**************************************************************/
/* Utility module by HA3FLT, HA2OS @ 2022-2023                */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_log.h>
#include <esp_system.h>

/**********************/
/* Global definitions */
/**********************/
#define EX_UART_NUM         UART_NUM_0

/***********************/
/* Function prototypes */
/***********************/
extern bool utils_is_valid_char(char ch);
extern bool utils_is_valid_chars(char* str);

extern void utils_get_log_level_string_from_level(char* log_level_string, esp_log_level_t log_level);
extern void utils_get_log_level_from_level_string(esp_log_level_t* log_level, char* log_level_string);
extern esp_err_t utils_set_loglevel_by_string(char* module_name, char* log_level_string);

extern esp_err_t utils_set_loglevel_from_nvs(char* module_name);

extern esp_err_t utils_set_syslog(char* ipaddr_with_port, char* name, char* password);

extern esp_err_t utils_GetStatus(bool all_info);

extern esp_err_t utils_init_internal_temperature_sensor();
extern esp_err_t utils_get_internal_temperature(float* tsens_value);
