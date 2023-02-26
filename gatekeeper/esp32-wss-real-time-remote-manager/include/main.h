/**************************************************************/
/* HTTPS+WSS Server by HA3FLT, HA2OS @ 2022-2023              */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <inttypes.h>
#include "utils.h"

#include <esp_log.h>
#include <esp_system.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define countof(x)  (sizeof(x)/sizeof(x[0]))
#define SRSW_INVALID_HANDLE             0

#define SRSW_SETTINGS_PARTITION_NAME    "settings"

#define LED_TASK_STACK_SIZE             1536

#define BLINKGING_DELAY_SHORT           250
#define BLINKGING_DELAY_NORMAL          500
#define BLINKGING_DELAY_LONG            1000

// Global firmware status values - there can be more states with the same LED signal
typedef enum {
    eGFirmwareStatusInitializing,
    eGFirmwareStatusNetworkDisconnected,
    eGFirmwareStatusNetworkConnected,
    eGFirmwareStatusUsersConnected,
    eGFirmwareStatusAdminConnected,
    eGFirmwareStatusError
} __attribute__((__packed__)) Global_Firmware_Status_t;

// Led status values
typedef enum {
    eGLedStatusInitializing,
    eGLedStatusNetworkDisconnected,
    eGLedStatusNetworkConnected,
    eGLedStatusUsersConnected,
    eGLedStatusAdminConnected,
    eGLedStatusError
} __attribute__((__packed__)) Global_Led_Status_t;

/***********************/
/* Function prototypes */
/***********************/
extern void main_set_firmware_status(Global_Firmware_Status_t status);
extern Global_Firmware_Status_t main_get_firmware_status(void);

extern esp_err_t main_load_all_settings(void);

extern void main_watchdog_callback(void);
