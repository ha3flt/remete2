/**************************************************************/
/* User manager by HA3FLT, HA2OS @ 2022-2023                  */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/
#include "user_manager.h"
#include "serial_io.h"
#include "main.h"

#include "string.h"

/*********************/
/* Local definitions */
/*********************/
static const char *TAG = "SRSw-user_manager";

// Session database, only in memory
static Session_Data_t s_SessionData[USRMGR_MAX_SESSIONS];

/***************************/
/* Initialize user_manager */
/***************************/
esp_err_t usrmgr_init(void)
{
    ESP_LOGI(TAG, "usrmgr_init()");

    usrmgr_end_all_sessions();
    return ESP_OK;
}

//#if SRSW_DEBUG
void usrmgr_dump_sessions(void)
{
    char buf[128];
    sprintf(buf, "*** Session Dump ***\r\n");
    UWRITE(buf);

    for (int idx = 0; idx < USRMGR_MAX_SESSIONS; idx++) {
        sprintf(buf, "Idx: %02d, SessionId: %d, UserName: \"%s\", SessionTimer: %u\r\n",
            idx,
            s_SessionData[idx].SessionId,
            s_SessionData[idx].UserName,
            s_SessionData[idx].SessionTimer);
        UWRITE(buf);
    }

    sprintf(buf, "********************\r\n");
    UWRITE(buf);
}
//#endif

/****************************/
/* user_manager module code */
/****************************/
esp_err_t usrmgr_get_user_info(char* user_name, char* buffer)
{
    ESP_LOGI(TAG, "usrmgr_get_user_info() for user \"%s\"", user_name);

    return ESP_OK;
}

esp_err_t usrmgr_set_user_info(char* user_name)
{
    ESP_LOGI(TAG, "usrmgr_set_user_info() for user \"%s\"", user_name);

    return ESP_OK;
}

esp_err_t usrmgr_add_user(char* user_name)
{
    ESP_LOGI(TAG, "usrmgr_add_user() \"%s\"", user_name);

    return ESP_OK;
}

esp_err_t usrmgr_remove_user(char* user_name)
{
    ESP_LOGI(TAG, "usrmgr_remove_user() \"%s\"", user_name);

    return ESP_OK;
}

esp_err_t usrmgr_set_password(char* user_name, char* password)
{
    ESP_LOGI(TAG, "usrmgr_set_password() for user \"%s\", ********", user_name);

    return ESP_OK;
}

/***********************/
/* Sessions and logins */
/***********************/
static esp_err_t get_session_ptr_by_id(uint16_t session_id, Session_Data_t** slot_address)
{
    if (slot_address == NULL) {
        ESP_LOGE(TAG, "get_session_ptr_by_id(): Invalid 'slot_address' parameter");
        return ESP_FAIL;
    }

    *slot_address = &s_SessionData[session_id - 1];
    return ESP_OK;
}

static esp_err_t get_session_ptr_by_name(char* user_name, Session_Data_t** slot_address)
{
    if (user_name == NULL) {
        ESP_LOGE(TAG, "get_session_ptr_by_name(): Invalid 'user_name' parameter");
        return ESP_FAIL;
    }
    if (strlen(user_name) < USRMGR_MIN_USERNAME_LEN || strlen(user_name) >= USRMGR_MAX_USERNAME_LEN) {
        ESP_LOGE(TAG, "get_session_ptr_by_name(): User name \"%s\" length is invalid (not in %d..%d)",
            user_name, USRMGR_MIN_USERNAME_LEN, USRMGR_MAX_USERNAME_LEN);
        return ESP_FAIL;
    }
    if (slot_address == NULL) {
        ESP_LOGE(TAG, "get_session_ptr_by_name(): Invalid 'slot_address' parameter");
        return ESP_FAIL;
    }

    for (int idx = 0; idx < USRMGR_MAX_SESSIONS; idx++) {
        if (s_SessionData[idx].SessionId > 0 && strcmp(user_name, s_SessionData[idx].UserName) == 0) {
            *slot_address = &s_SessionData[idx];
            return ESP_OK;
        }
    }

    return ESP_FAIL;
}

