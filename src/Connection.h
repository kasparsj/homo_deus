#pragma once

#include "Config.h"
#include "LPOwner.h"
#include "Port.h"
#include "LPLight.h"

class Intersection;

class Connection : public LPOwner {

  public:
    Intersection *from;
    Intersection *to;
    Port *fromPort;
    Port *toPort;
    uint8_t numLeds = 0;
    bool pixelDir;
    uint16_t fromPixel;
    uint16_t toPixel;
    
    Connection(Intersection *from, Intersection *to, uint8_t group);
    void addLight(LPLight *light);
    void emitLight(LPLight* light);
    void updateLight(LPLight *light);
    void outgoing(LPLight *light);
    uint16_t getPixel(uint16_t i) {
      return fromPixel + (i * (pixelDir ? 1 : -1));
    }
    uint16_t getFromPixel();
    uint16_t getToPixel();
};
