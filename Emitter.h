#pragma once

#include "Config.h"
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
    LightList *lightLists[MAX_LIGHT_LISTS] = {0};
    long int nextEmit = 0;
    float pixelValues[PIXEL_COUNT];
    
    Emitter(Model (&models)[NUM_MODELS], Intersection (&intersections)[14]) {
      this->models = models;
      this->intersections = intersections;
    }
    
    float randomSpeed();
    int randomLife();
    int randomModel();
    int randomLength();
    float randomBriThresh();
    void emit();

    void emitNew(int which, float speed, int life);
    
    void emitNew(int which) {
      emitNew(which, randomSpeed(), randomLife());
    }
    
    void emitNew() {
      emitNew(randomModel(), randomSpeed(), randomLife());
    }

    void update();

};
