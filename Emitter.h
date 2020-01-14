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
    Model *lastModel;
    Model *models;
    Intersection *intersections;
    long int nextEmit = 0;
    
    Emitter(Model (&models)[NUM_MODELS], Intersection (&intersections)[7]) {
      this->models = models;
      this->intersections = intersections;
    }
    
    void emit(int which, float speed, int life) {
      lastModel = &models[which];
      LightList *lightList = new LightList();
      lightList->setSpeed(speed);
      lightList->setModel(lastModel);
      lightList->setLinked(true);
      lightList->setLife(life);
      lightList->setupNoise(randomLength(), randomBriThresh());
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
      intersections[(int) random(7)].emit(lightList);
  
      // todo: fix
  //    OscMessage msg = new OscMessage("/hd/spawn");
  //    msg.add(life / 50.f);
  //    oscP5.send(msg, supercollider);
    }
    
    void emit(int which) {
      emit(which, randomSpeed(), randomLife());
    }
    
    void emit() {
      emit(randomModel(), randomSpeed(), randomLife());
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

    void update() {
      int ms = millis();
      if (nextEmit <= ms) {
        emit();
        nextEmit = (int) (ms + (30 + random(250) * 1000/60));
      }
    }

};
