#pragma once

#include "Port.h"
#include <NeoPixelBus.h>

class Model;

class Light {

  public:
    static const float SPEED;
    static const int LIFE;
    
    int id;
    float brightness;
    float speed;
    float position;
    int age = 0;
    int life;
    RgbColor color;
    Model *model = 0;
    Light *linkedPrev = 0;
    Port *inPort = 0;
    Port *outPort = 0;
    Port *outPorts[28] = {0};
    int pixel1 = -1;
    int pixel2 = -1;
    float pixel1Bri = 0;
    float pixel2Bri = 0;
    bool isExpired = false;
    
    Light(float brightness, float speed, int life, Model *model, Light *linkedPrev);
    
    Light(float brightness, float speed, int life) : Light(brightness, speed, life, 0, 0) {
    }
    
    Light(float brightness) : Light(brightness, SPEED, LIFE) {
    }
    
    Light() : Light(1.0) {
    }

    RgbColor getColor(float brightness = 1.f) {
      // todo: multiply by brightness
      return color;
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

    void setOutPort(Port *port, int intersectionId = -1);

    void resetPixels();
  
};
