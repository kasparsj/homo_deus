#pragma once

#include "LPLight.h"

class Light : public LPLight {

  public:

    Light(LightList *parent, float speed, int16_t life, uint16_t idx = 0, uint8_t maxBri = 255);
    
    Light(uint8_t maxBri, float speed, int16_t life) : Light(0, maxBri, speed, life, 0) {
    }
    
    Light(uint8_t maxBri) : Light(maxBri, DEFAULT_SPEED, INFINITE_LIFE) {
    }
    
    Light() : Light(255) {
    }

    float getSpeed() {
        return speed;
    }
    void setSpeed(float speed) {
        this->speed = speed;
    }
    ColorRGB getColor() {
        return color;
    }
    void setColor(ColorRGB color) {
      this->color = color;
    }

    uint8_t getBrightness();
    ColorRGB getPixelColor();
    void nextFrame();
    bool shouldExpire();
    
    Model* getModel();
    Behaviour* getBehaviour();

  private:

    float speed = DEFAULT_SPEED;
    ColorRGB color; // 3 bytes
    uint16_t age = 0;
    // int16_t pixel2 = -1; // 4 bytes
  
};
