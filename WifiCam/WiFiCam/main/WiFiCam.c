#include <stdio.h>
#include "esp_camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define CAMERA_MODEL_AI_THINKER

void app_main(void)
{
    char* taskName = pcTaskGetName(NULL);
    ESP_LOGI(taskName, "Hello, Starting up!");

    while (1) {
        vTaskDelay(1000);
    }
}
