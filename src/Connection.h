#pragma once

#include "Config.h"
#include "LPEmitter.h"
#include "LPBase.h"
#include "Port.h"
#include "LPLight.h"

class Intersection;

class Connection : public LPEmitter, public LPBase<uint16_t> {

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
    void update();
    void emitLightList(LightList* lightList) {
        LPBase::emitLightList(lightList);
    }
    void emitLight(LPLight* light);
    void updateLight(uint16_t i);
    void outgoing(LPLight *light, int16_t i = -1);
    uint16_t getPixel(uint16_t i) {
      return fromPixel + (i * (pixelDir ? 1 : -1));
    }
    uint16_t getFromPixel();
    uint16_t getToPixel();
};
