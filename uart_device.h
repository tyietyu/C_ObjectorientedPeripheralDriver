#ifndef UART_DEVICE_H
#define UART_DEVICE_H

#include "device_driver.h"
#include "stm32f4xx_hal.h"

typedef struct {
    UART_HandleTypeDef *uart_handle;
    uint32_t            timeout_ms;
} UARTConfig;

typedef struct {
    BaseDevice          base;     
    UART_HandleTypeDef *uart_handle;
    uint32_t            timeout_ms;
} UARTDevice;

// UART特有的IOCTL命令
enum {
    UART_IOCTL_SET_TIMEOUT = 0x01,
    UART_IOCTL_GET_TIMEOUT = 0x02,
    // 可以在此扩展更多命令，如修改波特率等
};

BaseDevice *uart_device_create(UARTConfig *config);

#endif


