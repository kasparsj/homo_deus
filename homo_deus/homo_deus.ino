#define PIXEL_PIN1 14
#define PIXEL_PIN2 26
#define BUTTON_PIN 25
#define WIFI_HOSTNAME "homo-deus"
#define WIFI_SSID "toplap"
#define WIFI_PASS "karlsruhe"
#define HD_OSC
#define HD_SERIAL
#define HD_DEBUGGER
//#define SC_HOST "192.168.43.101"
//#define SC_PORT 57120
//#define LP_OSC_REPLY(I) OscWiFi.publish(SC_HOST, SC_PORT, "/emit", (I));
#define PIXEL_COUNT1 524
#define PIXEL_COUNT2 395
#define PIXEL_COUNT (PIXEL_COUNT1 + PIXEL_COUNT2)
#define OSC_PORT 54321
//#define MAX_BRIGHTNESS 192
#define MAX_BRIGHTNESS 255
#define USE_NEOPIXELBUS
// #define USE_FASTLED

#include "BluetoothSerial.h"
#include "esp_bt.h"

#ifdef USE_NEOPIXELBUS
  #include <NeoPixelBus.h>
#endif
#ifdef USE_FASTLED
  #include <FastLED.h>
#endif

#include "src/HeptagonStar.h"
#include "src/Globals.h"
#include "src/LightList.h"
#include "src/LPRandom.h"

#ifdef WIFI_SSID
#include <WiFi.h>
#endif

#ifdef HD_OSC
#include <ArduinoOSCWiFi.h>
#endif

#ifdef HD_DEBUGGER
#include "src/LPDebugger.h"
#endif

#ifdef USE_NEOPIXELBUS
  // NeoPixelBus<NeoGrbFeature, NeoWs2813Method> strip1(PIXEL_COUNT1, PIXEL_PIN1);
  // NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt5Ws2812xMethod> strip2(PIXEL_COUNT2, PIXEL_PIN2);
  NeoPixelBus<NeoRgbFeature, NeoEsp32Rmt0Ws2811Method> strip1(PIXEL_COUNT1, PIXEL_PIN1);
  NeoPixelBus<NeoRgbFeature, NeoEsp32Rmt1Ws2811Method> strip2(PIXEL_COUNT2, PIXEL_PIN2);
  NeoGamma<NeoGammaTableMethod> colorGamma;
#endif

#ifdef USE_FASTLED
  CRGB leds1[PIXEL_COUNT1];
  CRGB leds2[PIXEL_COUNT2];
#endif

HeptagonStar heptagon(PIXEL_COUNT);
State *state;
bool showIntersections = false;
bool showConnections = false;
bool showPalette = false;
bool showAll = false;
#ifdef HD_DEBUGGER
LPDebugger *debugger;
#endif
#ifdef WIFI_SSID
bool wifiConnected = false;
#endif

void setup() {
  setupComms();

  #ifdef USE_NEOPIXELBUS
    strip1.Begin();
    strip1.Show();
    
    strip2.Begin();
    strip2.Show();
  #endif
  
  #ifdef USE_FASTLED
    FastLED.addLeds<WS2811, PIXEL_PIN1, RGB>(leds1, PIXEL_COUNT1);
    FastLED.addLeds<WS2811, PIXEL_PIN2, RGB>(leds2, PIXEL_COUNT2);
    FastLED.setBrightness(MAX_BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
  #endif

  pinMode(BUTTON_PIN, INPUT);

  state = new State(heptagon);

  #ifdef HD_DEBUGGER
  debugger = new LPDebugger(heptagon);
  #endif
  #ifdef HD_SERIAL
  Serial.println("setup complete");
  #endif
}

void setupComms() {
  Serial.begin(115200);

  esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);

  #ifdef WIFI_SSID

  #ifdef ESP_PLATFORM
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false, true);  // disable wifi, erase ap info
  delay(3000);
  #endif

  uint8_t numTries = 0;
  #ifdef WIFI_HOSTNAME
  WiFi.setHostname(WIFI_HOSTNAME);
  #endif
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (numTries < 10 && WiFi.status() != WL_CONNECTED) {
      numTries++;
      Serial.print(".");
      delay(500);
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.print("WiFi connected, IP = ");
    Serial.println(WiFi.localIP());
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  }
  else {
    // todo: should boot even if WiFi connection fails
    WiFi.disconnect(true, true);
    ESP.restart();
    Serial.println("WiFi failed to connect");
  }

  #endif
  #ifdef HD_OSC
  OscWiFi.subscribe(OSC_PORT, "/emit", onEmit);
  OscWiFi.subscribe(OSC_PORT, "/note_on", onNoteOn);
  OscWiFi.subscribe(OSC_PORT, "/note_off", onNoteOff);
  OscWiFi.subscribe(OSC_PORT, "/notes_set", onNotesSet);
  OscWiFi.subscribe(OSC_PORT, "/palette", onPalette);
  OscWiFi.subscribe(OSC_PORT, "/color", onColor);
  OscWiFi.subscribe(OSC_PORT, "/split", onSplit);
  OscWiFi.subscribe(OSC_PORT, "/command", onCommand);
  #endif
}

