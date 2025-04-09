# Panoptes

> Panoptes (Grego: Πανόπτης) é um gigante da mitologia grega com cem olhos que foram espalhados por todo o seu corpo, permitindo-lhe ver tudo o que acontecia ao seu redor.

Equipamento do LITel pra medir a intensidade de múltiplas fibras POF (Polymer Optical Fiber) utilizando um microcontrolador ESP32 e uma câmera OV2640.

Este monorepo possui 3 partes principais:

- WiFiCam: Firmware do ESP32. Responsável por capturar imagens da fibra, medir suas intensidades e servir os resultados via http. 
- Visualizer: Programa desktop em Python que mostra os resultados obtidos pelo equipamento. 
- Assets: TODO: Possui o STL do suporte da câmera e outros arquivos de design e impressão 3D.

Dentro de cada pasta, há um README.md com informações específicas e documentação de cada parte do projeto.
