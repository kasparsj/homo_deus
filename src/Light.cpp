#include <math.h>
#include "Light.h"
#include "LightList.h"

Light::Light(LightList *list, float speed, int16_t life, uint16_t idx, float maxBri) : LPLight(list, life, idx, maxBri) {
  this->speed = speed;
  this->color = ColorRGB(255, 255, 255);
}

float Light::getBrightness() {
  float value = fmod(bri, 2.f);
  value = (value > 1.f ? 2.f - value : value);
  value = (value - list->fadeThresh) / (1.f - list->fadeThresh);
  // hack: float inprecision
  return round(value * maxBri * 10000) / 10000.f;
}

ColorRGB Light::getPixelColor() {
    if (brightness == 1.f) {
        return color;
    }
    return color.Dim(255 * brightness);
}

void Light::nextFrame() {
  bri = list->getBri(this);
  brightness = max(0.f, getBrightness());
  if (list == NULL) {
    position += speed;
  }
  else {
    position = list->getPosition(this);
  }
  age++;
}

bool Light::shouldExpire() {
  if (life == 0) {
    return false;
  }
  return age >= life && (list->fadeSpeed == 0 || brightness == 0);
}

Model* Light::getModel() {
  if (list != NULL) {
    return list->model;
  }
  return NULL;
}

Behaviour* Light::getBehaviour() {
  if (list != NULL) {
    return list->behaviour;
  }
  return NULL;
}
