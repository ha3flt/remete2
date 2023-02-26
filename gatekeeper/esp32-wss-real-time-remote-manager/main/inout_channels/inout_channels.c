/**************************************************************/
/* Input/output channels by HA3FLT, HA2OS @ 2022-2023         */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "inout_channels.h"
#include "main.h"

#include <nvs.h>
#include <nvs_flash.h>

#include <string.h>

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-inout_channels";

Output_Database_t s_OutputDB;
Digital_Input_Database_t s_InputDB;
Analog_Input_Database_t s_AnalogInputDB;

uint8_t digital_output_states[IOCH_DIGITAL_OUTPUTS_NR];

static void set_io_db_defaults(void);

/***************************/
/* Initialize user_manager */
/***************************/
esp_err_t inout_init(void)
{
    esp_err_t esp_err;
    ESP_LOGI(TAG, "inout_init()");

    //TODO: We only need it if we could not read it from the flash
    set_io_db_defaults();

    esp_err = inout_load_channel_config_from_flash();
    if (esp_err == ESP_OK) {
        s_OutputDB.ChannelGpio[0] = IOCH_SW0;
        s_OutputDB.ChannelGpio[1] = IOCH_SW1;
        s_OutputDB.ChannelGpio[2] = IOCH_SW2;
        s_OutputDB.ChannelGpio[3] = IOCH_SW3;
        s_OutputDB.ChannelGpio[4] = IOCH_SW4;
        s_OutputDB.ChannelGpio[5] = IOCH_SW5;
        s_OutputDB.ChannelGpio[6] = IOCH_SW6;
        s_OutputDB.ChannelGpio[7] = IOCH_SW7;

        s_InputDB.ChannelGpio[0] = IOCH_DI0;
        s_InputDB.ChannelGpio[1] = IOCH_DI1;
        s_InputDB.ChannelGpio[2] = IOCH_DI2;
        s_InputDB.ChannelGpio[3] = IOCH_DI3;

        s_AnalogInputDB.ChannelGpio[0] = IOCH_AI0;
        s_AnalogInputDB.ChannelGpio[1] = IOCH_AI1;

        //s_OutputDB.ChannelDefaults = ChannelDefaults[IOCH_DIGITAL_OUTPUTS_NR];
        //s_InputDB;
        //s_AnalogInputDB;
    } else {
        //Init to defaults?
    }

    return ESP_OK;
}

static void set_io_db_defaults(void)
{
    int idx;

    // Digital outputs
    for (idx = 0; idx < IOCH_DIGITAL_OUTPUTS_NR; idx++) {
        sprintf(s_OutputDB.ChannelName[idx], "Switch %d", idx + 1);
        s_OutputDB.ChannelType[idx] = (uint8_t)eOutputNormal;
        s_OutputDB.ChannelDefaults[idx] = 0;
        s_OutputDB.ChannelDependencies[idx] = 0;
        s_OutputDB.ChannelUserRights[idx] = (uint8_t)eNormalChannel;
    }
    // Digital inputs
    for (idx = 0; idx < IOCH_DIGITAL_INPUTS_NR; idx++) {
        sprintf(s_InputDB.InputName[idx], "Input %d", idx + 1);
        s_InputDB.ChannelType[idx] = (uint8_t)eDigitalInputEnabled;
        s_InputDB.ChannelUserRights[idx] = (uint8_t)eNormalChannel;
    }
    // Analog inputs
    for (idx = 0; idx < IOCH_ANALOG_INPUTS_NR; idx++) {
        sprintf(s_AnalogInputDB.ChannelName[idx], "Value %d", idx + 1);
        s_InputDB.ChannelType[idx] = (uint8_t)eAnalogInputEnabled;
        s_InputDB.ChannelUserRights[idx] = (uint8_t)eNormalChannel;
    }
}

