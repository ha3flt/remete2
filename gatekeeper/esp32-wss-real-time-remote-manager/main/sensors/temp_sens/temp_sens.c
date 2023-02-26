/**************************************************************/
/* Temperature sensors by HA3FLT, HA2OS @ 2022-2023           */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "sensors/temp_sens.h"
#include "main.h"

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-temp_sens";

/************************/
/* Initialize temp_sens */
/************************/
esp_err_t tsens_init(void)
{
    ESP_LOGI(TAG, "temp_init()");

    ESP_LOGI(TAG, "temp_init(), end");
    return ESP_OK;
}

int tsens_get_celsius(Global_Temp_Sensor_t sensor_nr)
{
    //TODO: Call this on error:
    //main_set_firmware_status(eGFirmwareStatusError);

    int sensor_temp = 0;
    switch (sensor_nr) {
      case eGTempSensorRoom:
      default:
        //TODO:
        sensor_temp = 0;
        break;
      case eGTempSensorOutside:
        //TODO:
        sensor_temp = 0;
        break;
    }
    return sensor_temp;
}
