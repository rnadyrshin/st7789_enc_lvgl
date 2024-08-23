#pragma once
#include <stdint.h>
#include <stddef.h>
#include <driver/i2c_master.h>

i2c_master_bus_handle_t *i2cmaster_init(uint8_t i2c_bus);//, i2c_master_bus_handle_t **bus_handle);
void i2cmaster_deinit(i2c_master_bus_handle_t* bus_handle);
bool i2cmaster_test(void *bus_handle, uint8_t i2c_addr);
void *i2cmaster_dev_init(i2c_master_bus_handle_t *bus_handle, uint8_t i2c_addr);
void i2cmaster_dev_deinit(void *dev_handle);
bool i2cmaster_write(void *dev_handle, uint8_t *buff, size_t len);
bool i2cmaster_read(void *dev_handle, uint8_t *buff, size_t len);
bool i2cmaster_write_read(void *dev_handle, uint8_t *tx_buff, size_t tx_len, uint8_t *rx_buff, size_t rx_len);
