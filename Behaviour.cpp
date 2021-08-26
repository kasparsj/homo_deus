#include "Behaviour.h"
#include "Light.h"
#include "Model.h"
#include "Globals.h"

float Behaviour::getBri(Light *light) {
  switch (briChangeBe) {
    case B_BRI_CONST_NOISE:
      light->bri = gPerlinNoise.GetValue(light->parent->id * 10, light->pixel1 * 100);
      break;
  }
  return light->bri + light->parent->fadeSpeed;
}

float Behaviour::getPosition(Light *light) {
  switch (posChangeBe) {
    case B_POS_CHANGE_FADE:
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