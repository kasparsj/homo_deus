#pragma once

#include "Intersection.h"

class Emitter {

  public:
  
    float minSpeed = 0.5;
    float maxSpeed = 2.5;
    int minLife = 30;
    int maxLife = 330;
    int minLength = 10;
    int maxLength = 30;
    float minBriThresh = 0.5;
    float maxBriThresh = 0.75;
    Model *models;
    Intersection *intersections;
    LightList *lightLists[MAX_LIGHT_LISTS];
    long int nextEmit = 0;
    float pixelValues[PIXEL_COUNT];
    
    Emitter(Model (&models)[NUM_MODELS], Intersection (&intersections)[14]) {
      this->models = models;
      this->intersections = intersections;
    }
    
    float randomSpeed() {
      return minSpeed + random(max(maxSpeed - minSpeed, 0.f));
    }
    
    int randomLife() {
      return minLife + (int) random(max(maxLife - minLife, 0));
    }
    
    int randomModel() {
      return floor(random(NUM_MODELS));
    }
    
    int randomLength() {
      return (int) (minLength + random(max(maxLength - minLength, 0)));
    }
    
    float randomBriThresh() {
      return minBriThresh + random(max(maxBriThresh - minBriThresh, 0.f));
    }

    void emit() {
      int ms = millis();
      if (nextEmit <= ms) {
        emitNew();
        nextEmit = (int) (ms + (30 + random(250) * 1000/60));
      }
    }

    void emitNew(int which, float speed, int life) {
      for (int i=0; i<MAX_LIGHT_LISTS; i++) {
        if (lightLists[i] == NULL) {
          Model *model = &models[which];
          lightLists[i] = new LightList();
          lightLists[i]->setSpeed(speed);
          lightLists[i]->setModel(model);
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
        }
      }
    }
    
    void emitNew(int which) {
      emitNew(which, randomSpeed(), randomLife());
    }
    
    void emitNew() {
      emitNew(randomModel(), randomSpeed(), randomLife());
    }

    void update() {
      memset(pixelValues, 0, sizeof(pixelValues));
      for (int i=0; i<MAX_LIGHT_LISTS; i++) {
        if (lightLists[i] == NULL) continue;
        bool allExpired = true;
        for (int j=0; j<lightLists[i]->numLights; i++) {
          if (lightLists[i]->lights[j]->expired()) continue;
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

};
