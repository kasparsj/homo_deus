#include "Behaviour.h"
#include "Light.h"
#include "Model.h"

float Behaviour::getPosition(Light *light) {
  switch (bePos) {
    case B_POS_FADE:
      if (round(fmod(light->bri, 2.f) * 1000) == 0) {
        return random(light->getModel()->getMaxLength());
      }
      return light->position;
      break;
    default:
      return light->position + light->speed;
  }
}

RgbColor Behaviour::changeColor(Light *light) {
  if (light->linkedPrev != NULL) {
    return light->linkedPrev->color;
  }
  return RgbColor(random(255), random(255), random(255));
}