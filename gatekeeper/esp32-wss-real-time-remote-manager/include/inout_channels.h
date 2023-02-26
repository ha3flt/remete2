/**************************************************************/
/* Input/output channels by HA3FLT, HA2OS @ 2022-2023         */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#pragma once

#include <esp_system.h>
#include "driver/gpio.h"

#include "user_manager.h"

#include "main.h"

#define IOCH_INOUT_CHANNELS_MODULE_NAME     "inout_channel"

/******************************************************************************************************************/
/* Hardware description - ESP32S2 Saola-1, WROOM or VROVER                                                        */
/*                                                                                                                */
/* Digital outputs:                                                                                               */
/*          SW7          SW6          SW5          SW4          SW3          SW2          SW1          SW0        */
/*        GPIO_13      GPIO_12      GPIO_11      GPIO_10      GPIO_4       GPIO_3       GPIO_2       GPIO_1       */
/*         Pin_15       Pin_14       Pin_13       Pin_12       Pin_6        Pin_5        Pin_4        Pin_3       */
/*                                                                                                                */
/* Digital inputs:                                                                                                */
/*          DI3          DI2          DI1          DI0                                                            */
/*        GPIO_15      GPIO_14      GPIO_6       GPIO_5                                                           */
/*         Pin_17       Pin_16       Pin_8        Pin_7                                                           */
/*                                                                                                                */
/* Analog inputs:                                                                                                 */
/*          AI1          AI0                                                                                      */
/*        GPIO_16      GPIO_7                                                                                     */
/*         Pin_18       Pin_9                                                                                     */
/*        ADC2_5       ADC1_6                                                                                     */
/*                                                                                                                */
/******************************************************************************************************************/

// Define the physical I/O pins for our input/output numbers (1-based)
#define IOCH_SW0    GPIO_NUM_1
#define IOCH_SW1    GPIO_NUM_2
#define IOCH_SW2    GPIO_NUM_3
#define IOCH_SW3    GPIO_NUM_4
#define IOCH_SW4    GPIO_NUM_10
#define IOCH_SW5    GPIO_NUM_11
#define IOCH_SW6    GPIO_NUM_12
#define IOCH_SW7    GPIO_NUM_13

#define IOCH_DI0    GPIO_NUM_5
#define IOCH_DI1    GPIO_NUM_6
#define IOCH_DI2    GPIO_NUM_14
#define IOCH_DI3    GPIO_NUM_15

#define IOCH_AI0    GPIO_NUM_7
#define IOCH_AI1    GPIO_NUM_16

#define IOCH_DIGITAL_OUTPUTS_NR     8
#define IOCH_DIGITAL_INPUTS_NR      4
#define IOCH_ANALOG_INPUTS_NR       2

//ChannelGPIOs[] = { GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, }

typedef enum {
    eNormalChannel,
    eAdminOnlyChannel
} __attribute__((__packed__)) Channel_Rights_t;

/*******************/
/* Digital outputs */
/*******************/
typedef enum {
    eOutputDisabled,
    eOutputNormal
} __attribute__((__packed__)) Output_Channel_Types_t;

typedef struct {
    uint8_t ChannelGpio[IOCH_DIGITAL_OUTPUTS_NR];
    // Null terminated names of the digital output channels
    char ChannelName[IOCH_DIGITAL_OUTPUTS_NR][16];
    // Output_Channel_Types_t
    uint8_t ChannelType[IOCH_DIGITAL_OUTPUTS_NR];
    uint8_t ChannelDefaults[IOCH_DIGITAL_OUTPUTS_NR];
    uint8_t ChannelDependencies[IOCH_DIGITAL_OUTPUTS_NR];
    uint8_t ChannelUserRights[IOCH_DIGITAL_OUTPUTS_NR]; //TODO: Admin, Normal?
} __attribute__((__packed__)) Output_Database_t;

/******************/
/* Digital inputs */
/******************/
typedef enum {
    eDigitalInputDisabled,
    eDigitalInputEnabled
} __attribute__((__packed__)) Digital_InputChannel_Types_t;

typedef struct {
    uint8_t ChannelGpio[IOCH_DIGITAL_INPUTS_NR];
    // Null terminated names of the digital input channels
    char InputName[IOCH_DIGITAL_INPUTS_NR][16];
    uint8_t ChannelType[IOCH_DIGITAL_INPUTS_NR];
    uint8_t ChannelUserRights[IOCH_DIGITAL_INPUTS_NR];
} __attribute__((__packed__)) Digital_Input_Database_t;

//TODO: Reactions to an input - inhibit, turn off, etc.

/*****************/
/* Analog inputs */
/*****************/
typedef enum {
    eAnalogInputDisabled,
    eAnalogInputEnabled
} __attribute__((__packed__)) Analog_Input_Channel_Types_t;

typedef struct {
    uint8_t ChannelGpio[IOCH_ANALOG_INPUTS_NR];
    // Null terminated names of the analog input channels
    char ChannelName[IOCH_ANALOG_INPUTS_NR][16];
    uint8_t ChannelType[IOCH_ANALOG_INPUTS_NR];
    uint8_t ChannelUserRights[IOCH_ANALOG_INPUTS_NR];
} __attribute__((__packed__)) Analog_Input_Database_t;

/*************************/
/* Digital output states */
/*************************/
extern uint8_t digital_output_states[IOCH_DIGITAL_OUTPUTS_NR];

/***********************/
/* Function prototypes */
/***********************/
extern esp_err_t inout_init(void);

extern esp_err_t inout_save_channel_config_to_flash();
extern esp_err_t inout_load_channel_config_from_flash();

extern esp_err_t inout_get_digital_output(int ionr, int* value);
extern esp_err_t inout_get_digital_outputs_as_string(char** buf, bool disp_short);

extern esp_err_t inout_get_digital_input(int ionr, int* value);
extern esp_err_t inout_get_digital_inputs_as_string(char** buf, bool disp_short);

extern esp_err_t inout_get_analog_input(int ionr, int* value);
extern esp_err_t inout_get_analog_inputs_as_string(char** buf, bool disp_short);

extern esp_err_t inout_set_digital_output(int ionr, int value);
extern esp_err_t inout_set_digital_outputs_as_string(char* buf);
