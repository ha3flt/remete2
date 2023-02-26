/**************************************************************/
/* Remote Switch commands by HA3FLT, HA2OS @ 2022-2023        */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "srsw_commands.h"
#include "inout_channels.h"
#include "serial_io.h"
#include "settings_mgr.h"
#include "user_manager.h"
#include "main.h"

#include <string.h>

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-srsw_commands";

static esp_err_t srswcmd_evaluate_cmd(char* cmd, char* params1, char* params2, char* params3);

/******************************/
/* Initialize serial_commands */
/******************************/
esp_err_t srswcmd_init(void)
{
    ESP_LOGI(TAG, "srswcmd_init()");

    return ESP_OK;
}

/*******************************/
/* serial_commands module code */
/*******************************/
static void send_version(void)
{
    char buf[65];

    sprintf(buf, "Safe Remote Switch v%d.%d.%d\r\n",
        SAFE_REMOTE_SWITCH_VERSION_MAJOR, SAFE_REMOTE_SWITCH_VERSION_MINOR, SAFE_REMOTE_SWITCH_VERSION_BUILD);
    UWRITE(buf);
}

static void send_help(void)
{
    char buf[150];

    sprintf(buf, "/***************************************************************************/\r\n");    UWRITE(buf);
    sprintf(buf, "** ");        UWRITE(buf);
    send_version();             UWRITE(buf);
    sprintf(buf, " **\r\n");    UWRITE(buf);
    sprintf(buf, "/***************************************************************************/\r\n");    UWRITE(buf);
    sprintf(buf, "/* Serial commands:                                                        */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                                                                         */\r\n");    UWRITE(buf);
    sprintf(buf, "/* help, '?', 'h'                   -- e.g. 'help' or 'help time'          */\r\n");    UWRITE(buf);
    sprintf(buf, "/* login <username> <password>      -- login and create session            */\r\n");    UWRITE(buf);
    sprintf(buf, "/* logout <username>                -- logout and delete session           */\r\n");    UWRITE(buf);
//#if SRSW_DEBUG
    sprintf(buf, "/* sdump, '.'                       -- dump sessions                       */\r\n");    UWRITE(buf);
//#endif
    sprintf(buf, "/* message [new message of the day] -- (MOTD, extra long or quotes help?)  */\r\n");    UWRITE(buf);
    sprintf(buf, "/* echo [on|off]                    -- character echo                      */\r\n");    UWRITE(buf);
    sprintf(buf, "/* getwifi, 'gw'                    -- Get wifi connection details         */\r\n");    UWRITE(buf);
    sprintf(buf, "/* setwifi, 'sw' <SSID> <password>  -- Set wifi AP, reconnect              */\r\n");    UWRITE(buf);
    sprintf(buf, "/* getloglevel, 'gl'                                                       */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                  [module name|*] -- get it for a module or all modules  */\r\n");    UWRITE(buf);
    sprintf(buf, "/* setloglevel, 'sl'                                                       */\r\n");    UWRITE(buf);
    sprintf(buf, "/*     <module name|*>                                                     */\r\n");    UWRITE(buf);
    sprintf(buf, "/*     <log level string> [save]    -- set it, save: store setting in NVS  */\r\n");    UWRITE(buf);
    sprintf(buf, "/* getlog, 'l'                      -- get log entries (last n days, etc.) */\r\n");    UWRITE(buf);
    sprintf(buf, "/* resetlog                         -- delete log from NVS                 */\r\n");  /* (after 2-3 questions...) */   UWRITE(buf);
    sprintf(buf, "/* setsyslog, 'sy' <ipaddr[:port]>                                         */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                <name> <password> -- Set syslog server's conn. details,  */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                                     set ipaddr to zero to deactivate it */\r\n");    UWRITE(buf);
    sprintf(buf, "/* adduser <user name>              -- add a user, normal or admin         */\r\n");    UWRITE(buf);
    sprintf(buf, "/*     <password> [admin] ???            (or rights bit field?)            */\r\n");    UWRITE(buf);
    sprintf(buf, "/* deluser <user name>              -- delete a user                       */\r\n");    UWRITE(buf);
    sprintf(buf, "/* getuser <user name>              -- get user information                */\r\n");    UWRITE(buf);
    sprintf(buf, "/* setuser <user name> ???          -- (rights, name, etc.)                */\r\n");    UWRITE(buf);
    sprintf(buf, "/* setpass [user name]              -- set password for the                */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                                       logged in or another user         */\r\n");    UWRITE(buf);
    sprintf(buf, "/* gettime, 'gt'                    -- get RTC time                        */\r\n");    UWRITE(buf);
    sprintf(buf, "/* settime, 'st' <yyyy/mm/dd hh:mm> -- set RTC date/time (24h)             */\r\n");    UWRITE(buf);
    sprintf(buf, "/* resetall                         -- clear all data from NVS             */\r\n"); /* (after 2-3 questions...) */    UWRITE(buf);
    sprintf(buf, "/* status, '/'                      -- get status data of everything       */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                                     - message of the day                */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                                     - memory                            */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                                     - flash (checksum calculation)      */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                                     - inputs                            */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                                     - outputs                           */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                                     - last log errors                   */\r\n");    UWRITE(buf);
    sprintf(buf, "/* temp, 't'                        -- get internal temperature            */\r\n");    UWRITE(buf);
    sprintf(buf, "/* memory, 'm'                      -- get memory status                   */\r\n");    UWRITE(buf);
    sprintf(buf, "/* getoutput, 'o' [output nr|*]                                            */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                        [short|s] -- get one or all the digital outputs  */\r\n");    UWRITE(buf);
    sprintf(buf, "/* getinput, 'i' [input nr|*]                                              */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                        [short|s] -- get one or all the digital inputs   */\r\n");    UWRITE(buf);
    sprintf(buf, "/* getainput, 'a' [input nr|*]                                             */\r\n");    UWRITE(buf);
    sprintf(buf, "/*                        [short|s] -- get one or all the analog inputs    */\r\n");    UWRITE(buf);
    sprintf(buf, "/* setoutput, 'so'                                                         */\r\n");    UWRITE(buf);
    sprintf(buf, "/*           <outpur nr> <value(s)> -- set one or all the digital outputs  */\r\n");    UWRITE(buf);
    sprintf(buf, "/***************************************************************************/\r\n");    UWRITE(buf);
}

