#pragma once

#include "Config.h"
#include "Light.h"
#include "Port.h"
#include "LightList.h"
#include "Behaviour.h"

class Intersection {

  public:

    static uint8_t nextId;
  
    uint8_t id;
    uint8_t numPorts;
    Port **ports; // 2 or 4 ports
    uint16_t topPixel;
    int16_t bottomPixel;
    Light *lights[EMITTER_MAX_LIGHTS] = {0};
    uint8_t freeLight = 0;
    uint8_t numLights = 0;
    LightList *lightLists[EMITTER_MAX_LIGHT_LISTS] = {0};
    int8_t outgoingLights[EMITTER_MAX_LIGHTS] = {-1};
    uint8_t freeOutgoing = 0;

    Intersection(uint8_t numPorts, uint16_t topPixel, int16_t bottomPixel = -1);
  
    ~Intersection() {
      delete[] ports;
      delete[] lights;
    }
    
    void addPort(Port *p);
    void add(LightList *lightList);
    void emit(uint8_t i);
    void addLight(Light *light);
    void queueOutgoing(uint8_t i);
    Port* sendOut(uint8_t i);
    void update();
    void updateLight(uint8_t i);
    void removeLight(uint8_t i);
    float sumW(Model *model, Port *incoming);
    Port *randomPort(Port *incoming, Behaviour *behaviour);
    Port *choosePort(Model *model, Light* light);
  
};
