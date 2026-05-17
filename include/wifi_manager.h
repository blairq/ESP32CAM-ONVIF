#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_netif.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64

// WiFi network info structure for scanning
typedef struct {
    char ssid[MAX_SSID_LEN];
    int8_t rssi;
    wifi_auth_mode_t authmode;
} wifi_network_info_t;

// Connect using stored credentials or fall back to AP mode
bool wifi_manager_begin(void);

// Handle reconnection and monitoring
void wifi_manager_loop(void);

// AP mode functions
void wifi_manager_start_ap(void);
bool wifi_manager_is_ap_mode(void);

// Credential management
bool wifi_manager_save_credentials(const char *ssid, const char *password);
bool wifi_manager_load_credentials(char *ssid, char *password);

// Network Info
esp_ip4_addr_t wifi_manager_get_local_ip(void);
const char* wifi_manager_get_ssid(void);
bool wifi_manager_is_connected(void);

#ifdef __cplusplus
}
#endif
