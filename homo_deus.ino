//#include <NeoPixelBrightnessBus.h>
#include <NeoPixelBus.h>
#include "BluetoothSerial.h"

#include "Config.h"
#include "HeptagonStar.h"
#include "Emitter.h"

NeoGamma<NeoGammaTableMethod> colorGamma;
NeoPixelBus<NeoGrbFeature, NeoWs2813Method> strip(PIXEL_COUNT, PIXEL_PIN);
HeptagonStar heptagon;
Emitter *emitter;
bool showIntersections = false;
bool showConnections = false;

void setup() {
  Serial.begin(115200);

  heptagon.setup();

  // todo: should not use BrightnessBus
  // strip.SetBrightness(32);
  strip.Begin();
  strip.Show();

  pinMode(BUTTON_PIN, INPUT);

  emitter = new Emitter(heptagon.models, heptagon.outerNeurons);

  #ifdef HD_DEBUG
  Serial.println("setup complete");
  #endif
}

void update() {
  emitter->emit();
  heptagon.update();
  emitter->update();
}

void draw() {
  for (int i=0; i<PIXEL_COUNT; i++) {
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
    strip.SetPixelColor(i, color);
  }
  strip.Show();
}

void loop() {
  update();
  draw();
  #ifdef HD_DEBUG
  readSerial();
  printHeap();
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

void printHeap() {    
  static unsigned long lastDebug = 0;
  unsigned long ms = millis();
  if (ms - lastDebug > 10000) {
    lastDebug = ms;
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());
  }
}