/******************************/
/* inout_channels module code */
/******************************/
esp_err_t inout_load_channel_config_from_flash()
{
    esp_err_t esp_err;
    nvs_handle_t nvs_handle;

    //TODO: Move setting codes to settings_mgr

    // Init/open
    esp_err = nvs_flash_init_partition(SRSW_SETTINGS_PARTITION_NAME);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x initializing NVS partition \"%s\"", esp_err, SRSW_SETTINGS_PARTITION_NAME);
        return esp_err;
    }
    esp_err = nvs_open_from_partition(SRSW_SETTINGS_PARTITION_NAME,
                IOCH_INOUT_CHANNELS_MODULE_NAME, NVS_READWRITE, &nvs_handle);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x opening NVS partition \"%s\", namespace \"%s\"",
            esp_err, SRSW_SETTINGS_PARTITION_NAME, IOCH_INOUT_CHANNELS_MODULE_NAME);
        return esp_err;
    }

    // Read previously saved blob if available
    size_t struct_size = sizeof(s_OutputDB);
    esp_err = nvs_get_blob(nvs_handle, "Channel.Outdata", &s_OutputDB, &struct_size);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x reading Channel.OutData", esp_err);
        //goto lccff_clean_exit;
    }

    // Read previously saved blob if available
    struct_size = sizeof(s_InputDB);
    esp_err = nvs_get_blob(nvs_handle, "Channel.Inputdata", &s_InputDB, &struct_size);
    if (esp_err != ESP_OK && struct_size == sizeof(s_InputDB)) {
        ESP_LOGE(TAG, "Error %02x reading Channel.InputData", esp_err);
        //goto lccff_clean_exit;
    }

    // Close
//lccff_clean_exit:
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "NVS partition %s closed", SRSW_SETTINGS_PARTITION_NAME);
    return esp_err;
}

esp_err_t inout_save_channel_config_to_flash()
{
    esp_err_t esp_err;
    nvs_handle_t nvs_handle;

    // Init/open
    esp_err = nvs_flash_init_partition(SRSW_SETTINGS_PARTITION_NAME);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x initializing NVS partition \"%s\"", esp_err, SRSW_SETTINGS_PARTITION_NAME);
        return esp_err;
    }
    esp_err = nvs_open_from_partition(SRSW_SETTINGS_PARTITION_NAME,
                IOCH_INOUT_CHANNELS_MODULE_NAME, NVS_READWRITE, &nvs_handle);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x opening NVS partition \"%s\", namespace \"%s\"",
            esp_err, SRSW_SETTINGS_PARTITION_NAME, IOCH_INOUT_CHANNELS_MODULE_NAME);
        return esp_err;
    }

    // Write s_OutputDB
    esp_err = nvs_set_blob(nvs_handle, "Channel.Outputdata", &s_OutputDB, sizeof(s_OutputDB));
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x writing Channel.OutData", esp_err);
        goto scctf_clean_exit;
    }

    // Write s_InputDB
    esp_err = nvs_set_blob(nvs_handle, "Channel.Inputdata", &s_InputDB, sizeof(s_InputDB));
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x writing Channel.InputData", esp_err);
        goto scctf_clean_exit;
    }

    // After setting any values, nvs_commit() must be called to ensure changes
    // are written to flash storage. Implementations may write to storage at
    // other times, but this is not guaranteed.
    esp_err = nvs_commit(nvs_handle);
    if (esp_err != ESP_OK) {
        ESP_LOGE(TAG, "Error %02x commiting NVS partition %s", esp_err, SRSW_SETTINGS_PARTITION_NAME);
        return esp_err;
    }

    // Close
scctf_clean_exit:
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "NVS partition %s closed", SRSW_SETTINGS_PARTITION_NAME);
    return esp_err;
}

esp_err_t inout_get_digital_output(int ionr, int* value)
{
    if (value == NULL) {
        return ESP_FAIL;
    }
    if (ionr < 1 || ionr > IOCH_DIGITAL_OUTPUTS_NR) {
        return ESP_FAIL;
    }
    *value = gpio_get_level(s_OutputDB.ChannelGpio[ionr - 1]);
    return ESP_OK;
}

esp_err_t inout_get_digital_outputs_as_string(char** buf, bool disp_short)
{
    *buf = malloc(IOCH_DIGITAL_OUTPUTS_NR * 32);
    if (buf == NULL) {
        ESP_LOGE(TAG, "Out of memory in inout_get_digital_outputs_as_string()");
        return ESP_FAIL;
    }

    char* p = *buf;
    *p = '\0';
    if (disp_short) {
        for (int idx = 0; idx < IOCH_DIGITAL_OUTPUTS_NR; idx++) {
            int value = gpio_get_level(s_OutputDB.ChannelGpio[idx]);
            sprintf(p, "%d ", value);
            p += strlen(p);
        }
    } else {
        for (int idx = 0; idx < IOCH_DIGITAL_OUTPUTS_NR; idx++) {
            int value = gpio_get_level(s_OutputDB.ChannelGpio[idx]);
            sprintf(p, "Digital output %d: %d\r\n", idx + 1, value);
            p += strlen(p);
        }
    }
    (*buf)[strlen(*buf) - 1] = '\0';
    return ESP_OK;
}

