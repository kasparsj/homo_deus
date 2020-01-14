#include "LightList.h"
#include "Light.h"
#include "Model.h"

FastNoise LightList::fastNoise;

void LightList::setup(int numLights) {
  lights = new Light*[numLights]();
}

LightList* LightList::setupRandom(int numLights, boolean linked) {
  setup(numLights);
  for (int i=0; i<numLights; i++) {
    Light *linkedPrev = linked && i > 0 ? (*this)[i - 1] : NULL;
    (*this)[i] = new Light(random(1.0), speed, life, model, linkedPrev);
  }
  return this;
}

LightList* LightList::setupNoise(int numLights, float threshold) {
  setup(numLights);
  float noiseId = random(100000);
  for (int i=0; i<numLights; i++) {
    Light *linkedPrev = linked && i > 0 ? (*this)[i - 1] : NULL;
    float whiteNoise = LightList::fastNoise.GetWhiteNoiseInt(noiseId, i);
    (*this)[i] = new Light(threshold + ((whiteNoise + 1.0) / 2.0) * (1.0 - threshold), speed, life, model, linkedPrev);
  }
  return this;
}

LightList* LightList::setModel(Model *model) {
  this->model = model;
  for (int i=0; i<numLights; i++) {
    (*this)[i]->model = model; 
  }
  return this;    
}

LightList* LightList::setLinked(bool linked) {
  this->linked = linked;
  for (int i=1; i<numLights; i++) {
    (*this)[i]->linkedPrev = linked ? (*this)[i-1] : NULL; 
  }
  return this;
}

LightList* LightList::setSpeed(float speed) {
  this->speed = speed;
  for (int i=0; i<numLights; i++) {
    (*this)[i]->speed = speed;
  }
  return this;
}

LightList* LightList::setLife(int numFrames) {
  this->life = numFrames;
  for (int i=0; i<numLights; i++) {
    (*this)[i]->life = numFrames;
  }
  return this;
}
