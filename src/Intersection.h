#pragma once

#include "Config.h"
#include "LPBase.h"
#include "Port.h"
#include "Behaviour.h"

class Intersection : public LPBase<uint8_t> {

  public:

    static uint8_t nextId;
  
    uint8_t id;
    uint8_t numPorts;
    Port **ports; // 2 or 4 ports
    uint16_t topPixel;
    int16_t bottomPixel;
    int8_t outgoingLights[EMITTER_MAX_LIGHTS] = {-1};
    uint8_t freeOutgoing = 0;

    Intersection(uint8_t numPorts, uint16_t topPixel, int16_t bottomPixel, uint8_t group);
    Intersection(uint16_t topPixel, int16_t bottomPixel, uint8_t group) : Intersection(4, topPixel, bottomPixel, group) {
    }
    Intersection(uint16_t topPixel, uint8_t group) : Intersection(2, topPixel, -1, group) {

    }
  
    ~Intersection() {
      delete[] ports;
    }
    
    void addPort(Port *p);
    void emitLight(Light* light);
    void queueOutgoing(uint8_t i);
    Port* sendOut(uint8_t i);
    void update();
    void updateLight(uint8_t i);
    float sumW(Model *model, Port *incoming);
    Port *randomPort(Port *incoming, Behaviour *behaviour);
    Port *choosePort(Model *model, Light* light);
  
};
