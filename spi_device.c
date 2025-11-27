// spi_device.c
#include "spi_device.h"
#include <stdlib.h>
#include <string.h>

static int spi_device_init(BaseDevice *self, void *config)
{
    SPIDevice *spi_dev = (SPIDevice *)self;
    SPIConfig *cfg     = (SPIConfig *)config;

    if (!spi_dev || !cfg) {
        return -1;
    }

    spi_dev->spi_handle = cfg->spi_handle;
    spi_dev->cs_port    = cfg->cs_port;
    spi_dev->cs_pin     = cfg->cs_pin;
    spi_dev->timeout_ms = cfg->timeout_ms;

    HAL_GPIO_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_PIN_SET);

    self->state = DEVICE_STATE_READY;
    return 0;
}

static int spi_device_deinit(BaseDevice *self)
{
    SPIDevice *spi_dev = (SPIDevice *)self;

    if (!spi_dev) {
        return -1;
    }

    HAL_GPIO_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_PIN_SET);
    self->state = DEVICE_STATE_UNINIT;
    return 0;
}

static int spi_device_read(BaseDevice *self, uint8_t *buffer, size_t length)
{
    SPIDevice        *spi_dev = (SPIDevice *)self;
    HAL_StatusTypeDef status;

    if (!spi_dev || !buffer || length == 0) {
        return -1;
    }
    if (self->state != DEVICE_STATE_READY) {
        return -2;
    }

    self->state = DEVICE_STATE_BUSY;

    HAL_GPIO_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_PIN_RESET);
    status = HAL_SPI_Receive(spi_dev->spi_handle, buffer, length, spi_dev->timeout_ms);
    HAL_GPIO_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_PIN_SET);

    self->state = DEVICE_STATE_READY;

    if (status != HAL_OK) {
        self->state      = DEVICE_STATE_ERROR;
        self->error_code = status;
        return -3;
    }

    return (int)length;
}

static int spi_device_write(BaseDevice *self, const uint8_t *data, size_t length)
{
    SPIDevice        *spi_dev = (SPIDevice *)self;
    HAL_StatusTypeDef status;

    if (!spi_dev || !data || length == 0) {
        return -1;
    }
    if (self->state != DEVICE_STATE_READY) {
        return -2;
    }

    self->state = DEVICE_STATE_BUSY;

    HAL_GPIO_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_PIN_RESET);
    status = HAL_SPI_Transmit(spi_dev->spi_handle, (uint8_t *)data, length,
                              spi_dev->timeout_ms);
    HAL_GPIO_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_PIN_SET);

    self->state = DEVICE_STATE_READY;

    if (status != HAL_OK) {
        self->state      = DEVICE_STATE_ERROR;
        self->error_code = status;
        return -3;
    }

    return (int)length;
}

static int spi_device_ioctl(BaseDevice *self, uint32_t cmd, void *arg)
{
    SPIDevice *spi_dev = (SPIDevice *)self;

    switch (cmd) {
    case SPI_IOCTL_SET_TIMEOUT:
        if (arg) {
            spi_dev->timeout_ms = *(uint32_t *)arg;
            return 0;
        }
        break;
    case SPI_IOCTL_GET_TIMEOUT:
        if (arg) {
            *(uint32_t *)arg = spi_dev->timeout_ms;
            return 0;
        }
        break;
    default:
        return -1;
    }

    return -1;
}

static void spi_device_destroy(BaseDevice *self)
{
    if (self) {
        spi_device_deinit(self);
        free(self);
    }
}

BaseDevice *spi_device_create(SPIConfig *config)
{
    SPIDevice *spi_dev = (SPIDevice *)malloc(sizeof(SPIDevice));
    if (!spi_dev) {
        return NULL;
    }

    memset(spi_dev, 0, sizeof(SPIDevice));

    spi_dev->base.type       = DEVICE_TYPE_SPI;
    spi_dev->base.state      = DEVICE_STATE_UNINIT;
    spi_dev->base.id         = 0;
    spi_dev->base.error_code = 0;

    spi_dev->base.init    = spi_device_init;
    spi_dev->base.deinit  = spi_device_deinit;
    spi_dev->base.read    = spi_device_read;
    spi_dev->base.write   = spi_device_write;
    spi_dev->base.ioctl   = spi_device_ioctl;
    spi_dev->base.destroy = spi_device_destroy;

    if (config) {
        spi_dev->spi_handle = config->spi_handle;
        spi_dev->cs_port    = config->cs_port;
        spi_dev->cs_pin     = config->cs_pin;
        spi_dev->timeout_ms = config->timeout_ms;
    }

    return (BaseDevice *)spi_dev;
}
