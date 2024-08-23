#include <math.h>
#include <driver/i2c_master.h>
#include "../i2cmaster/i2cmaster.h"
#include "../dio/tca9535.h"
#include "esp_log.h"
#include "lvgl.h"
#include "../backlight/bl.h"
#include "lvgl_demo_4.h"

#define LEDS_NUM        16
#define LED_SIZE        20
#define LED_STEP        28
#define LED_ARRAY_Y     45

#define DIO16_I2C_ADDR  0x20

static char *TAG = "DEMO4";

static lv_timer_t* tmr1 = NULL;
static lv_obj_t * leds[LEDS_NUM] = {0};
static i2c_master_bus_handle_t *bus_handle = NULL;
static i2c_master_dev_handle_t* dio16_handle = NULL;
static uint16_t dio_state = 0;

void demo_next();

static void init_leds()
{
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "DIO16  OUTPUTS");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -145);

    for (int i = 0; i < 8; i++) {
        label = lv_label_create(lv_scr_act());
        lv_label_set_text_fmt(label, "DO %d", i);
        lv_obj_set_pos(label, 30, LED_ARRAY_Y + 2 + i * LED_STEP);
 
        lv_obj_t* led  = lv_led_create(lv_scr_act());
        lv_obj_set_pos(led, 5, LED_ARRAY_Y + i * LED_STEP);
        lv_obj_set_size(led, LED_SIZE, LED_SIZE);
        lv_led_set_color(led, lv_palette_main(LV_PALETTE_GREEN));
        lv_led_off(led);
                
        leds[i] = led;
    }

    for (int i = 8; i < 16; i++) {
        label = lv_label_create(lv_scr_act());
        lv_label_set_text_fmt(label, "DO %d", i);
        lv_obj_set_pos(label, 115, LED_ARRAY_Y + 2 + (i - 8) * LED_STEP);
 
        lv_obj_t* led  = lv_led_create(lv_scr_act());
        lv_obj_set_pos(led, 90, LED_ARRAY_Y + (i - 8) * LED_STEP);
        lv_obj_set_size(led, LED_SIZE, LED_SIZE);
        lv_led_set_color(led, lv_palette_main(LV_PALETTE_GREEN));
        lv_led_off(led);
        
        leds[i] = led;
    }
}

static void dio_tick(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    dio_state <<= 1;

    if (dio_state & (1 << 15))
        dio_state &= ~1;

    else if (dio_state & (1 << 1))
        dio_state |= 1;

    if (!dio_state)
        dio_state = 1;

    tca9535_write(dio16_handle, dio_state);

    for (int i = 0; i < 16; i++) {
        if (dio_state & (1 << i))
            lv_led_on(leds[i]);
        else
            lv_led_off(leds[i]);
    }
}

static void dio_start() {
    bus_handle = i2cmaster_init(1);

    if (!i2cmaster_test(bus_handle, DIO16_I2C_ADDR))
        ESP_LOGE(TAG, "DIO16 module not found!");

    dio16_handle = tca9535_init(bus_handle, DIO16_I2C_ADDR, 0xFFFF, 0x0000);
    tmr1 = lv_timer_create(dio_tick, 30, NULL);
}

static void dio_stop() {
    dio_state = 0;
    tca9535_write(dio16_handle, dio_state);

    tca9535_deinit(dio16_handle);
    dio16_handle = NULL;

    i2cmaster_deinit(bus_handle);
    bus_handle = NULL;
}

static void next_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        lv_timer_pause(tmr1);
        lv_timer_reset(tmr1);
        dio_stop();
        demo_next();
    }
}

static void lvgl_create_next_btn() {
    lv_obj_t* btn_next = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_next, next_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn_next, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_t* label = lv_label_create(btn_next);
    lv_label_set_text(label, ">>");
    lv_obj_center(label);
}

void lvgl_demo_4() {
    lv_obj_clean(lv_scr_act());
    init_leds();
    dio_start();
    lvgl_create_next_btn();
}
