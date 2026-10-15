#ifndef PC_UART

#include "esp_err.h"
#include "driver/uart.h"

esp_err_t pc_bridge_config(void);

esp_err_t SendBytesToPC(const char buffer[], size_t len);

int ReadBytesFromPC(unsigned char *buffer, size_t len, TickType_t timeout);

#endif // !PC_UART