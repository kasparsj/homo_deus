#include "Emitter.h"
#include "Model.h"
#include "HeptagonStar.h"
#include <Arduino.h>

float Emitter::randomSpeed() {
  return EMITTER_MIN_SPEED + random(max(EMITTER_MAX_SPEED - EMITTER_MIN_SPEED, 0.f));
}

int Emitter::randomLife() {
  return EMITTER_MIN_LIFE + (int) random(max(EMITTER_MAX_LIFE - EMITTER_MIN_LIFE, 0));
}

int Emitter::randomModel() {
  return floor(random(NUM_MODELS));
}

int Emitter::randomLength() {
  return (int) (EMITTER_MIN_LENGTH + random(max(EMITTER_MAX_LENGTH - EMITTER_MIN_LENGTH, 0)));
}

float Emitter::randomBriThresh() {
  return EMITTER_MIN_BRI + random(max(EMITTER_MAX_BRI - EMITTER_MIN_BRI, 0.f));
}

void Emitter::emit() {
  int ms = millis();
  if (enabled && nextEmit <= ms) {
    emitNew();
    nextEmit = (int) (ms + (30 + random(250) * 1000/60));
  }
}

void Emitter::emitNew(int which, float speed, int life, int length) {
  for (int i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) {
      lightLists[i] = new LightList();
      lightLists[i]->setSpeed(speed);
      lightLists[i]->setModel(&models[which]);
      lightLists[i]->setLinked(true);
      lightLists[i]->setLife(life);
      lightLists[i]->setupNoise(length, randomBriThresh());
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
      intersections[(int) random(14)].emit(lightLists[i]);
      // todo: fix
      //OscMessage msg = new OscMessage("/hd/spawn");
      //msg.add(life / 50.f);
      //oscP5.send(msg, supercollider);
      break;  
    }
  }
  //#ifdef HD_DEBUG
  //Serial.println("No free light lists");
  //#endif
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
        if (((j+1) < lightLists[i]->numLights) && (lightLists[i]->lights[(j+1)] != NULL)) {
          lightLists[i]->lights[(j+1)]->linkedPrev = NULL;
        }
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
      lightLists[i]->lights[j]->update();
    }
    if (allExpired) {
      delete lightLists[i];
      lightLists[i] = NULL;
      #ifdef HD_DEBUG
      Serial.print("Deleted lightList");
      Serial.println(i);
      #endif
    }
  }
}

#ifdef HD_TEST
void Emitter::debug() {
  for (int i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    String lights = "";
    for (int j=0; j<lightLists[i]->numLights; j++) {
      if (lightLists[i]->lights[j] == NULL || lightLists[i]->lights[j]->isExpired) {
        continue;
      }
      else {
        lights += j;
        lights += "(";
        lights += lightLists[i]->lights[j]->pixel1;
        lights += ")";
        lights += ", ";
      }
    }
    Serial.print("LightList");
    Serial.print(i);
    Serial.print("(");
    Serial.print(lightLists[i]->numLights);
    Serial.print(")");
    Serial.print(" active lights: ");
    Serial.println(lights);
  }
}
#endif