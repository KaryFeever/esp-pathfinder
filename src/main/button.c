#include <stdio.h>
#include <stdlib.h>
#include "button.h"
#include "driver/adc_common.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

//ADC Channels
#define ADC1_EXAMPLE_CHAN0 ADC1_CHANNEL_0
//ADC Attenuation
#define ADC_EXAMPLE_ATTEN ADC_ATTEN_DB_11

#define PRESS_INTERVAL 500000

static uint32_t voltage = 0;

button_config_t adc_button = {1, 2800, 3000};
static QueueHandle_t xQueueKeyStateO = NULL;

static void button_task(void *argv) {
    ESP_LOGW("BUTTON", "ENTER_TASK");
    int last_button_pressed = -1;
    int button_pressed = -1;
    int64_t backup_time = esp_timer_get_time();
    int64_t last_time = esp_timer_get_time();

    //ADC1 config
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_EXAMPLE_CHAN0, ADC_EXAMPLE_ATTEN));

    while (1)
    {
        // ESP_LOGW("BUTTON", "TASK");
        voltage = adc1_get_raw(ADC1_EXAMPLE_CHAN0);
        backup_time = esp_timer_get_time();
        // ESP_LOGW("BUTTON", "TASK: %d", voltage);
        if ((voltage >= adc_button.min_voltage) && (voltage <= adc_button.max_voltage))
        {
            
            button_pressed = adc_button.button_index;
            if ((button_pressed != last_button_pressed) || ((backup_time - last_time) > PRESS_INTERVAL))
            {
                last_button_pressed = button_pressed;
                last_time = backup_time;
                ESP_LOGW("BUTTON", "TASK");
                xQueueOverwrite(xQueueKeyStateO, &button_pressed);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void register_button(const QueueHandle_t key_state) {
    ESP_LOGW("BUTTON", "TASK");
    xQueueKeyStateO = key_state;
    xTaskCreatePinnedToCore(button_task, "button_scan_task", 3 * 1024, NULL, 5, NULL, 0);
}
