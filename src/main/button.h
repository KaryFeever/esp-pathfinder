#pragma once 

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"

typedef struct {
    int button_index;
    int min_voltage;
    int max_voltage;
} button_config_t;

void register_button(const QueueHandle_t key_state);