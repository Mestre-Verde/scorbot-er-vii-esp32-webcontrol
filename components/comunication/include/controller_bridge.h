#ifndef CONTROLLER_UART

#include "esp_err.h"
#include "driver/uart.h"

esp_err_t controller_bridge_config(void);

esp_err_t SendBytesToController(const char buffer[], size_t len);

int ReadBytesFromController(char *buffer, size_t len, TickType_t timeout);

#endif // !CONTROLLER_UART

