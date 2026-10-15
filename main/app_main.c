/**
 * @file app_main.c
 * @brief Program start point, main.
 */
#include <stdio.h>
#include <esp_err.h>

#include "wifi.h"
#include "uart_common.h"
#include "core/tasks.h"

void app_main(void)
{
    esp_err_t state = ESP_OK;


    // UART STARTUP
    state = startUARTService();
    if (state != ESP_OK)
    {
        printf("UART fail.Value:%s\n", esp_err_to_name(state));
    }

    state = UARTTaskInit();
    if (state != ESP_OK)
    {
        printf("UART init task failed.Value:%s\n",esp_err_to_name(state));
    }

    //  WIFI STARTUP
    state = startWifiService();

    if (state != ESP_OK)
    {
        printf("Houve um erro ao iniciar o Wi-Fi: %s\n", esp_err_to_name(state));
        return;
    }

    const char *ssid = "Black hat's Acess Point";
    const char *pass = "THE ONE GUY";

    state = connectToSta(ssid, pass, 5);

    if (state != ESP_OK)
    {
        printf("Failed to start STA connection: %s\n", esp_err_to_name(state));
        return;
    }

    // state = start_mdns_service();
    // if (state)
    // {
    //     printf("MDNS Init failed: %s\n", esp_err_to_name(state));
    //     return;
    // }
}
