#define PIXEL_PIN1 14
#define PIXEL_PIN2 26
#define BUTTON_PIN 25
#define WIFI_HOSTNAME "homo-deus"
#define WIFI_SSID "k"
#define WIFI_PASS "letmeinplease"
//#define WIFI_SSID "Redmi"
//#define WIFI_PASS "kaspars123"
//#define WIFI_SSID "VA37-3"
//#define WIFI_PASS "fdsa4321");
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
#define MAX_BRIGHTNESS 255

#include "BluetoothSerial.h"
#include "esp_bt.h"
#include <NeoPixelBus.h>
#include "src/HeptagonStar.h"
#include "src/Globals.h"
#include "src/LPRandom.h"

#ifdef WIFI_SSID
#include <WiFi.h>
#endif
#ifdef HD_OSC
#include <ArduinoOSC.h>
#endif
#ifdef HD_DEBUGGER
#include "src/LPDebugger.h"
#endif

NeoPixelBus<NeoGrbFeature, NeoWs2813Method> strip1(PIXEL_COUNT1, PIXEL_PIN1);
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt5Ws2812xMethod> strip2(PIXEL_COUNT2, PIXEL_PIN2);
NeoGamma<NeoGammaTableMethod> colorGamma;
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

  strip1.Begin();
  strip1.Show();

  strip2.Begin();
  strip2.Show();

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
    WiFi.disconnect(true, true);
    ESP.restart();
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
    case 'r':
      ESP.restart();
      break;
    case 'e':
      state->autoEnabled = !state->autoEnabled;
      Serial.printf("AutoEmitter is %s\n", state->autoEnabled ? "enabled" : "disabled");
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
    case '6': {
      EmitParams params(command - '1', LPRandom::randomSpeed());
      doEmit(params); 
      break;
    }
    case '7': {
      EmitParams params(M_STAR);
      params.colorChangeGroups |= GROUP1;
      doEmit(params);
      break;
    }
    case '+': {
      EmitParams params(M_SPLATTER, LPRandom::randomSpeed());
      params.linked = false;
      params.duration = max(1, (int) (1.f/params.speed) + 1) * EmitParams::frameMs();
      doEmit(params);
      break;
    }
    case '*': {
      EmitParams params(EmitParams::DEFAULT_MODEL, 0);
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
      params.length = 1;
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
    switch (param) {
      case P_MODEL:
        p.model = m.arg<int8_t>(j);
        break;
      case P_SPEED:
        p.speed = m.arg<float>(j);
        break;
      case P_EASE:
        p.ease = m.arg<uint8_t>(j);
        break;        
      case P_LENGTH:
        p.length = m.arg<uint16_t>(j);
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
      case P_ORDER:
        p.order = static_cast<ListOrder>(m.arg<uint8_t>(j));
        break;
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
        p.behaviourFlags = m.arg<uint8_t>(j);
        break;
      case P_EMIT_GROUPS:
        p.emitGroups = m.arg<uint8_t>(j);
        break;
      case P_COLOR_CHANGE_GROUPS:
        p.colorChangeGroups = m.arg<uint8_t>(j);
        break;        
    }
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

void onPalette(const OscMessage& m) {
  if (m.size() > 0) {
    state->currentPalette = m.arg<uint8_t>(0);
  }
}

void onColor(const OscMessage &m) {
  if (m.size() > 0) {
    uint8_t i = m.arg<uint8_t>(0);
    if (m.size() > 1) {
      uint8_t color = m.arg<uint8_t>(1);
      state->lightLists[i]->setColor(state->paletteColor(color));
    }
    else {
      state->lightLists[i]->setColor(state->randomColor());
    }
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
