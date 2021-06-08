#pragma once

#include "Config.h"
#include "Port.h"
#include "Light.h"

class Intersection;

class Connection {

  public:
    Intersection *from;
    Intersection *to;
    Port *fromPort;
    Port *toPort;
    Light **lights;
    uint8_t numLeds;
    uint16_t maxLights = 0;
    uint16_t freeLight = 0;
    bool pixelDir;
    uint16_t fromPixel;
    uint16_t toPixel;
    uint16_t removeLights[MAX_OUTGOING_LIGHTS] = {-1};
    uint16_t freeRemove = 0;
    
    void setup(Intersection *from, Intersection *to);
    void addLight(Light *light);
    void queueRemove(uint16_t i);
    void removeLight(uint16_t i);
    void update();
    void postUpdate();
    void updateLight(uint16_t i);
    void outgoing(Light *light);
};
