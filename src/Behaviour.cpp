#include "Behaviour.h"
#include "LPLight.h"
#include "Model.h"
#include "Globals.h"

float Behaviour::getBri(LPLight *light) {
  if (flags & B_BRI_CONST_NOISE) {
    return gPerlinNoise.GetValue(light->parent->id * 10, light->pixel1 * 100);
  }
  return light->bri + light->parent->fadeSpeed;
}

float Behaviour::getPosition(LPLight *light) {
  if (flags & B_POS_CHANGE_FADE) {
    if (light->bri >= 2.f) {
      light->bri -= 2.f;
      return LP_RANDOM(light->getModel()->getMaxLength());
    }
  }
  return light->position + light->parent->speed;
}

ColorRGB Behaviour::getColor(LPLight *light, uint8_t group) {
  if (light->getPrev() != NULL) {
    return light->getPrev()->getColor();
  }
  return ColorRGB(LP_RANDOM(255), LP_RANDOM(255), LP_RANDOM(255));
}