static esp_err_t find_available_session_slot(uint16_t* session_id, Session_Data_t** slot_address)
{
    if (session_id == NULL) {
        ESP_LOGE(TAG, "find_available_session_slot(): Invalid 'id' parameter");
        return ESP_FAIL;
    }
    if (slot_address == NULL) {
        ESP_LOGE(TAG, "find_available_session_slot(): Invalid 'slot_address' parameter");
        return ESP_FAIL;
    }

    for (int idx = 0; idx < USRMGR_MAX_SESSIONS; idx++) {
        if (s_SessionData[idx].SessionId == 0) {
            *session_id = idx + 1;
            *slot_address = &s_SessionData[idx];
            ESP_LOGD(TAG, "Found an empty slot at position %d", idx);
            return ESP_OK;
        }
    }

    ESP_LOGW(TAG, "No session slot available");
    return ESP_FAIL;
}

static esp_err_t start_session(char* user_name, Session_Data_t** session_ptr)
{
    if (user_name == NULL) {
        ESP_LOGE(TAG, "start_session(): Invalid parameter user_name");
        return ESP_FAIL;
    }
    if (strlen(user_name) < USRMGR_MIN_USERNAME_LEN || strlen(user_name) >= USRMGR_MAX_USERNAME_LEN) {
        ESP_LOGE(TAG, "start_session(): User name \"%s\" length is invalid (not in %d..%d)",
            user_name, USRMGR_MIN_USERNAME_LEN, USRMGR_MAX_USERNAME_LEN);
        return ESP_FAIL;
    }
    if (session_ptr == NULL) {
        ESP_LOGE(TAG, "start_session(): Invalid parameter session_ptr");
        return ESP_FAIL;
    }

    esp_err_t esp_err;
    uint16_t session_id;
    esp_err = find_available_session_slot(&session_id, session_ptr);
    if (esp_err != ESP_OK) {
        return esp_err;
    }
    // Initialize session
    strcpy((*session_ptr)->UserName, user_name);
    (*session_ptr)->SessionTimer = USRMGR_SESSION_TIMEOUT;
    (*session_ptr)->SessionId = session_id;

    ESP_LOGD(TAG, "Session started for user name \"%s\"", user_name);
    return ESP_OK;
}

static esp_err_t end_session_by_id(uint16_t session_id)
{
    ESP_LOGD(TAG, "end_session() called for session id: %d...", session_id);

    esp_err_t esp_err;
    Session_Data_t* session_ptr;
    esp_err = get_session_ptr_by_id(session_id, &session_ptr);
    if (esp_err != ESP_OK) {
        ESP_LOGW(TAG, "Cannot find session %d", session_id);
        return esp_err;
    }
    ESP_LOGI(TAG, "Session is ending for user name \"%s\"...", session_ptr->UserName);
    memset(session_ptr, 0, sizeof(Session_Data_t));

    ESP_LOGD(TAG, "Session ended successfully");
    return esp_err;
}

static esp_err_t end_session_by_user_name(char* user_name)
{
    esp_err_t esp_err;
    Session_Data_t* session_ptr;
    esp_err = get_session_ptr_by_name(user_name, &session_ptr);
    if (esp_err != ESP_OK) {
        return esp_err;
    }
    return end_session_by_id(session_ptr->SessionId);
}

void usrmgr_end_all_sessions(void)
{
    ESP_LOGD(TAG, "usrmgr_end_all_sessions()");
//#if SRSW_DEBUG
    usrmgr_dump_sessions();
//#endif

    //TODO: Use a provided callback for tasks those are needed for timed out sessions?
    memset(s_SessionData, 0, sizeof(s_SessionData));

//#if SRSW_DEBUG
    usrmgr_dump_sessions();
//#endif
    ESP_LOGI(TAG, "All sessions ended");
}

esp_err_t usrmgr_is_session_valid(uint16_t session_id)
{
    ESP_LOGD(TAG, "Checking for session validity for id: %d...", session_id);
//#if SRSW_DEBUG
    usrmgr_dump_sessions();
//#endif

    esp_err_t esp_err;
    Session_Data_t* session_ptr;
    esp_err = get_session_ptr_by_id(session_id, &session_ptr);
    if (esp_err != ESP_OK) {
        return ESP_FAIL;
    }

    //TODO: Check for vaildity
    //session_ptr->SessionTimer, USRMGR_SESSION_TIMEOUT;

    ESP_LOGI(TAG, "Session started for user name \"%s\"", session_ptr->UserName);
    return ESP_OK;
}

