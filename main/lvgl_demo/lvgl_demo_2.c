#include "esp_log.h"
#include "lvgl.h"
#include "../backlight/bl.h"
#include "lvgl_demo_2.h"

static char *TAG = "DEMO2";
static lv_timer_t* tmr = NULL;

void demo_next();

static void draw_event_cb(lv_event_t * e) {
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->part == LV_PART_ITEMS) {
        lv_obj_t * obj = lv_event_get_target(e);
        lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
        uint32_t cnt = lv_chart_get_point_count(obj);
        /*Make older value more transparent*/
        dsc->rect_dsc->bg_opa = (LV_OPA_COVER *  dsc->id) / (cnt - 1);

        /*Make smaller values blue, higher values red*/
        lv_coord_t * x_array = lv_chart_get_x_array(obj, ser);
        lv_coord_t * y_array = lv_chart_get_y_array(obj, ser);
        /*dsc->id is the tells drawing order, but we need the ID of the point being drawn.*/
        uint32_t start_point = lv_chart_get_x_start_point(obj, ser);
        uint32_t p_act = (start_point + dsc->id) % cnt; /*Consider start point to get the index of the array*/
        lv_opa_t x_opa = (x_array[p_act] * LV_OPA_50) / 20;
        lv_opa_t y_opa = (y_array[p_act] * LV_OPA_50) / 80;

        dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_RED),
                                               lv_palette_main(LV_PALETTE_BLUE),
                                               x_opa + y_opa);
    }
}

static void add_data(lv_timer_t * timer) {
    LV_UNUSED(timer);
    lv_obj_t * chart = timer->user_data;
    lv_chart_set_next_value2(chart, lv_chart_get_series_next(chart, NULL), lv_rand(0, 20), lv_rand(0, 80));
}

static void lv_example_chart() {
    lv_obj_t * chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 135, 250);
    lv_obj_set_pos(chart, 32, 3);

    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_set_style_line_width(chart, 0, LV_PART_ITEMS);

    lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);

    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 8, 3, 5, 5, true, 30);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 8, 3, 9, 5, true, 50);

    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, 20);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 80);

    lv_chart_set_point_count(chart, 50);

    lv_chart_series_t * ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    uint32_t i;
    for(i = 0; i < 50; i++) {
        lv_chart_set_next_value2(chart, ser, lv_rand(0, 200), lv_rand(0, 1000));
    }

    tmr = lv_timer_create(add_data, 100, chart);
}

static void next_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        lv_timer_pause(tmr);
        lv_timer_reset(tmr);
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

void lvgl_demo_2() {
    lv_anim_del_all();
    lv_obj_clean(lv_scr_act());
    lv_example_chart();
    lvgl_create_next_btn();
}
