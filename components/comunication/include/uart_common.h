#ifndef UART_COM

#include "esp_err.h"
#include "driver/uart.h"

esp_err_t installUartDriver(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags);

esp_err_t configUartParam(uart_port_t uart_num, uart_config_t *uart_config);

esp_err_t setUartPin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num, int dtr_io_num, int dsr_io_num);

esp_err_t checkCurrentConfigs(uart_port_t uart_num);

bool isUartDriverInstalled(uart_port_t uart_num);

esp_err_t UARTCommonInit(void);

esp_err_t startUARTService(void);

#endif // !UART_COM
