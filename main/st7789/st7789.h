#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_LCD_BK_LIGHT_GPIO  15
#define TEST_LCD_RST_GPIO       -1
#define TEST_LCD_CS_GPIO        16
#define TEST_LCD_DC_GPIO        17
#define TEST_LCD_PCLK_GPIO      8
#define TEST_LCD_MOSI_GPIO      9

#define TEST_LCD_PIXEL_CLOCK_HZ (30 * 1000 * 1000)

void st7789_init(uint16_t width, uint16_t height);
void st7789_deinit();
void st7789_DrawPixel(int16_t x, int16_t y, uint16_t color);
uint16_t st7789_GetPixel(int16_t x, int16_t y);
void st7789_Update();
void st7789_SetBL(uint8_t Value);
void st7789_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

uint16_t st7789_GetWidth();
uint16_t st7789_GetHeight();

void lcd_panel_test();

void GC9A01A_SleepMode(uint8_t Mode);
void GC9A01A_DisplayPower(uint8_t On);

#ifdef __cplusplus
}
#endif
