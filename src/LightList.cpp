#include "LightList.h"
#include "Light.h"
#include "Model.h"
#include "Globals.h"

uint16_t LightList::nextId = 0;

void LightList::init(uint16_t numLights) {
  this->numLights = numLights;
  lights = new Light*[numLights]();
  for (uint16_t i=0; i<numLights; i++) {
    lights[i] = 0;
  }
}

void LightList::setup(uint16_t numLights, ColorRGB color, float brightness, float fadeSpeed, float fadeThresh) {
  init(numLights + trail);
  setFade(fadeSpeed, fadeThresh);
  for (uint16_t i=0; i<numLights; i++) {
    Light *linkedPrev = linked && i > 0 ? (*this)[i - 1] : 0;
    Light *light = new Light(brightness, speed, life, this, linkedPrev);
    light->setColor(color);
    #ifdef LP_DEBUG
    light->id = i;
    #endif
    (*this)[i] = light;
  }
  for (uint16_t i=0; i<trail; i++) {
    Light *linkedPrev = linked ? (*this)[numLights + i - 1] : 0;
    float bri = (255.f - (255.f / (trail + 1)) * (i + 1)) / 255.f;
    Light *light = new Light(brightness * bri, speed, life, this, linkedPrev);
    light->setColor(color);
    #ifdef LP_DEBUG
    light->id = numLights + i;
    #endif
    (*this)[numLights + i] = light;
  }
}

void LightList::setLinked(bool linked) {
  this->linked = linked;
  for (uint16_t i=1; i<numLights; i++) {
    if ((*this)[i] == 0) continue;
    (*this)[i]->linkedPrev = linked ? (*this)[i-1] : 0; 
  }
}

void LightList::setSpeed(float speed) {
  this->speed = speed;
  for (uint16_t i=0; i<numLights; i++) {
    if ((*this)[i] == 0) continue;
    (*this)[i]->speed = speed;
  }
}

void LightList::setLife(int16_t numFrames) {
  this->life = numFrames;
  for (uint16_t i=0; i<numLights; i++) {
    if ((*this)[i] == 0) continue;
    (*this)[i]->life = numFrames;
  }
}

void LightList::setColor(ColorRGB color) {
  for (uint16_t i=0; i<numLights; i++) {
    if ((*this)[i] == 0) continue;
    (*this)[i]->setColor(color);
  }
}

void LightList::initEmit() {
  for (uint16_t i=0; i<numLights; i++) {
    Light *light = (*this)[i];
    initPosition(i, light);
    initBri(i, light);
    initLife(i, light);
  }
}

float LightList::getPosition(Light *light) {
  if (behaviour != 0) {
    return behaviour->getPosition(light);
  }
  return light->position + light->speed;
}

void LightList::initPosition(uint16_t i, Light* light) {
  float position = i * (speed != 0 ? -1.f : 1.f);
  if (order == LIST_ORDER_RANDOM) {
    position = LP_RANDOM(model->getMaxLength());
  }
  light->position = position;
}

void LightList::initBri(uint16_t i, Light* light) {
  switch (order) {
    case LIST_ORDER_RANDOM:
      if (fadeThresh > 0) {
        light->bri = (LP_RANDOM(fadeThresh * 3000.f) - (fadeThresh * 2000.f)) / 1000.f;
      }
      break;
    case LIST_ORDER_NOISE:
      light->bri = gPerlinNoise.GetValue(id * 10, i * 100);
      break;
  }
}

float LightList::getBri(Light *light) {
  if (behaviour != 0) {
    return behaviour->getBri(light);
  }
  return light->bri + fadeSpeed;
}

void LightList::initLife(uint16_t i, Light* light) {
  int16_t life = light->life;    
  if (life > 0 && order == LIST_ORDER_SEQUENTIAL) {
    life += ceil(1.0 / light->speed * i);
  }
  light->life = life;
}

void LightList::split() {
  numSplits++;
  if (numSplits < numLights) {
    for (uint8_t i=0; i<numSplits; i++) {
      uint16_t split = (i+1)*(numLights/(numSplits+1));
      if ((*this)[split] == 0) continue;
      (*this)[split]->linkedPrev = 0;
    }
    // todo: modify trail
  }
}
