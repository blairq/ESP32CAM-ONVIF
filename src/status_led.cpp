#include "config.h"
#if STATUS_LED_ENABLED

#include "status_led.h"
#include "board_config.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Patterns enumerator
enum LedPattern {
    LED_OFF,
    LED_SOLID,
    LED_SLOW_BLINK, // WiFi Connecting
    LED_FAST_BLINK  // Error
};

static LedPattern _current_pattern = LED_OFF;
static TickType_t _last_toggle = 0;
static bool _led_state = false; // Internal state, will be inverted if needed

static void write_led(bool state) {
    // Apply inversion
    bool pinState = STATUS_LED_INVERT ? !state : state;
    gpio_set_level((gpio_num_t)STATUS_LED_PIN, pinState ? 1 : 0);
}

void status_led_init() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << STATUS_LED_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    status_led_off();
}

void status_led_loop() {
    TickType_t now = xTaskGetTickCount();
    
    switch (_current_pattern) {
        case LED_OFF:
            write_led(false);
            break;
            
        case LED_SOLID:
            write_led(true);
            break;
            
        case LED_SLOW_BLINK:
            if ((now - _last_toggle) >= pdMS_TO_TICKS(500)) { // 500ms on/off
                _last_toggle = now;
                _led_state = !_led_state;
                write_led(_led_state);
            }
            break;
            
        case LED_FAST_BLINK:
            if ((now - _last_toggle) >= pdMS_TO_TICKS(100)) { // 100ms on/off
                _last_toggle = now;
                _led_state = !_led_state;
                write_led(_led_state);
            }
            break;
    }
}

void status_led_wifi_connecting() {
    _current_pattern = LED_SLOW_BLINK;
}

void status_led_connected() {
    _current_pattern = LED_OFF; // User requested OFF when connected
}

void status_led_error() {
    _current_pattern = LED_FAST_BLINK;
}

void status_led_off() {
    _current_pattern = LED_OFF;
    write_led(false);
}

// Blocking flash helper for startup/events
void status_led_flash(int count, int delayMs) {
    // Ensure we start from known state
    write_led(false);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    for (int i = 0; i < count; i++) {
        write_led(true);
        vTaskDelay(pdMS_TO_TICKS(delayMs));
        write_led(false);
        if (i < count - 1) vTaskDelay(pdMS_TO_TICKS(delayMs));
    }
}

#else
// Stub implementations when status LED is disabled
void status_led_init() {}
void status_led_loop() {}
void status_led_wifi_connecting() {}
void status_led_connected() {}
void status_led_error() {}
void status_led_off() {}
void status_led_flash(int count, int delayMs) {}
#endif
