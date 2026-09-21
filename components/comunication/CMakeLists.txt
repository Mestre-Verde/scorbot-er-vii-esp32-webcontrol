idf_component_register(
    SRCS
        "src/uart_common.c" 
        "src/controller_bridge.c"
        "src/pc_bridge.c"

    INCLUDE_DIRS 
        "include"
        
    PRIV_REQUIRES
        esp_driver_uart

    REQUIRES 
        driver
        utils

        
)