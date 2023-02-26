/**************************************************************/
/* Utility module by HA3FLT, HA2OS @ 2022-2023                */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include <esp_log.h>
#include <esp_system.h>

#include "driver/temperature_sensor.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "string.h"

#include "settings_mgr.h"

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-utils";

/***********************/
/* Utility module code */
/***********************/
bool utils_is_valid_char(char ch)
{
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
//TODO: ??
//            || ch == ;
}

bool utils_is_valid_chars(char* str)
{
    if (str == NULL) {
        ESP_LOGE(TAG, "Invalid parameter for utils_is_valid_chars()");
        return false;
    }

    while (*str != '\0') {
        if (!utils_is_valid_char(*str)) {
            return false;
        }
        ++str;
    }

    return true;
}

void utils_get_log_level_string_from_level(char* log_level_string, esp_log_level_t log_level)
{
    if (log_level_string == NULL) {
        ESP_LOGI(TAG, "Invalid parameter for utils_get_log_level_string_from_level()");
    }

    if (log_level == ESP_LOG_NONE) {
        strcpy(log_level_string, "none");
    } else
    if (log_level == ESP_LOG_DEBUG) {
        strcpy(log_level_string, "debug");
    } else
    if (log_level == ESP_LOG_INFO) {
        strcpy(log_level_string, "info");
    } else
    if (log_level == ESP_LOG_WARN) {
        strcpy(log_level_string, "warning");
    } else
    if (log_level == ESP_LOG_ERROR) {
        strcpy(log_level_string, "error");
    } else {
        strcpy(log_level_string, "UNKNOWN");
    }
}

void utils_get_log_level_from_level_string(esp_log_level_t* log_level, char* log_level_string)
{
    if (log_level_string == NULL) {
        ESP_LOGI(TAG, "Invalid parameter for utils_get_log_level_from_level_string()");
    }

    if (strcmp(log_level_string, "none") == 0) {
        *log_level = ESP_LOG_NONE;
    } else
    if (strcmp(log_level_string, "debug") == 0) {
        *log_level = ESP_LOG_DEBUG;
    } else
    if (strcmp(log_level_string, "info") == 0) {
        *log_level = ESP_LOG_INFO;
    } else
    if (strcmp(log_level_string, "warn") == 0 || strcmp(log_level_string, "warning") == 0) {
        *log_level = ESP_LOG_WARN;
    } else
    if (strcmp(log_level_string, "error") == 0) {
        *log_level = ESP_LOG_ERROR;
    } else {
        ESP_LOGE(TAG, "Invalid parameter \"%s\" for utils_get_log_level_from_level_string()", log_level_string);
        *log_level = ESP_LOG_NONE;
    }
}

esp_err_t utils_set_loglevel_by_string(char* module_name, char* log_level_string)
{
    esp_err_t esp_err = ESP_OK;
    if (module_name == NULL || log_level_string == NULL) {
        ESP_LOGI(TAG, "Invalid parameter(s) for utils_set_loglevel_by_string()");
        return ESP_FAIL;
    }

    esp_log_level_t log_level;
    utils_get_log_level_from_level_string(&log_level, log_level_string);

    esp_log_level_set(module_name, log_level);
    return esp_err;
}

esp_err_t utils_set_loglevel_from_nvs(char* module_name)
{
    esp_err_t esp_err = ESP_OK;

    if (module_name == NULL) {
        ESP_LOGI(TAG, "Invalid parameter for utils_set_stored_loglevel()");
    }
    ESP_LOGI(TAG, "Setting log level for module \"%s\" to the stored value...", module_name);

    esp_log_level_t log_level;
    esp_err = settings_get_loglevel(&log_level);
    if (esp_err == ESP_OK) {
        esp_log_level_set(module_name, log_level);
    } else {
        ESP_LOGE(TAG, "Error setting log level to the stored value");
    }
    return esp_err;
}

esp_err_t utils_set_syslog(char* ipaddr_with_port, char* name, char* password)
{
    esp_err_t esp_err = ESP_OK;

    //TODO: find colon and optional port number
    //char* ipaddr[];
    //char* port[];

    return esp_err;
}

esp_err_t utils_GetStatus(bool all_info)
{
    // MALLOC_CAP_32BIT      Memory must allow for aligned 32-bit data accesses
    // MALLOC_CAP_8BIT       Memory must allow for 8/16/...-bit data accesses
    // MALLOC_CAP_DMA        Memory must be able to accessed by DMA
    // MALLOC_CAP_SPIRAM     Memory must be in SPI RAM
    // MALLOC_CAP_INTERNAL   Memory must be internal; specifically it should not disappear when flash/spiram cache is switched off
    // MALLOC_CAP_DEFAULT    Memory can be returned in a non-capability-specific memory allocation (e.g. malloc(), calloc()) call
    // MALLOC_CAP_IRAM_8BIT  Memory must be in IRAM and allow unaligned access
    // MALLOC_CAP_RETENTION 
    // MALLOC_CAP_RTCRAM     Memory must be in RTC fast memory

    size_t ctsDefault = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    size_t cfsDefault = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    ESP_LOGI(TAG, "Default memory...: Total=%d, Free=%d", (int)ctsDefault, (int)cfsDefault);

    size_t ctsInternal = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    size_t cfsInternal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    ESP_LOGI(TAG, "Internal memory..: Total=%d, Free=%d", (int)ctsInternal, (int)cfsInternal);

    size_t ctsDMA = heap_caps_get_total_size(MALLOC_CAP_DMA);
    size_t cfsDMA = heap_caps_get_free_size(MALLOC_CAP_DMA);
    ESP_LOGI(TAG, "DMA memory.......: Total=%d, Free=%d", (int)ctsDMA, (int)cfsDMA);

    return ESP_OK;
}

/**********************/
/* Temperature sensor */
/**********************/
static temperature_sensor_handle_t s_temp_sensor = NULL;

esp_err_t utils_init_internal_temperature_sensor()
{
    esp_err_t esp_err;

    if (s_temp_sensor != NULL) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Installing temperature sensor, expected temp ranger range: -10~80 ℃");
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    if ((esp_err = temperature_sensor_install(&temp_sensor_config, &s_temp_sensor)) != ESP_OK) {
        ESP_LOGE(TAG, "Error installing temperature sensor driver, status: %d", esp_err);
        return esp_err;
    }
    return ESP_OK;
}

esp_err_t utils_get_internal_temperature(float* tsens_value)
{
    if (tsens_value == NULL || s_temp_sensor == NULL) {
        return ESP_FAIL;
    }

    //ESP_LOGD(TAG, "Enabling temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_enable(s_temp_sensor));
    //ESP_LOGD(TAG, "Reading temperature");
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(s_temp_sensor, tsens_value));
    //ESP_LOGD(TAG, "ESP32's temperature: %.02f ℃", *tsens_value);
    //ESP_LOGD(TAG, "Disabling temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_disable(s_temp_sensor));

    return ESP_OK;
}
