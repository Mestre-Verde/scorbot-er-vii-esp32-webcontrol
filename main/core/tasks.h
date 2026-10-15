#if !defined(TASKS)
#define TASKS

#include "esp_err.h"

/**
 * @brief Creates the UART communication tasks.
 *
 * Creates two FreeRTOS tasks to handle bidirectional communication between
 * the PC and the controller:
 * - PC to Controller: forwards data from the PC to the controller.
 * - Controller to PC: forwards data from the controller to the PC.
 *
 * Both tasks are created with a stack size of 4096 bytes and a priority of 5.
 * The function prints the creation result of each task to the console.
 *
 * @return ESP_OK on completion.
 */
esp_err_t UARTTaskInit(void);

#endif // TASKS

/*
Task PC → Controller
        │
        ▼
  uart_read_bytes()
        │
        │ não há dados?
        │
        ▼
     BLOQUEIA
        │
        │ chega um byte
        ▼
    acorda a task
        │
        ▼
    buffer recebe
        │
        ▼
SendBytesToController()
        │
        ▼
    volta ao while
        │
        ▼
  uart_read_bytes()
        │
        ▼
     BLOQUEIA
     */