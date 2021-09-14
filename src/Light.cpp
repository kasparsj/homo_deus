#include <math.h>
#include "Light.h"
#include "LightList.h"
#include "Globals.h"

Light::Light(LightList *list, float speed, uint32_t lifeMillis, uint16_t idx, uint8_t maxBri) : LPLight(list, idx, maxBri) {
    this->speed = speed;
    this->lifeMillis = lifeMillis;
    this->color = ColorRGB(255, 255, 255);
}

uint8_t Light::getBrightness() {
  uint16_t value = bri % 511;
  value = (value > 255 ? 511 - value : value);
  return (value - list->fadeThresh) / (255 - list->fadeThresh) * maxBri;
}

ColorRGB Light::getPixelColor() {
    if (brightness == 255) {
        return color;
    }
    return color.Dim(brightness);
}

void Light::nextFrame() {
  bri = list->getBri(this);
  brightness = getBrightness();
  if (list == NULL) {
    position += speed;
  }
  else {
    position = list->getPosition(this);
  }
}

bool Light::shouldExpire() {
  if (lifeMillis == INFINITE_DURATION) {
    return false;
  }
  return gMillis >= lifeMillis && (list->fadeSpeed == 0 || brightness == 0);
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
