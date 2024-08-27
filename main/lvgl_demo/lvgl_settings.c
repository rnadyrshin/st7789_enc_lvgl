#include "esp_log.h"
#include "lvgl.h"
#include "driver/gpio.h"
#include "led_strip.h"
#include "../backlight/bl.h"
#include "lvgl_settings.h"

#define BLINK_GPIO      0

static char *TAG = "SETTINGS";

static uint8_t bl_old = 0;
static lv_timer_t* tmr = NULL;
static led_strip_handle_t led_strip;

static lv_obj_t *btn;
static lv_disp_rot_t rotation = LV_DISP_ROT_NONE;
static lv_obj_t* bl_label;

static lv_obj_t* r_label;
static lv_obj_t* g_label;
static lv_obj_t* b_label;
static uint8_t r = 0;
static uint8_t g = 0;
static uint8_t b = 0;

void demo_next();

static void led_config() {
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 3, // at least one LED on board
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    led_strip_clear(led_strip);
}

static void led_apply() {
    led_strip_set_pixel(led_strip, 0, r, g, b);
    led_strip_set_pixel(led_strip, 1, r, g, b);
    led_strip_set_pixel(led_strip, 2, r, g, b);
    led_strip_refresh(led_strip);
}

static void display_bl_brightness(int value) {
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", value);
    lv_label_set_text(bl_label, buf);
}

static void value_to_label(lv_obj_t* label, int value) {
    char buf[4];
    lv_snprintf(buf, sizeof(buf), "%d", value);
    lv_label_set_text(label, buf);
}

static void bl_update_cb(lv_timer_t * timer) {
    LV_UNUSED(timer);
    lv_obj_t* slider = timer->user_data;

    uint8_t bl = bl_get();
    if (bl != bl_old) {
        display_bl_brightness(bl);
        lv_slider_set_value(slider, bl, LV_ANIM_ON);
    }
    bl_old = bl;
}

static void bl_event_cb(lv_event_t * e) {
    lv_obj_t* slider = lv_event_get_target(e);
    int value = (int) lv_slider_get_value(slider);
    display_bl_brightness(value);
    bl_set(value);
}

static void r_event_cb(lv_event_t * e) {
    lv_obj_t* slider = lv_event_get_target(e);
    lv_obj_t* label = lv_event_get_user_data(e);
    r = (uint8_t) lv_slider_get_value(slider);
    value_to_label(label, r);
    led_apply();
}

static void g_event_cb(lv_event_t * e) {
    lv_obj_t* slider = lv_event_get_target(e);
    lv_obj_t* label = lv_event_get_user_data(e);
    g = (uint8_t) lv_slider_get_value(slider);
    value_to_label(label, g);
    led_apply();
}

static void b_event_cb(lv_event_t * e) {
    lv_obj_t* slider = lv_event_get_target(e);
    lv_obj_t* label = lv_event_get_user_data(e);
    b = (uint8_t) lv_slider_get_value(slider);
    value_to_label(label, b);
    led_apply();
}

static void lvgl_sett() {
    lv_obj_t* label;
    lv_obj_t* slider;

    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "SETTINGS");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -145);

    label = lv_label_create(lv_scr_act());
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -115);
    lv_label_set_text(label, "Display brightness");

    slider = lv_slider_create(lv_scr_act());
    lv_obj_set_pos(slider, 12, 62);
    lv_obj_set_width(slider, 103);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, bl_get(), LV_ANIM_ON);
    lv_obj_add_event_cb(slider, bl_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    bl_label = lv_label_create(lv_scr_act());
    display_bl_brightness(bl_get());
    lv_obj_set_pos(bl_label, 135, 61);

    tmr = lv_timer_create(bl_update_cb, 100, slider);


    label = lv_label_create(lv_scr_act());
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -60);
    lv_label_set_text(label, "RGB LEDs");


    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "R");
    lv_obj_set_pos(label, 5, 111);
    r_label = lv_label_create(lv_scr_act());
    value_to_label(r_label, r);
    lv_obj_set_width(r_label, 27);
    lv_obj_set_style_text_align(r_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(r_label, 140, 111);
    slider = lv_slider_create(lv_scr_act());
    lv_obj_set_pos(slider, 30, 113);
    lv_obj_set_width(slider, 95);
    lv_slider_set_range(slider, 0, 255);
    lv_slider_set_value(slider, r, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider, r_event_cb, LV_EVENT_VALUE_CHANGED, r_label);

    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "G");
    lv_obj_set_pos(label, 5, 136);
    g_label = lv_label_create(lv_scr_act());
    value_to_label(g_label, g);
    lv_obj_set_width(g_label, 27);
    lv_obj_set_style_text_align(g_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(g_label, 140, 136);
    slider = lv_slider_create(lv_scr_act());
    lv_obj_set_pos(slider, 30, 138);
    lv_obj_set_width(slider, 95);
    lv_slider_set_range(slider, 0, 255);
    lv_slider_set_value(slider, g, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider, g_event_cb, LV_EVENT_VALUE_CHANGED, g_label);

    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "B");
    lv_obj_set_pos(label, 5, 161);
    b_label = lv_label_create(lv_scr_act());
    value_to_label(b_label, b);
    lv_obj_set_width(b_label, 27);
    lv_obj_set_style_text_align(b_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(b_label, 140, 161);
    slider = lv_slider_create(lv_scr_act());
    lv_obj_set_pos(slider, 30, 163);
    lv_obj_set_width(slider, 95);
    lv_slider_set_range(slider, 0, 255);
    lv_slider_set_value(slider, b, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider, b_event_cb, LV_EVENT_VALUE_CHANGED, b_label);
}

static void next_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        lv_timer_pause(tmr);
        lv_timer_reset(tmr);
        led_strip_del(led_strip);
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

void lvgl_settings() {
    led_config();
    lv_obj_clean(lv_scr_act());
    lvgl_sett();
    lvgl_create_next_btn();
}
