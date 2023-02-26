/**************************************************************/
/* HTTPS + WSS Server by HA3FLT, HA2OS @ 2022-2023            */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "sdkconfig.h"

#include <esp_chip_info.h>
#include <esp_event.h>
#include "esp_flash.h"
#include <esp_netif.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <esp_task_wdt.h>

#include "main.h"
#include "inout_channels.h"
#include "led_signals.h"
#include "sensors/fire_alarm.h"
#include "sensors/temp_sens.h"
#include "settings_mgr.h"
#include "serial_io.h"
#include "srsw_commands.h"
#include "status_storage.h"
#include "user_manager.h"
#include "webserver.h"

/**********************/
/* Code configuration */
/**********************/
#if !CONFIG_HTTPD_WS_SUPPORT
#error This project cannot be used unless HTTPD_WS_SUPPORT is enabled in esp-http-server component configuration
#endif

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-main";

static Global_Firmware_Status_t s_firmware_status = eGFirmwareStatusInitializing;

void set_led_status(Global_Led_Status_t led_status);

/*******************************/
/* Can be called by any module */
/*******************************/
void main_set_firmware_status(Global_Firmware_Status_t status)
{
    if (status == s_firmware_status)
        return;
    s_firmware_status = status;

    switch (s_firmware_status) {
      case eGFirmwareStatusInitializing:
        set_led_status(eGLedStatusInitializing);
        break;
      case eGFirmwareStatusNetworkDisconnected:
        set_led_status(eGLedStatusNetworkDisconnected);
        break;
      case eGFirmwareStatusNetworkConnected:
        set_led_status(eGLedStatusNetworkConnected);
        break;
      case eGFirmwareStatusUsersConnected:
        set_led_status(eGLedStatusUsersConnected);
        break;
      case eGFirmwareStatusAdminConnected:
        set_led_status(eGLedStatusAdminConnected);
        break;
      case eGFirmwareStatusError:
        default:
        set_led_status(eGLedStatusError);
        break;
    }
}

Global_Firmware_Status_t main_get_firmware_status(void)
{
    return s_firmware_status;
}

/*******************************/
/* Change the displayed signal */
/*******************************/
void set_led_status(Global_Led_Status_t led_status)
{
    (void) ledsigs_set_led_status(led_status);

    switch (led_status) {
      case eGLedStatusInitializing:
        (void) ledsigs_set_led_delay(BLINKGING_DELAY_SHORT / portTICK_PERIOD_MS);
        ledsigs_set_led_color(DLED_ACTUAL_LEVEL, DLED_ACTUAL_LEVEL, 0);
        break;
      case eGLedStatusNetworkDisconnected:
        (void) ledsigs_set_led_delay(BLINKGING_DELAY_NORMAL / portTICK_PERIOD_MS);
        ledsigs_set_led_color(0, DLED_ACTUAL_LEVEL, 0);
        break;
      case eGLedStatusNetworkConnected:
        (void) ledsigs_set_led_delay(BLINKGING_DELAY_LONG / portTICK_PERIOD_MS);
        ledsigs_set_led_color(0, 0, DLED_ACTUAL_LEVEL);
        break;
      case eGLedStatusUsersConnected:
        (void) ledsigs_set_led_delay(BLINKGING_DELAY_LONG / portTICK_PERIOD_MS);
        ledsigs_set_led_color(DLED_ACTUAL_LEVEL, DLED_ACTUAL_LEVEL, DLED_ACTUAL_LEVEL);
        break;
      case eGLedStatusAdminConnected:
        (void) ledsigs_set_led_delay(BLINKGING_DELAY_LONG / portTICK_PERIOD_MS);
        ledsigs_set_led_color(DLED_ACTUAL_LEVEL, 0, DLED_ACTUAL_LEVEL);
        break;
      case eGLedStatusError:
      default:
        (void) ledsigs_set_led_delay(BLINKGING_DELAY_SHORT / portTICK_PERIOD_MS);
        ledsigs_set_led_color(DLED_ACTUAL_LEVEL, 0, 0);
        break;
    }

    //TODO: Locking?
    //blink_led();
}