esp_err_t usrmgr_remove_timed_out_sessions(void)
{
    ESP_LOGD(TAG, "usrmgr_remove_timed_out_sessions()");

    esp_err_t esp_err = ESP_OK;
    for (int idx = 0; idx < USRMGR_MAX_SESSIONS; idx++) {
        if (s_SessionData[idx].SessionId != 0) {
            //TODO: Be careful if the timer is a timer tick and its variable overflows to zero
            /*if (time > s_SessionData[idx].SessionTimer + USRMGR_SESSION_TIMEOUT) {
                ESP_LOGD(TAG, "Found a timed out session, id: %u at position %u", idx);
            }*/
//#if SRSW_DEBUG
            usrmgr_dump_sessions();
//#endif
        }
    }

    //TODO: usermgr_get_next_timed_out_session() calls until there is no more?

    //TODO: ESP_FAIL if there is at least one session that has been timed out
    return esp_err;
}

esp_err_t usrmgr_login(char* user_name, char* password)
{
    if (user_name == NULL) {
        ESP_LOGE(TAG, "usrmgr_login(): Invalid parameter 'user_name'");
        return ESP_FAIL;
    }
    if (strlen(user_name) < USRMGR_MIN_USERNAME_LEN || strlen(user_name) >= USRMGR_MAX_USERNAME_LEN) {
        ESP_LOGE(TAG, "usrmgr_login(): User name \"%s\" length is invalid (not in %d..%d)",
            user_name, USRMGR_MIN_USERNAME_LEN, USRMGR_MAX_USERNAME_LEN);
        return ESP_FAIL;
    }
    if (password == NULL) {
        ESP_LOGE(TAG, "usrmgr_login(): Invalid parameter 'password'");
        return ESP_FAIL;
    }
    if (strlen(password) >= USRMGR_MAX_PASSWORD_LEN) {
        ESP_LOGE(TAG, "usrmgr_login(): Password is too long");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "usrmgr_login(): \"%s\", ********", user_name);
//#if SRSW_DEBUG
    usrmgr_dump_sessions();
//#endif

    esp_err_t esp_err;
    Session_Data_t* session_ptr;

    esp_err = get_session_ptr_by_name(user_name, &session_ptr);
    if (esp_err == ESP_OK) {
        ESP_LOGI(TAG, "User \"%s\" is already logged in", user_name);
        return ESP_OK;
    }

    //TODO: Check password
    if (strcmp(password, "password") != 0) {
        ESP_LOGE(TAG, "usrmgr_login(): Access denied");
        return ESP_FAIL;
    }

    esp_err = start_session(user_name, &session_ptr);
    if (esp_err != ESP_OK) {
        return esp_err;
    }

    ESP_LOGI(TAG, "User \"%s\" is logged in", user_name);
//#if SRSW_DEBUG
    usrmgr_dump_sessions();
//#endif
    return esp_err;
}

esp_err_t usrmgr_logout_by_id(uint16_t session_id)
{
    ESP_LOGD(TAG, "usrmgr_logout_by_id(): %d", session_id);
//#if SRSW_DEBUG
    usrmgr_dump_sessions();
//#endif

    esp_err_t esp_err = end_session_by_id(session_id);

    ESP_LOGI(TAG, "User is logged out");
//#if SRSW_DEBUG
    usrmgr_dump_sessions();
//#endif
    return esp_err;
}

esp_err_t usrmgr_logout(char* user_name)
{
    if (user_name == NULL) {
        ESP_LOGE(TAG, "usrmgr_logout(): Invalid parameter user_name");
        return ESP_FAIL;
    }
    if (strlen(user_name) < USRMGR_MIN_USERNAME_LEN || strlen(user_name) >= USRMGR_MAX_USERNAME_LEN) {
        ESP_LOGE(TAG, "usrmgr_logout(): User name \"%s\" length is invalid (not in %d..%d)",
            user_name, USRMGR_MIN_USERNAME_LEN, USRMGR_MAX_USERNAME_LEN);
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "usrmgr_logout(): \"%s\"", user_name);
//#if SRSW_DEBUG
    usrmgr_dump_sessions();
//#endif

    esp_err_t esp_err = end_session_by_user_name(user_name);
    if (esp_err != ESP_OK) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "User \"%s\" is logged out", user_name);
//#if SRSW_DEBUG
    usrmgr_dump_sessions();
//#endif
    return esp_err;
}
