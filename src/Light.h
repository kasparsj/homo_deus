#pragma once

#include "LPLight.h"
#include "Globals.h"

class Light : public LPLight {

  public:

    Light(LightList *parent, float speed, uint32_t lifeMillis, uint16_t idx = 0, uint8_t maxBri = 255);
    
    Light(uint8_t maxBri, float speed, uint32_t lifeMillis) : Light(0, maxBri, speed, lifeMillis, 0) {
    }
    
    Light(uint8_t maxBri) : Light(maxBri, DEFAULT_SPEED, INFINITE_DURATION) {
    }
    
    Light() : Light(255) {
    }

    float getSpeed() {
        return speed;
    }
    void setSpeed(float speed) {
        this->speed = speed;
    }
    uint32_t getLife() {
        return lifeMillis;
    }
    void setDuration(uint32_t durMillis) {
        lifeMillis = gMillis + durMillis;
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
    // int16_t pixel2 = -1; // 4 bytes
  
};
