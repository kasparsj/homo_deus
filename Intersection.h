#pragma once

#include "Config.h"
#include "Light.h"
#include "Port.h"
#include "LightList.h"

class Intersection {

  public:

    static uint8_t nextId;
  
    uint8_t id;
    uint8_t numPorts;
    Port **ports; // 2 or 4 ports
    uint16_t topPixel;
    uint16_t bottomPixel;
    Light **lights;
    uint16_t maxLights;
    uint16_t freeLight = 0;
    Light *outgoingLights[MAX_OUTGOING_LIGHTS] = {0};
    uint16_t freeOutgoing = 0;
    uint16_t removeLights[MAX_OUTGOING_LIGHTS] = {-1};
    uint16_t freeRemove = 0;
    
    Intersection(int maxLights, int numPorts, int topPixel, int bottomPixel = -1);
  
    ~Intersection() {
      delete[] ports;
      delete[] lights;
    }
    
    void addPort(Port *p);
    void emit(LightList *lightList);
    void addLight(Light *light);
    void queueOutgoing(Light *light);
    void queueRemove(int i);
    void removeLight(int i);
    Port* sendOut(int i);
    void update();
    void postUpdate();
    void updateLight(int i);
    float sumW(Model *model, Port *incoming);
    Port *randomPort(Port *incoming);
    Port *choosePort(Model *model, Port *incoming);
  
};
