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
    int numLights = 0;
    Light **lights;
    
    LightList(float speed) {
      this->speed = speed;
    }
    
    LightList() : LightList(1.0) {
    }

    ~LightList() {
      delete[] lights;
    }

    void setup(int numLights);
    void setupRandom(int numLights, bool linked = true);
    void setupNoise(int numLights, float threshold);
    void setupNoise(int numLights) {
      return setupNoise(numLights, 0.0);
    }

    Light*& operator[](int i) {
      return lights[i];
    }
    
    void setLinked(bool linked);
    void setModel(Model *model);
    void setSpeed(float speed);
    void setLife(int numFrames);
  
};
