#include "Light.h"

const float Light::SPEED = 1.0;
const int Light::LIFE = 60;

void Light::update() {
  position += speed;
  age++;
}

void Light::resetPixels() {
  pixel1 = -1;
  pixel1Bri = 0;
  pixel2 = -1;
  pixel2Bri = 0;
}
