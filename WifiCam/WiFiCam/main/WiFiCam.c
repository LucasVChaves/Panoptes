#include <stdint.h>
#include <stdio.h>
#include "esp_camera.h"
#include "esp_psram.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "hal/ledc_types.h"
#include "portmacro.h"

#define CAMERA_MODEL_AI_THINKER

/* Tags para logs */

static const char* T_CAM = "CAMERA";
static const char* T_CV = "COMPUTER-VISION";
static const char* T_RAM = "PS-RAM";

/*===> Setup da Camera <===*/

// Valores default da documentacao da lib
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
    config.frame_size = FRAMESIZE_QVGA;     // 320x240
    config.jpeg_quality = 12;
    config.fb_count = 2;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(T_CAM, "Failed to initialize: 0x%x", err);
        return;
    }
    ESP_LOGI(T_CAM, "Camera setup with success");
}

/*===> Thread de Captura <===*/

QueueHandle_t frame_queue;

void camera_task(void *pvParams) {
    while (1) {
        camera_fb_t *frame = esp_camera_fb_get();
        if (frame) xQueueSend(frame_queue, &frame, portMAX_DELAY);
        ESP_LOGD(T_CAM, "Captured Frame");
        vTaskDelay(pdMS_TO_TICKS(1000/24));
    }
}

/*===> Thread de Visao Computacional <===*/

void process_img(camera_fb_t *frame) {
    // RGB565
    uint16_t *pixels = (uint16_t*)frame->buf;
    int width = frame->width;
    int height = frame->height;

    // Mascara de pixels vermelhos, ainda temos que calibrar
    // os thresholds desse trem
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            uint16_t pixel = pixels[i * width + j];
            uint8_t r = (pixel >> 11) & 0x1F;
            uint8_t g = (pixel >> 5) & 0x3F;
            uint8_t b = (pixel & 0x1f);
            
            // thresholds
            if (r > 0x18 && g < 0x0C && b < 0x0C) {
                ESP_LOGI(T_CV, "Red pixel found in (%d,%d)", j, i);
            }
        }
    }

    //TODO: Blob Detection
    //TODO: Calc Intensidade Media
}

void processing_task(void *pvParams) {
    camera_fb_t *frame;
    while(1) {
        if (xQueueReceive(frame_queue, &frame, portMAX_DELAY)) {
            process_img(frame);
            esp_camera_fb_return(frame);
        }
    }
}

/*===> Main <===*/

void app_main(void) {
    if (esp_psram_is_initialized()) {
        ESP_LOGI(T_RAM, "PSRAM enabled. Size: %d MB", esp_psram_get_size() / (1024*1024));
    } else {
        ESP_LOGE(T_RAM, "PSRAM not found.");
    }

    setup_camera();
    // Camera double buffer
    frame_queue = xQueueCreate(2, sizeof(camera_fb_t *));

    xTaskCreatePinnedToCore(camera_task, "camera", 4096, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(processing_task, "processing", 4096, NULL, 5, NULL, 1);
}
