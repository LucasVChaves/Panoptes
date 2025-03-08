#include "WifiCam.hpp"

static void serveMjpeg() {
  Serial.println("MJPEG streaming begin");
  auto startTime = millis();
  int nFrames = esp32cam::Camera.streamMjpeg(server.client());
  auto duration = millis() - startTime;
  Serial.printf("MJPEG streaming end: %dfrm %0.2ffps\n", nFrames, 1000.0 * nFrames / duration);
}

void addRequestHandlers() {
  server.on("/", HTTP_GET, [] {
    const long targetWidth = 320;
    const long targetHeight = 240;
    
    auto r = esp32cam::Camera.listResolutions().find(targetWidth, targetHeight);
    if (!r.isValid()) {
      server.send(404, "text/plain", "Resolução não disponível\n");
      return;
    }
    
    if (!esp32cam::Camera.update([=](esp32cam::Settings &settings) {
         settings.resolution = r;
    }, 500)) {
      server.send(500, "text/plain", "Erro ao atualizar resolução\n");
      return;
    }
    
    serveMjpeg();
  });
}