/*********************************************************/
/* Functions writing data to the buffer for the commands */
/*********************************************************/
void srswcmd_cmdStatus(char* buf, bool all_info)
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
    sprintf(buf, "Default memory........: Total=%d, Free=%d\r\n", (int)ctsDefault, (int)cfsDefault);
    UWRITE(buf);
    size_t ctsInternal = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    size_t cfsInternal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    sprintf(buf, "Internal memory.......: Total=%d, Free=%d\r\n", (int)ctsInternal, (int)cfsInternal);
    UWRITE(buf);
    size_t ctsDMA = heap_caps_get_total_size(MALLOC_CAP_DMA);
    size_t cfsDMA = heap_caps_get_free_size(MALLOC_CAP_DMA);
    sprintf(buf, "DMA memory............: Total=%d, Free=%d\r\n", (int)ctsDMA, (int)cfsDMA);
    UWRITE(buf);
    size_t ctsRTC = heap_caps_get_total_size(MALLOC_CAP_RTCRAM);
    size_t cfsRTC = heap_caps_get_free_size(MALLOC_CAP_RTCRAM);
    sprintf(buf, "RTC memory............: Total=%d, Free=%d\r\n", (int)ctsRTC, (int)cfsRTC);
    UWRITE(buf);
}

/********************************************/
/* Parsing the input and executing commands */
/********************************************/
esp_err_t srswcmd_parse_input(char* input_buffer, int buffer_len)
{
    ESP_LOGD(TAG, "srswcmd_parse_input(): \"%s\", len: %u", input_buffer, buffer_len);

    if (buffer_len < 1) {
        //ESP_LOGE(TAG, "srswcmd_parse_input(): Internal error. Empty buffer?");
        //return ESP_FAIL;
        UWRITE("\r\n>");
        return ESP_OK;
    }

    char* p;
    char* ptr;
    char cmd[SRSW_CMD_MAX_LEN] = "";
    char params[SRSW_NR_OF_PARAMS][SRSW_PARAMS_MAX_LEN];
    for (int param_nr = 0; param_nr < SRSW_NR_OF_PARAMS; param_nr++) {
        params[param_nr][0] = '\0';
    }

    p = ptr = input_buffer;
    // Cmd
    while (*p == ' ' && buffer_len-- > 0) { p++; }  // Skip leading spaces
    ptr = p;
    while (*p != ' ' && *p != '\0' && buffer_len-- > 0) { p++; }
    if (p > ptr && p - ptr < SRSW_CMD_MAX_LEN) {
        strncpy(cmd, ptr, p - ptr);
        cmd[p - ptr] = '\0';
        // Params
        int param_nr = 0;
        while (*p != '\0' && param_nr < SRSW_NR_OF_PARAMS) {
            while (*p == ' ' && buffer_len-- > 0) {
                p++;    // Skip leading spaces
            }
            ptr = p;
            while (*p != ' ' && *p != '\0' && buffer_len-- > 0) {
                p++;
            }
            if (p > ptr && p - ptr < SRSW_PARAMS_MAX_LEN) {
                strncpy(params[param_nr], ptr, p - ptr);
                params[param_nr][p - ptr] = '\0';
            }
            param_nr++;
        }
    }

    // Just for the safety...
    cmd[sizeof(cmd) - 1] = '\0';
    for (int param_nr = 0; param_nr < SRSW_NR_OF_PARAMS; param_nr++) {
        params[param_nr][sizeof(params[param_nr]) - 1] = '\0';
    }

    srswcmd_evaluate_cmd(cmd, params[0], params[1], params[2]);
    return ESP_OK;
}

