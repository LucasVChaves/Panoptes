#ifndef CV_H
#define CV_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "portmacro.h"
#include "config.h"
#include "camera.h"

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
                //ESP_LOGI(T_CV, "Red pixel found in (%d,%d)", j, i);
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

#endif // !CV_H
