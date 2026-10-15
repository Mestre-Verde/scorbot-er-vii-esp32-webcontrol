#ifndef CONTROLLER_UART
#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"

bool controller_handle_control_byte(uint8_t byte);

esp_err_t controller_bridge_config(void);

esp_err_t SendBytesToController(const unsigned char *buffer, size_t len);

/**
 * @brief Read bytes from the controller UART RX buffer.
 * @param buffer Destination buffer.
 * @param len Maximum number of bytes to read.
 * @param timeout Maximum wait time for received data.
 */
int ReadBytesFromController(unsigned char *buffer, size_t len, TickType_t timeout);

#endif // !CONTROLLER_UART
