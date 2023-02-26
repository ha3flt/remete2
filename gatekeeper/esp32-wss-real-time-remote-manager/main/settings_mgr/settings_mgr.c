/**************************************************************/
/* Settings manager by HA3FLT, HA2OS @ 2022-2023              */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "settings_mgr.h"
#include "inout_channels.h"
#include "serial_io.h"
#include "main.h"
#include <string.h>

#include "esp_wifi_types.h"

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-settings_mgr";

nvs_handle_t s_settings_handle = SRSW_INVALID_HANDLE;

/***************************/
/* Initialize settings_mgr */
/***************************/
esp_err_t settings_init(void)
{
    ESP_LOGI(TAG, "settings_init()");

    esp_err_t esp_err;
    esp_err = settings_init_partition();
    if (esp_err != ESP_OK) {
        return esp_err;
    }
    esp_err = settings_open();
    return esp_err;
}

esp_err_t settings_init_partition(void)
{
    esp_err_t esp_err;
    esp_err = nvs_flash_init_partition(SRSW_SETTINGS_PARTITION_NAME);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x initializing NVS partition \"%s\"", esp_err, SRSW_SETTINGS_PARTITION_NAME);
    }
    return esp_err;
}

/****************************/
/* settings_mgr module code */
/****************************/
esp_err_t settings_open(void)
{
    if (s_settings_handle != SRSW_INVALID_HANDLE) {
        return ESP_OK;
    }
    esp_err_t esp_err =
        nvs_open_from_partition(SRSW_SETTINGS_PARTITION_NAME,
            IOCH_INOUT_CHANNELS_MODULE_NAME, NVS_READWRITE, &s_settings_handle);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x opening NVS partition \"%s\", namespace \"%s\"",
            esp_err, SRSW_SETTINGS_PARTITION_NAME, IOCH_INOUT_CHANNELS_MODULE_NAME);
    }
    return esp_err;
}

esp_err_t settings_close(void)
{
    if (s_settings_handle == SRSW_INVALID_HANDLE) {
        return ESP_OK;
    }
    nvs_close(s_settings_handle);
    s_settings_handle = SRSW_INVALID_HANDLE;
    return ESP_OK;
}

esp_err_t settings_commit(void)
{
    if (s_settings_handle == SRSW_INVALID_HANDLE) {
        return ESP_FAIL;
    }
    esp_err_t esp_err = nvs_commit(s_settings_handle);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x commiting NVS partition %s", esp_err, SRSW_SETTINGS_PARTITION_NAME);
    }
    return esp_err;
}

esp_err_t settings_destroy_all(void)
{
    if (s_settings_handle == SRSW_INVALID_HANDLE) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "settings_destroy_all()");
    esp_err_t esp_err = nvs_erase_all(s_settings_handle);

    (void)settings_commit();
    return esp_err;
}

/************/
/* Settings */
/************/
esp_err_t settings_get_echo(bool* echo)
{
    if (echo == NULL) {
        ESP_LOGE(TAG, "Invalid parameter in settings_get_echo()");
        return ESP_FAIL;
    }

    esp_err_t esp_err = ESP_OK;
    return esp_err;
}

esp_err_t settings_set_echo(bool echo)
{
    esp_err_t esp_err = ESP_OK;

    //
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x in settings_set_echo()", esp_err);
        return esp_err;
    }
    return settings_commit();
}

esp_err_t settings_get_loglevel(esp_log_level_t* level)
{
    // typedef enum {
    //    ESP_LOG_NONE,       // No log output
    //    ESP_LOG_ERROR,      // Critical errors, software module can not recover on its own
    //    ESP_LOG_WARN,       // Error conditions from which recovery measures have been taken
    //    ESP_LOG_INFO,       // Information messages which describe normal flow of events
    //    ESP_LOG_DEBUG,      // Extra information which is not necessary for normal use (values, pointers, sizes, etc).
    //    ESP_LOG_VERBOSE     // Bigger chunks of debugging information, or frequent messages which can potentially flood the output.
    // } esp_log_level_t;

    esp_err_t esp_err = ESP_OK;

    if (level == NULL) {
        ESP_LOGE(TAG, "Invalid parameter in settings_get_loglevel()");
        return ESP_FAIL;
    }

    uint8_t byte_level;
    esp_err = nvs_get_u8(s_settings_handle, "loglevel", &byte_level);

    if (esp_err == ESP_OK) {
        // It's an important function, so check it before using this value
        if (byte_level < ESP_LOG_ERROR || byte_level > ESP_LOG_VERBOSE) {
            ESP_LOGE(TAG, "Invalid value %d stored in nvs for log level", byte_level);
            return ESP_FAIL;
        }
        *level = byte_level;
    }
    return esp_err;
}

esp_err_t settings_set_loglevel(esp_log_level_t level)
{
    esp_err_t esp_err = ESP_OK;

    if (level < ESP_LOG_ERROR || level > ESP_LOG_VERBOSE) {
        ESP_LOGE(TAG, "Invalid parameter in settings_set_loglevel()");
        return ESP_FAIL;
    }

    esp_err = nvs_set_u8(s_settings_handle, "loglevel", (uint8_t)level);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x in settings_set_loglevel()", esp_err);
        return esp_err;
    }
    return settings_commit();
}

esp_err_t settings_getWifiCreds(char* ssid)
{
    esp_err_t esp_err = ESP_OK;

    if (ssid == NULL) {
        ESP_LOGE(TAG, "Invalid SSID in settings_setWifiCreds()");
        return ESP_FAIL;
    }
    if (strlen(ssid) > MAX_SSID_LEN) {
        ESP_LOGE(TAG, "Invalid SSID in settings_setWifiCreds()");
        return ESP_FAIL;
    }

    size_t len = MAX_SSID_LEN + 1;
    esp_err = nvs_get_str(s_settings_handle, "wifi-ssid", ssid, &len);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x retrieving wifi-ssid", esp_err);
        return esp_err;
    }
    return settings_commit();
}

esp_err_t settings_setWifiCreds(char* ssid, char* password)
{
    esp_err_t esp_err = ESP_OK;

    if (ssid == NULL) {
        ESP_LOGE(TAG, "Invalid SSID in settings_setWifiCreds()");
        return ESP_FAIL;
    }
    // MAX_SSID_LEN
    if (strlen(ssid) >= SETTINGS_MAX_STRING_PARAM_LEN) {
        ESP_LOGE(TAG, "Invalid SSID in settings_setWifiCreds()");
        return ESP_FAIL;
    }

    if (password == NULL) {
        ESP_LOGE(TAG, "Invalid Password in settings_setWifiCreds()");
        return ESP_FAIL;
    }
    // MAX_PASSPHRASE_LEN is much longer but we don't need such long passphrases
    if (strlen(password) >= SETTINGS_MAX_STRING_PARAM_LEN) {
        ESP_LOGE(TAG, "Invalid Password in settings_setWifiCreds()");
        return ESP_FAIL;
    }

    esp_err = nvs_set_str(s_settings_handle, "wifi-ssid", ssid);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x storing wifi-ssid", esp_err);
        return esp_err;
    }
    esp_err = nvs_set_str(s_settings_handle, "wifi-password", ssid);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x storing wifi-password", esp_err);
        return esp_err;
    }
    return settings_commit();
}
