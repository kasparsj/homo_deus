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
    
    Intersection(int maxLights, int numPorts, int topPixel, int bottomPixel = -1);
  
    ~Intersection() {
      delete[] ports;
      delete[] lights;
    }
    
    void addPort(Port *p);
    void emit(LightList *lightList);
    void addLight(Light *light);
    void removeLight(int i);
    void outgoing(Light *light);
    void update();
    float sumW(Model *model, Port *incoming);
    Port *randomPort(Port *incoming);
    Port *choosePort(Model *model, Port *incoming);
  
};
