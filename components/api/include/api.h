#ifndef API_H
#define API_H

#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>

#define API_UPLOAD_CHUNK_SIZE 120

/**
 * @brief Initializes a new file upload.
 *
 * Stores the filename and expected file size and resets the upload state.
 *
 * @param filename Name of the file being uploaded.
 * @param file_size Expected size of the file in bytes.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t api_upload_begin(const char *filename, size_t file_size);

/**
 * @brief Processes the next chunk of an uploaded file.
 *
 * Accepts chunks of up to API_UPLOAD_CHUNK_SIZE bytes and tracks
 * the total number of bytes received.
 *
 * @param data Pointer to the chunk data.
 * @param length Number of bytes in the chunk.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t api_upload_chunk(const uint8_t *data, size_t length);

/**
 * @brief Finalizes the current file upload.
 *
 * Verifies that the expected number of bytes has been received.
 *
 * @return ESP_OK if the upload is complete, or an error code otherwise.
 */
esp_err_t api_upload_end(void);

#endif // API_H