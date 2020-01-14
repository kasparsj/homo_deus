#include <NeoPixelBrightnessBus.h>
#include "BluetoothSerial.h"

#include "HeptagonStar.h"
#include "Emitter.h"

const uint16_t PixelCount = 922; // make sure to set this to the number of pixels in your strip
const uint8_t PixelPin = 14;
const uint16_t ButtonPin = 26;

NeoGamma<NeoGammaTableMethod> colorGamma;
NeoPixelBrightnessBus<NeoGrbFeature, NeoWs2813Method> strip(PixelCount, PixelPin);
HeptagonStar heptagon;
Emitter *emitter;

void setup() {
  Serial.begin(115200);

  strip.SetBrightness(32);
  strip.Begin();
  strip.Show();

  pinMode(ButtonPin, INPUT);

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
  emitter->update();
  heptagon.update();
}

void draw() {
  //strip.SetPixelColor(seq.pos + j, seq.getColor(j));
  strip.Show();
}
