/**************************************************************/
/* Serial I/O by HA3FLT, HA2OS @ 2022-2023                    */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "serial_io.h"
#include "srsw_commands.h"
#include "main.h"

#include "os.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-serial_io";

// Set the number of consecutive and identical char's
//   received by receiver which defines a UART pattern
#define PATTERN_CHR_NUM     (3)

#define BUF_SIZE            (1024)
#define RD_BUF_SIZE         (BUF_SIZE)

static bool s_echo = true;

static QueueHandle_t s_uart0_queue;
//TODO: Create a type then use it
static esp_err_t (*s_rx_callback_ptr)(char*, int);

static void uart_event_task(void* pvParameters);

/************************/
/* Initialize serial_io */
/************************/
esp_err_t serio_init(esp_err_t (*rx_callback_ptr)(char*, int))
{
    ESP_LOGI(TAG, "serio_init()");

    //esp_err_t esp_err;
    s_rx_callback_ptr = rx_callback_ptr;

    // Configure parameters of an UART driver,
    //   communication pins and install the driver
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Install UART driver, and get the queue.
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &s_uart0_queue, 0);
    uart_param_config(EX_UART_NUM, &uart_config);

    // Set UART log level
    //esp_log_level_set(TAG, ESP_LOG_INFO);

    // Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(EX_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Create a task to handle UART event from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 3072, NULL, 12, NULL);

    ESP_LOGI(TAG, "serio_init(), end");
    return ESP_OK;
}

extern bool serio_get_echo(void)
{
    return s_echo;
}

extern void serio_set_echo(bool echo)
{
    s_echo = echo;
}

/**************************************************/
/* Serial I/O module code                         */
/*                                                */
/* Handle special UART events / reads data        */
/* from UART0 directly, and echoes it to console. */
/*                                                */
/* - Port: UART0                                  */
/* - Receive (Rx) buffer: on                      */
/* - Transmit (Tx) buffer: off                    */
/* - Flow control: off                            */
/* - Event queue: on                              */
/* - Pin assignment: TxD (default), RxD (default) */
/**************************************************/
static void uart_event_task(void* parameters)
{
    ESP_LOGI(TAG, "uart_event_task() starting...");

    //TODO: Error...
    //UNREFERENCED_PARAMETER(parameters);

    static uint8_t s_cmdbuf[SRSW_CMD_MAX_LEN + 3*(SRSW_PARAMS_MAX_LEN + 1) + 1];
    static int s_cmdbuflen = 0;

    uart_event_t event;
    //size_t buffered_size;
    uint8_t* dtmp = (uint8_t*)malloc(RD_BUF_SIZE);

    for(;;) {
        // Waiting for UART event.
        if (xQueueReceive(s_uart0_queue, (void*)&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, RD_BUF_SIZE);
            switch (event.type) {
                // Event of UART receving data
                // We'd better handler data event fast, there would be much more data events than
                // other types of events. If we take too much time on data event, the queue might
                // be full.
                case UART_DATA:
                    ESP_LOGD(TAG, "[UART DATA]: %d chars", event.size);
                    /*uart_get_buffered_data_len(EX_UART_NUM, &buffered_size);*/
                    uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
                    //ESP_LOGD(TAG, "[DATA EVT]:");
                    //TODO: Protocol; read a minimum nr. of chars, use hash, etc.
                    //      We may call functions in the command processor to determine reading length...
                    if (s_rx_callback_ptr != NULL) {
                        // Check for end of line. Add characters to command line buffer
                        // Non-printable characters including the new line are simply ignored
                        uint8_t* ptr = dtmp;
                        while (*ptr != '\r' && *ptr != '\n' && event.size-- > 0) {
                            if (*ptr < (char)7 || *ptr > (char)127) {
                                continue;
                            }
                            if (*ptr == (char)7) {
                                //TODO: Implement Backspace
                                continue;
                            }
                            // Copy character
                            s_cmdbuf[s_cmdbuflen] = *(ptr++);
                            // Echo character
                            if (s_echo) {
                                uart_write_bytes(EX_UART_NUM, (const char*)(&s_cmdbuf[s_cmdbuflen]), 1);
                            }
                            // If line is too long, just drop it
                            if (++s_cmdbuflen >= sizeof(s_cmdbuf)) {
                                s_cmdbuflen = 0;
                                bzero(s_cmdbuf, sizeof(s_cmdbuf));
                                //TODO: Increase an error counter, may show an error or something
                            }
                        }
                        // Forward the just built up command line string
                        if (*ptr == '\r' || *ptr == '\n') {
                            // Process line
                            *(++ptr) = '\0';
                            //TODO: use uint8_t everywhere?
                            s_rx_callback_ptr((char*)s_cmdbuf, s_cmdbuflen);
                            // Start it over
                            s_cmdbuflen = 0;
                            bzero(s_cmdbuf, sizeof(s_cmdbuf));
                        }
                    } else {
                        uart_write_bytes(EX_UART_NUM, (const char*)dtmp, event.size);
                    }
                    break;
                // Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGW(TAG, "[UART EVT]: Hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(s_uart0_queue);
                    break;
                // Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGW(TAG, "[UART EVT]: Ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(s_uart0_queue);
                    break;
                // Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGW(TAG, "[UART EVT]: Uart rx break");
                    break;
                // Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGW(TAG, "[UART EVT]: Uart parity error");
                    break;
                // Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGW(TAG, "[UART EVT]: Uart frame error");
                    break;
                // Others
                default:
                    ESP_LOGW(TAG, "[UART EVT]: Uart event type %d", event.type);
                    break;
            }
        }
    }

    free(dtmp);
    vTaskDelete(NULL);

    ESP_LOGE(TAG, "uart_event_task() ended, but how??");
}
