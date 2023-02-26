/**************************************************************/
/* NeoPixel LED signals by HA3FLT, HA2OS @ 2022-2023          */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_system.h>

#include "led_strip.h"

#define DLED_MIN_LEVEL      1
#define DLED_NORMAL_LEVEL   10
#define DLED_MAX_LEVEL      255

/***********************/
/* Function prototypes */
/***********************/
extern esp_err_t ledsigs_init();

extern esp_err_t ledsigs_set_led_status(uint8_t led_status);

extern esp_err_t ledsigs_set_led_delay(uint16_t led_delay);
extern esp_err_t ledsigs_set_led_color(uint8_t led_color_r, uint8_t led_color_g, uint8_t led_color_b);
