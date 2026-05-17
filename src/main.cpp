#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "config.h"

// Modulos migrados
#include "status_led.h"
#include "auto_flash.h"
#include "serial_console.h"
#include "wifi_manager.h" // Fase 3

static const char *TAG = "main";

extern "C" void app_main() {
    ESP_LOGI(TAG, "\n\n--- ESP32-CAM STARTING (ESP-IDF NATIVE) ---");

    // Inicializar NVS (Requisito para WiFi y almacenamiento de ESP-IDF)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    status_led_init();
    auto_flash_init();

    // Inicializar y conectar WiFi
    wifi_manager_begin();

    while (1) {
        status_led_loop();
        auto_flash_loop();
        serial_console_loop();
        wifi_manager_loop();

        static TickType_t last_log = 0;
        if (xTaskGetTickCount() - last_log > pdMS_TO_TICKS(5000)) {
            ESP_LOGI(TAG, "Main loop running... Free Heap: %lu bytes", esp_get_free_heap_size());
            last_log = xTaskGetTickCount();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
