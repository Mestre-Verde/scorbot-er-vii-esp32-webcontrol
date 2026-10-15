/**
 * @file pc_bridge.c
 * @brief UART0, this one communicate with the PC. It Is used for debug and not for normal use
 */

#include "esp_err.h"
#include "driver/uart.h"
#include "uart_common.h"

#define PC_BRIDGE UART_NUM_0
#define PC_TX_PIN 43
#define PC_RX_PIN 44

#define PC_RX_BUFFER_SIZE (SOC_UART_FIFO_LEN + 1)
#define PC_TX_BUFFER_SIZE (SOC_UART_FIFO_LEN + 1)

esp_err_t pc_bridge_config(void)
{
    uart_config_t config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    esp_err_t state = installUartDriver(PC_BRIDGE, PC_RX_BUFFER_SIZE, PC_TX_BUFFER_SIZE, 0, NULL, 0);
    if (state != ESP_OK)
    {
        return state;
    }

    state = configUartParam(PC_BRIDGE, &config);
    if (state != ESP_OK)
    {
        return state;
    }

    state = setUartPin(PC_BRIDGE, PC_TX_PIN, PC_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (state != ESP_OK)
    {
        return state;
    }

    return checkCurrentConfigs(PC_BRIDGE);
}

int ReadBytesFromPC(unsigned char *buffer, size_t len, TickType_t timeout)
{
    return uart_read_bytes(PC_BRIDGE, buffer, len, timeout);
}

esp_err_t SendBytesToPC(const unsigned char buffer[], size_t len)
{
    int written = uart_write_bytes(PC_BRIDGE, buffer, len);

    if (written <= -1)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if ((size_t)written != len)
    {
        return ESP_ERR_INVALID_SIZE;
    }

    return ESP_OK;
}
