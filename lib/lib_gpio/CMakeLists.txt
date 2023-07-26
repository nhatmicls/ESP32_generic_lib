idf_component_register(SRCS "lib_gpio.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_netif esp_event nvs_flash hal
)
