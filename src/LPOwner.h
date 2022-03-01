#pragma once

#include "Config.h"
#include "LPLight.h"
#include "LightList.h"

class LPOwner
{
  public:
    uint8_t group;

    LPOwner(uint8_t group) : group(group) {}

    virtual void emit(LPLight* const light) const = 0;
    inline void add(LPLight* const light) const {
        light->owner = this;
        light->owner->update(light);
    }
    virtual void update(LPLight* const light) const = 0;
};

