/*
 * @Description: 
 * @Date: 2025-11-27 13:53:18
 * @FilePath: \C³éÏó·â×°Çý¶¯\device_driver.c
 */
#include "device_driver.h"
#include "spi_device.h"
#include "i2c_device.h"
// #include "uart_device.h"

#include <stddef.h>

BaseDevice *device_create(DeviceType type, void *config)
{
    BaseDevice *device = NULL;

    switch (type) {
    case DEVICE_TYPE_SPI:
        device = spi_device_create((SPIConfig *)config);
        break;
    case DEVICE_TYPE_I2C:
        device = i2c_device_create((I2CConfig *)config);
        break;
    case DEVICE_TYPE_UART:
        // device = uart_device_create((UARTConfig *)config);
        break;
    default:
        return NULL;
    }

    return device;
}

void device_destroy(BaseDevice *device)
{
    if (device && device->destroy) {
        device->destroy(device);
    }
}

int device_init(BaseDevice *device, void *config)
{
    if (!device || !device->init) {
        return -1;
    }
    return device->init(device, config);
}

int device_read(BaseDevice *device, uint8_t *buffer, size_t length)
{
    if (!device || !device->read) {
        return -1;
    }
    return device->read(device, buffer, length);
}

int device_write(BaseDevice *device, const uint8_t *data, size_t length)
{
    if (!device || !device->write) {
        return -1;
    }
    return device->write(device, data, length);
}

int device_ioctl(BaseDevice *device, uint32_t cmd, void *arg)
{
    if (!device || !device->ioctl) {
        return -1;
    }
    return device->ioctl(device, cmd, arg);
}

DeviceState device_get_state(BaseDevice *device)
{
    if (!device) {
        return DEVICE_STATE_UNINIT;
    }
    return device->state;
}

const char *device_get_type_string(BaseDevice *device)
{
    if (!device) {
        return "UNKNOWN";
    }

    switch (device->type) {
    case DEVICE_TYPE_SPI:
        return "SPI";
    case DEVICE_TYPE_I2C:
        return "I2C";
    case DEVICE_TYPE_UART:
        return "UART";
    default:
        return "UNKNOWN";
    }
}
