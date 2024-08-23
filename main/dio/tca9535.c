#include "../i2cmaster/i2cmaster.h"
#include "esp_log.h"
#include "tca9535.h"

static const char *TAG = "tca9535";

#define REG_INPUT0  0x00
#define REG_INPUT1  0x01
#define REG_OUTPUT0 0x02
#define REG_OUTPUT1 0x03
#define REG_INVERT0 0x04
#define REG_INVERT1 0x05
#define REG_CONFIG0 0x06
#define REG_CONFIG1 0x07

#define REG_MAX     REG_CONFIG1

static bool tca9535_write_reg_pair(void *dev_handle, uint8_t reg_addr, uint16_t value) {
    if (reg_addr + 1 > REG_MAX)
        return false;

    uint8_t tx_buff[3];
    tx_buff[0] = reg_addr;
    tx_buff[1] = value & 0xFF;
    tx_buff[2] = value >> 8;
    return i2cmaster_write(dev_handle, tx_buff, sizeof(tx_buff));
}

static bool tca9535_read_reg_pair(void *dev_handle, uint8_t reg_addr, uint16_t* buff) {
    if (reg_addr + 1 > REG_MAX)
        return false;

    uint8_t tx_buff = reg_addr;
    return i2cmaster_write_read(dev_handle, &tx_buff, 1, (uint8_t*) buff, 2);
}

void *tca9535_init(void* bus_handle, uint8_t i2c_addr, uint16_t out_mask, uint16_t inv_mask) {
    if (!i2cmaster_test(bus_handle, i2c_addr))
        return 0;
    
    i2c_master_dev_handle_t dev_handle;
    dev_handle = i2cmaster_dev_init(bus_handle, i2c_addr);
    if (!dev_handle)
        return 0;

    tca9535_write_reg_pair(dev_handle, REG_OUTPUT0, 0);
    tca9535_write_reg_pair(dev_handle, REG_INVERT0, inv_mask);
    tca9535_write_reg_pair(dev_handle, REG_CONFIG0, ~out_mask);
    
    return dev_handle;
}

void tca9535_deinit(void* dev_handle) {
    i2c_master_dev_handle_t handle = (i2c_master_dev_handle_t *) dev_handle;
    i2cmaster_dev_deinit(handle);
}

bool tca9535_write(void *dev_handle, uint16_t outs) {
    return tca9535_write_reg_pair(dev_handle, REG_OUTPUT0, outs);
}

bool tca9535_read(void *dev_handle, uint16_t* in_buff) {
    return tca9535_read_reg_pair(dev_handle, REG_INPUT0, in_buff);
}
