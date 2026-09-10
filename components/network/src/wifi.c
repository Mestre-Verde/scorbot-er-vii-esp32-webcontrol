#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"
#include "nvs_flash.h"

// const char AP_SSID[MAX_SSID_LEN] = "Scorbot Acess Point";                                                     // 19 + \0
// const char AP_PASS[MAX_PASSPHRASE_LEN] = "4y-Wy4G#--WyayyOq#Oqqqq4i44ii4WWWaWWWaWWOOaaaOOOWWWWWW4&tavsa0"; // 63 + \0

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

    wifi_init_config_t manualWifiConfig = WIFI_INIT_CONFIG_DEFAULT();

    state = initWifiService(&manualWifiConfig);
    if (state != ESP_OK)
    {
        printf("Failed to initialize WIFI: %s\n", esp_err_to_name(state));
        return state;
    }

    puts("WiFi iniciado.");

    state = esp_wifi_start();
    if (state != ESP_OK)
    {
        printf("Failed to start WIFI: %s\n", esp_err_to_name(state));
        return state;
    }

    puts("WiFi started.");

    return ESP_OK;
}

esp_err_t connectToSta(const char ssid[], const char password[], uint8_t retries)
{
    printf("\n========== Wi-Fi STA CONNECT ==========\n");
    printf("Number of retries: %u", retries);

    // Get the current Wi-Fi operating mode
    wifi_mode_t currentWifiMode = WIFI_MODE_NULL;

    esp_err_t state = getWifiMode(&currentWifiMode);

    if (state != ESP_OK)
    {
        printf("[STA] Failed to get Wi-Fi mode: %s\n", esp_err_to_name(state));
        return state;
    }

    printf("[STA] Current Wi-Fi mode: %d\n", currentWifiMode);

    // Ensure that the Wi-Fi interface has STA capability enabled
    if (currentWifiMode != WIFI_MODE_STA && currentWifiMode != WIFI_MODE_APSTA)
    {
        printf("[STA] STA mode not enabled. Switching to STA...\n");

        state = setWifiMode(WIFI_MODE_STA);
        if (state != ESP_OK)
        {
            printf("[STA] Failed to set STA mode: %s\n",
                   esp_err_to_name(state));
            return state;
        }

        printf("[STA] STA mode enabled.\n");
    }
    else
    {
        printf("[STA] STA capability already enabled.\n");
    }

    bool persistentConnection = retries > 2;

    // Create the STA configuration using default values
    wifi_config_t wifiConfig = {
        .sta = {
            .ssid = "",
            .password = "",
            .channel = 0,       // no preference
            .bssid_set = false, // no need of users to check the MAC address of the router.
            .scan_method = persistentConnection ? WIFI_ALL_CHANNEL_SCAN : WIFI_FAST_SCAN,
            .failure_retry_cnt = retries}};

    // Copy the network credentials into the Wi-Fi configuration
    strncpy((char *)wifiConfig.sta.ssid, ssid, MAX_SSID_LEN - 1);

    strncpy((char *)wifiConfig.sta.password, password, MAX_PASSPHRASE_LEN - 1);

    printf("[STA] SSID configured: %s\n", (char *)wifiConfig.sta.ssid);

    printf("[STA] Channel: %u (automatic)\n", wifiConfig.sta.channel);

    printf("[STA] BSSID selection: automatic\n");

    // Apply the STA configuration to the Wi-Fi driver
    state = esp_wifi_set_config(WIFI_IF_STA, &wifiConfig);
    if (state != ESP_OK)
    {
        printf("[STA] Failed to apply configuration: %s\n",
               esp_err_to_name(state));
        return state;
    }

    printf("[STA] Configuration applied successfully.\n");

    // Start the connection to the configured access point
    state = esp_wifi_connect();
    if (state != ESP_OK)
    {
        printf("[STA] Failed to start connection: %s\n", esp_err_to_name(state));
        return state;
    }

    printf("[STA] Connection attempt started.\n");
    printf("=======================================\n\n");
    return state;
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