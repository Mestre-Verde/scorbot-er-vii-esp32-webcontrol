#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_err.h"

/**
 * @brief Starts the HTTP server and registers the available URI handlers.
 *
 * Initializes the HTTP server using the default configuration and registers
 * the GET "/" handler to serve the web interface and the POST "/upload"
 * handler to receive program uploads.
 *
 * If the server fails to start or a URI handler cannot be registered,
 * the function stops the server when necessary and returns the error code.
 *
 * @param arg Optional argument (unused).
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t startHttpServer(void);

#endif