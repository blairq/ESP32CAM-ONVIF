#include "serial_console.h"
#include "config.h"
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Temporarily commented out until other modules are migrated
// #include "wifi_manager.h"
// #include "camera_control.h"

// static const char *TAG = "console";

/* static void process_command(const char *cmd) {

    if (strlen(cmd) == 0) return;

    ESP_LOGI(TAG, "> %s", cmd);

    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
        ESP_LOGI(TAG, "--- Serial Console Help ---");
        ESP_LOGI(TAG, "status       : Show system status settings");
        ESP_LOGI(TAG, "reboot       : Restart device");
    }
    else if (strcmp(cmd, "status") == 0) {
        ESP_LOGI(TAG, "--- System Status ---");
        ESP_LOGI(TAG, "Uptime: %lu ms", (unsigned long)pdTICKS_TO_MS(xTaskGetTickCount()));
        ESP_LOGI(TAG, "Heap: %lu bytes", esp_get_free_heap_size());
    }
    else if (strcmp(cmd, "reboot") == 0) {
        ESP_LOGI(TAG, "Rebooting...");
        vTaskDelay(pdMS_TO_TICKS(100));
        esp_restart();
    }
    else {
        ESP_LOGW(TAG, "Unknown command. Type 'help'.");
    }
}

// In ESP-IDF, simple stdin reading can be done with fgetc/getchar
// However, non-blocking reading requires specific handling (like VFS/select)
// For now, we implement a simple stub that can be extended later using esp_console component
*/

void serial_console_loop() {
    // Stub: To be replaced with esp_console or non-blocking UART read
}
