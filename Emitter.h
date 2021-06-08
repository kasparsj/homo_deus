#pragma once

#include "Config.h"
#include "Intersection.h"

class Emitter {

  public:
  
    Model *models;
    Intersection *intersections;
    LightList *lightLists[MAX_LIGHT_LISTS] = {0};
    uint16_t totalLights = 0;
    unsigned long nextEmit = 0;
    uint16_t pixelValuesR[PIXEL_COUNT];
    uint16_t pixelValuesG[PIXEL_COUNT];
    uint16_t pixelValuesB[PIXEL_COUNT];
    uint8_t pixelDiv[PIXEL_COUNT];
    bool enabled = false;
    
    Emitter(Model (&models)[NUM_MODELS], Intersection (&intersections)[14]) {
      this->models = models;
      this->intersections = intersections;
    }
    
    float randomSpeed();
    uint16_t randomLife();
    uint8_t randomModel();
    uint16_t randomLength();
    float randomBriThresh();
    uint16_t randomNextEmit();
    void emit(unsigned long millis);
    void emitNew(uint8_t which, float speed, uint16_t life, uint16_t length, RgbColor color);
    void emitNew(uint8_t which, float speed, uint16_t life, uint16_t length) {
      emitNew(which, speed, life, length, RgbColor(random(255), random(255), random(255)));
    }
    void emitNew(uint8_t which, float speed, uint16_t life) {
      emitNew(which, speed, life, randomLength());
    }
    void emitNew(uint8_t which, float speed) {
      emitNew(which, speed, randomLife(), randomLength());
    }
    void emitNew(uint8_t which) {
      emitNew(which, randomSpeed(), randomLife(), randomLength());
    }
    void emitNew() {
      emitNew(randomModel(), randomSpeed(), randomLife(), randomLength());
    }
    void update();
    #ifdef HD_TEST
    uint16_t numLights();
    void debug();
    #endif

};
