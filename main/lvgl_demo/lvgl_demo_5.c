#include <math.h>
#include <driver/i2c_master.h>
#include "../i2cmaster/i2cmaster.h"
#include "../dio/tca9535.h"
#include "esp_log.h"
#include "lvgl.h"
#include "../backlight/bl.h"
#include "lvgl_demo_4.h"

static char *TAG = "DEMO5";

void demo_next();

static void event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        ESP_LOGI(TAG, "Click");
        LV_LOG_USER("Clicked");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        ESP_LOGI(TAG, "Toggle");
        LV_LOG_USER("Toggled");
    }
}

static void init_leds()
{
    lv_obj_t* label;
    lv_obj_t* btn;
    lv_obj_t* dd;
    lv_obj_t* sw;

    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "CONTROLS");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -145);

    btn = lv_btn_create(lv_scr_act());
    label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, -110);

    btn = lv_btn_create(lv_scr_act());
    label = lv_label_create(btn);
    lv_label_set_text(label, "Toggle button");
    lv_obj_center(label);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, -65);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn, LV_SIZE_CONTENT);

    dd = lv_dropdown_create(lv_scr_act());
    lv_dropdown_set_options(dd, "Mode 1\n"
                            "Mode 2\n"
                            "Mode 3\n"
                            "Mode 4");

    lv_obj_align(dd, LV_ALIGN_CENTER, 0, -20);


    sw = lv_switch_create(lv_scr_act());
    lv_obj_align(sw, LV_ALIGN_CENTER, -35, 25);
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Switch 1");
    lv_obj_align(label, LV_ALIGN_CENTER, 25, 25);

    sw = lv_switch_create(lv_scr_act());
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_align(sw, LV_ALIGN_CENTER, -35, 60);
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Switch 2");
    lv_obj_align(label, LV_ALIGN_CENTER, 25, 60);

    sw = lv_switch_create(lv_scr_act());
    lv_obj_add_state(sw, LV_STATE_DISABLED);
    lv_obj_align(sw, LV_ALIGN_CENTER, -35, 95);
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Switch 3");
    lv_obj_align(label, LV_ALIGN_CENTER, 25, 95);
}

static void next_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
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

void lvgl_demo_5() {
    lv_obj_clean(lv_scr_act());
    init_leds();
    lvgl_create_next_btn();
}
