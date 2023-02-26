/**************************************************************/
/* SSL web&socket server by HA3FLT, HA2OS @ 2022-2023         */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_https_server.h>
#include <esp_system.h>

/***********************/
/* Function prototypes */
/***********************/
extern esp_err_t websvr_init(void);

extern httpd_handle_t* websvr_get_server(void);
extern void websvr_wss_server_send_messages(httpd_handle_t* server);
