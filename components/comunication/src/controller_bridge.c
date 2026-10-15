/**
 * @file controller_bridge.c
 * @brief UART1, the controller functions
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

#include "uart_common.h"
#include "utils.h"

#define CONTROLLER_BRIDGE UART_NUM_1
#define CONTROLLER_RX_PIN 18
#define CONTROLLER_TX_PIN 17

#define CONTROLLER_RX_BUFFER_SIZE (SOC_UART_FIFO_LEN * 2)
#define CONTROLLER_TX_BUFFER_SIZE (SOC_UART_FIFO_LEN + 1)

#define CONTROLLER_SEND_DELAY 100


int ReadBytesFromController(unsigned char *buffer, size_t len, TickType_t timeout)
{
    return uart_read_bytes(CONTROLLER_BRIDGE, buffer, len, timeout);
}

esp_err_t SendBytesToController(const unsigned char *buffer, size_t len)
{
    if (buffer == NULL && len > 0)
    {
        return ESP_ERR_INVALID_ARG;
    }

    for (size_t i = 0; i < len; i++)
    {
        int written = uart_write_bytes(CONTROLLER_BRIDGE, &buffer[i], 1);

        if (written != 1)
        {
            return ESP_FAIL;
        }

        if (i + 1 < len)
        {
            vTaskDelay(pdMS_TO_TICKS(CONTROLLER_SEND_DELAY));
        }
    }

    return uart_wait_tx_done(CONTROLLER_BRIDGE, pdMS_TO_TICKS(1000));
}

esp_err_t controller_bridge_config(void)
{
    uart_config_t config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    esp_err_t state = installUartDriver(
        CONTROLLER_BRIDGE,
        CONTROLLER_RX_BUFFER_SIZE,
        CONTROLLER_TX_BUFFER_SIZE,
        0,
        NULL,
        0);
    if (state != ESP_OK)
    {
        return state;
    }

    state = configUartParam(CONTROLLER_BRIDGE, &config);
    if (state != ESP_OK)
    {
        return state;
    }

    state = setUartPin(
        CONTROLLER_BRIDGE,
        CONTROLLER_TX_PIN,
        CONTROLLER_RX_PIN,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE);
    if (state != ESP_OK)
    {
        return state;
    }

    return checkCurrentConfigs(CONTROLLER_BRIDGE);
}
