#pragma once

#include "Config.h"
#include <NeoPixelBus.h>

class Light;

class Behaviour {

  public:
    PosBehaviour posChangeBe = B_CHANGE_POS_SPEED;
    uint8_t colorChangeGroups = 0;

    Behaviour() {}

    float getPosition(Light *light);

    RgbColor getColor(Light *light, uint8_t group);
};