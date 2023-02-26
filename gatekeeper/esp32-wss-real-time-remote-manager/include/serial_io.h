/**************************************************************/
/* Serial I/O by HA3FLT, HA2OS @ 2022-2023                    */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_system.h>
#include <inttypes.h>
#include "driver/uart.h"

/**********************/
/* Global definitions */
/**********************/
#define EX_UART_NUM     UART_NUM_0
#define UWRITE(buf)     uart_write_bytes(EX_UART_NUM, (const char*)buf, strlen(buf))

/***********************/
/* Function prototypes */
/***********************/
//TODO: Use a more specific type instead of int
extern esp_err_t serio_init(esp_err_t (*rx_callback_ptr)(char*, int));

extern bool serio_get_echo(void);
extern void serio_set_echo(bool echo);
