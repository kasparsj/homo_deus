#pragma once

#include "Intersection.h"
#include "Connection.h"
#include "Model.h"

class Object {

  public:
    virtual Model* getModel(int i);
    virtual Intersection* getIntersection(uint8_t i, uint8_t groups);
    virtual Intersection* getFreeIntersection(uint8_t groups);

};