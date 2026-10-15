/**
 * @file tasks.c
 * @brief File to Initiate the interupt tasks
 */
#include <stdio.h>

#include "esp_err.h"
#include "portmacro.h"

#include "uart_common.h"


esp_err_t UARTTaskInit(void)
{
    // criar as duas tasks
    puts("Creating PC -> Controller task");
    BaseType_t ret1 = xTaskCreate(
        pcToControllerTask,
        "PC_to_Controller",
        4096,
        NULL,
        5,
        NULL);

    printf("PC -> Controller task result: %d\n", ret1);

    puts("Creating Controller -> PC task");
    BaseType_t ret2 = xTaskCreate(
        controllerToPcTask,
        "Controller_to_PC",
        4096,
        NULL,
        5,
        NULL);

    printf("Controller -> PC task result: %d\n", ret2);
    return ESP_OK;
}

