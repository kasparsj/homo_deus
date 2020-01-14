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
    Light *lights[MAX_LIGHTS];
    float brightness;
    
    Intersection(int numPorts, int topPixel, int bottomPixel = -1) {
      this->numPorts = numPorts;
      this->ports = new Port*[numPorts]();
      this->topPixel = topPixel;
      this->bottomPixel = bottomPixel;
    }
  
    ~Intersection() {
      delete[] ports;
      delete[] lights;
    }
    
    void addPort(Port *p);
    void emit(LightList *lightList);
    void addLight(Light *light);
    void outgoing(Light *light);
    void update();
    float sumW(Model *model, Port *incoming);
    Port *randomPort(Port *incoming);
    Port *choosePort(Model *model, Port *incoming);
  
};
