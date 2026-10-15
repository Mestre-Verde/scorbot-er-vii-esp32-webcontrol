#if !defined(WIFI)
#define WIFI

#include "esp_err.h"
#include "esp_wifi_types_generic.h"

/**
 * @brief Starts the Wi-Fi service.
 *
 * Initializes and starts the Wi-Fi functionality implemented by the service.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t startWifiService(void);

/**
 * @brief Gets the current Wi-Fi operating mode.
 *
 * Retrieves the current Wi-Fi mode and stores it in the supplied variable.
 *
 * @param[out] currentMode Pointer to the variable where the current
 *                         Wi-Fi mode will be stored.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t getWifiMode(wifi_mode_t *currentMode);

/**
 * @brief Retrieves and displays information about the connected Wi-Fi AP.
 * 
 * Gets the access point information associated with the current station
 * connection and displays its SSID, BSSID, signal strength, channel,
 * authentication mode, cipher, bandwidth and supported PHY capabilities.
 * 
 * @return ESP_OK if the AP information is retrieved successfully.
 */
esp_err_t getStaInfo(void);

/**
 * @brief Connects the ESP32 to a Wi-Fi network in station mode.
 *
 * Attempts to connect to the specified Wi-Fi network. The retries
 * parameter controls the connection retry behavior. According to the
 * current implementation, values greater than 2 also trigger a scan
 * across Wi-Fi channels.
 *
 * @param[in] ssid Name of the Wi-Fi network to connect to.
 * @param[in] password Password used to authenticate with the network.
 * @param[in] retries Number of connection attempts or retry parameter
 *                    used by the implementation.
 *
 * @return ESP_OK on success, or an ESP-IDF error code on failure.
 */
esp_err_t connectToSta(const char *ssid, const char *password, uint8_t retries);

#endif // WIFI

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