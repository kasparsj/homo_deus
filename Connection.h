#pragma once

#include "Config.h"
#include <Arduino.h>
#include "Port.h"
#include "Light.h"

class Intersection;

class Connection {

  public:
    Intersection *from;
    Intersection *to;
    Port *fromPort;
    Port *toPort;
    Light *lights[MAX_LIGHTS];
    int numLeds;
    float *brightness;
    
    void setup(Intersection *from, Intersection *to, int numLeds);
    void addLight(Light *light);
    void update();
};
