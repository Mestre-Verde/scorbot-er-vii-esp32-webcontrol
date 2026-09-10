#ifndef PC_UART

#include "esp_err.h"
#include "driver/uart.h"
#include "uart_common.h"

esp_err_t SendBytesToPC(const char buffer[], size_t len);

int ReadBytesFromPC(char *buffer, size_t len, TickType_t timeout);

esp_err_t pc_bridge_config(void);

#endif // !PC_UART