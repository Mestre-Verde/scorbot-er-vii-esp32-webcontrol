#ifndef PC_UART

#include "esp_err.h"
#include "driver/uart.h"

/**
 * @brief Configures and initializes the PC communication bridge.
 *
 * Prepares the communication interface used to exchange data
 * between the ESP32 and the PC.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t pc_bridge_config(void);

/**
 * @brief Sends data to the PC.
 *
 * Transmits the specified buffer through the PC communication interface.
 *
 * @param buffer Pointer to the character buffer to transmit.
 * @param len Number of bytes to transmit.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t SendBytesToPC(const unsigned char buffer[], size_t len);

/**
 * @brief Reads data received from the PC.
 *
 * Attempts to read up to the specified number of bytes from the
 * PC communication interface, waiting according to the supplied timeout.
 *
 * @param buffer Pointer to the destination buffer where received bytes
 *               will be stored.
 * @param len Maximum number of bytes to read.
 * @param timeout Maximum time to wait for data, expressed in FreeRTOS ticks.
 *
 * @return Number of bytes read on success, or a negative value on failure,
 *         depending on the implementation.
 */
int ReadBytesFromPC(unsigned char *buffer, size_t len, TickType_t timeout);

#endif // !PC_UART