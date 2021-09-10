#pragma once

#include "LPObject.h"

#define AVG_FPS_FRAMES 120

class LPDebugger
{

  public:

    LPDebugger(LPObject &object);
    ~LPDebugger();
    
    void update(unsigned long millis);
    float getFPS();
    bool isModelWeight(uint8_t id, uint16_t i);
    bool isIntersection(uint16_t i);
    bool isConnection(uint16_t i);
    void dumpConnections();
    void dumpIntersections();

  private:
    LPObject &object;
    bool **weightPixels;
    bool *interPixels;
    bool *connPixels;
    float fps[AVG_FPS_FRAMES] = {0.f};
    uint8_t fpsIndex = 0;
    unsigned long prevMillis = 0;

};
