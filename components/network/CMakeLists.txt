idf_component_register(
    SRCS "DNS.c"
        "wireless_protocols.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES espressif__mdns
)