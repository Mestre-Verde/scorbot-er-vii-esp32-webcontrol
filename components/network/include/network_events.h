#if !defined(NETWORK_EVENTS_H)
#define NETWORK_EVENTS_H

#include "esp_err.h"

/**
 * @brief Registers handlers for Wi-Fi and IP network events.
 *
 * Registers the network event handler for all Wi-Fi events, the event
 * indicating that the station has obtained an IP address, and the event
 * indicating that the station has lost its IP address.
 *
 * If a registration fails, previously registered handlers are unregistered
 * to avoid leaving a partially configured event system.
 *
 * @return
 *      - ESP_OK: All event handlers were registered successfully.
 *      - Other error code: Registration failed.
 */
esp_err_t registerNetworkEvents(void);

#endif

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