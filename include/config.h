#pragma once
#include <Arduino.h>

#define BOARD_AI_THINKER_ESP32CAM
#define VIDEO_CODEC_MJPEG
#define FIRMWARE_VERSION "v1.4.0"
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define AP_SSID "ESP32CAM-ONVIF"
#define AP_PASSWORD "esp32cam"
#define STATIC_IP_ENABLED false
#define WEB_PORT 80
#define RTSP_PORT 554
#define ONVIF_PORT 8000
#define WEB_USER "admin"
#define WEB_PASS "esp123"
#define ENABLE_DAILY_RECORDING false
#define RECORD_SEGMENT_SEC 300
#define MAX_DISK_USAGE_PCT 90
#define ENABLE_MOTION_DETECTION false
#define FLASH_LED_ENABLED true
#define FLASH_LED_INVERT false
#define DEFAULT_AUTO_FLASH true
#define STATUS_LED_ENABLED true
#define STATUS_LED_INVERT true
#define PTZ_ENABLED false
#define DEFAULT_ONVIF_ENABLED true
#define DEVICE_MANUFACTURER "John-Varghese-EH"
#define DEVICE_MODEL "ESP32-CAM-ONVIF"
#define DEVICE_VERSION "1.3"
#define DEVICE_HARDWARE_ID "ESP32CAM-J0X"
#define GITHUB_REPO_OWNER "John-Varghese-EH"
#define GITHUB_REPO_NAME "ESP32CAM-ONVIF"
#define MQTT_ENABLED false
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 19800
#define DAYLIGHT_OFFSET 0
#define DEBUG_MODE true
#define DEBUG_LEVEL 2


// Auto-detect Bluetooth stack based on chip
#ifdef BLUETOOTH_ENABLED
#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(ESP32S3) || defined(ARDUINO_ESP32S3_DEV)
#define USE_BLUEDROID
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(ESP32C3)
#define USE_BLUEDROID
#elif defined(CONFIG_IDF_TARGET_ESP32P4) || defined(ESP32P4)
#define USE_BLUEDROID
#else
#define USE_NIMBLE
#endif
#endif

#if DEBUG_MODE
#define LOG_E(x) Serial.println("[ERROR] " + String(x))
#define LOG_I(x) if (DEBUG_LEVEL >= 2) Serial.println("[INFO] " + String(x))
#define LOG_D(x) if (DEBUG_LEVEL >= 3) Serial.println("[DEBUG] " + String(x))
#else
#define LOG_E(x)
#define LOG_I(x)
#define LOG_D(x)
#endif

// Fill empty strings for depends on to prevent errors
#ifndef MQTT_BROKER
#define MQTT_BROKER "192.168.1.100"
#endif
#ifndef MQTT_PORT
#define MQTT_PORT 1883
#endif
#ifndef MQTT_USER
#define MQTT_USER "homeassistant"
#endif
#ifndef MQTT_PASS
#define MQTT_PASS "secret"
#endif
#ifndef MQTT_TOPIC_BASE
#define MQTT_TOPIC_BASE "home/camera/esp32cam"
#endif
#ifndef HA_DISCOVERY_PREFIX
#define HA_DISCOVERY_PREFIX "homeassistant"
#endif

#ifndef STATIC_IP_ADDR
#define STATIC_IP_ADDR 192, 168, 0, 150
#endif
#ifndef STATIC_GATEWAY
#define STATIC_GATEWAY 192, 168, 0, 1
#endif
#ifndef STATIC_SUBNET
#define STATIC_SUBNET 255, 255, 255, 0
#endif
#ifndef STATIC_DNS
#define STATIC_DNS 8, 8, 8, 8
#endif

enum AudioSource {
  AUDIO_SOURCE_NONE = 0,
  AUDIO_SOURCE_HARDWARE_I2S = 1,
  AUDIO_SOURCE_BLUETOOTH_HFP = 2
};

struct AppSettings {
  bool btEnabled;
  bool btStealthMode;
  char btPresenceMac[18];
  int btPresenceTimeout;
  int btMicGain;
  int hwMicGain;
  AudioSource audioSource;
  bool mqttEnabled;
  char mqttBroker[64];
  int mqttPort;
  char mqttUser[32];
  char mqttPassword[32];
  bool telegramEnabled;
  char telegramBotToken[64];
  char telegramChatId[32];
  bool googleDriveEnabled;
  bool googleDriveMotion;
  char googleDriveScriptUrl[128];
  bool webDavEnabled;
  bool continuousRecordingEnabled;
  int continuousRecordingChunkSize;
  char ntpServer[64];
  char timeZone[64];
};

void printBanner();
void fatalError(const char *msg);
extern AppSettings appSettings;
void loadSettings();
void saveSettings();
