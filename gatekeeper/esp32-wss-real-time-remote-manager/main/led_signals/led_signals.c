/**************************************************************/
/* NeoPixel LED signals by HA3FLT, HA2OS @ 2022-2023          */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "led_signals.h"
#include "led_strip.h"
#include "main.h"

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-led_signals";

typedef struct {
    uint8_t LedStatus;
    uint8_t LedState;
    uint16_t LedDelay;
    uint8_t LedColorR;
    uint8_t LedColorG;
    uint8_t LedColorB;
    led_strip_handle_t LedStrip;
} __attribute__((__packed__)) Led_Info_t;

// eGStatusInitializing, fast blinking, orange light
static Led_Info_t s_led_info = { eGLedStatusInitializing, 1,
    BLINKGING_DELAY_SHORT / portTICK_PERIOD_MS, DLED_ACTUAL_LEVEL, DLED_ACTUAL_LEVEL, 0, NULL };

static void configure_led(void);
static void set_led_color(void);
static void led_task(void *arg);

/**************************/
/* Initialize led_signals */
/**************************/
esp_err_t ledsigs_init()
{
    ESP_LOGI(TAG, "ledsigs_init()");

    configure_led();
    (void) xTaskCreate(led_task, "led_task", LED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);  // pdTRUE

    return ESP_OK;
}

/************************************/
/* Signal property setter functions */
/************************************/
esp_err_t ledsigs_set_led_status(uint8_t led_status)
{
    //TODO: Range check
    s_led_info.LedStatus = led_status;

    return ESP_OK;
}

esp_err_t ledsigs_set_led_delay(uint16_t led_delay)
{
    //TODO: Range check
    s_led_info.LedDelay = led_delay;

    return ESP_OK;
}

esp_err_t ledsigs_set_led_color(uint8_t led_color_r, uint8_t led_color_g, uint8_t led_color_b)
{
    //TODO: Range check
    s_led_info.LedColorR = led_color_r;
    s_led_info.LedColorG = led_color_g;
    s_led_info.LedColorB = led_color_b;
    set_led_color();

    return ESP_OK;
}

/***************************/
/* led_signals module code */
/***************************/
static void configure_led(void)
{
    // LED strip initialization with the GPIO and pixels number
    led_strip_config_t strip_config = {
        .strip_gpio_num = NEOPIXEL_PIN,
        .max_leds = 1, // at least one LED on board
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &s_led_info.LedStrip));
    // Set all LED off to clear all pixels
    led_strip_clear(s_led_info.LedStrip);
}

static void set_led_color(void)
{
    // Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color
    led_strip_set_pixel(s_led_info.LedStrip, 0,
        s_led_info.LedColorR, s_led_info.LedColorG, s_led_info.LedColorB);
    // Refresh the strip to send data
    led_strip_refresh(s_led_info.LedStrip);
}

static void blink_led(void)
{
    // If the addressable LED is enabled
    if (s_led_info.LedState) {
        set_led_color();
    } else {
        // Set all LED off to clear all pixels
        led_strip_clear(s_led_info.LedStrip);
    }
    s_led_info.LedState = !s_led_info.LedState;
}

// The task
static void led_task(void *arg)
{
    while (1) {
        //TODO: Locking?
        blink_led();
        vTaskDelay(s_led_info.LedDelay);

        main_watchdog_callback();
    }
}
