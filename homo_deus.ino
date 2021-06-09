//#include <NeoPixelBrightnessBus.h>
#include <NeoPixelBus.h>
#include "BluetoothSerial.h"

#include "Config.h"
#include "HeptagonStar.h"
#include "Emitter.h"
#include "esp_bt.h"

#ifdef HD_WIFI
#include <WiFi.h>
#endif
#ifdef HD_OSC
#include <ArduinoOSC.h>
#endif

NeoGamma<NeoGammaTableMethod> colorGamma;
NeoPixelBus<NeoGrbFeature, NeoWs2813Method> strip1(PIXEL_COUNT1, PIXEL_PIN1);
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt5Ws2812xMethod> strip2(PIXEL_COUNT2, PIXEL_PIN2);
HeptagonStar heptagon;
Emitter *emitter;
bool showIntersections = false;
bool showConnections = false;
bool showAll = false;
unsigned long prevMillis = 0;
float fps = 0.f;

void setup() {
  setupComms();

  heptagon.setup();

  // todo: should not use BrightnessBus
  // strip.SetBrightness(32);
  strip1.Begin();
  strip1.Show();

  strip2.Begin();
  strip2.Show();

  pinMode(BUTTON_PIN, INPUT);

  emitter = new Emitter(heptagon.models, heptagon.outerNeurons);

  #ifdef HD_DEBUG
  Serial.println("setup complete");
  #endif
}

void setupComms() {
  Serial.begin(115200);

  esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);

  #ifdef HD_WIFI

  #ifdef ESP_PLATFORM
  WiFi.disconnect(true, true);  // disable wifi, erase ap info
  delay(1000);
  WiFi.mode(WIFI_STA);
  #endif

  WiFi.begin("VA37-3", "fdsa4321");
  //WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.print("WiFi connected, IP = ");
  Serial.println(WiFi.localIP());

  #endif

  #ifdef HD_OSC
  OscWiFi.subscribe(54321, "/emit", onOscReceived);
  #endif
}

void update() {
  unsigned long ms = millis();
  fps = 1000.f / float(ms - prevMillis);
  prevMillis = ms;
  emitter->emit(ms);
  heptagon.update();
  emitter->update();
}

void draw() {
  for (uint16_t i=0; i<PIXEL_COUNT1; i++) {
    strip1.SetPixelColor(i, getColor(i));
  }
  for (uint16_t i=0; i<PIXEL_COUNT2; i++) {
    strip2.SetPixelColor(i, getColor(PIXEL_COUNT1+i));
  }
  strip1.Show();
  strip2.Show();
}

RgbColor getColor(uint16_t i) {
  RgbColor color = RgbColor(0, 0, 0);
  if (emitter->pixelDiv[i]) {
    color.R = min(emitter->pixelValuesR[i] / emitter->pixelDiv[i] / 255.f, 1.f) * MAX_BRIGHTNESS;
    color.G = min(emitter->pixelValuesG[i] / emitter->pixelDiv[i] / 255.f, 1.f) * MAX_BRIGHTNESS;
    color.B = min(emitter->pixelValuesB[i] / emitter->pixelDiv[i] / 255.f, 1.f) * MAX_BRIGHTNESS;
  }
  #ifdef HD_TEST
  if (showAll) {
    color.R = MAX_BRIGHTNESS / 3;
  }
  if (showConnections) {
    color.G = (heptagon.isConnection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showIntersections) {
    color.B = (heptagon.isIntersection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  #endif
  return color;
}

void loop() {
  #ifdef HD_OSC
  OscWiFi.update();
  #endif

  update();
  draw();

  #ifdef HD_DEBUG
  readSerial();
  #endif
}

void readSerial() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    switch (incomingByte) {
      case 'r':
        ESP.restart();
        break;
      case 's':
        emitter->enabled = !emitter->enabled;
        Serial.printf("Emitter is %s", emitter->enabled ? "enabled" : "disabled");
        break;
      #ifdef HD_TEST
      case 'f':
        Serial.printf("FPS: %f\n", fps);
        break;
      case 'h':
        Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
        break;
      case 'a':
        showAll = !showAll;
        break;
      case 'i':
        showIntersections = !showIntersections;
        break;
      case 'c':
        showConnections = !showConnections;
        break;
      case 'l':
        Serial.printf("Total %d lights\n", emitter->totalLights);
        break;
      case 'L':
        emitter->debug();
        break;
      case 'C':
        heptagon.debugConnections();
        break;
      case 'I':
        heptagon.debugIntersections();
        break;
      #endif
      case '1':
      case '2':
      case '3':
      case '4':
        emitter->emitNew(incomingByte - '1');
        break;
    }
  }
}

#ifdef HD_OSC
void onOscReceived(const OscMessage& m) {
  if (m.size() > 0) {
    uint8_t which = m.arg<uint8_t>(0);
    if (m.size() > 1) {
      float speed = m.arg<float>(1);
      if (m.size() > 2) {
        uint16_t life = m.arg<uint16_t>(2);
        if (m.size() > 3) {
          uint16_t length = m.arg<uint16_t>(3);
          emitter->emitNew(which, speed, life, length);
        }
        else {
          emitter->emitNew(which, speed, life);
        }
      }
      else {
        emitter->emitNew(which, speed);
      }
    }
    else {
      emitter->emitNew(which);
    }
  }
}
#endif
