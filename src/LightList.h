#pragma once

#include "Config.h"
#include "ofxEasing.h"
#include "Behaviour.h"

class Model;
class Light;
class LPLight;

class LightList {

  public:

    static uint16_t nextId;
  
    uint16_t id;
    uint16_t noteId;
    float speed = DEFAULT_SPEED;
    ofxeasing::function ease = ofxeasing::linear::easeNone;
    float fadeSpeed = 0;
    float fadeThresh = 0;
    float minBri = 0;
    ofxeasing::function fadeEase = ofxeasing::linear::easeNone;
    int16_t life = INFINITE_LIFE;
    ListOrder order = LIST_ORDER_SEQUENTIAL;
    ListHead head = LIST_HEAD_FRONT;
    bool linked = true;
    Model *model = 0;
    Behaviour *behaviour = 0;
    uint16_t numLights = 0;
    uint16_t lead = 0;
    uint16_t trail = 0;
    ColorRGB color;
    LPLight **lights;
    uint16_t age = 0;
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
    void setup(uint16_t numLights, float brightness = 1);
    float getBriMult(uint16_t i) {
        float mult = 1.f;
        if (i < lead) {
            mult = (255.f / (lead + 1)) * (i + 1) / 255.f;
        }
        else if (i >= lead + body()) {
            uint16_t j = i - (lead + body());
            mult = (255.f - (255.f / (trail + 1)) * (j + 1)) / 255.f;
        }
        return mult;
    }

    LPLight* operator [] (uint16_t i) const {
      return lights[i];
    }

    LPLight*& operator [] (uint16_t i) {
      return lights[i];
    }
    
    void setSpeed(float speed, uint8_t ease = 0) {
        this->speed = speed;
        this->ease = ease == EASE_NONE ?
            ofxeasing::linear::easeNone :
            ofxeasing::easing(static_cast<ofxeasing::Function>((ease - 1) / 3), static_cast<ofxeasing::Type>((ease - 1) % 3));
    }
    void setFade(float fadeSpeed, float fadeThresh = 0, uint8_t fadeEase = 0) {
        this->fadeSpeed = fadeSpeed;
        this->fadeThresh = fadeThresh;
        this->fadeEase = fadeEase == EASE_NONE ?
            ofxeasing::linear::easeNone :
            ofxeasing::easing(static_cast<ofxeasing::Function>((fadeEase - 1) / 3), static_cast<ofxeasing::Type>((fadeEase - 1) % 3));
    }
    void setLeadTrail(uint16_t trail);
    void setLife(int16_t numFrames);
    void setColor(ColorRGB color);
    void initEmit(uint8_t posOffset = 0);
    void update() {
        age++;
    }
    void split();
    float getPosition(LPLight *light);
    float getBri(LPLight *light);

  private:

    LPLight* createLight(uint16_t i, float brightness);
    void initPosition(uint16_t i, LPLight* light);
    void initBri(uint16_t i, LPLight* light);
    void initLife(uint16_t i, LPLight* light);
    inline uint16_t body() {
        return numLights - lead - trail;
    }

};
