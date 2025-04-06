/*
* TODO: WiFi, conectar em uma rede ou gerar o proprio beacon?
* TODO: Mandar o que pro PC? Intensidades? Talvez desenhar um "esquema" para visualizacao?
* TODO: Arquivo de configuracao para FPS, qualidade e credenciais do WiFi
* Nota: As IDEs reclamam de flags de compilacao e imports, mas ao compilar nao tem erro.
*/
#include <stdint.h>
#include <stdio.h>
#include "esp_camera.h"
#include "esp_psram.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "portmacro.h"
#include "config.h"
#include "camera.h"
#include "computer_vision.h"
#include "wifi.h"

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
    wifi_init_ap();

    xTaskCreatePinnedToCore(camera_task, "camera", 4096, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(processing_task, "processing", 4096, NULL, 5, NULL, 1);
}
