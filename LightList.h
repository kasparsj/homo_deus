#pragma once

#include "Config.h"
#include <NeoPixelBus.h>
//#include "FastNoise.h"

class Model;
class Light;

enum ListOrder { 
  LIST_SEQUENTIAL, 
  LIST_RANDOM, 
};

class LightList {

  public:

    //static FastNoise fastNoise;
  
    uint16_t noteId;
    float speed = DEFAULT_SPEED;
    int16_t life = INFINITE_LIFE;
    ListOrder order = LIST_SEQUENTIAL;
    bool linked = true;
    Model *model;
    uint16_t numLights = 0;
    uint16_t trail = 0;
    Light **lights;
    uint16_t numEmitted = 0;
    uint8_t numSplits = 0;
    
    LightList() {
    }

    ~LightList() {
      delete lights;
    }

    void init(uint16_t numLights);
    void setup(uint16_t numLights, RgbColor color, float brightness = 1.f, float fade = 0.f);

    Light* operator [] (uint16_t i) const {
      return lights[i];
    }

    Light*& operator [] (uint16_t i) {
      return lights[i];
    }
    
    void setOrder(ListOrder order) {
      this->order = order;
    }
    void setSpeed(float speed);
    void setModel(Model *model);
    void setLife(int16_t numFrames);
    void setColor(RgbColor color);
    void setLinked(bool linked);
    void setTrail(uint16_t length) {
      trail = length;
    }
    void setNoteId(uint16_t noteId) {
      this->noteId = noteId;
    }
    void initEmit();
    void split();

  private:

    void initPosition(uint16_t i, Light* light);
    void initLife(uint16_t i, Light* light);
  
};
