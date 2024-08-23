//#include <driver/i2c_types.h>
#include <driver/i2c_master.h>
#include "esp_log.h"
#include "i2cmaster.h"

static const char *TAG = "i2cmaster";

i2c_master_bus_handle_t* i2cmaster_init(uint8_t i2c_bus) {
    if (i2c_bus > I2C_NUM_MAX)
        return 0;

    i2c_master_bus_config_t bus_cfg;
    bus_cfg.i2c_port = i2c_bus;
    bus_cfg.sda_io_num = i2c_bus ? 47 : 13;
    bus_cfg.scl_io_num = i2c_bus ? 48 : 14;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_cfg.glitch_ignore_cnt = 7;
    bus_cfg.intr_priority = 0;
    bus_cfg.trans_queue_depth = 0;
    bus_cfg.flags.enable_internal_pullup = 0;
    
    i2c_master_bus_handle_t* bus_handle;
    if (i2c_new_master_bus(&bus_cfg, &bus_handle) == ESP_OK) {
        ESP_LOGI(TAG, "I2C%d init ok!", i2c_bus);
        return bus_handle;
    }

    ESP_LOGE(TAG, "I2C%d init error!", i2c_bus);
    return 0;
}

void i2cmaster_deinit(i2c_master_bus_handle_t* bus_handle) {
    i2c_del_master_bus(bus_handle);
}

bool i2cmaster_test(void *bus_handle, uint8_t i2c_addr) {
    return i2c_master_probe(bus_handle, i2c_addr, 20) == ESP_OK;
}

void* i2cmaster_dev_init(i2c_master_bus_handle_t* bus_handle, uint8_t i2c_addr) {
    if (!bus_handle)
        return 0;

    i2c_master_dev_handle_t dev_handle;
    i2c_device_config_t i2c_dev_conf = {
        .scl_speed_hz = 100000,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = i2c_addr,
    };
    
    if (i2c_master_bus_add_device(bus_handle, &i2c_dev_conf, &dev_handle) != ESP_OK)
        ESP_LOGE(TAG, "Device init error!");

    return dev_handle;
}

void i2cmaster_dev_deinit(void *dev_handle) {
    i2c_master_dev_handle_t dev = (i2c_master_dev_handle_t) dev_handle;
    i2c_master_bus_rm_device(dev);
}

bool i2cmaster_write(void *dev_handle, uint8_t *buff, size_t len) {
    return i2c_master_transmit(dev_handle, buff, len, 100) == ESP_OK;
}

bool i2cmaster_read(void *dev_handle, uint8_t *buff, size_t len) {
    return i2c_master_receive(dev_handle, buff, len, 100) == ESP_OK;
}

bool i2cmaster_write_read(void *dev_handle, uint8_t* tx_buff, size_t tx_len, uint8_t* rx_buff, size_t rx_len) {
    return i2c_master_transmit_receive(dev_handle, tx_buff, tx_len, rx_buff, rx_len, 100) == ESP_OK;
}
