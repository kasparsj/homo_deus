#include "Behaviour.h"
#include "Light.h"
#include "Model.h"
#include "Globals.h"

float Behaviour::getBri(Light *light) {
  if (flags & B_BRI_CONST_NOISE) {
    return gPerlinNoise.GetValue(light->parent->id * 10, light->pixel1 * 100);
  }
  return light->bri + light->parent->fadeSpeed;
}

float Behaviour::getPosition(Light *light) {
  if (flags & B_POS_CHANGE_FADE) {
    if (light->bri >= 2.f) {
      light->bri -= 2.f;
      return LP_RANDOM(light->getModel()->getMaxLength());
    }  
  }
  return light->position + light->speed;
}

ColorRGB Behaviour::getColor(Light *light, uint8_t group) {
  if (light->linkedPrev != NULL) {
    return light->linkedPrev->color;
  }
  return ColorRGB(LP_RANDOM(255), LP_RANDOM(255), LP_RANDOM(255));
}
