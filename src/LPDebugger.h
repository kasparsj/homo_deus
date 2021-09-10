#pragma once

#include "LPObject.h"

class LPDebugger
{

  public:

    LPDebugger(LPObject &object);
    ~LPDebugger();

    LPObject &object;
    bool **weightPixels;
    bool isModelWeight(uint8_t id, uint16_t i);
    bool *interPixels;
    bool isIntersection(uint16_t i);
    bool *connPixels;
    bool isConnection(uint16_t i);
    void dumpConnections();
    void dumpIntersections();

};
