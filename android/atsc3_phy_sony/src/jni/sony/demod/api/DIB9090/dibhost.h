#ifndef INTERFACE_LINUX_I2C_H
#define INTERFACE_LINUX_I2C_H

#include "dibbusdescriptor.h"
#include "IT9300.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dibDataBusHost * host_i2c_interface_attach(Endeavour* endeavour, unsigned char i2c_bus, unsigned char chip_idx, void *priv);
void host_i2c_release(struct dibDataBusHost *);

#ifdef __cplusplus
}
#endif

#endif
