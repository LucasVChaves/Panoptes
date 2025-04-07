#ifndef WIFI_H
#define WIFI_H

#include "config.h"
#include "esp_err.h"
#include "esp_interface.h"
#include "esp_netif_ip_addr.h"
#include "esp_netif_types.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_wifi_types_generic.h"
#include "nvs_flash.h"
#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Funcoes helper para config de IP 
// -----------------------------------------------------------------------

static inline void ip_str_to_octets(const char* ip_str, uint8_t octets[4]) {
    char ip_copy[16];
    strncpy(ip_copy, ip_str, sizeof(ip_copy) - 1);
    ip_copy[15] = '\0';
    
    // strtok nao eh segura, causa buffer overflow regularmente,
    // futuramente vou escrever uma solucao autoral
    char* token = strtok(ip_copy, ".");
    for (int i = 0; i < 4 && token != NULL; i++) {
        octets[i] = (uint8_t)atoi(token);
        token = strtok(NULL, ".");
    }
}

#define IP_STR_TO_ADDR4(ip_var, ip_str) do { \
    uint8_t octets[4]; \
    ip_str_to_octets(ip_str, octets); \
    IP4_ADDR(&(ip_var), octets[0], octets[1], octets[2], octets[3]); \
} while(0)

// ------------------------------------------------------------------------

void wifi_init_ap() {
    // O WiFi do esp depende de memoria livre do Non-Volatile Storage
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

    esp_netif_ip_info_t ip_info;
    // Valores extraidos do config.h
    IP_STR_TO_ADDR4(ip_info.ip, ip_addr);
    IP_STR_TO_ADDR4(ip_info.gw, gateway);
    IP_STR_TO_ADDR4(ip_info.netmask, netmask);

    esp_netif_dhcps_stop(ap_netif);
    esp_netif_set_ip_info(ap_netif, &ip_info);
    esp_netif_dhcps_start(ap_netif);

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

static esp_err_t data_handler(httpd_req_t *req) {
    char json[512];
    snprintf(json, sizeof(json),"{\"TEST JSON\":[%d, %d]}\n", 1, 2);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
    return ESP_OK;
}

void start_webserver() {
    httpd_config_t server_cfg = HTTPD_DEFAULT_CONFIG();
    httpd_uri_t data_uri = {
        .uri = "/data",
        .method = HTTP_GET,
        .handler = data_handler
    };

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &server_cfg) == ESP_OK) {
        httpd_register_uri_handler(server, &data_uri);
    }
    
    ESP_LOGI(T_WIFI, "HTTP WebServer initialized with success at %s/data", ip_addr);
}

#endif // !WIFI_H
