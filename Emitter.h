#pragma once

#include "Config.h"
#include "Intersection.h"

class Emitter {

  public:
  
    Model *models;
    Intersection *intersections;
    LightList *lightLists[MAX_LIGHT_LISTS] = {0};
    long int nextEmit = 0;
    int pixelValuesR[PIXEL_COUNT];
    int pixelValuesG[PIXEL_COUNT];
    int pixelValuesB[PIXEL_COUNT];
    int pixelDiv[PIXEL_COUNT];
    bool enabled = false;
    
    Emitter(Model (&models)[NUM_MODELS], Intersection (&intersections)[14]) {
      this->models = models;
      this->intersections = intersections;
    }
    
    float randomSpeed();
    int randomLife();
    int randomModel();
    int randomLength();
    float randomBriThresh();
    int randomNextEmit();
    void emit(unsigned long millis);
    void emitNew(int which, float speed, int life, int length);
    void emitNew(int which, float speed) {
      emitNew(which, speed, randomLife(), randomLength());
    }
    void emitNew(int which) {
      emitNew(which, randomSpeed(), randomLife(), randomLength());
    }
    void emitNew() {
      emitNew(randomModel(), randomSpeed(), randomLife(), randomLength());
    }
    void update();
    #ifdef HD_TEST
    void debug();
    #endif

};
