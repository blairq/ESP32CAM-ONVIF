#include "wifi_manager.h"
#include "config.h"
#include "status_led.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

static const char *TAG = "wifi_mgr";

// Event group for tracking connection status
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static bool _ap_mode = false;
static char _current_ssid[MAX_SSID_LEN] = {0};
static esp_ip4_addr_t _current_ip;

static esp_netif_t *sta_netif = NULL;
static esp_netif_t *ap_netif = NULL;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        status_led_error();
        EventBits_t bits = xEventGroupGetBits(wifi_event_group);
        if (!(bits & WIFI_CONNECTED_BIT)) {
            // Initial connection attempt failed
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        } else {
            // Disconnected after being connected, try to reconnect
            xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
            ESP_LOGW(TAG, "Disconnected from AP. Reconnecting...");
            esp_wifi_connect();
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        _current_ip = event->ip_info.ip;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&_current_ip));
        status_led_connected();
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        xEventGroupClearBits(wifi_event_group, WIFI_FAIL_BIT);
    }
}

static bool wifi_initialized = false;

static void ensure_wifi_init() {
    if (!wifi_initialized) {
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &event_handler,
                                                            NULL,
                                                            &instance_any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &event_handler,
                                                            NULL,
                                                            &instance_got_ip));
        wifi_initialized = true;
    }
}

static bool wifi_init_sta(const char *ssid, const char *password) {
    if (sta_netif == NULL) {
        sta_netif = esp_netif_create_default_wifi_sta();
    }

    ensure_wifi_init();

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    if (password != NULL) {
        strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    }
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished. Waiting for connection...");
    status_led_wifi_connecting();

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            pdMS_TO_TICKS(15000));

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to SSID:%s", ssid);
        strncpy(_current_ssid, ssid, sizeof(_current_ssid));
        return true;
    } else {
        ESP_LOGE(TAG, "Failed to connect to SSID:%s", ssid);
        esp_wifi_stop(); // Stop before trying fallback
        return false;
    }
}

void wifi_manager_start_ap(void) {
    _ap_mode = true;
    if (ap_netif == NULL) {
        ap_netif = esp_netif_create_default_wifi_ap();
    }

    ensure_wifi_init();

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.ap.ssid, AP_SSID, sizeof(wifi_config.ap.ssid));
    wifi_config.ap.ssid_len = strlen(AP_SSID);
    strncpy((char *)wifi_config.ap.password, AP_PASSWORD, sizeof(wifi_config.ap.password));
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(ap_netif, &ip_info);
    _current_ip = ip_info.ip;
    strncpy(_current_ssid, AP_SSID, sizeof(_current_ssid));

    ESP_LOGI(TAG, "AP Mode started. SSID: %s, IP: " IPSTR, _current_ssid, IP2STR(&_current_ip));
    status_led_wifi_connecting();
}


bool wifi_manager_save_credentials(const char *ssid, const char *password) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("wifi_creds", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return false;

    nvs_set_str(my_handle, "ssid", ssid);
    nvs_set_str(my_handle, "pass", password);
    nvs_commit(my_handle);
    nvs_close(my_handle);
    ESP_LOGI(TAG, "WiFi credentials saved to NVS");
    return true;
}

bool wifi_manager_load_credentials(char *ssid, char *password) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("wifi_creds", NVS_READONLY, &my_handle);
    if (err != ESP_OK) return false;

    size_t required_size = MAX_SSID_LEN;
    err = nvs_get_str(my_handle, "ssid", ssid, &required_size);
    if (err != ESP_OK) {
        nvs_close(my_handle);
        return false;
    }

    required_size = MAX_PASS_LEN;
    err = nvs_get_str(my_handle, "pass", password, &required_size);
    nvs_close(my_handle);
    return err == ESP_OK;
}

bool wifi_manager_begin(void) {
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    char saved_ssid[MAX_SSID_LEN] = {0};
    char saved_pass[MAX_PASS_LEN] = {0};

    // 1. Try NVS credentials
    if (wifi_manager_load_credentials(saved_ssid, saved_pass)) {
        ESP_LOGI(TAG, "Trying saved credentials from NVS...");
        if (wifi_init_sta(saved_ssid, saved_pass)) {
            return true;
        }
    }

    // 2. Try hardcoded config.h credentials
    #ifdef WIFI_SSID
    if (strlen(WIFI_SSID) > 0 && strcmp(WIFI_SSID, "YOUR_WIFI_SSID") != 0) {
        ESP_LOGI(TAG, "Trying config.h credentials...");
        if (wifi_init_sta(WIFI_SSID, WIFI_PASSWORD)) {
            return true;
        }
    }
    #endif

    // 3. Fallback to AP Mode
    ESP_LOGW(TAG, "Failed to connect to STA. Falling back to AP mode.");
    wifi_manager_start_ap();
    return false;
}

void wifi_manager_loop(void) {
    if (_ap_mode) return;

    // Watchdog and timeout logic could go here
    // esp_wifi handles basic auto-reconnect internally due to event_handler
}

bool wifi_manager_is_ap_mode(void) {
    return _ap_mode;
}

esp_ip4_addr_t wifi_manager_get_local_ip(void) {
    return _current_ip;
}

const char* wifi_manager_get_ssid(void) {
    return _current_ssid;
}

bool wifi_manager_is_connected(void) {
    if (_ap_mode) return true;
    EventBits_t bits = xEventGroupGetBits(wifi_event_group);
    return (bits & WIFI_CONNECTED_BIT) != 0;
}
