#include "Behaviour.h"
#include "LPLight.h"
#include "Model.h"
#include "Globals.h"

uint16_t Behaviour::getBri(LPLight *light) {
  if (flags & B_BRI_CONST_NOISE) {
    return gPerlinNoise.GetValue(light->list->id * 10, light->pixel1 * 100) * 255;
  }
  return light->bri + light->list->fadeSpeed;
}

float Behaviour::getPosition(LPLight *light) {
  if (flags & B_POS_CHANGE_FADE) {
    if (light->bri >= 511) {
      light->bri -= 511;
      return LP_RANDOM(light->getModel()->getMaxLength());
    }
  }
  return light->position + light->list->speed;
}

ColorRGB Behaviour::getColor(LPLight *light, uint8_t group) {
  if (light->getPrev() != NULL) {
    return light->getPrev()->getColor();
  }
  return ColorRGB(LP_RANDOM(255), LP_RANDOM(255), LP_RANDOM(255));
}
