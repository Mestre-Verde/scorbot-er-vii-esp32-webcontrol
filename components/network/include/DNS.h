#if !defined(DNS)
#define DNS

#include "esp_err.h"

/**
 * @brief Start the DNS service with the defined names.
 */
esp_err_t start_mdns_service();

#endif // DNS