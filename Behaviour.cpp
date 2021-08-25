#include "Behaviour.h"
#include "Light.h"
#include "Model.h"

float Behaviour::getPosition(Light *light) {
  switch (posChangeBe) {
    case B_CHANGE_POS_FADE:
      if (light->bri >= 2.f) {
        light->bri -= 2.f;
        return random(light->getModel()->getMaxLength());
      }
  }
  return light->position + light->speed;
}

RgbColor Behaviour::getColor(Light *light, uint8_t group) {
  if (light->linkedPrev != NULL) {
    return light->linkedPrev->color;
  }
  return RgbColor(random(255), random(255), random(255));
}