/**************************************************************/
/* Status storage by HA3FLT, HA2OS @ 2022-2023                */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "status_storage.h"
#include "main.h"

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-status_storage";

/******************************/
/* Initialize serial_commands */
/******************************/
esp_err_t status_init(void)
{
    ESP_LOGI(TAG, "status_init()");

    return ESP_OK;
}