void update() {
  gMillis = millis();
  #ifdef HD_DEBUGGER
  debugger->update(gMillis);
  #endif
  state->autoEmit(gMillis);
  state->update();
}

void draw() {
  #ifdef USE_NEOPIXELBUS
    for (uint16_t i=0; i<PIXEL_COUNT1; i++) {
      strip1.SetPixelColor(i, getNeoPixelColor(i));
    }
    for (uint16_t i=0; i<PIXEL_COUNT2; i++) {
      strip2.SetPixelColor(i, getNeoPixelColor(PIXEL_COUNT1+i));
    }
    strip1.Show();
    strip2.Show();
  #endif
  
  #ifdef USE_FASTLED
    for (uint16_t i=0; i<PIXEL_COUNT1; i++) {
      leds1[i] = getFastLEDColor(i);
    }
    for (uint16_t i=0; i<PIXEL_COUNT2; i++) {
      leds2[i] = getFastLEDColor(PIXEL_COUNT1+i);
    }
    FastLED.show();
  #endif
}

#ifdef USE_NEOPIXELBUS
RgbColor getNeoPixelColor(uint16_t i) {
  ColorRGB pixel = state->getPixel(i, MAX_BRIGHTNESS);
  RgbColor color = RgbColor(pixel.R, pixel.G, pixel.B);
  #ifdef HD_DEBUGGER
  if (showAll) {
    color.R = MAX_BRIGHTNESS / 2;
  }
  if (showConnections) {
    color.G = (debugger->isConnection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showIntersections) {
    color.B = (debugger->isIntersection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showPalette && i < 256) {
    pixel = state->paletteColor(i);
    color = RgbColor(pixel.R, pixel.G, pixel.B);
  }
  #endif
  return colorGamma.Correct(color);
}
#endif

#ifdef USE_FASTLED
CRGB getFastLEDColor(uint16_t i) {
  ColorRGB pixel = state->getPixel(i, MAX_BRIGHTNESS);
  CRGB color = CRGB(pixel.R, pixel.G, pixel.B);
  #ifdef HD_DEBUGGER
  if (showAll) {
    color.r = MAX_BRIGHTNESS / 2;
  }
  if (showConnections) {
    color.g = (debugger->isConnection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showIntersections) {
    color.b = (debugger->isIntersection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showPalette && i < 256) {
    pixel = state->paletteColor(i);
    color = CRGB(pixel.R, pixel.G, pixel.B);
  }
  #endif
  return color;
}
#endif

void loop() {
  #ifdef HD_OSC
  OscWiFi.update();
  #endif

  update();
  draw();

  #ifdef HD_SERIAL
  readSerial();
  #endif
}

void readSerial() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    doCommand(incomingByte);
  }
}

void doCommand(char command) {
  switch (command) {
    case 'd': {
      EmitParams params(M_STAR, 0.5);
      params.setLength(3);
      params.from = 1;
      // todo: fix, infinite duration does not work
      // params.duration = INFINITE_DURATION;
      params.duration = 100000;
      doEmit(params); 
      break;
    }
    case 'r':
      ESP.restart();
      break;
    case '.':
      state->stopAll();
      break;
    case '!':
      state->colorAll();
      break;        
    case 's':
      state->splitAll();
      break;
    case 'h':
      Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
      break;
    #ifdef HD_DEBUGGER
    case 'f':
      Serial.printf("FPS: %f (%d)\n", debugger->getFPS(), state->totalLights);
      break;
    case 'e':
      Serial.printf("Emits per frame: %f\n", debugger->getNumEmits());
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
      if (state->currentPalette < 32)
      state->currentPalette++;
      break;
    case '<':
      if (state->currentPalette > 0) {
        state->currentPalette--;
      }
      break;
    case 'l':
      Serial.printf("Total %d lights\n", state->totalLights);
      break;
    case 'E':
      state->autoEnabled = !state->autoEnabled;
      Serial.printf("AutoEmitter is %s\n", state->autoEnabled ? "enabled" : "disabled");
      break;      
    case 'L':
      state->debug();
      break;
    case 'C':
      debugger->dumpConnections();
      break;
    case 'I':
      debugger->dumpIntersections();
      break;
    #endif
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7': {
      int model = command - '1';
      if (model <= HeptagonStarModel::M_LAST) {
        EmitParams params(model, LPRandom::randomSpeed());
        doEmit(params); 
      }
      else { // 8 and up
        EmitParams params(M_STAR);
        params.colorChangeGroups |= GROUP1;
        doEmit(params);
      }
      break;
    }
    case '+': {
      EmitParams params(M_SPLATTER, LPRandom::randomSpeed());
      params.linked = false;
      params.duration = max(1, (int) (LPRandom::MAX_SPEED/params.speed) + 1) * EmitParams::frameMs();
      doEmit(params);
      break;
    }
    case '*': {
      // works reliably with M_STAR, other models might or might not work
      EmitParams params(M_STAR);
      params.speed = 0;
      params.fadeSpeed = 1;
      params.fadeThresh = 127;
      params.order = LIST_ORDER_RANDOM;
      params.behaviourFlags |= B_POS_CHANGE_FADE;
      doEmit(params);
      break;        
    }
    case '/': {
      EmitParams params;
      params.behaviourFlags |= B_RENDER_SEGMENT;
      params.setLength(1);
      doEmit(params);
      break;
    }
    case '-': { // emitBounce
      EmitParams params(M_STAR);
      params.behaviourFlags |= B_FORCE_BOUNCE;
      doEmit(params);
      break;
    }
    case '?': { // emitNoise
      EmitParams params;
      //params.order = LIST_NOISE;
      params.behaviourFlags |= B_BRI_CONST_NOISE;
      doEmit(params);
      break;
    }
  }
}

void doEmit(EmitParams &params) {
  int8_t i = state->emit(params);
  #ifdef HD_DEBUGGER
  debugger->countEmit();
  #endif
  // todo: I think HD_DEBUG does not exist anymore
  #ifdef HD_DEBUG
  LP_LOGF("emitting %d %s lights (%d/%.1f/%d/%d/%d/%d), total: %d (%d)\n",
    state->lightLists[i]->numLights, 
    (params.linked ? "linked" : "random"), 
    params.model, 
    params.speed, 
    state->lightLists[i]->length, 
    state->lightLists[i]->lifeMillis, 
    state->lightLists[i]->maxBri, 
    params.fadeSpeed, 
    state->totalLights + state->lightLists[i]->numLights, 
    state->totalLightLists + 1);
  #endif  
}

#ifdef HD_OSC
void parseParams(EmitParams &p, const OscMessage &m) {
  for (uint8_t i=0; i<m.size() / 2; i++) {
    EmitParam param = static_cast<EmitParam>(m.arg<uint8_t>(i*2));
    uint8_t j = i*2+1;
    parseParam(p, m, param, j);
  }
}

void parseParam(EmitParams &p, const OscMessage &m, EmitParam &param, uint8_t j) {
  switch (param) {
      case P_MODEL: {
        int8_t model = m.arg<int8_t>(j);
        if (model >= HeptagonStarModel::M_FIRST && model <= HeptagonStarModel::M_LAST) {
          p.model = model;
        }
        break;
      }
      case P_SPEED:
        p.speed = m.arg<float>(j);
        break;
      case P_EASE:
        p.ease = m.arg<uint8_t>(j);
        break;        
      case P_LENGTH:
        p.setLength(m.arg<uint16_t>(j));
        break;
      case P_TRAIL:
        p.trail = m.arg<uint16_t>(j);
        break;
      case P_FADE:
        p.fadeSpeed = m.arg<uint8_t>(j);
        break;
      case P_FADE_THRESH:
        p.fadeThresh = m.arg<uint8_t>(j);
        break;
      case P_ORDER: {
        uint8_t order = m.arg<uint8_t>(j);
        if (order >= ListOrder::LO_FIRST && order <= ListOrder::LO_LAST) {
          p.order = static_cast<ListOrder>(order);
        }
        break;
      }
      case P_LINKED:
        p.linked = m.arg<uint8_t>(j) > 0;
        break;
      case P_FROM:
        p.from = m.arg<int8_t>(j);
        break;
      case P_DURATION_MS:
        p.duration = m.arg<uint32_t>(j);
        break;
      case P_DURATION_FRAMES:
        p.duration = m.arg<uint32_t>(j);
        break;
      case P_COLOR:
        p.color = ColorRGB(m.arg<uint32_t>(j));        
        break;
      case P_COLOR_INDEX: {
        int16_t index = m.arg<int16_t>(j);
        if (index == RANDOM_COLOR) {
          p.color.setRandom();
        }
        else {
          p.color = state->paletteColor(index);        
        }
        break;
      }
      case P_NOTE_ID:
        p.noteId = m.arg<uint16_t>(j);
        break;
      case P_MIN_BRI:
        p.minBri = m.arg<uint8_t>(j);
        break;
      case P_MAX_BRI:
        p.maxBri = m.arg<uint8_t>(j);
        break;
      case P_BEHAVIOUR:
        p.behaviourFlags = m.arg<uint16_t>(j);
        break;
      case P_EMIT_GROUPS:
        p.emitGroups = m.arg<uint8_t>(j);
        break;
      case P_COLOR_CHANGE_GROUPS:
        p.colorChangeGroups = m.arg<uint8_t>(j);
        break;
    }
}

void onCommand(const OscMessage& m) {
  String command = m.arg<String>(0);
  for (uint8_t i=0; i<command.length(); i++) {
    doCommand(command.charAt(i));
  }
}

void onEmit(const OscMessage& m) {
  EmitParams params;
  parseParams(params, m);
  doEmit(params);
}

void onNoteOn(const OscMessage& m) {
  EmitParams params;
  params.duration = INFINITE_DURATION;
  parseParams(params, m);
  doEmit(params);
}

void onNoteOff(const OscMessage& m) {
  if (m.size() > 0) {
    uint16_t noteId = m.arg<uint16_t>(0);
    state->stopNote(noteId);
  }
  else {
    state->stopAll();
  }
}

void onNotesSet(const OscMessage& m) {
  EmitParams notesSet[MAX_NOTES_SET];
  for (uint8_t i=0; i<m.size() / 3; i++) {
    uint16_t noteId = m.arg<uint16_t>(i*3);
    uint8_t k = 0;
    for (k; k<MAX_NOTES_SET; k++) {
      if (notesSet[k].noteId == 0 || notesSet[k].noteId == noteId) {
        notesSet[k].noteId = noteId;
        break;
      }
    }
    EmitParam param = static_cast<EmitParam>(m.arg<uint8_t>(i*3+1));
    uint8_t j = i*3+2;
    parseParam(notesSet[k], m, param, j);
  }
  for (uint8_t i=0; i<MAX_NOTES_SET; i++) {
    if (notesSet[i].noteId > 0) {
      doEmit(notesSet[i]);      
    }
  }
}

void onPalette(const OscMessage& m) {
  if (m.size() > 0) {
    state->currentPalette = m.arg<uint8_t>(0);
  }
}

void onColor(const OscMessage &m) {
  if (m.size() > 0) {
    uint8_t i = m.arg<uint8_t>(0);
    ColorRGB color;
    if (m.size() > 1) {
      color.set(m.arg<uint32_t>(1));
    }
    else {
      color.setRandom();
    }
    state->lightLists[i]->setColor(color);
  }
  else {
    state->colorAll();
  }
}

void onSplit(const OscMessage &m) {
  if (m.size() > 0) {
    uint8_t i = m.arg<uint8_t>(0);
    state->lightLists[i]->split();
  }
  else {
    state->splitAll();
  }
}

void onAuto(const OscMessage &m) {
  state->autoEnabled = !state->autoEnabled;
}
#endif
