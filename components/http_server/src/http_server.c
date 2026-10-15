/**
 * @file http_server.c
 * @brief Implements the HTTP server and handles client HTTP requests.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "api.h"
#include "esp_http_server.h"

static const char *htmlPage =
    "<!DOCTYPE html>"
    "<html lang=\"pt\">"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<title>Scorbot-API</title>"
    "</head>"
    "<body>"
    "<h1>Scorbot-API</h1>"
    "<h2>Enviar programa</h2>"
    "<form id=\"uploadForm\">"
    "<input type=\"file\" id=\"programFile\" required>"
    "<button type=\"submit\" id=\"uploadButton\">Enviar programa</button>"
    "</form>"
    "<p id=\"status\">À espera de um ficheiro.</p>"
    "<script>"
    "const CHUNK_SIZE = 120;"

    "document.getElementById('uploadForm').onsubmit = async function(e) {"
    "e.preventDefault();"

    "const file = document.getElementById('programFile').files[0];"
    "if (!file) return;"

    "const status = document.getElementById('status');"
    "const button = document.getElementById('uploadButton');"

    "button.disabled = true;"

    "try {"
    "status.textContent = 'A iniciar transferência...';"

    "let response = await fetch('/upload/start', {"
    "method: 'POST',"
    "headers: {"
    "'X-Filename': encodeURIComponent(file.name),"
    "'X-File-Size': String(file.size)"
    "}"
    "});"

    "if (!response.ok) throw new Error(await response.text());"

    "for (let offset = 0; offset < file.size; offset += CHUNK_SIZE) {"
    "const chunk = file.slice(offset, offset + CHUNK_SIZE);"

    "status.textContent = 'A enviar: ' + "
    "Math.min(offset + chunk.size, file.size) + "
    "' / ' + file.size + ' bytes';"

    "response = await fetch('/upload/chunk', {"
    "method: 'POST',"
    "headers: {"
    "'Content-Type': 'application/octet-stream',"
    "'X-Offset': String(offset)"
    "},"
    "body: chunk"
    "});"

    "if (!response.ok) throw new Error(await response.text());"
    "}"

    "status.textContent = 'A finalizar transferência...';"

    "response = await fetch('/upload/end', {"
    "method: 'POST'"
    "});"

    "if (!response.ok) throw new Error(await response.text());"

    "status.textContent = await response.text();"

    "} catch (error) {"
    "status.textContent = 'Erro: ' + error.message;"
    "} finally {"
    "button.disabled = false;"
    "}"
    "};"
    "</script>"
    "</body>"
    "</html>";

static size_t expectedUploadOffset = 0;

/**
 * @brief Serves the HTML page to the client in response to a GET "/" request.
 */
static esp_err_t indexHandler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    return httpd_resp_send(req, htmlPage, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t uploadStartHandler(httpd_req_t *req)
{
    char encodedFilename[385];
    char fileSizeStr[24];

    size_t filenameLen = httpd_req_get_hdr_value_len(req, "X-Filename");
    size_t sizeLen = httpd_req_get_hdr_value_len(req, "X-File-Size");

    if (filenameLen == 0 || filenameLen >= sizeof(encodedFilename) || sizeLen == 0 || sizeLen >= sizeof(fileSizeStr))
    {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "Missing or invalid upload headers.");
    }

    if (httpd_req_get_hdr_value_str(req, "X-Filename", encodedFilename, sizeof(encodedFilename)) != ESP_OK ||
        httpd_req_get_hdr_value_str(req, "X-File-Size", fileSizeStr, sizeof(fileSizeStr)) != ESP_OK)
    {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "Could not read upload headers.");
    }

    char *end;
    unsigned long long parsedSize = strtoull(fileSizeStr, &end, 10);

    if (*fileSizeStr == '\0' || *end != '\0' || parsedSize == 0 || parsedSize > SIZE_MAX)
    {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "Invalid file size.");
    }

    // TODO: Decode the URL-encoded filename before passing it to the API.
    // For now, filenames containing percent-encoded characters are not decoded.

    esp_err_t state = api_upload_begin(encodedFilename, (size_t)parsedSize);

    if (state != ESP_OK)
    {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, esp_err_to_name(state));
    }

    expectedUploadOffset = 0;

    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_sendstr(req, "Upload started.");
}

