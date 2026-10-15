#include <stdio.h>
#include <string.h>

#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"
#include "nvs_flash.h"
#include "network_events.h"

#include "esp_netif.h"

esp_err_t initNvsService(void)
{
    return nvs_flash_init();
}

esp_err_t initWifiService(wifi_init_config_t *wifiConfig)
{
    return esp_wifi_init(wifiConfig);
}

esp_err_t initEventService(void)
{
    return esp_event_loop_create_default();
}

/**
 * @brief Set a wifi mode.
 *
 * @param mode Wifi mode to define
 */
esp_err_t setWifiMode(wifi_mode_t mode)
{
    return esp_wifi_set_mode(mode);
}

esp_err_t getWifiMode(wifi_mode_t *var)
{
    // Check if the reference is a valid memory adress
    if (var == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    // Get the current operating mode
    esp_err_t state = esp_wifi_get_mode(var);

    return state;
}

esp_err_t getStaInfo(void)
{
    wifi_ap_record_t apInfo;

    esp_err_t state = esp_wifi_sta_get_ap_info(&apInfo);

    if (state == ESP_OK)
    {
        printf("SSID: %s\n", apInfo.ssid);
        printf("RSSI: %d dBm\n", apInfo.rssi);
        printf("Channel: %d\n", apInfo.primary);
    }

    return state;
}

esp_err_t connectToSta(const char *ssid, const char *password, uint8_t retries)
{
    if (ssid == NULL || password == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    printf("\n========== Wi-Fi STA CONNECT ==========\n");
    printf("Number of retries: %" PRIu8 "\n", retries);

    // Check the current Wi-Fi mode
    wifi_mode_t currentWifiMode = WIFI_MODE_NULL;

    esp_err_t state = getWifiMode(&currentWifiMode);

    if (state != ESP_OK)
    {
        printf("[STA] Failed to get Wi-Fi mode: %s\n", esp_err_to_name(state));
        return state;
    }

    // Ensure STA mode is enabled
    if (currentWifiMode != WIFI_MODE_STA && currentWifiMode != WIFI_MODE_APSTA)
    {
        printf("[STA] Enabling STA mode...\n");

        state = setWifiMode(WIFI_MODE_STA);

        if (state != ESP_OK)
        {
            printf("[STA] Failed to set STA mode: %s\n", esp_err_to_name(state));
            return state;
        }
    }

    // Prepare the station configuration
    wifi_config_t wifiConfig = {0};

    size_t ssidLength = strlen(ssid);
    size_t passwordLength = strlen(password);

    if (ssidLength == 0 || ssidLength > sizeof(wifiConfig.sta.ssid) || passwordLength >= sizeof(wifiConfig.sta.password))
    {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(wifiConfig.sta.ssid, ssid, ssidLength);
    memcpy(wifiConfig.sta.password, password, passwordLength);

    wifiConfig.sta.channel = 0;
    wifiConfig.sta.bssid_set = false;

    // Apply configuration
    state = esp_wifi_set_config(WIFI_IF_STA, &wifiConfig);

    if (state != ESP_OK)
    {
        printf("[STA] Failed to apply configuration: %s\n", esp_err_to_name(state));
        return state;
    }

    printf("[STA] Configuration applied for SSID: %s\n", ssid);

    // Start the connection attempt
    state = esp_wifi_connect();

    if (state != ESP_OK)
    {
        printf("[STA] Failed to start connection: %s\n", esp_err_to_name(state));
        return state;
    }

    printf("[STA] Connection attempt started.\n");
    printf("=======================================\n");

    return ESP_OK;
}

esp_err_t startWifiService(void)
{
    esp_err_t state = ESP_OK;

    state = initNvsService();
    if (state != ESP_OK)
    {
        printf("Failed to initialize NVS: %s\n", esp_err_to_name(state));
        return state;
    }

    puts("NVS iniciado.");

    state = initEventService();
    if (state != ESP_OK)
    {
        printf("Failed to initialize event loop: %s\n", esp_err_to_name(state));
        return state;
    }

    puts("Event loop iniciado.");

    state = esp_netif_init();
    if (state != ESP_OK)
    {
        printf("Failed to initialize network interface: %s\n", esp_err_to_name(state));
        return state;
    }

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();

    if (sta_netif == NULL)
    {
        printf("Failed to create default Wi-Fi STA interface.\n");
        return ESP_FAIL;
    }

    wifi_init_config_t manualWifiConfig = WIFI_INIT_CONFIG_DEFAULT();

    state = initWifiService(&manualWifiConfig);
    if (state != ESP_OK)
    {
        printf("Failed to initialize WIFI: %s\n", esp_err_to_name(state));
        return state;
    }

    puts("WiFi iniciado.");

    // Registar os handlers dos eventos antes de iniciar o Wi-Fi
    state = registerNetworkEvents();
    if (state != ESP_OK)
    {
        printf("Failed to register network events: %s\n", esp_err_to_name(state));
        return state;
    }

    puts("Eventos de rede registados.");

    // Iniciar o Wi-Fi uma única vez
    state = esp_wifi_start();
    if (state != ESP_OK)
    {
        printf("Failed to start WIFI: %s\n", esp_err_to_name(state));
        return state;
    }

    puts("WiFi iniciado com sucesso.");

    return ESP_OK;
}

/*
esp_wifi_init()
       ↓
inicializa driver

esp_wifi_set_mode(STA)
       ↓
define STA

esp_wifi_set_config(...)
       ↓
diz QUAL AP usar

esp_wifi_start()
       ↓
liga/inicia Wi-Fi

esp_wifi_connect()
       ↓
tenta efetivamente associar ao AP
*/