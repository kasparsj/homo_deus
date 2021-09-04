#pragma once

#include "Config.h"
#include "Port.h"
#include "Light.h"

class Intersection;

class Connection {

  public:
    Intersection *from;
    Intersection *to;
    uint8_t group;
    Port *fromPort;
    Port *toPort;
    Light **lights;
    uint8_t numLeds = 0;
    uint16_t maxLights = 0;
    uint16_t freeLight = 0;
    bool pixelDir;
    uint16_t fromPixel;
    uint16_t toPixel;
    
    void setup(Intersection *from, Intersection *to, uint8_t group);
    void addLight(Light *light);
    void removeLight(uint16_t i);
    void update();
    void updateLight(uint16_t i);
    void outgoing(Light *light, int16_t i = -1);
    uint16_t getPixel(uint16_t i) {
      return fromPixel + (i * (pixelDir ? 1 : -1));
    }
    uint16_t getFromPixel();
    uint16_t getToPixel();
};
