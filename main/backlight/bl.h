#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

void bl_init(uint8_t pin);
void bl_set(uint8_t value);
uint8_t bl_get();

#ifdef __cplusplus
}
#endif
