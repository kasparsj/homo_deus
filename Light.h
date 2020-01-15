#pragma once

#include "Port.h"

class Model;

class Light {

  public:
    static const float SPEED;
    static const int LIFE;
    
    float brightness;
    float speed;
    float position;
    int age;
    int life;
    Model *model;
    Light *linkedPrev;
    Port *inPort;
    Port *outPort;
    int pixel1 = -1;
    int pixel2 = -1;
    float pixel1Bri = 0;
    float pixel2Bri = 0;
    bool isExpired = false;
    
    Light(float brightness, float speed, int life, Model *model, Light *linkedPrev) {
      this->brightness = brightness;
      this->speed = speed;
      this->life = life;
      this->model = model;
      this->linkedPrev = linkedPrev;
      position = -1;
    }
    
    Light(float brightness, float speed, int life) : Light(brightness, speed, life, 0, 0) {
    }
    
    Light(float brightness) : Light(brightness, SPEED, LIFE) {
    }
    
    Light() : Light(1.0) {
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
    
    void setOutPort(Port *port) {
      outPort = port;
    }

    void resetPixels();
  
};