static esp_err_t uploadChunkHandler(httpd_req_t *req)
{
    uint8_t buffer[API_UPLOAD_CHUNK_SIZE];
    size_t totalReceived = 0;

    if (req->content_len == 0 ||
        req->content_len > sizeof(buffer))
    {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "Invalid chunk size.");
    }

    while (totalReceived < req->content_len)
    {
        int received = httpd_req_recv(
            req,
            (char *)buffer + totalReceived,
            req->content_len - totalReceived);

        if (received == HTTPD_SOCK_ERR_TIMEOUT)
            continue;

        if (received <= 0)
        {
            httpd_resp_set_status(req, "500 Internal Server Error");
            return httpd_resp_sendstr(req, "Failed to receive chunk.");
        }

        totalReceived += received;
    }

    // Pass the received bytes to the API.
    // Read and validate the chunk offset.
    char offsetStr[24];

    size_t offsetLen =
        httpd_req_get_hdr_value_len(req, "X-Offset");

    if (offsetLen == 0 || offsetLen >= sizeof(offsetStr))
    {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "Missing or invalid X-Offset.");
    }

    if (httpd_req_get_hdr_value_str(
            req, "X-Offset", offsetStr, sizeof(offsetStr)) != ESP_OK)
    {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "Could not read X-Offset.");
    }

    char *end;
    unsigned long long parsedOffset = strtoull(offsetStr, &end, 10);

    if (offsetStr[0] == '\0' ||
        *end != '\0' ||
        parsedOffset > SIZE_MAX ||
        (size_t)parsedOffset != expectedUploadOffset)
    {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "Invalid chunk offset.");
    }

    // Send the received bytes through the API.
    esp_err_t state = api_upload_chunk(buffer, totalReceived);

    if (state != ESP_OK)
    {
        httpd_resp_set_status(req, "500 Internal Server Error");
        return httpd_resp_sendstr(req, esp_err_to_name(state));
    }

    // Advance only after the API accepts the chunk.
    expectedUploadOffset += totalReceived;

    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_sendstr(req, "Chunk accepted.");
}

static esp_err_t uploadEndHandler(httpd_req_t *req)
{
    esp_err_t state = api_upload_end();

    if (state != ESP_OK)
    {
        expectedUploadOffset = 0;

        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, esp_err_to_name(state));
    }

    expectedUploadOffset = 0;

    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_sendstr(req, "Upload completed successfully.");
}

esp_err_t startHttpServer(void *arg)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.recv_wait_timeout = 15;
    config.send_wait_timeout = 15;

    esp_err_t state = httpd_start(&server, &config);
    if (state != ESP_OK)
    {
        printf("Falha ao iniciar HTTP: %s\n",
               esp_err_to_name(state));
        return state;
    }

    // Página principal: GET /
    httpd_uri_t indexUri = {.uri = "/",
                            .method = HTTP_GET,
                            .handler = indexHandler,
                            .user_ctx = NULL};
    state = httpd_register_uri_handler(server, &indexUri);
    if (state != ESP_OK)
    {
        printf("[HTTP] Falha ao registar GET /\n");
        httpd_stop(server);
        return state;
    }

    // Iniciar upload: POST /upload/start
    httpd_uri_t uploadStartUri = {.uri = "/upload/start",
                                  .method = HTTP_POST,
                                  .handler = uploadStartHandler,
                                  .user_ctx = NULL};
    state = httpd_register_uri_handler(server, &uploadStartUri);
    if (state != ESP_OK)
    {
        printf("[HTTP] Falha ao registar POST /upload/start\n");
        httpd_stop(server);
        return state;
    }

    // Enviar um bloco: POST /upload/chunk
    httpd_uri_t uploadChunkUri = {.uri = "/upload/chunk",
                                  .method = HTTP_POST,
                                  .handler = uploadChunkHandler,
                                  .user_ctx = NULL};
    state = httpd_register_uri_handler(server, &uploadChunkUri);
    if (state != ESP_OK)
    {
        printf("[HTTP] Falha ao registar POST /upload/chunk\n");
        httpd_stop(server);
        return state;
    }

    // Finalizar upload: POST /upload/end
    httpd_uri_t uploadEndUri = {.uri = "/upload/end",
                                .method = HTTP_POST,
                                .handler = uploadEndHandler,
                                .user_ctx = NULL};
    state = httpd_register_uri_handler(server, &uploadEndUri);
    if (state != ESP_OK)
    {
        printf("[HTTP] Falha ao registar POST /upload/end\n");
        httpd_stop(server);
        return state;
    }

    printf("[HTTP] Servidor iniciado na porta %d\n", config.server_port);

    return ESP_OK;
}