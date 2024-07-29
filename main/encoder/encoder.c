#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/time.h>
#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "encoder.h"

#define TABLE_ROWS      7
#define TABLE_COLS      4

#define DIR_NONE        0x0   // No complete step yet.
#define DIR_CW          0x10  // Clockwise step.
#define DIR_CCW         0x20  // Anti-clockwise step.
#define DIR_BUT_PRESS   0x40  // Button pressed.

// Create the half-step state table (emits a code at 00 and 11)
#define R_START         0x0
#define H_CCW_BEGIN     0x1
#define H_CW_BEGIN      0x2
#define H_START_M       0x3
#define H_CW_BEGIN_M    0x4
#define H_CCW_BEGIN_M   0x5

static const uint8_t _table_half[TABLE_ROWS][TABLE_COLS] = {
    // 00                  01              10            11                   // BA
    {H_START_M,            H_CW_BEGIN,     H_CCW_BEGIN,  R_START},            // R_START (00)
    {H_START_M | DIR_CCW,  R_START,        H_CCW_BEGIN,  R_START},            // H_CCW_BEGIN
    {H_START_M | DIR_CW,   H_CW_BEGIN,     R_START,      R_START},            // H_CW_BEGIN
    {H_START_M,            H_CCW_BEGIN_M,  H_CW_BEGIN_M, R_START},            // H_START_M (11)
    {H_START_M,            H_START_M,      H_CW_BEGIN_M, R_START | DIR_CW},   // H_CW_BEGIN_M
    {H_START_M,            H_CCW_BEGIN_M,  H_START_M,    R_START | DIR_CCW},  // H_CCW_BEGIN_M
};

static char *TAG = "ENC";
static bool need_to_stop = false;
static bool task_started = false;
static uint8_t state = R_START;
static bool old_btn_state = true;
static encoder_cb_t _cb = NULL;
static QueueHandle_t gpio_evt_queue = NULL;
static sEncoderInfo encoder_info = {0};


static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint8_t gpio_num = (uint8_t) arg;
    if ((gpio_num != ENC_CLK_GPIO) && (gpio_num != ENC_DT_GPIO))
        return;

    uint8_t pin_state = (gpio_get_level(ENC_CLK_GPIO) << 1) | gpio_get_level(ENC_DT_GPIO);
    state = _table_half[state & 0xf][pin_state];

    uint8_t event = state & 0x30;
    bool send_event = false;

    switch (event)
    {
    case DIR_CW:
        encoder_info.pos++;
        encoder_info.dir = 1;
        send_event = true;
        break;
    case DIR_CCW:
        encoder_info.pos--;
        encoder_info.dir = -1;
        send_event = true;
        break;
    default:
        break;
    }

    if (send_event)
        xQueueSendFromISR(gpio_evt_queue, &encoder_info, NULL);
}

static void enc_task(void* arg)
{
    task_started = true;

    uint8_t event;
    while (!need_to_stop) {
        if (xQueueReceive(gpio_evt_queue, &event, 50)) {
            if (_cb) {
                encoder_info.event = event;
                _cb(encoder_info);
            }
        } else {
            bool btn_state = gpio_get_level(ENC_SW_GPIO);
            if (btn_state != old_btn_state) {
                if ((!btn_state) && _cb) {
                    encoder_info.event = DIR_BUT_PRESS;
                    _cb(encoder_info);
                }
                old_btn_state = btn_state;
            }
        }
    }

    task_started = false;
}

void encoder_init() {

    gpio_evt_queue = xQueueCreate(50, sizeof(uint8_t));

    // ENC_CLK_GPIO
    gpio_config_t pin_config = {
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
        .pin_bit_mask = 1ULL << ENC_CLK_GPIO
    };
    gpio_config(&pin_config);

    // ENC_DT_GPIO
    pin_config.pin_bit_mask = 1ULL << ENC_DT_GPIO;
    gpio_config(&pin_config);

    // ENC_SW_GPIO
    pin_config.pin_bit_mask = 1ULL << ENC_SW_GPIO;
    gpio_config(&pin_config);

    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(ENC_CLK_GPIO, gpio_isr_handler, (void*) ENC_CLK_GPIO);
    gpio_isr_handler_add(ENC_DT_GPIO, gpio_isr_handler, (void*) ENC_DT_GPIO);
}

void encoder_deinit() {
    encoder_stop();

    gpio_reset_pin(ENC_CLK_GPIO);
    gpio_reset_pin(ENC_DT_GPIO);
    gpio_reset_pin(ENC_SW_GPIO);
}

void encoder_start() {
    if (!task_started)
        xTaskCreate(enc_task, "gpio_input", 4*2048, NULL, 10, NULL);
}

void encoder_stop() {
    need_to_stop = true;
    
    while (task_started)
        vTaskDelay(1); 

    need_to_stop = false;
}

void encoder_set_cb(encoder_cb_t cb) {
    _cb = cb;
}
