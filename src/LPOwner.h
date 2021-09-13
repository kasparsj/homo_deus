#pragma once

#include "Config.h"
#include "LPLight.h"
#include "LightList.h"

class LPOwner
{
  public:
    uint8_t group;

    LPOwner(uint8_t group) : group(group) {}

    virtual void emitLight(LPLight* light) = 0;
    void addLight(LPLight *light) {
        light->owner = this;
        light->update();
    }
    virtual void updateLight(LPLight *light) = 0;
};

