#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

// TODO: Replace with ESP-IDF native equivalents
// #include "camera_control.h"
// #include "rtsp_server.h"
// #include "onvif_server.h"
// #include "web_config.h"
// #include "sd_recorder.h"
// #include "motion_detection.h"
// #include "config.h"
// #include "wifi_manager.h"
// #include "serial_console.h"
// #include "auto_flash.h"
// #include "status_led.h"
// #include "mqtt_manager.h"

static const char *TAG = "main";

extern "C" void app_main() {
    ESP_LOGI(TAG, "\n\n--- ESP32-CAM STARTING (ESP-IDF NATIVE) ---");

    // TODO: Re-implement application logic using ESP-IDF native APIs
    // - Initialize NVS
    // - Initialize Camera
    // - Initialize WiFi
    // - Start Web Server / RTSP / ONVIF

    while (1) {
        ESP_LOGI(TAG, "Main loop running... Free Heap: %lu bytes", esp_get_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
