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
float fps = 0.f;

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

  //WiFi.begin("VA37-3", "fdsa4321");
  WiFi.begin("Redmi", "marlena123");
  //WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.print("WiFi connected, IP = ");
  Serial.println(WiFi.localIP());

  #endif

  #ifdef HD_OSC
  OscWiFi.subscribe(OSC_PORT, "/emit", onEmit);
  OscWiFi.subscribe(OSC_PORT, "/note_on", onNoteOn);
  OscWiFi.subscribe(OSC_PORT, "/note_off", onNoteOff);
  OscWiFi.subscribe(OSC_PORT, "/palette", onPalette);
  OscWiFi.subscribe(OSC_PORT, "/color", onColor);
  OscWiFi.subscribe(OSC_PORT, "/split", onSplit);
  #endif
}

void update() {
  gMillis = millis();
  fps = 1000.f / float(gMillis - gPrevMillis);
  gPrevMillis = gMillis;
  emitter->autoEmit(gMillis);
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
      case '7':
        emitter->emit(incomingByte - '1');
        break;
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
void onEmit(const OscMessage& m) {
  if (m.size() > 0) {
    uint8_t model = m.arg<uint8_t>(0);
    if (m.size() > 1) {
      float speed = m.arg<float>(1);
      if (m.size() > 2) {
        uint16_t length = m.arg<uint16_t>(2);
        if (m.size() > 3) {
          ListOrder order = static_cast<ListOrder>(m.arg<uint8_t>(3));
          if (m.size() > 4) {
            bool linked = m.arg<uint8_t>(4) > 0;
            Serial.println(linked);
            if (m.size() > 5) {
              int16_t life = m.arg<int16_t>(5);
              if (m.size() > 6) {
                uint8_t color = m.arg<uint8_t>(6);
                emitter->emit(model, speed, length, order, linked, life, color);
              }
              else {
                emitter->emit(model, speed, length, order, linked, life);
              }
            }
            else {
              emitter->emit(model, speed, length, order, linked);
            }
          }
          else {
            emitter->emit(model, speed, length, order);
          }
        }
        else {
          emitter->emit(model, speed);
        }
      }
      else {
        emitter->emit(model, speed);
      }
    }
    else {
      emitter->emit(model);
    }
  }
}

void onNoteOn(const OscMessage& m) {
  if (m.size() > 0) {
    uint16_t noteId = m.arg<uint16_t>(0);
    uint8_t i = 0;
    if (m.size() > 1) {
      uint8_t model = m.arg<uint8_t>(1);
      if (m.size() > 2) {
        float speed = m.arg<float>(2);
        if (m.size() > 3) {
          uint16_t length = m.arg<uint16_t>(3);
          if (m.size() > 4) {
            ListOrder order = static_cast<ListOrder>(m.arg<uint8_t>(4));
            if (m.size() > 5) {
              bool linked = m.arg<bool>(5);
              if (m.size() > 6) {
                uint8_t color = m.arg<uint8_t>(6);
                i = emitter->emit(model, speed, length, order, linked, -1, color);
              }
              else {
                i = emitter->emit(model, speed, length, order, linked, -1);
              }
            }
            else {
              i = emitter->emit(model, speed, length, order);
            }
          }
          else {
            i = emitter->emit(model, speed, length);
          }
        }
        else {
          i = emitter->emit(model, speed);
        }
      }
      else {
        i = emitter->emit(model);
      }
    }
    else {
      i = emitter->emit(emitter->randomModel());
    }
    emitter->lightLists[i]->setLife(-1);
    emitter->lightLists[i]->noteId = noteId;
  }
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
#endif
