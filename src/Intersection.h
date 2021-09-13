#pragma once

#include "Config.h"
#include "LPOwner.h"
#include "Port.h"
#include "Behaviour.h"

class Intersection : public LPOwner {

  public:

    static uint8_t nextId;
  
    uint8_t id;
    uint8_t numPorts;
    Port **ports; // 2 or 4 ports
    uint16_t topPixel;
    int16_t bottomPixel;

    Intersection(uint8_t numPorts, uint16_t topPixel, int16_t bottomPixel, uint8_t group);
    Intersection(uint16_t topPixel, int16_t bottomPixel, uint8_t group) : Intersection(4, topPixel, bottomPixel, group) {
    }
    Intersection(uint16_t topPixel, uint8_t group) : Intersection(2, topPixel, -1, group) {

    }
  
    ~Intersection() {
      delete[] ports;
    }
    
    void addPort(Port *p);
    void emit(LPLight* light);
    void update(LPLight *light);

  private:

    Port* sendOut(LPLight *light);
    float sumW(Model *model, Port *incoming);
    Port *randomPort(Port *incoming, Behaviour *behaviour);
    Port *choosePort(Model *model, LPLight* light);
  
};
