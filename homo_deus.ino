//#include <NeoPixelBrightnessBus.h>
#include <NeoPixelBus.h>
#include "BluetoothSerial.h"

#include "Config.h"
#include "HeptagonStar.h"
#include "Emitter.h"

NeoGamma<NeoGammaTableMethod> colorGamma;
NeoPixelBus<NeoGrbFeature, NeoWs2813Method> strip1(PIXEL_COUNT1, PIXEL_PIN1);
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt5Ws2812xMethod> strip2(PIXEL_COUNT2, PIXEL_PIN2);
HeptagonStar heptagon;
Emitter *emitter;
bool showIntersections = false;
bool showConnections = false;
unsigned long prevMillis = 0;
float fps = 0.f;

void setup() {
  Serial.begin(115200);

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

void update() {
  unsigned long ms = millis();
  fps = 1000.f / float(ms - prevMillis);
  prevMillis = ms;
  emitter->emit(ms);
  heptagon.update();
  emitter->update();
}

void draw() {
  for (int i=0; i<PIXEL_COUNT1; i++) {
    strip1.SetPixelColor(i, getColor(i));
  }
  for (int i=0; i<PIXEL_COUNT2; i++) {
    strip2.SetPixelColor(i, getColor(PIXEL_COUNT1+i));
  }
  strip1.Show();
  strip2.Show();
}

RgbColor getColor(int i) {
  RgbColor color = RgbColor(0, 0, 0);
  color.R = min(emitter->pixelValues[i], 1.f) * MAX_BRIGHTNESS;
  #ifdef HD_TEST
  if (showIntersections) {
    color.B = (heptagon.isIntersection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showConnections) {
    color.G = (heptagon.isConnection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  #endif
  return color;
}

void loop() {
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
      case 'i':
        showIntersections = !showIntersections;
        break;
      case 'c':
        showConnections = !showConnections;
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
