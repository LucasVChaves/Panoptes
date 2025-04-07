#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
#include "esp_camera.h"
#include "esp_log_level.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "hal/ledc_types.h"
#include "portmacro.h"
#include "config.h"

#define CAMERA_MODEL_AI_THINKER


/* Setup da Camera */
// Alguns valores sao default da documentacao da lib
void setup_camera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = 5;
    config.pin_d1 = 18;
    config.pin_d2 = 19;
    config.pin_d3 = 21;
    config.pin_d4 = 36;
    config.pin_d5 = 39;
    config.pin_d6 = 34;
    config.pin_d7 = 35;
    config.pin_xclk = 0;
    config.pin_pclk = 22;
    config.pin_vsync = 25;
    config.pin_href = 23;
    config.pin_sccb_sda = 26;
    config.pin_sccb_scl = 27;
    config.pin_pwdn = 32;
    config.pin_reset = -1;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_RGB565; 
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = JPEG_COMPRESSION;
    config.fb_count = 2;

    if (framesize_opt == 1) {
        ESP_LOGI(T_CAM, "Camera resolution set to QQVGA - 160x120 px");
        config.frame_size = FRAMESIZE_QQVGA;
    } else if (framesize_opt == 2) {
        ESP_LOGI(T_CAM, "Camera resolution set to QVGA - 320x240 px");
        config.frame_size = FRAMESIZE_QVGA;
    } else if (framesize_opt == 3) {
        ESP_LOGI(T_CAM, "Camera resolution set to HVGA - 480x320 px");
        config.frame_size = FRAMESIZE_HVGA;
    } else {
        ESP_LOGE(T_CAM, "Invalid camera resolution: Config set to %d", framesize_opt);
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(T_CAM, "Failed to initialize: 0x%x", err);
        return;
    }
    ESP_LOGI(T_CAM, "Camera setup with success");

    // O HAL da camera ta com mania de printar cada vez que 
    // liga o VSYNC e polui o serial, aqui to setando pra 
    // printar so erros
    esp_log_level_set("cam_hal", ESP_LOG_ERROR);
}

/*===> Thread de Captura <===*/

QueueHandle_t frame_queue;

void camera_task(void *pvParams) {
    while (1) {
        camera_fb_t *frame = esp_camera_fb_get();
        if (frame) xQueueSend(frame_queue, &frame, portMAX_DELAY);
        //ESP_LOGD(T_CAM, "Captured Frame");
        vTaskDelay(pdMS_TO_TICKS(1000/FPS));
    }
}

#endif //CAMERA_H
