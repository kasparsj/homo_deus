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
    int numLeds;
    int maxLights = 0;
    int freeLight = 0;
    int pixelDir;
    int fromPixel;
    int toPixel;
    
    void setup(Intersection *from, Intersection *to);
    void addLight(Light *light);
    void removeLight(int i);
    void update();
    void outgoing(Light *light);
};
