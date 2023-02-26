/**************************************************************/
/* Fire alarm unit by HA3FLT, HA2OS @ 2022-2023               */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "sensors/fire_alarm.h"
#include "main.h"

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-fire_alarm";

/*************************/
/* Initialize fire_alarm */
/*************************/
esp_err_t fire_alarm_init(void)
{
    ESP_LOGI(TAG, "fire_alarm_init()");

    ESP_LOGI(TAG, "fire_alarm_init(), end");
    return ESP_OK;
}

bool fire_alarm_is_fire(void)
{
    //TODO: It must set a global error flag to communicate with the caller
    return false;
}
