#pragma once

#include "Config.h"
#include "Port.h"

class LightList;
class Model;
class Behaviour;

class Light {

  public:
    #ifdef LP_DEBUG
    uint16_t id;
    #endif
    float maxBri = 1.f;
    float bri = 1.f;
    float brightness = 0;
    float speed = DEFAULT_SPEED;
    float position;
    uint16_t age = 0;
    int16_t life = INFINITE_LIFE;
    ColorRGB color; // 3 bytes
    LightList *parent = 0;
    Light *linkedPrev = 0;
    Port *inPort = 0;
    Port *outPort = 0;
    Port *outPorts[OUT_PORTS_MEMORY] = {0}; // 4 bytes * 7
    int8_t outPortsInt[OUT_PORTS_MEMORY] = {-1};
    int16_t pixel1 = -1;
    // int16_t pixel2 = -1; // 4 bytes
    bool isExpired = false;
    
    Light(float maxBri, float speed, int16_t life, LightList *parent, Light *linkedPrev);
    
    Light(float maxBri, float speed, int16_t life) : Light(maxBri, speed, life, 0, 0) {
    }
    
    Light(float maxBri) : Light(maxBri, DEFAULT_SPEED, INFINITE_LIFE) {
    }
    
    Light() : Light(1.0) {
    }

    float getBrightness();

    ColorRGB getColor() {
      if (brightness == 1.f) {
        return color;
      }
      return color.Dim(255 * brightness);
    }

    void setColor(ColorRGB color) {
      this->color = color;
    }

    void setParent(LightList *parent) {
      this->parent = parent;
    }
    
    void update();
    
    bool shouldExpire();
    
    void setInPort(Port *port) {
      inPort = port;
    }

    Port* getOutPort(uint8_t intersectionId);
    void setOutPort(Port *port, int8_t intersectionId = -1);

    void resetPixels();

    Model* getModel();
    Behaviour* getBehaviour();

    uint16_t* getPixels();

  private:
    static uint16_t pixels[CONNECTION_MAX_LEDS];    
  
};
