// spi_device.h
#ifndef SPI_DEVICE_H
#define SPI_DEVICE_H

#include "device_driver.h"
#include "stm32f4xx_hal.h"

typedef struct {
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef      *cs_port;
    uint16_t           cs_pin;
    uint32_t           timeout_ms;
} SPIConfig;

typedef struct {
    BaseDevice         base;
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef      *cs_port;
    uint16_t           cs_pin;
    uint32_t           timeout_ms;
} SPIDevice;

enum {
    SPI_IOCTL_SET_TIMEOUT = 0x01,
    SPI_IOCTL_GET_TIMEOUT = 0x02,
};

BaseDevice *spi_device_create(SPIConfig *config);

#endif
