#include "LightList.h"
#include "Light.h"
#include "Model.h"
#include <Arduino.h>

//FastNoise LightList::fastNoise;

void LightList::setup(uint16_t numLights) {
  this->numLights = numLights;
  lights = new Light*[numLights]();
  for (uint16_t i=0; i<numLights; i++) {
    lights[i] = NULL;
  }
}

void LightList::setupRandom(uint16_t numLights, bool linked) {
  setup(numLights);
  for (uint16_t i=0; i<numLights; i++) {
    Light *linkedPrev = linked && i > 0 ? get(i - 1) : 0;
    Light *light = new Light(random(1.0), speed, life, model, linkedPrev);
    //light->id = i;
    set(i, light);
  }
}

// void LightList::setupNoise(uint16_t numLights, float threshold) {
//   setup(numLights);
//   float noiseId = random(100000);
//   for (uint16_t i=0; i<numLights; i++) {
//     float whiteNoise = LightList::fastNoise.GetWhiteNoiseInt(noiseId, i);
//     Light *linkedPrev = linked && i > 0 ? get(i - 1) : 0;
//     Light *light = new Light(threshold + ((whiteNoise + 1.0) / 2.0) * (1.0 - threshold), speed, life, model, linkedPrev);
//     light->id = i;
//     set(i, light);
//   }
// }

void LightList::setupFull(uint16_t numLights, RgbColor color) {
  setup(numLights + trail);
  for (uint16_t i=0; i<numLights; i++) {
    Light *linkedPrev = linked && i > 0 ? get(i - 1) : 0;
    Light *light = new Light(1.0, speed, life, model, linkedPrev);
    light->setColor(color);
    //light->id = i;
    set(i, light);
  }
  for (uint16_t i=0; i<trail; i++) {
    Light *linkedPrev = linked ? get(numLights + i - 1) : 0;
    Light *light = new Light(1.0, speed, life, model, linkedPrev);
    uint8_t dim = 255 - (255 / (trail + 1)) * (i + 1);
    light->setColor(color.Dim(dim));
    //light->id = numLights + i;
    set(numLights + i, light);
  }
}

void LightList::setModel(Model *model) {
  this->model = model;
  for (int i=0; i<numLights; i++) {
    get(i)->model = model; 
  }
}

void LightList::setLinked(bool linked) {
  this->linked = linked;
  for (uint16_t i=1; i<numLights; i++) {
    get(i)->linkedPrev = linked ? get(i-1) : 0; 
  }
}

void LightList::setSpeed(float speed) {
  this->speed = speed;
  for (uint16_t i=0; i<numLights; i++) {
    get(i)->speed = speed;
  }
}

void LightList::setLife(uint16_t numFrames) {
  this->life = numFrames;
  for (uint16_t i=0; i<numLights; i++) {
    get(i)->life = numFrames;
  }
}
