/*
 * @Description: 
 * @Date: 2025-11-27 13:53:18
 * @FilePath: \C³éÏó·â×°Çý¶¯\i2c_device.h
 */
// i2c_device.h
#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H

#include "device_driver.h"
#include "stm32f4xx_hal.h"

typedef struct {
    I2C_HandleTypeDef *i2c_handle;
    uint8_t            device_addr;
    uint32_t           timeout_ms;
} I2CConfig;

typedef struct {
    BaseDevice         base;
    I2C_HandleTypeDef *i2c_handle;
    uint8_t            device_addr;
    uint32_t           timeout_ms;
} I2CDevice;

enum {
    I2C_IOCTL_SET_ADDR = 0x01,
    I2C_IOCTL_GET_ADDR = 0x02,
};

BaseDevice *i2c_device_create(I2CConfig *config);

#endif
