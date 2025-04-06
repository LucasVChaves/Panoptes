/*
* Este arquivo possui configuracoes gerais para o programa
* Ajuste conforme necessidade
*/
#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>

/* WIFI
 * SSID = Nome da Rede
 * PASSWD = Senha da Rede
*/
static const char* SSID = "ESPCAM POF MultiSensor";
static const char* PASSWD = "cafezin2077";

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

#endif // CONFIG_H
