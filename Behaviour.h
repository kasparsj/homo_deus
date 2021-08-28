#pragma once

#include "Config.h"
#include "EmitParams.h"
#include <NeoPixelBus.h>

class Light;

class Behaviour {

  public:
    uint8_t flags = 0;
    uint8_t colorChangeGroupFlags = 0;

    Behaviour(EmitParams &params) {
      flags = params.behaviourFlags;
      colorChangeGroupFlags = params.colorChangeGroupFlags;
    }

    float getBri(Light *light);
    float getPosition(Light *light);

    RgbColor getColor(Light *light, uint8_t group);
};