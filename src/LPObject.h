#pragma once

#include "Intersection.h"
#include "Connection.h"
#include "Model.h"

class LPObject {

  public:
    static LPObject* instance;
    uint16_t pixelCount;

    LPObject(uint16_t pixelCount) : pixelCount(pixelCount) {
        instance = this;
    }
    virtual Model* getModel(int i) = 0;
    virtual Intersection* getIntersection(uint8_t i, uint8_t groups) = 0;
    virtual Intersection* getFreeIntersection(uint8_t groups) = 0;

};
