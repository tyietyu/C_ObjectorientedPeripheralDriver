/*
 * @Description: 
 * @Date: 2025-11-27 13:53:18
 * @FilePath: \C³éÏó·â×°Çý¶¯\device_driver.h
 */
#ifndef DEVICE_DRIVER_H
#define DEVICE_DRIVER_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    DEVICE_STATE_UNINIT = 0,
    DEVICE_STATE_INIT,
    DEVICE_STATE_READY,
    DEVICE_STATE_BUSY,
    DEVICE_STATE_ERROR
} DeviceState;

typedef enum {
    DEVICE_TYPE_SPI = 0,
    DEVICE_TYPE_I2C,
    DEVICE_TYPE_UART,
    DEVICE_TYPE_MAX
} DeviceType;

typedef struct BaseDevice BaseDevice;

struct BaseDevice {
    DeviceType   type;
    DeviceState  state;
    uint32_t     id;
    uint32_t     error_code;

    int  (*init)(BaseDevice *self, void *config);
    int  (*deinit)(BaseDevice *self);
    int  (*read)(BaseDevice *self, uint8_t *buffer, size_t length);
    int  (*write)(BaseDevice *self, const uint8_t *data, size_t length);
    int  (*ioctl)(BaseDevice *self, uint32_t cmd, void *arg);
    void (*destroy)(BaseDevice *self);

    void *private_data;
};

BaseDevice *device_create(DeviceType type, void *config);
void        device_destroy(BaseDevice *device);
int         device_init(BaseDevice *device, void *config);
int         device_read(BaseDevice *device, uint8_t *buffer, size_t length);
int         device_write(BaseDevice *device, const uint8_t *data, size_t length);
int         device_ioctl(BaseDevice *device, uint32_t cmd, void *arg);
DeviceState device_get_state(BaseDevice *device);
const char *device_get_type_string(BaseDevice *device);

#endif
