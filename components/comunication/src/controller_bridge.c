/*
UART1 is the Controller
*/
#include <stdio.h>

#include "esp_err.h"
#include "driver/uart.h"
#include "uart_common.h"

#define XON 0x11  // Represents the byte "DC1".
#define XOFF 0x13 // Represents the byte "DC3".

#define CONTROLLER_BRIDGE UART_NUM_1
#define CONTROLLER_RX_PIN 18
#define CONTROLLER_TX_PIN 17

#define CONTROLLER_RX_BUFFER_SIZE (SOC_UART_FIFO_LEN + 1)
#define CONTROLLER_TX_BUFFER_SIZE (SOC_UART_FIFO_LEN + 1)

// Control Var, is connceted with the XON\XOFF protocoll (XON -> var = 1,XOFF -> var = 0)
bool canSendCommands = true;

int ReadBytesFromController(char *buffer, size_t len, TickType_t timeout)
{
    return uart_read_bytes(CONTROLLER_BRIDGE, buffer, len, timeout);
}

/*
ao analizar os dados verificar XON e XOFF com:

if (byte == XOFF)
    canSendCommands = false;
    continue;

else if (byte == XON)
    canSendCommands = true;
    continue;

*/

void sendByteSlowly(const char *msg) {}

esp_err_t controller_bridge_config(void)
{
    uart_config_t config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    esp_err_t state = installUartDriver(CONTROLLER_BRIDGE, CONTROLLER_RX_BUFFER_SIZE, CONTROLLER_TX_BUFFER_SIZE, 0, NULL, 0);
    if (state != ESP_OK)
    {
        return state;
    }

    state = configUartParam(CONTROLLER_BRIDGE, &config);
    if (state != ESP_OK)
    {
        return state;
    }

    state = setUartPin(CONTROLLER_BRIDGE, CONTROLLER_TX_PIN, CONTROLLER_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (state != ESP_OK)
    {
        return state;
    }

    return checkCurrentConfigs(CONTROLLER_BRIDGE);
}
