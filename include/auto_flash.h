#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void auto_flash_init(void);
void auto_flash_loop(void);
void auto_flash_set_enabled(bool enabled);
bool auto_flash_is_enabled(void);

#ifdef __cplusplus
}
#endif
