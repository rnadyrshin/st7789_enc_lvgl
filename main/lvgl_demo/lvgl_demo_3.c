#include <math.h>
#include "esp_log.h"
#include "lvgl.h"
#include "../backlight/bl.h"
#include "lvgl_demo_3.h"

static char *TAG = "DEMO3";
static float x = 0.0;
static lv_timer_t* tmr1 = NULL;
static lv_timer_t* tmr2 = NULL;

void demo_next();

static void add_data(lv_timer_t * timer) {
    LV_UNUSED(timer);
    lv_obj_t * chart = timer->user_data;
    lv_chart_series_t * ser1 = lv_chart_get_series_next(chart, NULL);
    lv_chart_set_next_value(chart, ser1, lv_rand(0, 50));
    lv_chart_series_t * ser2 = lv_chart_get_series_next(chart, ser1);
    lv_chart_set_next_value(chart, ser2, lv_rand(40, 90));
}

static void add_data_sin(lv_timer_t * timer) {
    LV_UNUSED(timer);
    lv_obj_t * chart = timer->user_data;
    lv_chart_series_t * ser1 = lv_chart_get_series_next(chart, NULL);
    lv_chart_set_next_value(chart, ser1, 20 * sin(x) + 50);
    lv_chart_series_t * ser2 = lv_chart_get_series_next(chart, ser1);
    lv_chart_set_next_value(chart, ser2, 20 * sin(x * 2) + 50);

    x = x + 0.2;
}

static void init_chart_1() {
    lv_obj_t * chart;
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 164, 130);
    lv_obj_set_pos(chart, 3, 3);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   // Show lines and points too
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);

    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_div_line_count(chart, 10, 10);
    lv_chart_set_point_count(chart, 30);

    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);

    tmr1 = lv_timer_create(add_data, 100, chart);
}

static void init_chart_2() {
    lv_obj_t * chart;
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 164, 130);
    lv_obj_set_pos(chart, 3, 136);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   // Show lines and points too
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);

    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_div_line_count(chart, 10, 10);
    lv_chart_set_point_count(chart, 30);

    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_SECONDARY_Y);

    tmr2 = lv_timer_create(add_data_sin, 50, chart);
}

static void next_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        lv_timer_pause(tmr1);
        lv_timer_reset(tmr1);
        lv_timer_pause(tmr2);
        lv_timer_reset(tmr2);
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

void lvgl_demo_3() {
    lv_anim_del_all();
    lv_obj_clean(lv_scr_act());
    init_chart_1();
    init_chart_2();
    lvgl_create_next_btn();
}
