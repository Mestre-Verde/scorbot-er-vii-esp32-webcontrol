#ifndef UART_COM

#include "esp_err.h"
#include "driver/uart.h"

/**
 * @brief Task that transfers data from the PC to the controller.
 *
 * Receives data from the PC-side communication interface and forwards it
 * to the controller through UART.
 *
 * @param arg Task argument. May be NULL if no argument is required.
 * @return This task normally runs indefinitely and does not return.
 */
void pcToControllerTask(void *arg);

/**
 * @brief Task that transfers data from the controller to the PC.
 *
 * Receives data from the controller through UART and forwards it
 * to the PC-side communication interface.
 *
 * @param arg Task argument. May be NULL if no argument is required.
 * @return This task normally runs indefinitely and does not return.
 */
void controllerToPcTask(void *arg);

/**
 * @brief Installs the UART driver.
 *
 * Configures the UART driver buffers, event queue and interrupt allocation
 * flags according to the supplied parameters.
 *
 * @param uart_num UART port to install the driver on.
 * @param rx_buffer_size Size of the RX buffer in bytes.
 * @param tx_buffer_size Size of the TX buffer in bytes.
 * @param queue_size Number of UART events the queue can hold.
 * @param uart_queue Pointer to the handle that receives the UART event queue.
 * @param intr_alloc_flags Flags used when allocating UART interrupts.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t installUartDriver(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags);

/**
 * @brief Configures the UART communication parameters.
 *
 * Applies the supplied UART configuration, including baud rate,
 * data bits, parity, stop bits and hardware flow control.
 *
 * @param uart_num UART port to configure.
 * @param uart_config Pointer to the UART configuration structure.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t configUartParam(uart_port_t uart_num, uart_config_t *uart_config);

/**
 * @brief Assigns GPIO pins to the UART signals.
 *
 * Configures the TX, RX and optional hardware flow-control pins
 * for the specified UART port.
 *
 * @param uart_num UART port to configure.
 * @param tx_io_num GPIO number used for UART TX.
 * @param rx_io_num GPIO number used for UART RX.
 * @param rts_io_num GPIO number used for UART RTS, or UART_PIN_NO_CHANGE.
 * @param cts_io_num GPIO number used for UART CTS, or UART_PIN_NO_CHANGE.
 * @param dtr_io_num GPIO number used for UART DTR, if supported by the API.
 * @param dsr_io_num GPIO number used for UART DSR, if supported by the API.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t setUartPin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num, int dtr_io_num, int dsr_io_num);

/**
 * @brief Checks the current configuration of a UART port.
 *
 * Performs the configuration checks implemented by the function
 * for the specified UART port.
 *
 * @param uart_num UART port whose configuration is to be checked.
 *
 * @return ESP_OK if the checks pass, or an ESP-IDF error code otherwise.
 */
esp_err_t checkCurrentConfigs(uart_port_t uart_num);

/**
 * @brief Starts the UART communication service.
 *
 * Initializes or starts the UART communication functionality
 * implemented by the service.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t startUARTService(void);

#endif // !UART_COM

/*
1. Install Driver
       ↓
2. Communication Parameters
       ↓
3. Communication Pins
       ↓
4. Communication
       ↓
5. Interrupts (opcional)
       ↓
6. Delete Driver (quando já não precisamos)

-----------------------
Reception:
programa
   │
   │ uart_write_bytes()
   ▼
TX buffer/FIFO
   │
   ▼
UART FSM
   │
   ├── start bit
   ├── data bits
   ├── parity (se existir)
   └── stop bit
   │
   ▼
GPIO TX

Transmissions:
GPIO RX
   │
   ▼
UART FSM
   │
   ├── deteta start bit
   ├── lê os bits
   ├── verifica parity (se existir)
   └── reconstrói o byte
   │
   ▼
RX FIFO
   │
   │ uart_read_bytes()
   ▼
programa
*/