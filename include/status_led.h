#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void status_led_init(void);
void status_led_loop(void);
void status_led_wifi_connecting(void);
void status_led_connected(void);
void status_led_error(void);
void status_led_off(void);
void status_led_flash(int count, int delayMs);

#ifdef __cplusplus
}
#endif