/**************************/
/* Print chip information */
/**************************/
static esp_err_t log_chip_info(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "This is %s chip with %d CPU core(s), WiFi%s%s:",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    ESP_LOGI(TAG, "- Silicon revision %d", chip_info.revision);

    uint32_t flash_size;
    if (esp_flash_get_size(NULL, &flash_size) == ESP_OK) {
        ESP_LOGI(TAG, "- %luMB %s flash", flash_size / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    } else {
        ESP_LOGE(TAG, "Get flash size failed");
    }

    ESP_LOGI(TAG, "- Minimum free heap size: %ld bytes", esp_get_minimum_free_heap_size());
    ESP_LOGI(TAG, "- portTICK_PERIOD_MS: %ld", portTICK_PERIOD_MS);

    return ESP_OK;
}

esp_err_t main_load_all_settings(void)
{
    esp_err_t esp_err = ESP_OK;

    bool bpar;
    //int ipar;
    //char strpar[SETTINGS_MAX_STRING_PARAM_LEN];

    (void)settings_get_echo(&bpar);
    serio_set_echo(bpar);

    return esp_err;
}

//#if SRSW_DEBUG
//extern void nvs_dump(const char *partName);

void get_nvs_info(void)
{
    esp_err_t esp_err;
    nvs_stats_t stats;

    /*ESP_LOGI(TAG, "NVS statistics for the default, \"%s\" partition:", NVS_DEFAULT_PART_NAME);
    esp_err = nvs_get_stats(NVS_DEFAULT_PART_NAME, &stats);
    if (esp_err == ESP_OK) {
        ESP_LOGI(TAG, "- Amount of used entries: %u", stats.used_entries);
        ESP_LOGI(TAG, "- Amount of free entries: %u", stats.free_entries);
        ESP_LOGI(TAG, "- Amount all available entries: %u", stats.total_entries);
        ESP_LOGI(TAG, "- Amount name space: %u", stats.namespace_count);
    } else {
        ESP_LOGE(TAG, "Error 0x%02x getting NVS statistics for partition \"%s\"", esp_err, NVS_DEFAULT_PART_NAME);
    }*/
    //nvs_dump(NVS_DEFAULT_PART_NAME);

    esp_err = nvs_flash_init_partition(SRSW_SETTINGS_PARTITION_NAME);
    if (esp_err == ESP_OK) {
        ESP_LOGI(TAG, "NVS statistics for the \"%s\" partition:", SRSW_SETTINGS_PARTITION_NAME);
        esp_err = nvs_get_stats(SRSW_SETTINGS_PARTITION_NAME, &stats);
        if (esp_err == ESP_OK) {
            ESP_LOGI(TAG, "- Amount of used entries: %u", stats.used_entries);
            ESP_LOGI(TAG, "- Amount of free entries: %u", stats.free_entries);
            ESP_LOGI(TAG, "- Amount all available entries: %u", stats.total_entries);
            ESP_LOGI(TAG, "- Amount name space: %u", stats.namespace_count); 
        } else {
            ESP_LOGE(TAG, "Error 0x%02x getting NVS statistics for partition \"%s\"", esp_err, SRSW_SETTINGS_PARTITION_NAME);
        }
    } else {
        ESP_LOGE(TAG, "Error 0x%02x initializing NVS partition \"%s\"", esp_err, SRSW_SETTINGS_PARTITION_NAME);
    }
    //nvs_dump(SRSW_SETTINGS_PARTITION_NAME);
}
//#endif

/******************************/
/* Watchdog callback function */
/******************************/
void main_watchdog_callback(void)
{
    esp_task_wdt_reset();
}

/**************************/
/*          MAIN          */
/**************************/
void app_main(void)
{
    esp_err_t esp_err = ESP_OK;

    // Start with the core initalization
#if SRSW_DEBUG
    esp_log_level_set("*", ESP_LOG_DEBUG);
#else
    esp_log_level_set("*", ESP_LOG_INFO);
#endif
    ESP_LOGI(TAG, "*****************************************");
    ESP_LOGI(TAG, "* Remete -  T H E  B R I D G E   - HG5C *");
    ESP_LOGI(TAG, "*                                       *");
    ESP_LOGI(TAG, "*     Safe Remote Switch v%02d.%02d.%03d     *",
        SAFE_REMOTE_SWITCH_VERSION_MAJOR, SAFE_REMOTE_SWITCH_VERSION_MINOR, SAFE_REMOTE_SWITCH_VERSION_BUILD);
    ESP_LOGI(TAG, "*            by HA3FLT, HA2OS           *");
    ESP_LOGI(TAG, "*                                       *");
    ESP_LOGI(TAG, "*      R E M E T E  @  H A 5 B V K      *");
    ESP_LOGI(TAG, "*****************************************");

    //TODO: Error checkings must include changing the LED status to ERROR, or is it only safe somewhere below?
    ESP_ERROR_CHECK(log_chip_info());
    ESP_ERROR_CHECK(ledsigs_init());
    // Init the board
    ESP_ERROR_CHECK(utils_init_internal_temperature_sensor());
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Stop for a while only to show the blinking orange light longer...
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Init the settings
    ESP_ERROR_CHECK(settings_init());
    main_load_all_settings();
    ESP_ERROR_CHECK(status_init());
    // Init potentially settings-dependent modules
    ESP_ERROR_CHECK(inout_init());
    ESP_ERROR_CHECK(fire_alarm_init());
    ESP_ERROR_CHECK(tsens_init());
    // Init/enable higher level functionality
    ESP_ERROR_CHECK(srswcmd_init());
//TODO: Test only, later we only show this on debug level
//#if SRSW_DEBUG
    get_nvs_info();
//#endif
    ESP_ERROR_CHECK(serio_init(&srswcmd_parse_input));
//TODO: Test only, later we only show this on debug level
//#if SRSW_DEBUG
    char buf[150];
    srswcmd_cmdStatus(buf, true);
//#endif
    // Make current status visible
    main_set_firmware_status(eGFirmwareStatusNetworkDisconnected);
    esp_err = websvr_init();
    if (esp_err != ESP_OK) {
        main_set_firmware_status(eGFirmwareStatusError);
        ESP_ERROR_CHECK(esp_err);
    }
    // Change log level to the stored setting
    utils_set_loglevel_from_nvs((char*)"*");
    // End of the initialization

    //TODO: This function demonstrates periodic sending Websocket messages to all
    //      connected clients to this server. This is currently a call that never ends.
    websvr_wss_server_send_messages(websvr_get_server());
    // End of normal working of our firmware

    // We should never reach this point, currently.
    main_set_firmware_status(eGFirmwareStatusError);
}
