#include <stdio.h>
// #include <inttypes.h>

#include "wifi.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "http_server.h"


static void network_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    (void)arg;

    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_CONNECTED:
            printf("[EVENT] STA connected to access point.\n");

            esp_err_t state = getStaInfo();
            if (state != ESP_OK)
            {
                printf("Could not get STA info: %s\n", esp_err_to_name(state));
            }
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
        {
            wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;

            printf("[EVENT] STA disconnected. Reason: %d\n", event->reason);
            break;
        }

        default:
            printf("[EVENT] Unknown event: %i.\n", (int)event_id);
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
        {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

            printf("[EVENT] Got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));

            static bool httpServerStarted = false;

            if (!httpServerStarted)
            {
                esp_err_t state = startHttpServer();

                if (state == ESP_OK)
                {
                    httpServerStarted = true;
                    printf("[EVENT] Servidor HTTP iniciado.\n");
                }
                else
                {
                    printf("[ERROR] Falha ao iniciar servidor HTTP: %s\n", esp_err_to_name(state));
                }
            }

            break;
        }

        case IP_EVENT_STA_LOST_IP:
            printf("[EVENT] STA lost IP address.\n");
            break;

        default:
            printf("[EVENT] Unknown event: %i.\n", (int)event_id);
            break;
        }
    }
}

esp_err_t registerNetworkEvents(void)
{
    esp_err_t state;

    state = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, network_event_handler, NULL);
    if (state != ESP_OK)
    {
        return state;
    }

    state = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, network_event_handler, NULL);
    if (state != ESP_OK)
    {
        esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, network_event_handler);
        return state;
    }

    state = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_LOST_IP, network_event_handler, NULL);

    if (state != ESP_OK)
    {
        esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, network_event_handler);

        esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, network_event_handler);
        return state;
    }

    return ESP_OK;
}

//     A["Inicializar NVS"] --> B["Criar event loop"]
//     B --> C["Inicializar netif e interfaces"]
//     C --> D["Inicializar driver Wi-Fi"]
//     D --> E["Registar handlers"]
//     E --> F["Definir modo STA"]
//     F --> G["Configurar credenciais"]
//     G --> H["Iniciar driver Wi-Fi"]
//     H --> I["Chamar esp_wifi_connect"]
//     I --> J["Aguardar eventos"]
//     J --> K["IP_EVENT_STA_GOT_IP"]
