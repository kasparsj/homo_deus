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

NeoPixelBus<NeoGrbFeature, NeoWs2813Method> strip1(PIXEL_COUNT1, PIXEL_PIN1);
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt5Ws2812xMethod> strip2(PIXEL_COUNT2, PIXEL_PIN2);
NeoGamma<NeoGammaTableMethod> colorGamma;
HeptagonStar heptagon;
Emitter *emitter;
bool showIntersections = false;
bool showConnections = false;
bool showPalette = false;
bool showAll = false;
unsigned long gMillis;
unsigned long gPrevMillis = 0;
#ifdef HD_TEST
float fps[AVG_FPS_FRAMES] = {0.f};
uint8_t fpsIndex = 0;
#endif
#ifdef HD_WIFI
bool wifiConnected = false;
#endif

void setup() {
  setupComms();

  heptagon.setup();

  strip1.Begin();
  strip1.Show();

  strip2.Begin();
  strip2.Show();

  pinMode(BUTTON_PIN, INPUT);

  emitter = new Emitter(heptagon.models);

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

  uint8_t numTries = 0;
  //WiFi.begin("VA37-3", "fdsa4321");
  WiFi.begin("Redmi", "marlena123");
  //WiFi.config(ip, gateway, subnet);
  while (numTries < 10 && WiFi.status() != WL_CONNECTED) {
      numTries++;
      Serial.print(".");
      delay(500);
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.print("WiFi connected, IP = ");
    Serial.println(WiFi.localIP());
  }
  else {
    WiFi.disconnect();
    Serial.println("WiFi failed to connect");
  }

  #endif
  #ifdef HD_OSC
  OscWiFi.subscribe(OSC_PORT, "/emit", onEmit);
  OscWiFi.subscribe(OSC_PORT, "/note_on", onNoteOn);
  OscWiFi.subscribe(OSC_PORT, "/note_off", onNoteOff);
  OscWiFi.subscribe(OSC_PORT, "/palette", onPalette);
  OscWiFi.subscribe(OSC_PORT, "/color", onColor);
  OscWiFi.subscribe(OSC_PORT, "/split", onSplit);
  OscWiFi.subscribe(OSC_PORT, "/auto", onAuto);
  #endif
}

void update() {
  gMillis = millis();
  #ifdef HD_TEST
  fps[fpsIndex] = 1000.f / float(gMillis - gPrevMillis);
  fpsIndex = (fpsIndex + 1) % AVG_FPS_FRAMES;
  #endif
  gPrevMillis = gMillis;
  emitter->autoEmit(gMillis);
  heptagon.update();
  emitter->update();
}

float getFPS() {
  float avg = 0;
  for (uint8_t i=0; i<AVG_FPS_FRAMES; i++) {
    avg += fps[i];
  }
  return avg / AVG_FPS_FRAMES;
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
    color.R = MAX_BRIGHTNESS / 2;
  }
  if (showConnections) {
    color.G = (heptagon.isConnection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showIntersections) {
    color.B = (heptagon.isIntersection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showPalette && i < 256) {
    color = emitter->paletteColor(i);
  }
  #endif
  return colorGamma.Correct(color);
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
      case 'e':
        emitter->autoEnabled = !emitter->autoEnabled;
        Serial.printf("AutoEmitter is %s", emitter->autoEnabled ? "enabled" : "disabled");
        break;
      case '.':
        emitter->stopAll();
        break;
      case '!':
        emitter->colorAll();
        break;        
      case 's':
        emitter->splitAll();
        break;
      #ifdef HD_TEST
      case 'f':
        Serial.printf("FPS: %f\n", getFPS());
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
      case 'p':
        showPalette = !showPalette;
        break;
      case '>':
        if (emitter->currentPalette < 32)
        emitter->currentPalette++;
        break;
      case '<':
        if (emitter->currentPalette > 0) {
          emitter->currentPalette--;
        }
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
      case '5':
      case '6':
        emitter->emit(incomingByte - '1');
        break;
      case '7': {
        EmitParams p;
        p.model = M_STAR;
        p.colorChangeGroups = GROUP1;
        emitter->emit(p);
        break;
      }
      case '+':
        emitter->emitSplatter();
        break;
      case '*':
        emitter->emitRandom();
        break;        
    }
  }
}

#ifdef HD_OSC
void parseParams(EmitParams &p, const OscMessage &m) {
  for (uint8_t i=0; i<m.size() / 2; i++) {
    EmitParam param = static_cast<EmitParam>(m.arg<uint8_t>(i*2));
    uint8_t j = i*2+1;
    switch (param) {
      case P_MODEL:
        p.model = m.arg<int8_t>(j);
        break;
      case P_SPEED:
        p.speed = m.arg<float>(j);
        break;
      case P_LENGTH:
        p.length = m.arg<uint16_t>(j);
        break;
      case P_FADE:
        p.fadeSpeed = m.arg<float>(j);
        break;
      case P_ORDER:
        p.order = static_cast<ListOrder>(m.arg<uint8_t>(j));
        break;
      case P_LINKED:
        p.linked = m.arg<uint8_t>(j) > 0;
        break;
      case P_FROM:
        p.from = m.arg<int8_t>(j);
        break;
      case P_LIFE:
        p.life = m.arg<int16_t>(j);
        break;
      case P_COLOR:
        p.color = m.arg<int8_t>(j);        
        break;
      case P_NOTE_ID:
        p.noteId = m.arg<uint16_t>(j);
        break;
      case P_BRIGHTNESS:
        p.brightness = m.arg<float>(j);
        break;
    }
  }
}

void onEmit(const OscMessage& m) {
  EmitParams params;
  parseParams(params, m);
  emitter->emit(params);
}

void onNoteOn(const OscMessage& m) {
  EmitParams params;
  params.life = INFINITE_LIFE;
  parseParams(params, m);
  emitter->emit(params);
}

void onNoteOff(const OscMessage& m) {
  if (m.size() > 0) {
    uint16_t noteId = m.arg<uint16_t>(0);
    emitter->stopNote(noteId);
  }
  else {
    emitter->stopAll();
  }
}

void onPalette(const OscMessage& m) {
  if (m.size() > 0) {
    emitter->currentPalette = m.arg<uint8_t>(0);
  }
}

void onColor(const OscMessage &m) {
  if (m.size() > 0) {
    uint8_t i = m.arg<uint8_t>(0);
    if (m.size() > 1) {
      uint8_t color = m.arg<uint8_t>(1);
      emitter->lightLists[i]->setColor(emitter->paletteColor(color));
    }
    else {
      emitter->lightLists[i]->setColor(emitter->randomColor());
    }
  }
  else {
    emitter->colorAll();
  }
}

void onSplit(const OscMessage &m) {
  if (m.size() > 0) {
    uint8_t i = m.arg<uint8_t>(0);
    emitter->lightLists[i]->split();
  }
  else {
    emitter->splitAll();
  }
}

void onAuto(const OscMessage &m) {
  emitter->autoEnabled = !emitter->autoEnabled;
}
#endif
