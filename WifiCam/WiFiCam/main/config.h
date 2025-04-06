/*
* Este arquivo possui configuracoes gerais para o programa
* Ajuste conforme necessidade
*/
#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>

/* WIFI
 * SSID = Nome da Rede - MAX 32 chars
 * PASSWD = Senha da Rede - MIN 8 MAX 63 chars
 * MAX_CONNECTIONS = Numero maximo de conexoes com o ESP, default = 1 
 * AP_CHANNEL = Escolha entre 1, 6 e 11, sao bandas que nao se sobrepoem, default = 6
*/
#define WIFI_SSID "ESPCAM POF MultiSensor"
#define WIFI_PASSWD "cafezin2077"
#define MAX_CONNECTIONS 1
#define AP_CHANNEL 6

/* Framesize (resolucao):
 * 1 - Low (QQVGA - 160x120 px)
 * 2 - Med (QVGA - 320x240 px) - default
 * 3 - High (HVGA - 480x320 px)
*/
static const uint8_t framesize_opt = 2;

/* FPS
 * Default - 24
 * FPS > 30 nao recomendado
*/
static const uint8_t FPS = 24;

/* Tags para logs */
static const char* T_CAM = "CAMERA";
static const char* T_CV = "COMPUTER-VISION";
static const char* T_RAM = "PS-RAM";
static const char* T_WIFI = "WIFI ACCESS POINT";

#endif // CONFIG_H
