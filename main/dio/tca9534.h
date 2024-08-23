#pragma once

void *tca9534_init(void *bus_handle, uint8_t i2c_addr, uint8_t out_mask, uint8_t inv_mask);
bool tca9534_write(void *dev_handle, uint8_t outs);
bool tca9534_read(void *dev_handle, uint8_t* in_buff);
