/**************************************************************/
/* SSL web&socket server by HA3FLT, HA2OS @ 2022-2023         */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "webserver.h"
#include "main.h"

#include "protocol_examples_common.h"

#include <esp_event.h>
#include <esp_netif.h>
#include <esp_wifi.h>

#include "lwip/sockets.h"

/*********************/
/* Local definitions */
/*********************/
#define KEEP_ALIVE          1

#if KEEP_ALIVE
 #include "keep_alive.h"
#endif

typedef struct {
    httpd_handle_t hd;
    int fd;
} __attribute__((__packed__)) Async_Resp_Arg_t;

static const char *TAG = "SRSw-webserver";
static const size_t max_clients = 21;               // It is <= CONFIG_LWIP_MAX_ACTIVE_TCP - 3
static const size_t keep_alive_max_clients = 12;    // Memory limited even if it is less than max_clients

static httpd_handle_t s_server = NULL;

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/************************/
/* Initialize webserver */
/************************/
esp_err_t websvr_init(void)
{
    ESP_LOGI(TAG, "websvr_init()");

    // Register event handlers to start server when Wi-Fi or Ethernet is connected, and stop server when disconnection happens.
#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &s_server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &s_server));
#endif // CONFIG_EXAMPLE_CONNECT_WIFI
#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &connect_handler, &s_server));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &disconnect_handler, &s_server));
#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET

    // This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    ESP_ERROR_CHECK(example_connect());

    return ESP_OK;
}

/*************************/
/* webserver module code */
/*************************/
httpd_handle_t* websvr_get_server(void)
{
    return &s_server;
}

/**************/
/* WS handler */
/**************/
static esp_err_t ws_handler(httpd_req_t *req)
{
    int sockfd = httpd_req_to_sockfd(req);
    ESP_LOGI(TAG, "ws_handler: fd = %d", sockfd);

    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }

    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

    // First receive the full ws message
    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
    }

    if (ws_pkt.type == HTTPD_WS_TYPE_PONG) {
        // If it was a PONG, update the keep-alive
        ESP_LOGD(TAG, "Received PONG message");
        if (buf != NULL) {  // For the safety...
            free(buf);
        }
#if KEEP_ALIVE
        return wss_keep_alive_client_is_active(httpd_get_global_user_ctx(req->handle), httpd_req_to_sockfd(req));
#else
        return ret;
#endif
    } else if (ws_pkt.type == HTTPD_WS_TYPE_TEXT) {
        // If it was a TEXT message, just echo it back
        ESP_LOGI(TAG, "Received packet with message: %s", ws_pkt.payload);
        ret = httpd_ws_send_frame(req, &ws_pkt);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
        }
        ESP_LOGI(TAG, "ws_handler: httpd_handle_t=%p, sockfd=%d, client_info:%d", req->handle,
            httpd_req_to_sockfd(req), httpd_ws_get_fd_info(req->handle, httpd_req_to_sockfd(req)));
        if (buf != NULL) {  // For the safety...
            free(buf);
        }
        return ret;
    }

    if (buf != NULL) {  // For the safety...
        free(buf);
    }
    return ESP_OK;
}

esp_err_t wss_open_fd(httpd_handle_t hd, int sockfd)
{
    ESP_LOGI(TAG, "New client connected %d", sockfd);
#if KEEP_ALIVE
    wss_keep_alive_t h = httpd_get_global_user_ctx(hd);
    ESP_LOGI(TAG, "keep_alive handle = %d", (int)&h);
    return wss_keep_alive_add_client(h, sockfd);
#else
    return ESP_OK;
#endif
}

void wss_close_fd(httpd_handle_t hd, int sockfd)
{
    ESP_LOGI(TAG, "Client disconnected %d", sockfd);
#if KEEP_ALIVE
    wss_keep_alive_t h = httpd_get_global_user_ctx(hd);
    ESP_LOGI(TAG, "keep_alive handle = %d", (int)&h);
    wss_keep_alive_remove_client(h, sockfd);
#else
    close(sockfd);
#endif
    /*struct linger so_linger;
    so_linger.l_onoff = true;
    so_linger.l_linger = 0;
    setsocketopt(sockfd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));*/
//    httpd_sess_close_lru(hd);
    // See: void httpd_sess_delete(struct httpd_data *hd, struct sock_db *session)
}

