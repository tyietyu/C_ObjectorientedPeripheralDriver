// i2c_device.c
#include "i2c_device.h"
#include <stdlib.h>
#include <string.h>

static int i2c_device_init(BaseDevice *self, void *config)
{
    I2CDevice *i2c_dev = (I2CDevice *)self;
    I2CConfig *cfg     = (I2CConfig *)config;

    if (!i2c_dev || !cfg) {
        return -1;
    }

    i2c_dev->i2c_handle  = cfg->i2c_handle;
    i2c_dev->device_addr = cfg->device_addr;
    i2c_dev->timeout_ms  = cfg->timeout_ms;

    self->state = DEVICE_STATE_READY;
    return 0;
}

static int i2c_device_deinit(BaseDevice *self)
{
    if (!self) {
        return -1;
    }
    self->state = DEVICE_STATE_UNINIT;
    return 0;
}

static int i2c_device_read(BaseDevice *self, uint8_t *buffer, size_t length)
{
    I2CDevice         *i2c_dev = (I2CDevice *)self;
    HAL_StatusTypeDef  status;

    if (!i2c_dev || !buffer || length == 0) {
        return -1;
    }
    if (self->state != DEVICE_STATE_READY) {
        return -2;
    }

    self->state = DEVICE_STATE_BUSY;

    status = HAL_I2C_Master_Receive(i2c_dev->i2c_handle,
                                    (uint16_t)(i2c_dev->device_addr << 1),
                                    buffer,
                                    length,
                                    i2c_dev->timeout_ms);

    self->state = DEVICE_STATE_READY;

    if (status != HAL_OK) {
        self->state      = DEVICE_STATE_ERROR;
        self->error_code = status;
        return -3;
    }

    return (int)length;
}

static int i2c_device_write(BaseDevice *self, const uint8_t *data, size_t length)
{
    I2CDevice         *i2c_dev = (I2CDevice *)self;
    HAL_StatusTypeDef  status;

    if (!i2c_dev || !data || length == 0) {
        return -1;
    }
    if (self->state != DEVICE_STATE_READY) {
        return -2;
    }

    self->state = DEVICE_STATE_BUSY;

    status = HAL_I2C_Master_Transmit(i2c_dev->i2c_handle,
                                     (uint16_t)(i2c_dev->device_addr << 1),
                                     (uint8_t *)data,
                                     length,
                                     i2c_dev->timeout_ms);

    self->state = DEVICE_STATE_READY;

    if (status != HAL_OK) {
        self->state      = DEVICE_STATE_ERROR;
        self->error_code = status;
        return -3;
    }

    return (int)length;
}

static int i2c_device_ioctl(BaseDevice *self, uint32_t cmd, void *arg)
{
    I2CDevice *i2c_dev = (I2CDevice *)self;

    switch (cmd) {
    case I2C_IOCTL_SET_ADDR:
        if (arg) {
            i2c_dev->device_addr = *(uint8_t *)arg;
            return 0;
        }
        break;
    case I2C_IOCTL_GET_ADDR:
        if (arg) {
            *(uint8_t *)arg = i2c_dev->device_addr;
            return 0;
        }
        break;
    default:
        return -1;
    }

    return -1;
}

static void i2c_device_destroy(BaseDevice *self)
{
    if (self) {
        i2c_device_deinit(self);
        free(self);
    }
}

BaseDevice *i2c_device_create(I2CConfig *config)
{
    I2CDevice *i2c_dev = (I2CDevice *)malloc(sizeof(I2CDevice));
    if (!i2c_dev) {
        return NULL;
    }

    memset(i2c_dev, 0, sizeof(I2CDevice));

    i2c_dev->base.type       = DEVICE_TYPE_I2C;
    i2c_dev->base.state      = DEVICE_STATE_UNINIT;
    i2c_dev->base.id         = 0;
    i2c_dev->base.error_code = 0;

    i2c_dev->base.init    = i2c_device_init;
    i2c_dev->base.deinit  = i2c_device_deinit;
    i2c_dev->base.read    = i2c_device_read;
    i2c_dev->base.write   = i2c_device_write;
    i2c_dev->base.ioctl   = i2c_device_ioctl;
    i2c_dev->base.destroy = i2c_device_destroy;

    if (config) {
        i2c_dev->i2c_handle  = config->i2c_handle;
        i2c_dev->device_addr = config->device_addr;
        i2c_dev->timeout_ms  = config->timeout_ms;
    }

    return (BaseDevice *)i2c_dev;
}
