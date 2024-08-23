#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/time.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "bl.h"

static uint8_t bl_last = 0;

void bl_init(uint8_t pin) {
    ledc_timer_config_t ledcfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 100000,
        //.clk_cfg = 0,
        .deconfigure = false,
    };
    ledc_timer_config(&ledcfg);

    ledc_channel_config_t ledchancfg = {
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    ledc_channel_config(&ledchancfg);

    bl_set(0);
}

void bl_set(uint8_t value) {
    bl_last = value;
    int val = value;
    val *= 255;
    val /= 100;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, val);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

uint8_t bl_get() {
    return bl_last;
}
