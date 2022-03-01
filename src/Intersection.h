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
    void emit(LPLight* const light);
    void update(LPLight* const light) const;

  private:

//    Port* sendOut(LPLight* const light);
    void sendOut(LPLight* const light) const;
    uint16_t sumW(const Model *model, const Port *incoming) const;
    Port* randomPort(const Port *incoming, const Behaviour *behaviour) const;
    Port* choosePort(const Model *model, const LPLight* light) const;
    void handleColorChange(LPLight* const light, const Port *port) const;
  
};
