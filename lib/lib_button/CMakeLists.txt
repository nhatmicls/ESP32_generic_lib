idf_component_register(SRCS "lib_button.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_netif esp_event nvs_flash hal esp_timer
)
