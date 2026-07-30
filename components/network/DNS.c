#include <stdio.h>
#include "esp_err.h"
#include "mdns.h"

// max number of bytes is 65: string + \0
const char hostName[MDNS_NAME_MAX_LEN] = "scorbot-api";
const char hostInstanceName[MDNS_NAME_MAX_LEN] = "Scorbot API";

esp_err_t start_mdns_service()
{
    // initialize mDNS service
    esp_err_t err = mdns_init();
    if (err)
    {
        printf("MDNS Init failed: %s\n", esp_err_to_name(err));
        return ESP_FAIL;
    }

    // set hostname
    err = mdns_hostname_set(hostName);
    if (err)
    {
        printf("MDNS set hostname failed: %s\n", esp_err_to_name(err));
        return ESP_FAIL;
    }

    // set default instance
    err  = mdns_instance_name_set(hostInstanceName);
    if (err)
    {
        printf("MDNS instance hostname failed: %s\n", esp_err_to_name(err));
        return ESP_FAIL;
    }

    return ESP_OK;
}