#pragma once

//#include "FastNoise.h"
#include <NeoPixelBus.h>

class Model;
class Light;

class LightList {

  public:

    //static FastNoise fastNoise;
  
    float speed;
    uint16_t life;
    bool linked;
    Model *model;
    uint16_t numLights = 0;
    uint16_t trail = 0;
    Light **lights;
    
    LightList(float speed) {
      this->speed = speed;
    }
    
    LightList() : LightList(1.0) {
    }

    ~LightList() {
      delete lights;
    }

    void setup(uint16_t numLights);
    void setupRandom(uint16_t numLights, bool linked = true);
    // void setupNoise(uint16_t numLights, float threshold);
    // void setupNoise(uint16_t numLights) {
    //   return setupNoise(numLights, 0.0);
    // }
    void setupFull(uint16_t numLights, RgbColor color);

    Light* get(uint16_t i) {
      return lights[i];
    }

    void set(uint16_t i, Light* light) {
      lights[i] = light;
    }
    
    void setLinked(bool linked);
    void setModel(Model *model);
    void setSpeed(float speed);
    void setLife(uint16_t numFrames);

    void setTrail(uint16_t length) {
      trail = length;
    }
  
};
