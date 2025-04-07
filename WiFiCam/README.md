# WiFiCam

Firmware do ESP32. Responsável por capturar imagens da fibra, medir as intensidade de luz e enviar as medidas para o computador via WiFi.

O arquivo `config.h` contém as configurações do firmware.

## Utilização

### Upload do Firmware

O ESP32-CAM não possui um conversor USB-Serial integrado, então é necessário um FTDI ou outro microcontrolador para fazer isso. Também é possível encontrar um [shield de breakout](https://makeradvisor.com/tools/esp32-cam-mb-usb-programmer/) para o ESP32-CAM que já possui um conversor USB-Serial integrado.

Além disso, é necessário conectar o pino `IO0` ao `GND` para colocar o ESP32-CAM em modo de programação. A placa shield possui um botão para isso.

Este projeto utiliza a SDK [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/index.html), certifique-se de que o ambiente de desenvolvimento está configurado corretamente. O firmware foi testado com a versão 5.5 do ESP-IDF.

Para fazer o upload do firmware, siga os passos:

1. Conecte o ESP32-CAM ao conversor USB-Serial certificando-se de que o pino `IO0` está conectado ao `GND` ou o botão `IO0` está sendo pressionado.
2. Abra o arquivo `config.h` e modifique as credenciais da rede no Arduino IDE.
3. Abra o terminal e navegue até a pasta do projeto.
4. Faça o upload do firmware com o comando: `idf.py -p <PORTA> flash`. No Linux a porta é algo como `/dev/ttyUSB0` e no Windows é algo como `COM3`.
5. Reinicie o ESP32-CAM. Tenha certeza de que o pino `IO0` não está mais conectado ao `GND`.
6. É possível ver o serial de debug com o comando `idf.py -p <PORTA> monitor`.  
   O serial de debug irá mostrar o IP do ESP32-CAM e o status da conexão com a rede WiFi.
7. Com o computador conectado na rede do ESP, abra o visualizador e digite o IP do ESP para ver os dados coletados.

## Configuração

### WiFi

O ESP32 irá criar um Access Point de WiFi e o computador deve se conectar a ele. Para configurar a conexão, é possível editar as variáveis `WIFI_SSID` e `WIFI_PASS` no arquivo `config.h`.

```c
#define WIFI_SSID = "Rede Impossível de Hackear";
#define WIFI_PASS = "senha123";
```

Também é possível alterar o canal de WiFi utilizado, o padrão é o canal 6. E a o número máximo de clientes conectados, o padrão é 1.

### IP

O IP em que o ESP32 irá servir a stream pode ser alterado no arquivo de configuração. A porta padrão é 80.  
Normalmente o IP escolhido é `192.168.4.1`.  
Os resultados são servidos na URL `http://<IP>/data`.

### Imagem

O firmware captura imagens da fibra e as envia para o computador. A imagem é capturada em formato JPEG e enviada via HTTP.  
É possível alterar o tamanho da imagem no arquivo de configuração:

```c
/* Framesize (resolucao):
 * 1 - Low (QQVGA - 160x120 px)
 * 2 - Med (QVGA - 320x240 px) - default
 * 3 - High (HVGA - 480x320 px)
*/
static const uint8_t framesize_opt = 2;
```

O tamanho da imagem pode ser alterado para `QQVGA`, `QVGA` ou `HVGA`. O padrão é `QVGA`.

Além disso, é possível alterar a qualidade do JPEG e o FPS de aquisição da imagem. O padrão é 12% de qualidade e 24 FPS.

## Microcontrolador

O microcontrolador utilizado é o ESP32-CAM, que possui um módulo de câmera OV2640 integrado. O módulo é conectado via UART e SPI ao ESP32.

Dados importantes:

- Pode ser adquirido [aqui](https://produto.mercadolivre.com.br/MLB-3245853539-modulo-esp32-cam-wifi-bluetooth-esp32-ov2640-camera-_JM#polycard_client=search-nordic&position=6&search_layout=grid&type=item&tracking_id=c26b08a2-5131-4f95-b570-45330e473960).
- O Datasheet do ESP32-CAM pode ser encontrado [aqui](https://components101.com/sites/default/files/component_datasheet/ESP32-CAM_datasheet.pdf).
- Pinout e Schematics:

<div>
    <img src="https://components101.com/sites/default/files/component_pin/ESP32-CAM-Pinout.jpg" alt="Pinout" style="width: 40%;">
    <img src="https://components101.com/sites/default/files/inline-images/ESP32-CAM-Schematics.png" alt="Schematics" style="width: 40%;">
</div>
