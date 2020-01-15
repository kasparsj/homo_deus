#include "Emitter.h"
#include "Model.h"
#include "HeptagonStar.h"
#include <Arduino.h>

float Emitter::randomSpeed() {
  return minSpeed + random(max(maxSpeed - minSpeed, 0.f));
}

int Emitter::randomLife() {
  return minLife + (int) random(max(maxLife - minLife, 0));
}

int Emitter::randomModel() {
  return floor(random(NUM_MODELS));
}

int Emitter::randomLength() {
  return (int) (minLength + random(max(maxLength - minLength, 0)));
}

float Emitter::randomBriThresh() {
  return minBriThresh + random(max(maxBriThresh - minBriThresh, 0.f));
}

void Emitter::emit() {
  int ms = millis();
  if (nextEmit <= ms) {
    emitNew();
    nextEmit = (int) (ms + (30 + random(250) * 1000/60));
  }
}

void Emitter::emitNew(int which, float speed, int life) {
  for (int i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) {
      lightLists[i] = new LightList();
      lightLists[i]->setSpeed(speed);
      lightLists[i]->setModel(&models[which]);
      lightLists[i]->setLinked(true);
      lightLists[i]->setLife(life);
      lightLists[i]->setupNoise(randomLength(), randomBriThresh());
      switch (which) {
        case M_DEFAULT:
          break;
        case M_INNER_CIRCLE:
          break;
        case M_STAR:
          break;
        case M_OUTER_STAR:
          break;
      }
      intersections[(int) random(7)].emit(lightLists[i]);
      // todo: fix
      //OscMessage msg = new OscMessage("/hd/spawn");
      //msg.add(life / 50.f);
      //oscP5.send(msg, supercollider);
      break;  
    }
  }
}

void Emitter::update() {
  memset(pixelValues, 0, sizeof(pixelValues));
  for (int i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    bool allExpired = true;
    for (int j=0; j<lightLists[i]->numLights; j++) {
      if (lightLists[i]->lights[j] == NULL) {
        continue;
      }
      if (lightLists[i]->lights[j]->isExpired) {
        delete lightLists[i]->lights[j];
        lightLists[i]->lights[j] = NULL;
        continue;
      }
      allExpired = false;
      if (lightLists[i]->lights[j]->pixel1 >= 0) {
        pixelValues[lightLists[i]->lights[j]->pixel1] += lightLists[i]->lights[j]->pixel1Bri;
      }
      if (lightLists[i]->lights[j]->pixel2 >= 0) {
        pixelValues[lightLists[i]->lights[j]->pixel2] += lightLists[i]->lights[j]->pixel2Bri;
      }
    }
    if (allExpired) {
      delete lightLists[i];
      lightLists[i] = NULL;
    }
  }
}
