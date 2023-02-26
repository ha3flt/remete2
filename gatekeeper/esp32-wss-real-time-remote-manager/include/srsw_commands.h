/**************************************************************/
/* Remote Switch commands by HA3FLT, HA2OS @ 2022-2023        */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_system.h>
//#include <inttypes.h>

/**********************/
/* Global definitions */
/**********************/

// Cmd. params length must be longer than the next values: MAX_SSID_LEN, ...
// This makes length cheking and data retrieving from the nvs simpler.
#define SRSW_NR_OF_PARAMS       3
#define SRSW_CMD_MAX_LEN        17
#define SRSW_PARAMS_MAX_LEN     33

/*******************************************************************************************************************/
/* Command syntax:                                                                                                 */
/*                                                                                                                 */
/* a: any character                                                                                                */
/* b: binary numbers                                                                                               */
/* n: decimal numbers                                                                                              */
/* h: hexadecimal numbers                                                                                          */
/*                                                                                                                 */
/* ... name: max. ... characters                                                                                   */
/*                                                                                                                 */
/* a                                        - e.g. help, 'h'                                                       */
/* aaa n                                    - e.g. 'o+ 2' turn on output 2                                         */
/* aaa n ??????                             - e.g. 'so 2 ' change output 2 to ???????                              */
/* aa a..a ??????                           - e.g. 'au username'                                                   */
/*                                                                                                                 */
/*******************************************************************************************************************/
/* Serial commands:                                                                                                */
/*                                                                                                                 */
/* help, '?', 'h'                                        -- e.g. "help" or "help time"                             */
/* login <username> <password>                           -- login and create session                               */
/* logout <username>                                     -- logout and delete session                              */
/* sdump                                                 -- dump sessions (only in debug builds)                   */
/* message [new message of the day]                      -- (message of the day, extra long? or quotes help?)      */
/*                                                          quotes are not implemented yet, use undescores         */
/* echo [on|off]                                         -- character echo                                         */
/* getwifi, 'gw'                                         -- Get wifi connection details                            */
/* setwifi, 'sw' <SSID> <password>                       -- Set wifi AP, reconnect                                 */
/* getloglevel <module name|*>                           -- get log level for a module or all modules              */
/* setloglevel <module name|*> <log level string> [save] -- set log level, save: store setting in NVS              */
/* getlog, 'L'                                           -- get log entries (last n days, etc.)                    */
/* resetlog                                              -- delete log from NVS                                    */
/* setsyslog, 'sy' <ipaddr[:port]> <name> <password>     -- Set syslog server's conn. details                      */
/*                                                          set ipaddr to zero to deactivate it                    */
/* adduser <user name> <password> [admin] ???            -- add a user, normal or admin (or rights bit field?)     */
/* deluser <user name>                                   -- delete a user                                          */
/* getuser <user name>                                   -- get user information                                   */
/* setuser <user name> ???                               -- (rights, name, etc.)                                   */
/* setpass [user name]                                   -- set password for logged in or another user             */
/* '@', gettime                                          -- get RTC time                                           */
/* gettime, 'gt'                                         -- get RTC time                                           */
/* settime <yyyy/mm/dd hh:mm>                            -- set RTC date/time (24h)                                */
/* resetall                                              -- clear all data from NVS (after 2-3 questions...)       */
/* status, '/'                                           -- get status data of everything                          */
/*                                                          - message of the day                                   */
/*                                                          - memory                                               */
/*                                                          - flash (checksum calculation)                         */
/*                                                          - inputs                                               */
/*                                                          - outputs                                              */
/*                                                          - last log errors                                      */
/* memory, '!'                                           -- get memory status                                      */
/* getoutput [<output nr.>|<short>] [short]              -- get digital output or all output states                */
/*                                                          - option 'short' is for computer processing            */
/* getinput [<input nr.>|<short>]                        -- get digital input or all input values                  */
/*                                                          - option 'short' is for computer processing            */
/* setoutput [outpur nr.]                                -- set digital output or all output states                */ 
/*                                                                                                                 */
/*******************************************************************************************************************/

/***********************/
/* Function prototypes */
/***********************/
extern esp_err_t srswcmd_init(void);

extern void srswcmd_cmdStatus(char* buf, bool all_info);

extern esp_err_t srswcmd_parse_input(char* input_buffer, int buffer_len);
