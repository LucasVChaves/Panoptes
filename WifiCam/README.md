# WiFiCam

Firmware do ESP32. Responsável por capturar imagens da fibra e servir uma stream de JPEGs via WiFi.

## Utilização

### WiFi

O ESP32 precisa de uma conexão WiFi para funcionar. Para configurar a conexão, é necessário editar as variáveis `WIFI_SSID` e `WIFI_PASS` no arquivo `WiFiCam.ino`. Por exemplo:

```c
static const char* WIFI_SSID = "Rede Impossível de Hackear";
static const char* WIFI_PASS = "senha123";
```

### IP

O IP em que o ESP32 irá servir a stream será printado no console serial. A porta padrão é 80.  
Normalmente o IP escolhido é `192.168.0.37`.  
Pode ser acessado via navegador no endereço `http://<IP>`

### Upload do Firmware

O ESP32-CAM não possui um conversor USB-Serial integrado, então é necessário um FTDI ou outro microcontrolador para fazer isso. Também é possível encontrar um [shield de breakout](https://makeradvisor.com/tools/esp32-cam-mb-usb-programmer/) para o ESP32-CAM que já possui um conversor USB-Serial integrado.

Além disso, é necessário conectar o pino `IO0` ao `GND` para colocar o ESP32-CAM em modo de programação. A placa shield possui um botão para isso.

É recomendado utilizar o [Arduino IDE](https://www.arduino.cc/en/software) para fazer o upload do firmware. Certifique-se de instalar o suporte ao ESP32 no Arduino IDE e a biblioteca [esp32cam](https://github.com/yoursunny/esp32cam/tree/main).

Para fazer o upload do firmware, siga os passos:

1. Conecte o ESP32-CAM ao conversor USB-Serial certificando-se de que o pino `IO0` está conectado ao `GND` ou o botão `IO0` está sendo pressionado.
2. Abra o arquivo `WiFiCam.ino` e modifique as credenciais da rede no Arduino IDE.
3. Selecione a placa `AI Thinker ESP32-CAM` nas configurações.
4. Selecione a porta serial correta.
5. Faça o upload do firmware.
6. Reinicie o ESP32-CAM. Tenha certeza de que o pino `IO0` não está mais conectado ao `GND`.
7. Ligue o monitor serial na baudrate 115200 para ver o IP do ESP32-CAM.

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
