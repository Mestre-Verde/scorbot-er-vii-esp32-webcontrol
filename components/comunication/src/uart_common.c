/**
 * @file uart_common.c
 * @brief Main Uart file with general uart functions.
 */

#include "esp_err.h"
#include "driver/uart.h"

#include "pc_bridge.h"
#include "controller_bridge.h"
#include "utils.h"

// | Teclas   | Byte |  HEX | Nome |
// | -------- | ---: | ---: | ---- |
// | `Ctrl+N` |   14 | `0E` | SO   |
// | `Ctrl+Q` |   17 | `11` | XON  |
// | `Ctrl+S` |   19 | `13` | XOFF |
// | `Enter`  |   13 | `0D` | CR   |
// | `Ctrl+J` |   10 | `0A` | LF   |

// Ctrl+N
static bool printHex = false;

void pcToControllerTask(void *arg)
{
       unsigned char buffer[64];

       while (1)
       {
              int len = ReadBytesFromPC(buffer, sizeof(buffer), pdMS_TO_TICKS(10));

              if (len <= 0)
              {
                     continue;
              }

              for (int i = 0; i < len; i++)
              {
                     unsigned char byte = buffer[i];

                     // Ignore non ASCII caracters
                     if (byte > 0x7F)
                     {
                            printf("Non ASCII byte detected: %02X\n", byte);
                            continue;
                     }

                     if (byte == 0x0E) // Ctrl+N
                     {
                            printHex = !printHex;
                            continue;
                     }
                     esp_err_t state = SendBytesToController(buffer, 1);

                     if (state != ESP_OK)
                     {
                            printf("UART transmission failed: %s\n", esp_err_to_name(state));
                     }
              }
       }
}

void controllerToPcTask(void *arg)
{
       unsigned char buffer[64];

       while (1)
       {
              int len = ReadBytesFromController(buffer, sizeof(buffer), pdMS_TO_TICKS(10));

              if (len <= 0)
              {
                     continue;
              }

              // Process received bytes and remove XON/XOFF.
              for (int i = 0; i < len; i++)
              {
                     unsigned char c = buffer[i];
                     if (c == XOFF)
                     {
                            puts("XOFF detetado!");
                     }
                     else if (c == XON)
                     {
                            puts("XON detetado!");
                     }
              }

              if (printHex)
              {
                     PrintBytesHEX((char *)buffer, len);
              }
              else
              {
                     SendBytesToPC((char *)buffer, len);
              }
       }
}

//-------

static bool isUartDriverInstalled(uart_port_t uart_num)
{
       return uart_is_driver_installed(uart_num);
}

esp_err_t checkCurrentConfigs(uart_port_t uart_num)
{
       uint32_t baudrate;
       uart_word_length_t word_length;
       uart_parity_t parity;
       uart_stop_bits_t stop_bits;
       uart_hw_flowcontrol_t flow_ctrl;

       esp_err_t state;

       if (!isUartDriverInstalled(uart_num))
       {
              return ESP_ERR_INVALID_STATE;
       }

       state = uart_get_baudrate(uart_num, &baudrate);
       if (state != ESP_OK)
              return state;

       state = uart_get_word_length(uart_num, &word_length);
       if (state != ESP_OK)
              return state;

       state = uart_get_parity(uart_num, &parity);
       if (state != ESP_OK)
              return state;

       state = uart_get_stop_bits(uart_num, &stop_bits);
       if (state != ESP_OK)
              return state;

       state = uart_get_hw_flow_ctrl(uart_num, &flow_ctrl);
       if (state != ESP_OK)
              return state;

       printf("\nUART Configuration:\n");
       printf("Baudrate: %lu\n", baudrate);
       switch (word_length)
       {
       case UART_DATA_5_BITS:
              printf("Word length: 5 bits\n");
              break;

       case UART_DATA_6_BITS:
              printf("Word length: 6 bits\n");
              break;

       case UART_DATA_7_BITS:
              printf("Word length: 7 bits\n");
              break;

       case UART_DATA_8_BITS:
              printf("Word length: 8 bits\n");
              break;

       default:
              printf("Word length: Unknown\n");
              break;
       }
       printf("Parity: %d\n", parity);
       printf("Stop bits: %d\n", stop_bits);
       printf("Flow control: %d\n", flow_ctrl);

       return ESP_OK;
}

esp_err_t installUartDriver(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags)
{
       return uart_driver_install(uart_num, rx_buffer_size, tx_buffer_size, queue_size, uart_queue, intr_alloc_flags);
}

// Configure UART parameters
esp_err_t configUartParam(uart_port_t uart_num, uart_config_t *uart_config)
{
       return uart_param_config(uart_num, uart_config);
}

// define the uart pins
esp_err_t setUartPin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num, int dtr_io_num, int dsr_io_num)
{
       return uart_set_pin(uart_num, tx_io_num, rx_io_num, rts_io_num, cts_io_num, dtr_io_num, dsr_io_num);
}

esp_err_t startUARTService(void)
{
       // Inicia a PC_Bridge
       esp_err_t state = pc_bridge_config();
       if (state != ESP_OK)
       {
              return state;
       }

       state = controller_bridge_config();
       if (state != ESP_OK)
       {
              return state;
       }

       return ESP_OK;
}

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