#pragma once

#include "FastNoise.h"

class Model;
class Light;

class LightList {

  public:

    static FastNoise fastNoise;
  
    float speed;
    int life;
    bool linked;
    Model *model;
    int numLights;
    Light **lights;
    
    LightList(float speed) {
      this->numLights = numLights;
      this->speed = speed;
    }
    
    LightList() : LightList(1.0) {
    }

    ~LightList() {
      delete[] lights;
    }

    void setup(int numLights);
    LightList *setupRandom(int numLights, bool linked = true);
    LightList *setupNoise(int numLights, float threshold);
    LightList *setupNoise(int numLights) {
      return setupNoise(numLights, 0.0);
    }

    Light*& operator[](int i) {
      return lights[i];
    }
    
    LightList *setLinked(bool linked);
    LightList *setModel(Model *model);
    LightList *setSpeed(float speed);
    LightList *setLife(int numFrames);
  
};
