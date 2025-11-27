#include "uart_device.h"
#include <stdlib.h>
#include <string.h>

// -------------------------------------------------------------------------
// 私有函数实现 (Static)
// -------------------------------------------------------------------------

static int uart_device_init(BaseDevice *self, void *config)
{
    UARTDevice *uart_dev = (UARTDevice *)self;
    UARTConfig *cfg      = (UARTConfig *)config;

    if (!uart_dev || !cfg || !cfg->uart_handle) {
        return -1;
    }

    // 保存配置
    uart_dev->uart_handle = cfg->uart_handle;
    uart_dev->timeout_ms  = cfg->timeout_ms;

    // 标记状态
    self->state = DEVICE_STATE_READY;
    return 0;
}

static int uart_device_deinit(BaseDevice *self)
{
    if (!self) {
        return -1;
    }
    self->state = DEVICE_STATE_UNINIT;
    return 0;
}

static int uart_device_read(BaseDevice *self, uint8_t *buffer, size_t length)
{
    UARTDevice       *uart_dev = (UARTDevice *)self;
    HAL_StatusTypeDef status;

    if (!uart_dev || !uart_dev->uart_handle || !buffer || length == 0) {
        return -1;
    }

    // 调用HAL库接收函数
    status = HAL_UART_Receive(uart_dev->uart_handle, buffer, (uint16_t)length, uart_dev->timeout_ms);

    if (status != HAL_OK) {
        uart_dev->base.error_code = status;
        return -1;
    }

    return (int)length; 
}

static int uart_device_write(BaseDevice *self, const uint8_t *data, size_t length)
{
    UARTDevice       *uart_dev = (UARTDevice *)self;
    HAL_StatusTypeDef status;

    if (!uart_dev || !uart_dev->uart_handle || !data || length == 0) {
        return -1;
    }

    // 调用HAL库发送函数
    status = HAL_UART_Transmit(uart_dev->uart_handle, (uint8_t *)data, (uint16_t)length, uart_dev->timeout_ms);

    if (status != HAL_OK) {
        uart_dev->base.error_code = status;
        return -1;
    }

    return (int)length;
}

static int uart_device_ioctl(BaseDevice *self, uint32_t cmd, void *arg)
{
    UARTDevice *uart_dev = (UARTDevice *)self;

    if (!uart_dev) {
        return -1;
    }

    switch (cmd) {
    case UART_IOCTL_SET_TIMEOUT:
        if (arg) {
            uart_dev->timeout_ms = *(uint32_t *)arg;
            return 0;
        }
        break;
    case UART_IOCTL_GET_TIMEOUT:
        if (arg) {
            *(uint32_t *)arg = uart_dev->timeout_ms;
            return 0;
        }
        break;
    default:
        return -1; 
    }

    return -1;
}

static void uart_device_destroy(BaseDevice *self)
{
    if (self) {
        uart_device_deinit(self); 
        free(self);
    }
}

// -------------------------------------------------------------------------
// 公共接口实现
// -------------------------------------------------------------------------

BaseDevice *uart_device_create(UARTConfig *config)
{
    UARTDevice *uart_dev = (UARTDevice *)malloc(sizeof(UARTDevice));
    if (!uart_dev) {
        return NULL;
    }

    memset(uart_dev, 0, sizeof(UARTDevice));

    uart_dev->base.type       = DEVICE_TYPE_UART;
    uart_dev->base.state      = DEVICE_STATE_UNINIT;
    uart_dev->base.id         = 0; // 可以根据需求分配ID
    uart_dev->base.error_code = 0;

    uart_dev->base.init    = uart_device_init;
    uart_dev->base.deinit  = uart_device_deinit;
    uart_dev->base.read    = uart_device_read;
    uart_dev->base.write   = uart_device_write;
    uart_dev->base.ioctl   = uart_device_ioctl;
    uart_dev->base.destroy = uart_device_destroy;

    if (config) {
        if (uart_device_init((BaseDevice *)uart_dev, config) != 0) {
            free(uart_dev);
            return NULL;
        }
    }
    return (BaseDevice *)uart_dev;
}
