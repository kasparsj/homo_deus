#pragma once

#include "Config.h"
#include "Behaviour.h"

class Model;
class Light;

class LightList {

  public:

    //static FastNoise fastNoise;
    static uint16_t nextId;
  
    uint16_t id;
    uint16_t noteId;
    float speed = DEFAULT_SPEED;
    float fadeSpeed = 0;
    float fadeThresh = 0;
    int16_t life = INFINITE_LIFE;
    ListOrder order = LIST_ORDER_SEQUENTIAL;
    ListHead head = LIST_HEAD_FRONT;
    bool linked = true;
    Model *model = 0;
    Behaviour *behaviour = 0;
    uint16_t numLights = 0;
    uint16_t lead = 0;
    uint16_t trail = 0;
    Light **lights;
    uint16_t numEmitted = 0;
    uint8_t numSplits = 0;
    
    LightList() {
      this->id = nextId++;
    }

    ~LightList() {
      delete lights;
      if (behaviour != NULL) {
        delete behaviour;
      }
    }

    void init(uint16_t numLights);
    void setup(uint16_t numLights, ColorRGB color, float brightness = 1);

    Light* operator [] (uint16_t i) const {
      return lights[i];
    }

    Light*& operator [] (uint16_t i) {
      return lights[i];
    }
    
    void setSpeed(float speed);
    void setFade(float fadeSpeed, float fadeThresh = 0) {
      this->fadeSpeed = fadeSpeed;
      this->fadeThresh = fadeThresh;
    }
    void setLeadTrail(uint16_t trail);
    void setLife(int16_t numFrames);
    void setColor(ColorRGB color);
    void setLinked(bool linked);
    void initEmit(uint8_t posOffset = 0);
    void split();
    float getPosition(Light *light);
    float getBri(Light *light);

  private:

    void initPosition(uint16_t i, Light* light);
    void initBri(uint16_t i, Light* light);
    void initLife(uint16_t i, Light* light);
    uint16_t body() {
        return numLights - lead - trail;
    }
  
};
