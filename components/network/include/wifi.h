#if !defined(WIFI)
#define WIFI

#include "esp_err.h"
#include "esp_wifi_types_generic.h"

/**
 * @brief start the wifi service.
 */
esp_err_t startWifiService(void);

/**
 * @brief Get the current Wi-Fi operating mode.
 *
 * @param[out] currentMode Pointer to the variable where the current mode will be stored.
 */
esp_err_t getWifiMode(wifi_mode_t *currentMode);

/**
 * @brief Get sta info and display it.
 */
esp_err_t getStaInfo(void);

/**
 * @brief connects to a STA network
 * 
 * @param ssid network name
 * @param password network password
 * @param retries number of times to try to connect, > 2 will do a scan of all channels
 */
esp_err_t connectToSta(const char ssid[], const char password[], uint8_t retries);

#endif // WIFI


