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

  // todo: should not use BrightnessBus
  // strip.SetBrightness(32);
  strip.Begin();
  strip.Show();

  pinMode(BUTTON_PIN, INPUT);

  setupModels();
}

void setupModels() {
  emitter = new Emitter(heptagon.models, heptagon.outerNeurons);
}

void loop() {
  update();
  draw();
}

void update() {
  emitter->emit();
  heptagon.update();
  emitter->update();
}

void draw() {
  for (int i=0; i<PIXEL_COUNT; i++) {
    strip.SetPixelColor(i, RgbColor(min(emitter->pixelValues[i], 1.f)));
  }
  strip.Show();
}
