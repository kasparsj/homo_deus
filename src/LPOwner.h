#pragma once

#include "Config.h"
#include "LPLight.h"
#include "LightList.h"

class LPOwner
{
  public:
    uint8_t group;

    LPOwner(uint8_t group) : group(group) {}

    virtual void emit(LPLight* light) = 0;
    void add(LPLight *light) {
        light->owner = this;
        light->owner->update(light);
    }
    virtual void update(LPLight *light) = 0;
};

