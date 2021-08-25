#pragma once

#include "Config.h"
#include <NeoPixelBus.h>

class Light;

class Behaviour {

  public:
    PosBehaviour bePos = B_POS_DEFAULT;
    uint8_t colorChangeGroups = 0;

    Behaviour() {}

    float getPosition(Light *light);

    RgbColor changeColor(Light *light);
};