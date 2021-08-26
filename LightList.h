#pragma once

#include "Config.h"
#include "Behaviour.h"
#include <NeoPixelBus.h>
//#include "FastNoise.h"

class Model;
class Light;

class LightList {

  public:

    //static FastNoise fastNoise;
  
    uint16_t noteId;
    float speed = DEFAULT_SPEED;
    float fadeSpeed = 0;
    float fadeThresh = 0;
    int16_t life = INFINITE_LIFE;
    ListOrder order = LIST_SEQUENTIAL;
    bool linked = true;
    Model *model = 0;
    Behaviour *behaviour = 0;
    uint16_t numLights = 0;
    uint16_t trail = 0;
    Light **lights;
    uint16_t numEmitted = 0;
    uint8_t numSplits = 0;
    
    LightList() {
    }

    ~LightList() {
      delete lights;
      if (behaviour != NULL) {
        delete behaviour;
      }
    }

    void init(uint16_t numLights);
    void setup(uint16_t numLights, RgbColor color, float brightness = 1, float fadeSpeed = 0, float fadeThresh = 0);

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
    void setFade(float fadeSpeed, float fadeThresh = 0) {
      this->fadeSpeed = fadeSpeed;
      this->fadeThresh = fadeThresh;
    }
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
    float getPosition(Light *light);

  private:

    void initPosition(uint16_t i, Light* light);
    void initBri(uint16_t i, Light* light);
    void initLife(uint16_t i, Light* light);
  
};
