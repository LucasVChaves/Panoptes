#ifndef WIFI_H
#define WIFI_H

#include "config.h"
#include "esp_err.h"
#include "esp_interface.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_wifi_types_generic.h"
#include "nvs_flash.h"

void wifi_init_ap() {
    // O WiFi do esp depende de memoria livre no NVS
    esp_err_t nvs_err = nvs_flash_init();
    if (nvs_err == ESP_ERR_NVS_NO_FREE_PAGES || nvs_err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs_err);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    if (!ap_netif) {
        ESP_LOGE(T_WIFI, "Failed to create Access Point interface");
        return;
    }

    wifi_init_config_t default_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&default_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_cfg = {
        .ap = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWD,
            .ssid_len = strlen(WIFI_SSID),
            .channel = AP_CHANNEL,
            .max_connection = MAX_CONNECTIONS,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
            }
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_cfg));
    esp_err_t wifi_err = esp_wifi_start();
    if (wifi_err != ESP_OK) {
        ESP_LOGE(T_WIFI, "Could not initialize WIFI Access Point: %s", esp_err_to_name(wifi_err));
        return;
    }

    // Desliga o powersave, nao sei se eh a melhor ideia, mas ta ai
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    ESP_LOGI(T_WIFI, "Access Point initialized with success. SSID: %s", WIFI_SSID);
}

#endif // !WIFI_H
