#pragma once

#include "Config.h"
#include "EmitParams.h"

class LPLight;

class Behaviour {

  public:
    uint8_t flags = 0;
    uint8_t colorChangeGroups = 0;

    Behaviour(EmitParams &params) {
      flags = params.behaviourFlags;
      colorChangeGroups = params.colorChangeGroups;
    }

    float getBri(LPLight *light);
    float getPosition(LPLight *light);
    ColorRGB getColor(LPLight *light, uint8_t group);

    bool renderSegment() {
        return flags & B_RENDER_SEGMENT;
    }
    bool allowBounce() {
        return flags & B_ALLOW_BOUNCE;
    }
    bool forceBounce() {
        return flags & B_FORCE_BOUNCE;
    }
    bool expireImmediately() {
        return flags && B_EXPIRE_IMMEDIATE;
    }
    bool emitFromConnection() {
        return flags && B_EMIT_FROM_CONN;
    }
};
