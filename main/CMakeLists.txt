idf_component_register(SRCS "ESP32.c"
    INCLUDE_DIRS "."
    REQUIRES app_gpio driver esp_netif esp_event nvs_flash app_button app_uart
)
