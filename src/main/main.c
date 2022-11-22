#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "camera_config.h"
#include "communication.h"

void app_main(void)
{
    static httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(nvs_flash_init());
    init_camera();
    initialise_wifi(&server);
}

// #include "button.h"
// #include "callibration.h"

// static QueueHandle_t xQueueButtonState = NULL;
// static QueueHandle_t xQueueHandler = NULL;





// void app_main(void) {
//     // gpio_config_t gpio_conf;
//     // gpio_conf.mode = GPIO_MODE_OUTPUT_OD;
//     // gpio_conf.intr_type = GPIO_INTR_DISABLE;
//     // gpio_conf.pin_bit_mask = 1LL << GPIO_NUM_3;
//     // gpio_config(&gpio_conf);

//     // xQueueButtonState = xQueueCreate(1, sizeof(int));
//     // xQueueHandler = xQueueCreate(1, sizeof(int));

//     // register_button(xQueueButtonState);
//     // register_handler(xQueueButtonState, xQueueHandler);

// }


