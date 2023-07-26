idf_component_register(SRCS "lib_uart.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_netif esp_event
)
