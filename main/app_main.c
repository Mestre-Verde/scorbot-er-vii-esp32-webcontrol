#include <stdio.h>
#include <esp_err.h>
// #include "wifi.h"
#include "uart_common.h"
#include "core/tasks.h"

void app_main(void)
{
    esp_err_t state = ESP_OK;

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
    


    // esp_err_t state = startWifiService();
    // if (state != ESP_OK)
    // {
    //     printf("Houve um erro ao iniciar o WiFi: %s\n", esp_err_to_name(state));
    //     return;
    // }

    // const char ssid[] = "NOS-BF54";
    // const char pass[] = "CN43Y74F";
    // state = connectToSta(ssid, pass,5);
    // if (state)
    // {
    //     printf("Failed to connect to STA: %s\n", esp_err_to_name(state));
    //     return;
    // }

    // state = getStaInfo();
    // if (state)
    // {
    //     printf("Could not get STA info: %s\n", esp_err_to_name(state));
    //     return;
    // }

    // state = start_mdns_service();
    // if (state)
    // {
    //     printf("MDNS Init failed: %s\n", esp_err_to_name(state));
    //     return;
    // }
}
