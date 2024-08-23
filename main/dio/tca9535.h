#pragma once

void *tca9535_init(void *bus_handle, uint8_t i2c_addr, uint16_t out_mask, uint16_t inv_mask);
void tca9535_deinit(void* dev_handle);
bool tca9535_write(void *dev_handle, uint16_t outs);
bool tca9535_read(void *dev_handle, uint16_t* in_buff);
