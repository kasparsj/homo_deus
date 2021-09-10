#define PIXEL_PIN1 14
#define PIXEL_PIN2 26
#define BUTTON_PIN 25
#define HD_WIFI
#define HD_OSC
#define SC_HOST "192.168.43.101"
#define SC_PORT 57120
//#define LP_OSC_REPLY(I) OscWiFi.publish(SC_HOST, SC_PORT, "/emit", (I));
#define PIXEL_COUNT1 524
#define PIXEL_COUNT2 395
#define PIXEL_COUNT (PIXEL_COUNT1 + PIXEL_COUNT2)
#define OSC_PORT 54321

#include "BluetoothSerial.h"
#include "esp_bt.h"
#include <NeoPixelBus.h>
#include "src/HeptagonStar.h"

#ifdef HD_WIFI
#include <WiFi.h>
#endif
#ifdef HD_OSC
#include <ArduinoOSC.h>
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
unsigned long gMillis;
unsigned long gPrevMillis = 0;
#ifdef LP_TEST
float fps[AVG_FPS_FRAMES] = {0.f};
uint8_t fpsIndex = 0;
#endif
#ifdef HD_WIFI
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

  #ifdef LP_DEBUG
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
  OscWiFi.subscribe(OSC_PORT, "/command", onCommand);
  #endif
}

void update() {
  gMillis = millis();
  #ifdef LP_TEST
  fps[fpsIndex] = 1000.f / float(gMillis - gPrevMillis);
  fpsIndex = (fpsIndex + 1) % AVG_FPS_FRAMES;
  #endif
  gPrevMillis = gMillis;
  state->autoEmit(gMillis);
  heptagon.update();
  state->update();
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
  ColorRGB pixel = state->getPixel(i);
  RgbColor color = RgbColor(pixel.R, pixel.G, pixel.B);
  #ifdef LP_TEST
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

  #ifdef LP_DEBUG
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
      Serial.printf("AutoEmitter is %s", state->autoEnabled ? "enabled" : "disabled");
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
    #ifdef LP_TEST
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
      heptagon.dumpConnections();
      break;
    case 'I':
      heptagon.dumpIntersections();
      break;
    #endif
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
      state->emit(command - '1');
      break;
    case '7': {
      EmitParams p;
      p.model = M_STAR;
      p.colorChangeGroups |= GROUP1;
      state->emit(p);
      break;
    }
    case '+':
      state->emitSplatter();
      break;
    case '*':
      state->emitRandom();
      break;        
    case '/': { // emitSegment
      EmitParams params;
      params.behaviourFlags |= B_RENDER_SEGMENT;
      params.length = 1;
      state->emit(params);
      break;
    }
    case '-': { // emitBounce
      EmitParams params;
      params.model = M_STAR;
      params.behaviourFlags |= B_FORCE_BOUNCE;
      state->emit(params);
      break;
    }
    case '?': { // emitNoise
      EmitParams params;
      //params.order = LIST_NOISE;
      params.behaviourFlags |= B_BRI_CONST_NOISE;
      state->emit(params);
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
      case P_TRAIL:
        p.trail = m.arg<uint16_t>(j);
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
  state->emit(params);
}

void onNoteOn(const OscMessage& m) {
  EmitParams params;
  params.life = INFINITE_LIFE;
  parseParams(params, m);
  state->emit(params);
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