static const httpd_uri_t ws = {
    .uri        = "/ws",
    .method     = HTTP_GET,
    .handler    = ws_handler,
    .user_ctx   = NULL,
    .is_websocket = true,
    .handle_ws_control_frames = true,
    .supported_subprotocol = NULL
};

static void send_hello(void *arg)
{
    static uint16_t hello_counter = 0;
    static char data[32];
    sprintf(data, "Hello client, %u.", ++hello_counter);
    Async_Resp_Arg_t *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

#if KEEP_ALIVE
static void send_ping(void *arg)
{
    Async_Resp_Arg_t *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = NULL;
    ws_pkt.len = 0;
    ws_pkt.type = HTTPD_WS_TYPE_PING;

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

bool client_not_alive_cb(wss_keep_alive_t h, int fd)
{
    ESP_LOGE(TAG, "Client not alive, closing fd %d", fd);
    httpd_sess_trigger_close(wss_keep_alive_get_user_ctx(h), fd);
    return true;
}

bool check_client_alive_cb(wss_keep_alive_t h, int fd)
{
    ESP_LOGD(TAG, "Checking if client (fd=%d) is alive", fd);
    Async_Resp_Arg_t *resp_arg = malloc(sizeof(Async_Resp_Arg_t));
    resp_arg->hd = wss_keep_alive_get_user_ctx(h);
    resp_arg->fd = fd;

    if (httpd_queue_work(resp_arg->hd, send_ping, resp_arg) == ESP_OK) {
        return true;
    }
    return false;
}
#endif

/*********************/
/* HTTP GET handlers */
/*********************/
static esp_err_t root_get_handler(httpd_req_t *req)
{
    extern const unsigned char root_html_file_start[] asm("_binary_root_html_start");
    extern const unsigned char root_html_file_end[] asm("_binary_root_html_end");
    const char* root_html_file = (char*)root_html_file_start;
    const unsigned int root_html_file_len = root_html_file_end - root_html_file_start - 1;

    httpd_resp_set_type(req, "text/html");
//httpd_resp_set_hdr(req, "Connection", "close");
//httpd_sess_trigger_close(req->handle, httpd_req_to_sockfd(req));
    httpd_resp_send(req, root_html_file, root_html_file_len);
close(httpd_req_to_sockfd(req));

    return ESP_OK;
}
static esp_err_t remote_switch_css_get_handler(httpd_req_t *req)
{
    extern const unsigned char remote_switch_css_file_start[] asm("_binary_remote_switch_css_start");
    extern const unsigned char remote_switch_css_file_end[] asm("_binary_remote_switch_css_end");
    const char* remote_switch_css_file = (char*)remote_switch_css_file_start;
    const unsigned int remote_switch_css_file_len = remote_switch_css_file_end - remote_switch_css_file_start - 1;

    httpd_resp_set_type(req, "text/css");
//httpd_resp_set_hdr(req, "Connection", "close");
//httpd_sess_trigger_close(req->handle, httpd_req_to_sockfd(req));
    httpd_resp_send(req, remote_switch_css_file, remote_switch_css_file_len);
close(httpd_req_to_sockfd(req));

    return ESP_OK;
}
static esp_err_t remote_switch_js_get_handler(httpd_req_t *req)
{
    extern const unsigned char remote_switch_js_file_start[] asm("_binary_remote_switch_js_start");
    extern const unsigned char remote_switch_js_file_end[] asm("_binary_remote_switch_js_end");
    const char* remote_switch_js_file = (char*)remote_switch_js_file_start;
    const unsigned int remote_switch_js_file_len = remote_switch_js_file_end - remote_switch_js_file_start - 1;

    httpd_resp_set_type(req, "text/javascript");
//httpd_resp_set_hdr(req, "Connection", "close");
//httpd_sess_trigger_close(req->handle, httpd_req_to_sockfd(req));
    httpd_resp_send(req, remote_switch_js_file, remote_switch_js_file_len);
close(httpd_req_to_sockfd(req));

    return ESP_OK;
}

/* WEB content */
static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler
};
static const httpd_uri_t remote_switch_css = {
    .uri       = "/remote_switch.css",
    .method    = HTTP_GET,
    .handler   = remote_switch_css_get_handler
};
static const httpd_uri_t remote_switch_js = {
    .uri       = "/remote_switch.js",
    .method    = HTTP_GET,
    .handler   = remote_switch_js_get_handler
};

/*********************************/
/* Web/ws server with keep alive */
/*********************************/
static httpd_handle_t start_webserver(void)
{
#if KEEP_ALIVE
    // Prepare keep-alive engine
    wss_keep_alive_config_t keep_alive_config = KEEP_ALIVE_CONFIG_DEFAULT();
    keep_alive_config.max_clients = keep_alive_max_clients;
    keep_alive_config.client_not_alive_cb = client_not_alive_cb;
    keep_alive_config.check_client_alive_cb = check_client_alive_cb;
    wss_keep_alive_t keep_alive = wss_keep_alive_start(&keep_alive_config);
#endif

    // Start the httpd server
    httpd_handle_t server = NULL;
    ESP_LOGI(TAG, "Starting server...");

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();
    conf.httpd.max_open_sockets = max_clients;
    //conf.httpd.max_uri_handlers = 8;
    conf.httpd.lru_purge_enable = true;
#if KEEP_ALIVE
    conf.httpd.global_user_ctx = keep_alive;
#endif
    conf.httpd.open_fn = wss_open_fd;
    conf.httpd.close_fn = wss_close_fd;

    extern const unsigned char servercert_pem_start[] asm("_binary_servercert_pem_start");
    extern const unsigned char servercert_pem_end[]   asm("_binary_servercert_pem_end");
    conf.servercert = servercert_pem_start;
    conf.servercert_len = servercert_pem_end - servercert_pem_start;

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers (WS, Root, etc.)");
    httpd_register_uri_handler(server, &ws);
#if KEEP_ALIVE
    wss_keep_alive_set_user_ctx(keep_alive, server);
#endif
    // Web content pages
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &remote_switch_css);
    httpd_register_uri_handler(server, &remote_switch_js);
    //httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
    return server;
}

