#ifndef CONTROLLER_UART
#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"

/**
 * @brief Configures and initializes the controller communication bridge.
 *
 * Prepares the communication interface used to exchange data
 * between the ESP32 and the controller.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t controller_bridge_config(void);

/**
 * @brief Sends data to the controller.
 *
 * Transmits the specified buffer through the controller UART interface.
 *
 * @param buffer Pointer to the buffer containing the bytes to transmit.
 * @param len Number of bytes to transmit.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t SendBytesToController(const unsigned char *buffer, size_t len);

/**
 * @brief Reads bytes from the controller UART RX buffer.
 *
 * Attempts to read received bytes from the controller UART interface,
 * waiting according to the specified timeout.
 *
 * @param buffer Destination buffer for the received bytes.
 * @param len Maximum number of bytes to read.
 * @param timeout Maximum wait time for received data, expressed in FreeRTOS ticks.
 *
 * @return Number of bytes read on success, or a negative value on failure,
 *         depending on the implementation.
 */
int ReadBytesFromController(unsigned char *buffer, size_t len, TickType_t timeout);

#endif // !CONTROLLER_UART
