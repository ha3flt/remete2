/**************************************************************/
/* User manager by HA3FLT, HA2OS @ 2022-2023                  */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_system.h>
#include <inttypes.h>

#define USRMGR_USER                 (1<<0)
#define USRMGR_ADMIN                (1<<1)

#define USRMGR_DISABLED             (1<<15)

#define USRMGR_MIN_USERNAME_LEN     5
#define USRMGR_MAX_USERNAME_LEN     21
#define USRMGR_MIN_PASSWORD_LEN     5
#define USRMGR_MAX_PASSWORD_LEN     21

//TODO: right fields
typedef struct {
    char UserName[USRMGR_MAX_USERNAME_LEN];
    char PasswordHash[USRMGR_MAX_PASSWORD_LEN];
    uint16_t UserRights;
    uint8_t ChannelRights;  // First = (1<<0)
} __attribute__((__packed__)) User_Database_t;

#define USRMGR_MAX_SESSIONS         20
#define USRMGR_SESSION_TIMEOUT      300     // Seconds

typedef struct {
    uint16_t SessionId;     // This is currently the index of Session Data array + 1
    char UserName[USRMGR_MAX_USERNAME_LEN];
    uint16_t SessionTimer;
} __attribute__((__packed__)) Session_Data_t;

/***********************/
/* Function prototypes */
/***********************/
extern esp_err_t usrmgr_init(void);
//#if SRSW_DEBUG
extern void usrmgr_dump_sessions(void);
//#endif

extern esp_err_t usrmgr_get_user_info(char* user_name, char* buffer);
extern esp_err_t usrmgr_set_user_info(char* user_name);
extern esp_err_t usrmgr_add_user(char* user_name);
extern esp_err_t usrmgr_remove_user(char* user_name);
extern esp_err_t usrmgr_set_password(char* user_name, char* password);

extern esp_err_t usrmgr_is_session_valid(uint16_t session_id);
extern void usrmgr_end_all_sessions(void);
extern esp_err_t usrmgr_login(char* user_name, char* password);
extern esp_err_t usrmgr_logout(char* user_name);