esp_err_t inout_get_digital_input(int ionr, int* value)
{
    if (value == NULL) {
        return ESP_FAIL;
    }
    if (ionr < 1 || ionr > IOCH_DIGITAL_INPUTS_NR) {
        return ESP_FAIL;
    }
    *value = gpio_get_level(s_InputDB.ChannelGpio[ionr - 1]);
    return ESP_OK;
}

esp_err_t inout_get_digital_inputs_as_string(char** buf, bool disp_short)
{
    *buf = malloc(IOCH_DIGITAL_INPUTS_NR * 32);
    if (buf == NULL) {
        ESP_LOGE(TAG, "Out of memory in inout_get_digital_inputs_as_string()");
        return ESP_FAIL;
    }

    char* p = *buf;
    *p = '\0';
    if (disp_short) {
        for (int idx = 0; idx < IOCH_DIGITAL_INPUTS_NR; idx++) {
            int value = gpio_get_level(s_InputDB.ChannelGpio[idx]);
            sprintf(p, "%d ", value);
            p += strlen(p);
        }
    } else {
        for (int idx = 0; idx < IOCH_DIGITAL_INPUTS_NR; idx++) {
            int value = gpio_get_level(s_InputDB.ChannelGpio[idx]);
            sprintf(p, "Digital input %d: %d\r\n", idx + 1, value);
            p += strlen(p);
        }
    }
    (*buf)[strlen(*buf) - 1] = '\0';
    return ESP_OK;
}

esp_err_t inout_get_analog_input(int ionr, int* value)
{
    if (value == NULL) {
        return ESP_FAIL;
    }
    if (ionr < 1 || ionr > IOCH_ANALOG_INPUTS_NR) {
        return ESP_FAIL;
    }
    //TODO: Analog?
    *value = gpio_get_level(s_AnalogInputDB.ChannelGpio[ionr - 1]);
    return ESP_OK;
}

esp_err_t inout_get_analog_inputs_as_string(char** buf, bool disp_short)
{
    *buf = malloc(IOCH_ANALOG_INPUTS_NR * 32);
    if (buf == NULL) {
        ESP_LOGE(TAG, "Out of memory in inout_get_analog_inputs_as_string()");
        return ESP_FAIL;
    }

    char* p = *buf;
    *p = '\0';
    if (disp_short) {
        for (int idx = 0; idx < IOCH_ANALOG_INPUTS_NR; idx++) {
            //TODO: Analog?
            int value = gpio_get_level(s_AnalogInputDB.ChannelGpio[idx]);
            sprintf(p, "%d ", value);
            p += strlen(p);
        }
    } else {
        for (int idx = 0; idx < IOCH_ANALOG_INPUTS_NR; idx++) {
            //TODO: Analog?
            int value = gpio_get_level(s_AnalogInputDB.ChannelGpio[idx]);
            sprintf(p, "Analog input %d: %d\r\n", idx + 1, value);
            p += strlen(p);
        }
    }
    (*buf)[strlen(*buf) - 1] = '\0';
    return ESP_OK;
}

esp_err_t inout_set_digital_output(int ionr, int value)
{
    if (ionr < 1 || ionr > IOCH_DIGITAL_OUTPUTS_NR) {
        return ESP_FAIL;
    }
    gpio_set_level(s_OutputDB.ChannelGpio[ionr - 1], value);
    return ESP_OK;
}

//TODO: gpio_output_set(uint32_t set_mask, uint32_t clear_mask, uint32_t enable_mask, uint32_t disable_mask);
esp_err_t inout_set_digital_outputs_as_string(char* buf)
{
    if (buf == NULL || strlen(buf) < 1) {
        return ESP_FAIL;
    }
    for (int bit_idx = 0; bit_idx < strlen(buf); bit_idx++) {
        gpio_set_level(s_OutputDB.ChannelGpio[bit_idx], (*buf++ == '1') ? 1:0);
    }
    return ESP_OK;
}
