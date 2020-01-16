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
  #ifndef HD_TEST
  emitter->emit();
  heptagon.update();
  emitter->update();
  #endif
}

void draw() {
  for (int i=0; i<PIXEL_COUNT; i++) {
    #ifdef HD_TEST
    strip.SetPixelColor(i, RgbColor((heptagon.isIntersection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS, 0, 0));
    #else
    strip.SetPixelColor(i, RgbColor(min(emitter->pixelValues[i], 1.f) * MAX_BRIGHTNESS, 0, 0));
    #endif
  }
  strip.Show();
}

void loop() {
  update();
  draw();
  #ifdef HD_DEBUG
  static unsigned long lastDebug = 0;
  unsigned long ms = millis();
  if (ms - lastDebug > 10000) {
    lastDebug = ms;
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());
  }
  #endif
}
