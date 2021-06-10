#pragma once

#include "Config.h"
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
    int16_t life;
    RgbColor color; // 3 bytes
    Model *model = 0;
    Light *linkedPrev = 0;
    Port *inPort = 0;
    Port *outPort = 0;
    Port *outPorts[OUT_PORTS_MEMORY] = {0}; // 4 bytes * 7
    int8_t outPortsInt[OUT_PORTS_MEMORY] = {-1};
    int16_t pixel1 = -1;
    float pixel1Bri = 0;
    // int16_t pixel2 = -1; // 4 bytes
    // float pixel2Bri = 0; // 4 bytes
    bool isExpired = false;
    
    Light(float brightness, float speed, int16_t life, Model *model, Light *linkedPrev);
    
    Light(float brightness, float speed, int16_t life) : Light(brightness, speed, life, 0, 0) {
    }
    
    Light(float brightness) : Light(brightness, SPEED, LIFE) {
    }
    
    Light() : Light(1.0) {
    }

    RgbColor getColor(float brightness = 1.f) {
      if (brightness == 1.f) {
        return color;
      }
      return color.Dim(255 * brightness);
    }

    void setColor(RgbColor color) {
      this->color = color;
    }
    
    void setModel(Model *model) {
      this->model = model;
    }
    
    void update();
    
    bool shouldExpire() {
      if (life < 0) {
        return false;
      }
      return age >= life;
    }
    
    void setInPort(Port *port) {
      inPort = port;
    }

    Port* getOutPort(uint8_t intersectionId);
    void setOutPort(Port *port, int8_t intersectionId = -1);

    void resetPixels();
  
};