// Making things simpler, params1..3 must be a valid pointer at least to an empty string
static esp_err_t srswcmd_evaluate_cmd(char* cmd, char* params1, char* params2, char* params3)
{
    ESP_LOGD(TAG, "srswcmd_evaluate_cmd(): \"%s\", \"%s\", \"%s\", \"%s\"", cmd, params1, params2, params3);

    esp_err_t esp_err = ESP_OK;
    char buf[150];
    char* pbuf = NULL;

    /*if (cmd[0] == '\r') {
        UWRITE("\r\n>");
        return ESP_OK;
    }*/

    // Every output should start in the following line
    UWRITE("\r\n\r\n");

    if (cmd[0] == 'v' || strcmp("version", cmd) == 0) {
        // Get version
        send_version();
    } else
    if (cmd[0] == '?' || cmd[0] == 'h' || strcmp("help", cmd) == 0) {
        // Get a general help
        send_help();
    } else

    // Basic settings
    if (strcmp("echo", cmd) == 0) {
        if (*params1 == '\0') {
            sprintf(buf, "echo is %s", (serio_get_echo() ? "on" : "off"));
            UWRITE(buf);
        } else
        if (strcmp(params1, "on") == 0) {
            serio_set_echo(true);
            sprintf(buf, "echo on");
            UWRITE(buf);
        } else
        if (strcmp(params1, "off") == 0) {
            serio_set_echo(false);
            sprintf(buf, "echo off");
            UWRITE(buf);
        } else {
            sprintf(buf, "Parameter error: %s", params1);
            UWRITE(buf);
            esp_err = ESP_FAIL;
        }
    } else

    // Wifi
    if (strcmp("getwifi", cmd) == 0 || strcmp("gw", cmd) == 0) {
        esp_err = settings_getWifiCreds(params1);
        if (esp_err == ESP_OK) {
            //TODO: We may print some status info as well here
            sprintf(buf, "Wifi AP SSID, name/password: %s, %s/****", params1, params2);
        }
        UWRITE(buf);
    } else
    if (strcmp("setwifi", cmd) == 0 || strcmp("sw", cmd) == 0) {
        sprintf(buf, "setwifi %s, %s/****", params1, params2);
        UWRITE(buf);
        if (*params1 == '\0' || *params2 == '\0') {
            goto ErrCmd;
        }
        esp_err = settings_setWifiCreds(params1, params2);
    } else

    // Login
    if (strcmp("login", cmd) == 0) {
        sprintf(buf, "login %s, ********", params1);
        UWRITE(buf);
        esp_err = usrmgr_login(params1, params2);
    } else
    if (strcmp("logout", cmd) == 0) {
        sprintf(buf, "logout %s", params1);
        UWRITE(buf);
        esp_err = usrmgr_logout(params1);
        if (esp_err == ESP_OK) {
            sprintf(buf, "Logged out user \"%s\"", params1);
            UWRITE(buf);
        }
    } else
//#if SRSW_DEBUG
    if (cmd[0] == '.' || strcmp("sdump", cmd) == 0) {
        sprintf(buf, "\r\n");
        UWRITE(buf);
        usrmgr_dump_sessions();
    } else
//#endif
    if (strcmp("message", cmd) == 0) {
        if (*params1 == '\0') {
            sprintf(buf, "message: MOTD is \"%s\"", "Karbantartás 2020.05.10-én délelőtt!");
            UWRITE(buf);
        } else {
            sprintf(buf, "message: new MOTD is \"%s\"", params1);
            UWRITE(buf);
        }
    } else

    // Log
    if (strcmp("getloglevel", cmd) == 0 || strcmp("gl", cmd) == 0) {
        if (*params1 == '\0') {
            strcpy(params1, "*");
        }
        esp_log_level_t log_level;
        log_level = esp_log_level_get(params1);
        char log_level_string[16];
        utils_get_log_level_string_from_level(log_level_string, log_level);
        sprintf(buf, "log level for module \"%s\" = \"%s\"", params1, log_level_string);
        UWRITE(buf);

        esp_err = settings_get_loglevel(&log_level);
        if (esp_err == ESP_OK) {
            char log_level_string[16];
            utils_get_log_level_string_from_level(log_level_string, log_level);
            sprintf(buf, "\r\nstored log level for \"%s\" = \"%s\"", params1, log_level_string);
            UWRITE(buf);
        }
    } else
    if (strcmp("setloglevel", cmd) == 0 || strcmp("sl", cmd) == 0) {
        sprintf(buf, "setloglevel %s %s", params1, params2);
        UWRITE(buf);

        if (*params1 == '\0') {
            strcpy(params1, "*");
        }
        bool save_to_nvs = false;
        if (strcmp(params1, "save") == 0) {
            save_to_nvs = true;
        }

        esp_log_level_t log_level;
        utils_get_log_level_from_level_string(&log_level, params2);
        //ESP_LOGE(TAG, "Parameter error: %s %s", params1, params2);
        //esp_err = ESP_FAIL;
        esp_log_level_set(params1, log_level);
        if (params1[0] == '*') {
            if (save_to_nvs) {
                (void)settings_set_loglevel(log_level);
            }
        }

        esp_err = settings_get_loglevel(&log_level);
        if (esp_err == ESP_OK) {
            char log_level_string[16];
            utils_get_log_level_string_from_level(log_level_string, log_level);
            sprintf(buf, "stored log level for \"%s\" = \"%s\"", params1, log_level_string);
        }
        UWRITE(buf);
    } else
    if (cmd[0] == 'l' || strcmp("getlog", cmd) == 0) {
        sprintf(buf, "getlog");
        UWRITE(buf);
        /*if (xxx(buf) != ESP_OK) {
            esp_err = ESP_FAIL;
        }*/
    } else
    if (strcmp("resetlog", cmd) == 0) {
        sprintf(buf, "resetlog");
        UWRITE(buf);
        /*if (xxx() != ESP_OK) {
            esp_err = ESP_FAIL;
        }*/
    } else
    if (strcmp("setsyslog", cmd) == 0 || strcmp("sy", cmd) == 0) {
        // <ipaddr[:port]> <name> <password>
        sprintf(buf, "setsyslog %s, %s/****", params1, params2);
        UWRITE(buf);
        esp_err = utils_set_syslog(params1, params2, params3);
    } else

    // Users
    if (strcmp("adduser", cmd) == 0) {
        sprintf(buf, "adduser %s", params1);
        UWRITE(buf);
        //TODO: Are you sure to add user ...?
        if (usrmgr_add_user(params1) != ESP_OK) {
            ESP_LOGD(TAG, "Parameter error: %s", params1);
            esp_err = ESP_FAIL;
        }
    } else
    if (strcmp("deluser", cmd) == 0) {
        sprintf(buf, "deluser %s", params1);
        UWRITE(buf);
        //TODO: Are you sure to delete user ...?
        if (usrmgr_remove_user(params1) != ESP_OK) {
            ESP_LOGD(TAG, "Parameter error: %s", params1);
            esp_err = ESP_FAIL;
        }
    } else
    if (strcmp("getuser", cmd) == 0) {
        sprintf(buf, "getuser %s", params1);
        UWRITE(buf);
        if (usrmgr_get_user_info(params1, buf) != ESP_OK) {
            ESP_LOGD(TAG, "Parameter error: %s", params1);
            esp_err = ESP_FAIL;
        }
    } else
    if (strcmp("setuser", cmd) == 0) {
        sprintf(buf, "setuser %s", params1);
        UWRITE(buf);
        ESP_LOGI(TAG, "setuser %s", params1);
        //TODO: Are you sure to set user ...?
        if (usrmgr_set_user_info(params1) != ESP_OK) {
            ESP_LOGD(TAG, "Parameter error: %s", params1);
            esp_err = ESP_FAIL;
        }
    } else
    if (strcmp("setpass", cmd) == 0) {
        sprintf(buf, "setpass %s %s", params1, params2);
        UWRITE(buf);
        //TODO: Are you sure to set...?
        if (usrmgr_set_password(params1, params2) != ESP_OK) {
            ESP_LOGD(TAG, "Parameter error: %s %s", params1, params2);
            esp_err = ESP_FAIL;
        }
    } else

    // RTC
    if (strcmp("gettime", cmd) == 0 || strcmp("gt", cmd) == 0) {
        sprintf(buf, "gettime %s", params1);
        UWRITE(buf);
        /*if (xxx(buf) != ESP_OK) {
            ESP_LOGD(TAG, "Parameter error: %s", params1);
            esp_err = ESP_FAIL;
        }*/
    } else
    if (strcmp("settime", cmd) == 0 || strcmp("st", cmd) == 0) {
        sprintf(buf, "settime %s", params1);
        UWRITE(buf);
        //TODO: Are you sure to set...?
        /*if (xxx(params1) != ESP_OK) {
            ESP_LOGD(TAG, "Parameter error: %s", params1);
            esp_err = ESP_FAIL;
        }*/
        //TODO: Do NOT forget to adjust counters in the session if we
        //      are not using continuously increasing ticks or something
    } else

    // Reset, status, etc.
    if (strcmp("resetall", cmd) == 0) {
        sprintf(buf, "resetall");
        UWRITE(buf);
        //TODO: Are you sure??
        if (settings_destroy_all() != ESP_OK) {
            esp_err = ESP_FAIL;
        }
    } else
    if (cmd[0] == '/' || strcmp("status", cmd) == 0) {
        srswcmd_cmdStatus(buf, true);
    } else
    if (cmd[0] == 'm' || strcmp("memory", cmd) == 0) {
        //TODO: TEMPORARY!!!
        srswcmd_cmdStatus(buf, true);
    } else
    if (cmd[0] == 't' || strcmp("temp", cmd) == 0) {
        float temp;
        if (utils_get_internal_temperature(&temp) != ESP_OK) {
            sprintf(buf, "Error reading internal temperature!");
            UWRITE(buf);
            goto ExitFn;
        }
        sprintf(buf, "Internal temperature = %.02f ℃\r\n", temp);
        UWRITE(buf);
    } else
    // I/O - Digital outputs
    if (cmd[0] == 'o' || strcmp("getoutput", cmd) == 0) {
        if (*params1 == '\0') {
            *params1 = '*';
        }
        //TEMPORARY!!! Not really good, create other commands for computer communication, remove this param.
        bool disp_short = (*params2 != '\0' && (*params2 == 's' || (strcmp(params2, "short") == 0)));
        if (*params1 == '*') {
            esp_err = inout_get_digital_outputs_as_string(&pbuf, disp_short);
            if (esp_err != ESP_OK) {
                sprintf(buf, "getoutput: Error %d reading digital outputs", esp_err);
                UWRITE(buf);
                goto ExitFn;
            }
            sprintf(buf, "getoutput: Digital outputs\r\n%s", pbuf);
            UWRITE(buf);
            free(pbuf);
        } else {
            int ionr = atoi(params1);
            if (ionr < 1 || ionr > IOCH_DIGITAL_OUTPUTS_NR) {
                sprintf(buf, "getoutput: Invalid digital output number %d (1..%d)", ionr, IOCH_DIGITAL_OUTPUTS_NR);
                UWRITE(buf);
                goto ExitFn;
            }
            int value = 0;
            esp_err = inout_get_digital_output(ionr, &value);
            if (esp_err != ESP_OK) {
                sprintf(buf, "getoutput: Error %d reading digital output %d", esp_err, ionr);
                UWRITE(buf);
                goto ExitFn;
            }
            sprintf(buf, "getoutput: Digital output %d: %d", ionr, value);
            UWRITE(buf);
        }
    } else
    // I/O - Digital inputs
    if (cmd[0] == 'i' || strcmp("getinput", cmd) == 0) {
        if (*params1 == '\0') {
            *params1 = '*';
        }
        //TEMPORARY!!! Not really good, create other commands for computer communication, remove this param.
        bool disp_short = (*params2 != '\0' && (*params2 == 's' || (strcmp(params2, "short") == 0)));
        if (*params1 == '*') {
            esp_err = inout_get_digital_inputs_as_string(&pbuf, disp_short);
            if (esp_err != ESP_OK) {
                sprintf(buf, "getinput: Error %d reading digital inputs", esp_err);
                UWRITE(buf);
                goto ExitFn;
            }
            sprintf(buf, "getinput: Digital inputs\r\n%s", pbuf);
            UWRITE(buf);
            free(pbuf);
        } else {
            int ionr = atoi(params1);
            if (ionr < 1 || ionr > IOCH_DIGITAL_INPUTS_NR) {
                sprintf(buf, "getinput: Invalid digital input number %d (1..%d)", ionr, IOCH_DIGITAL_INPUTS_NR);
                UWRITE(buf);
                goto ExitFn;
            }
            int value = 0;
            esp_err = inout_get_digital_input(ionr, &value);
            if (esp_err != ESP_OK) {
                sprintf(buf, "getinput: Error %d reading digital input %d", esp_err, ionr);
                UWRITE(buf);
                goto ExitFn;
            }
            sprintf(buf, "getinput: Digital input %d: %d", ionr, value);
            UWRITE(buf);
        }
    } else
    // I/O - Analog inputs
    if (cmd[0] == 'a' || strcmp("getainput", cmd) == 0) {
        if (*params1 == '\0') {
            *params1 = '*';
        }
        //TEMPORARY!!! Not really good, create other commands for computer communication, remove this param.
        bool disp_short = (*params2 != '\0' && (*params2 == 's' || (strcmp(params2, "short") == 0)));
        if (*params1 == '*') {
            esp_err = inout_get_analog_inputs_as_string(&pbuf, disp_short);
            if (esp_err != ESP_OK) {
                sprintf(buf, "getainput: Error %d reading analog inputs", esp_err);
                UWRITE(buf);
                goto ExitFn;
            }
            sprintf(buf, "getainput: Analog inputs\r\n%s", pbuf);
            UWRITE(buf);
            free(pbuf);
        } else {
            int ionr = atoi(params1);
            if (ionr < 1 || ionr > IOCH_ANALOG_INPUTS_NR) {
                sprintf(buf, "getainput: Invalid analog input number %d (1..%d)", ionr, IOCH_ANALOG_INPUTS_NR);
                UWRITE(buf);
                goto ExitFn;
            }
            int value = 0;
            esp_err = inout_get_analog_input(ionr, &value);
            if (esp_err != ESP_OK) {
                sprintf(buf, "getainput: Error %d reading analog input %d", esp_err, ionr);
                UWRITE(buf);
                goto ExitFn;
            }
            sprintf(buf, "getainput: Analog input %d: %d", ionr, value);
            UWRITE(buf);
        }
    } else
    // I/O - Set digital outputs
    if (strcmp("setoutput", cmd) == 0 || strcmp("so", cmd) == 0) {
        if (*params1 == '\0' || *params2 == '\0') {
            goto ErrCmd;
        }
        if (*params1 == '*') {
            esp_err = inout_set_digital_outputs_as_string(params2);
            if (esp_err != ESP_OK) {
                sprintf(buf, "setoutput: Error %d setting digital outputs to %s", esp_err, params2);
                UWRITE(buf);
                goto ExitFn;
            }
            sprintf(buf, "setoutput: Digital outputs set to %s", params2);
            UWRITE(buf);
        } else {
            int ionr = atoi(params1);
            if (ionr < 1 || ionr > IOCH_DIGITAL_OUTPUTS_NR) {
                sprintf(buf, "setoutput: Invalid digital output number %d (1..%d)", ionr, IOCH_DIGITAL_OUTPUTS_NR);
                UWRITE(buf);
                goto ExitFn;
            }
            int value = atoi(params2);
            if (value != 0 && value != 1) {
                sprintf(buf, "getoutput: Invalid digital output value %d for output %d", value, ionr);
                UWRITE(buf);
                goto ExitFn;
            }
            esp_err = inout_set_digital_output(ionr, value);
            if (esp_err != ESP_OK) {
                sprintf(buf, "setoutput: Error %d setting digital output %d", esp_err, ionr);
                UWRITE(buf);
                goto ExitFn;
            }
            sprintf(buf, "setoutput: Digital output %d set to %d", ionr, value);
            UWRITE(buf);
        }
    }

    // Command error
    else {
ErrCmd:
        sprintf(buf, "Invalid command: \"%s\" \"%s\" \"%s\" \"%s\"", cmd, params1, params2, params3);
        UWRITE(buf);
        esp_err = ESP_FAIL;
    }

ExitFn:
    UWRITE("\r\n>");
    return esp_err;
}

