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
    Light *lights[MAX_LIGHTS] = {0};
    int numLeds;
    int pixelDir;
    int fromPixel;
    int toPixel;
    
    void setup(Intersection *from, Intersection *to);
    void addLight(Light *light);
    void update();
    void outgoing(Light *light);
};
