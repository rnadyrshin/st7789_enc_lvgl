#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "soc/soc_caps.h"
#include "st7789.h"

#define TEST_LCD_H_RES          170
#define TEST_LCD_V_RES          320

#define TEST_SPI_HOST_ID  SPI2_HOST

static uint16_t _width = 1;
static uint16_t _height = 1;
static uint16_t *buffers[2] = {NULL, NULL};
static uint8_t curr_buff = 0;
static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;

uint16_t st7789_GetWidth() {
    return TEST_LCD_H_RES;
}

uint16_t st7789_GetHeight() {
    return TEST_LCD_V_RES;
}

static void lcd_common_init(esp_lcd_panel_io_handle_t *io_handle, esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done, void *user_data, int cmd_bits, int param_bits) {
    spi_bus_config_t buscfg = {
        .sclk_io_num = TEST_LCD_PCLK_GPIO,
        .mosi_io_num = TEST_LCD_MOSI_GPIO,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = _width * _height * sizeof(uint16_t),
    };
    spi_bus_initialize(TEST_SPI_HOST_ID, &buscfg, SPI_DMA_CH_AUTO);

    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = TEST_LCD_PIXEL_CLOCK_HZ,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .lcd_cmd_bits = cmd_bits,
        .lcd_param_bits = param_bits,
        .on_color_trans_done = on_color_trans_done,
        .user_ctx = user_data,
    };
    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)TEST_SPI_HOST_ID, &io_config, io_handle);
}

static void backlight_init() {
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
        .gpio_num = TEST_LCD_BK_LIGHT_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    ledc_channel_config(&ledchancfg);
/*
    // turn off backlight
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << TEST_LCD_BK_LIGHT_GPIO
    };
    gpio_config(&bk_gpio_config);
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 0);
*/
}

void st7789_SetBL(uint8_t Value) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, Value);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    //gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, Value ? 1 : 0);
}

void st7789_DrawPixel(int16_t x, int16_t y, uint16_t color) {
    uint16_t *buff = buffers[curr_buff];
    buff[(y * _width) + x] = color;
}

uint16_t st7789_GetPixel(int16_t x, int16_t y) {
    uint16_t *buff = buffers[curr_buff];
    return buff[(y * _width) + x];
}

void st7789_init(uint16_t width, uint16_t height) {
    _width = width;
    _height = height;

    backlight_init();
    lcd_common_init(&io_handle, NULL, NULL, 8, 8);

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE
    };

    esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);
    // the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
    //esp_lcd_panel_set_gap(panel_handle, 0, 20);
    esp_lcd_panel_set_gap(panel_handle, 35, 0);

    esp_lcd_panel_disp_on_off(panel_handle, true);
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 1);

    buffers[0] = heap_caps_malloc(width * height * 2, MALLOC_CAP_DMA);
    memset(buffers[0], 0, width * height * 2);
    buffers[1] = heap_caps_malloc(width * height * 2, MALLOC_CAP_DMA);
    memset(buffers[1], 0, width * height * 2);
}

void st7789_deinit() {
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 0);
    esp_lcd_panel_disp_on_off(panel_handle, false);
    esp_lcd_panel_del(panel_handle);
    esp_lcd_panel_io_del(io_handle);
    spi_bus_free(TEST_SPI_HOST_ID);
    if (buffers[0]) {
        free(buffers[0]);
        buffers[0] = NULL;
    }
    if (buffers[1]) {
        free(buffers[1]);
        buffers[1] = NULL;
    }
    //gpio_reset_pin(TEST_LCD_BK_LIGHT_GPIO);
}

void st7789_Update() {
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, _width, _height, buffers[curr_buff]);
    
    curr_buff++;
    curr_buff &= 0x01;
}

void st7789_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    uint16_t x_start = x;
    uint16_t y_start = y;
    uint16_t *buff = buffers[curr_buff];

    for (y = y_start; y < y_start + h; y++) {
        for (x = x_start; x < x_start + w; x++) {
            buff[(y * _width) + x] = color;
        }
    }
}
