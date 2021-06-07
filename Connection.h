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
    int removeLights[MAX_OUTGOING_LIGHTS] = {-1};
    int freeRemove = 0;
    
    void setup(Intersection *from, Intersection *to);
    void addLight(Light *light);
    void queueRemove(int i);
    void removeLight(int i);
    void update();
    void postUpdate();
    void updateLight(int i);
    void outgoing(Light *light);
};
