idf_component_register(
    SRCS 
        "src/DNS.c"
        "src/wifi.c"
        "src/network_events.c"

    INCLUDE_DIRS 
        "include"

    PRIV_REQUIRES 
        espressif__mdns
        esp_wifi
        nvs_flash
        esp_event
        http_server
)