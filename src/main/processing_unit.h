#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"
#include "camera_config.h"
#include <esp_wifi.h>

void init_processing_unit(char *camera_matrix);