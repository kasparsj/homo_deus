#pragma once

#include <Arduino.h>
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
    
    Light(float brightness, float speed, int life, Model *model, Light *linkedPrev) {
      this->brightness = brightness;
      this->speed = speed;
      this->life = life;
      this->model = model;
      this->linkedPrev = linkedPrev;
      position = -1;
    }
    
    Light(float brightness, float speed, int life) : Light(brightness, speed, life, NULL, NULL) {
    }
    
    Light(float brightness) : Light(brightness, SPEED, LIFE) {
    }
    
    Light() : Light(1.0) {
    }
    
    void setModel(Model *model) {
      this->model = model;
    }
    
    void update() {
      position += speed;
      age++;
    }
    
    bool expired() {
      return age >= life;
    }
    
    Light *setInPort(Port *port) {
      inPort = port;
      return this;
    }
    
    Light *setOutPort(Port *port) {
      outPort = port;
      return this;
    }
  
};
