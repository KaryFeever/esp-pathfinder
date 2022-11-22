idf_component_register(SRCS "main.c" "camera_config.c" "communication.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp32-camera esp_http_server nvs_flash esp_wifi)