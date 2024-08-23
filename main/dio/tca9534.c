#include "../i2cmaster/i2cmaster.h"
#include "esp_log.h"
#include "tca9534.h"

static const char *TAG = "tca9534";

#define REG_INPUT   0x00
#define REG_OUTPUT  0x01
#define REG_INVERT  0x02
#define REG_CONFIG  0x03

#define REG_MAX     REG_CONFIG

static bool tca9534_write_reg(void *dev_handle, uint8_t reg_addr, uint8_t value) {
    if (reg_addr > REG_MAX)
        return false;

    uint8_t tx_buff[2];
    tx_buff[0] = reg_addr;
    tx_buff[1] = value;
    return i2cmaster_write(dev_handle, tx_buff, sizeof(tx_buff));
}

static bool tca9534_read_reg(void *dev_handle, uint8_t reg_addr, uint8_t* buff) {
    if (reg_addr > REG_MAX)
        return false;

    uint8_t tx_buff = reg_addr;
    return i2cmaster_write_read(dev_handle, &tx_buff, 1, buff, 1);
}

void *tca9534_init(void* bus_handle, uint8_t i2c_addr, uint8_t out_mask, uint8_t inv_mask) {
    if (!i2cmaster_test(bus_handle, i2c_addr))
        return 0;
    
    i2c_master_dev_handle_t dev_handle;
    dev_handle = i2cmaster_dev_init(bus_handle, i2c_addr);
    if (!dev_handle)
        return 0;

    tca9534_write_reg(dev_handle, REG_OUTPUT, 0);
    tca9534_write_reg(dev_handle, REG_INVERT, inv_mask);
    tca9534_write_reg(dev_handle, REG_CONFIG, ~out_mask);
    
    return dev_handle;
}

bool tca9534_write(void *dev_handle, uint8_t outs) {
    return tca9534_write_reg(dev_handle, REG_OUTPUT, outs);
}

bool tca9534_read(void *dev_handle, uint8_t* in_buff) {
    return tca9534_read_reg(dev_handle, REG_INPUT, in_buff);
}