static void stop_webserver(httpd_handle_t server)
{
#if KEEP_ALIVE
    // Stop keep alive thread
    wss_keep_alive_stop(httpd_get_global_user_ctx(server));
#endif
    // Stop the httpd server
    httpd_ssl_stop(server);
}

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        *server = start_webserver();
    }
    main_set_firmware_status(eGFirmwareStatusNetworkConnected);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        stop_webserver(*server);
        *server = NULL;
    }
    main_set_firmware_status(eGFirmwareStatusNetworkDisconnected);
}

// Get all clients and send async message
void websvr_wss_server_send_messages(httpd_handle_t* server)
{
    bool send_messages = true;

    // Send async message to all connected clients that use websocket protocol every 10 seconds
    while (send_messages) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (!*server) { // httpd might not have been created by now
            continue;
        }
        size_t clients = max_clients;
        int client_fds[max_clients];
        if (httpd_get_client_list(*server, &clients, client_fds) == ESP_OK) {
            for (size_t i = 0; i < clients; i++) {
                int sock = client_fds[i];
                if (httpd_ws_get_fd_info(*server, sock) == HTTPD_WS_CLIENT_WEBSOCKET) {
                    ESP_LOGI(TAG, "Active client (fd=%d) -> sending async message", sock);
                    Async_Resp_Arg_t *resp_arg = malloc(sizeof(Async_Resp_Arg_t));
                    resp_arg->hd = *server;
                    resp_arg->fd = sock;
                    if (httpd_queue_work(resp_arg->hd, send_hello, resp_arg) != ESP_OK) {
                        ESP_LOGE(TAG, "httpd_queue_work failed!");
                        send_messages = false;
                        break;
                    }
                }
            }
        } else {
            ESP_LOGE(TAG, "httpd_get_client_list failed!");
            return;
        }
    }
}
