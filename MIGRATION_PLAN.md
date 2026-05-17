# Plan de Migración: Arduino a ESP-IDF Nativo

Este documento detalla la hoja de ruta para migrar completamente la aplicación de seguridad ONVIF/RTSP del framework Arduino a ESP-IDF puro utilizando PlatformIO.

La migración se realizará módulo por módulo para garantizar la estabilidad y aislar los errores.

## Fase 1: Core System & Logs (Completado)
- [x] Estructura de directorios PlatformIO (`src/`, `include/`, `boards/`).
- [x] Migración del punto de entrada: `setup()` y `loop()` a `extern "C" void app_main()`.
- [x] Configuración de CMake (`CMakeLists.txt`).
- [x] Reemplazo de dependencias de PlatformIO (`framework = espidf`).

## Fase 2: Logging y Utilidades Base
- Reemplazar todas las instancias de `Serial.print()` y `Serial.println()` con `ESP_LOGI()`, `ESP_LOGE()`, etc.
- **Librería ESP-IDF:** `esp_log.h`.
- Eliminar el uso de la clase `String` de C++ (Arduino) y reemplazarla por cadenas C estándar (`char*`, `snprintf`) o `std::string`.

## Fase 3: Networking Base (WiFi)
- Migrar `wifi_manager.cpp`.
- Reemplazar `WiFi.h` por la API nativa de ESP-IDF.
- Implementar el Event Loop predeterminado para manejar eventos de conexión/desconexión.
- **Librería ESP-IDF:** `esp_wifi.h`, `esp_event.h`, `esp_netif.h`.

## Fase 4: Almacenamiento y Configuración
- Migrar `config.cpp` y la persistencia de configuraciones.
- Reemplazar `Preferences.h` y `SPIFFS` por NVS (Non-Volatile Storage).
- **Librería ESP-IDF:** `nvs_flash.h`.
- Reemplazar `ArduinoJson` por `cJSON` (incluido nativamente en ESP-IDF).

## Fase 5: Servidor Web y API
- Migrar `web_config.cpp` y `webdav_server.cpp`.
- Reemplazar clases como `WebServer` o `AsyncWebServer` por el servidor HTTP nativo.
- **Librería ESP-IDF:** `esp_http_server.h`.
- Servir los archivos estáticos desde particiones de almacenamiento nativo (VFS).

## Fase 6: Cámara y Streaming (RTSP)
- Migrar `camera_control.cpp`.
- Utilizar el driver oficial de la cámara para ESP32: `esp32-camera`. (Este componente debe instalarse a través de `idf_component_register` o Component Registry de Espressif).
- **Librería ESP-IDF:** `esp_camera.h`.
- Adaptar `rtsp_server.cpp` eliminando dependencias tipo `WiFiClient` y usando Sockets POSIX puros (`<sys/socket.h>`).

## Fase 7: ONVIF y WS-Discovery
- Migrar `onvif_server.cpp`.
- Reemplazar lógica de red (UDP multicast) por Sockets UDP POSIX nativos.
- Reescribir las respuestas SOAP para no depender de la clase `String`.

## Fase 8: Integraciones (MQTT, Telegram, GDrive)
- Migrar `mqtt_manager.cpp` usando el cliente MQTT nativo: `mqtt_client.h`.
- Migrar clientes HTTP (`HTTPClient.h`) usados en Telegram y GDrive a `esp_http_client.h`.

## Fase 9: Hardware Periférico (LEDs, Motores, Audio)
- Migrar `status_led.cpp`, `auto_flash.cpp`. Reemplazar `digitalWrite` por `gpio_set_level`.
- Migrar controladores PWM (Servos) a `ledc` nativo o `mcpwm`.
- Integrar placas personalizadas desde la carpeta `boards/` usando macros en tiempo de compilación.

## Consideraciones Generales
- **RTOS:** Usar primitivas nativas de FreeRTOS (`xTaskCreate`, `xSemaphoreTake`, `xQueueSend`) en lugar de envoltorios de Arduino.
- **Seguridad:** Habilitar Watchdog Timer (TWDT) a través de `esp_task_wdt.h`.
