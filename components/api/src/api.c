#include "api.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdint.h>

#include "controller_bridge.h"

#define MAX_FILENAME_LENGTH 128

typedef struct
{
    char filename[MAX_FILENAME_LENGTH];
    size_t file_size;
    size_t bytes_received;
    int active;
} upload_state_t;

static upload_state_t upload = {0};

static bool is_allowed_file_type(const char *filename)
{
    if (filename == NULL)
    {
        return false;
    }

    const char *ext = strrchr(filename, '.');

    if (ext == NULL)
    {
        return false;
    }

    return (strcasecmp(ext, ".txt") == 0 || strcasecmp(ext, ".acl") == 0);
}

static void upload_abort(void)
{
    upload.active = false;
    upload.bytes_received = 0;
}

esp_err_t api_upload_begin(const char *filename, size_t file_size)
{
    if (!is_allowed_file_type(filename) || filename[0] == '\0' || file_size == 0 || strlen(filename) >= MAX_FILENAME_LENGTH)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (upload.active)
    {
        return ESP_ERR_INVALID_STATE;
    }

    memset(&upload, 0, sizeof(upload));

    strcpy(upload.filename, filename);
    upload.file_size = file_size;
    upload.bytes_received = 0;
    upload.active = 1;

    printf("[API] Upload started: %s (%zu bytes)\n", upload.filename, upload.file_size);

    return ESP_OK;
}

esp_err_t api_upload_chunk(const uint8_t *data, size_t length)
{
    if (!upload.active)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (data == NULL || length == 0 || length > API_UPLOAD_CHUNK_SIZE)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (upload.bytes_received > upload.file_size || length > upload.file_size - upload.bytes_received)
    {
        return ESP_ERR_INVALID_SIZE;
    }

    for (size_t i = 0; i < length; i++)
    {
        if (data[i] > 0x7F)
        {
            printf("[API] WARNING: Non-ASCII byte detected: 0x%02X at chunk offset %zu\n", (unsigned int)data[i], i);
            upload_abort();

            return ESP_ERR_INVALID_ARG;
        }
    }

    printf("[API] Sending chunk (%zu bytes)\n", length);

    esp_err_t state = SendBytesToController((const unsigned char *)data, length);

    if (state != ESP_OK)
    {
        printf("[API] UART transmission failed: %s\n", esp_err_to_name(state));

        upload_abort();
        return state;
    }

    upload.bytes_received += length;

    printf("[API] Chunk sent: %zu bytes (%zu/%zu)\n", length, upload.bytes_received, upload.file_size);

    return ESP_OK;
}

esp_err_t api_upload_end(void)
{
    if (!upload.active)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (upload.bytes_received != upload.file_size)
    {
        printf("[API] Upload incomplete: %zu/%zu bytes\n", upload.bytes_received, upload.file_size);

        upload_abort();
        return ESP_ERR_INVALID_SIZE;
    }

    printf("[API] Upload completed: %s (%zu bytes)\n", upload.filename, upload.bytes_received);

    upload.active = 0;
    upload.bytes_received = 0;

    return ESP_OK;
}
