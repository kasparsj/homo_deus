#pragma once

#include "Config.h"
#include "Light.h"
#include "Port.h"
#include "LightList.h"

class Intersection {

  public:
  
    int numPorts;
    Port **ports; // 2 or 4 ports
    int topPixel;
    int bottomPixel;
    Light **lights;
    int maxLights;
    int freeLight = 0;
    Light *outgoingLights[MAX_OUTGOING_LIGHTS] = {0};
    int freeOutgoing = 0;
    int removeLights[MAX_OUTGOING_LIGHTS] = {-1};
    int freeRemove = 0;
    
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
    float sumW(Model *model, Port *incoming);
    Port *randomPort(Port *incoming);
    Port *choosePort(Model *model, Port *incoming);
  
};
