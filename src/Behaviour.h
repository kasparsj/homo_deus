#pragma once

#include "Config.h"
#include "EmitParams.h"

class Light;

class Behaviour {

  public:
    uint8_t flags = 0;
    uint8_t emitGroups = 0;
    uint8_t colorChangeGroups = 0;

    Behaviour(EmitParams &params) {
      flags = params.behaviourFlags;
      emitGroups = params.emitGroups;
      colorChangeGroups = params.colorChangeGroups;
    }

    float getBri(Light *light);
    float getPosition(Light *light);
    ColorRGB getColor(Light *light, uint8_t group);

    bool isRenderSegment() {
      return flags & B_RENDER_SEGMENT;
    }
};