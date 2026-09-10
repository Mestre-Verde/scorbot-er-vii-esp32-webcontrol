#include <stdio.h>
#include <esp_event_base.h>
#include <wifi.h>
#include <esp_netif_types.h>

static void network_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_CONNECTED:
            printf("[EVENT] STA connected.\n");
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            printf("[EVENT] STA disconnected.\n");
            break;
        }
    }

    if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
            printf("[EVENT] Got IP address.\n");
            break;
        }
    }
}