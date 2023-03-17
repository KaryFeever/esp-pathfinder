#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "camera_config.h"
#include "communication.h"
#include "processing_unit.h"

void app_main(void)
{
    static httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(nvs_flash_init());
    init_camera();
    initialise_wifi(&server);
    // wait when communication will be closed
    while(!communicationFinished()){}
    init_processing_unit(camera_matrix);
}
