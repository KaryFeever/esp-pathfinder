idf_component_register(SRCS "main.c" "camera_config.c" "communication.c" "processing_unit.c" "features.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES opencvlib esp32-camera esp_http_server nvs_flash esp_wifi)