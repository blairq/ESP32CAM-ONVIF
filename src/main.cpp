#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "config.h"

// Modulos migrados (Fase 2)
#include "status_led.h"
#include "auto_flash.h"
#include "serial_console.h"

// TODO: Replace with ESP-IDF native equivalents
// #include "camera_control.h"
// #include "rtsp_server.h"
// #include "onvif_server.h"
// #include "web_config.h"
// #include "sd_recorder.h"
// #include "motion_detection.h"
// #include "wifi_manager.h"
// #include "mqtt_manager.h"

static const char *TAG = "main";

extern "C" void app_main() {
    ESP_LOGI(TAG, "\n\n--- ESP32-CAM STARTING (ESP-IDF NATIVE) ---");

    status_led_init();
    auto_flash_init();

    // TODO: Re-implement application logic using ESP-IDF native APIs
    // - Initialize NVS
    // - Initialize Camera
    // - Initialize WiFi
    // - Start Web Server / RTSP / ONVIF

    while (1) {
        status_led_loop();
        auto_flash_loop();
        serial_console_loop();

        // Print debug info every 5 seconds instead of blocking the loop
        static TickType_t last_log = 0;
        if (xTaskGetTickCount() - last_log > pdMS_TO_TICKS(5000)) {
            ESP_LOGI(TAG, "Main loop running... Free Heap: %lu bytes", esp_get_free_heap_size());
            last_log = xTaskGetTickCount();
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Pequeño delay para no ahogar el procesador
    }
}
