#pragma once

#include "Port.h"
#include <NeoPixelBus.h>

class Model;

class Light {

  public:
    static const float SPEED;
    static const uint16_t LIFE;
    
    //uint16_t id;
    float brightness;
    float speed;
    float position;
    uint16_t age = 0;
    uint16_t life;
    RgbColor color; // 3 bytes
    Model *model = 0;
    Light *linkedPrev = 0;
    Port *inPort = 0;
    Port *outPort = 0;
    Port *outPorts[28] = {0}; // 4 bytes * 7
    // todo: implement
    //int8_t outPortsInt[7] = {-1};
    int16_t pixel1 = -1;
    // int16_t pixel2 = -1; // 4 bytes
    float pixel1Bri = 0;
    // float pixel2Bri = 0; // 4 bytes
    bool isExpired = false;
    
    Light(float brightness, float speed, uint16_t life, Model *model, Light *linkedPrev);
    
    Light(float brightness, float speed, uint16_t life) : Light(brightness, speed, life, 0, 0) {
    }
    
    Light(float brightness) : Light(brightness, SPEED, LIFE) {
    }
    
    Light() : Light(1.0) {
    }

    RgbColor getColor(float brightness = 1.f) {
      if (brightness == 1.f) {
        return color;
      }
      return color.Dim(255 - 255 * brightness);
    }

    void setColor(RgbColor color) {
      this->color = color;
    }
    
    void setModel(Model *model) {
      this->model = model;
    }
    
    void update();
    
    bool shouldExpire() {
      return age >= life;
    }
    
    void setInPort(Port *port) {
      inPort = port;
    }

    void setOutPort(Port *port, int8_t intersectionId = -1);

    void resetPixels();
  
};
