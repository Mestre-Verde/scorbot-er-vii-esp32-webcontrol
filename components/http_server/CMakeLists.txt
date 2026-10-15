idf_component_register(
    SRCS 
        "src/http_server.c"

    INCLUDE_DIRS 
        "include"

    PRIV_REQUIRES 
        esp_http_server
        api
)