#pragma once

#include "Config.h"
#include "EmitParams.h"

class LPLight;

class Behaviour {

  public:
    uint16_t flags = 0;
    uint8_t colorChangeGroups = 0;

    Behaviour(EmitParams &params) {
      flags = params.behaviourFlags;
      colorChangeGroups = params.colorChangeGroups;
    }

    uint16_t getBri(const LPLight *light) const;
    float getPosition(LPLight *light) const;
    ColorRGB getColor(const LPLight *light, uint8_t group) const;

    bool renderSegment() const {
        return flags & B_RENDER_SEGMENT;
    }
    bool allowBounce() const {
        return flags & B_ALLOW_BOUNCE;
    }
    bool forceBounce() const {
        return flags & B_FORCE_BOUNCE;
    }
    bool expireImmediately() const {
        return flags & B_EXPIRE_IMMEDIATE;
    }
    bool emitFromConnection() const {
        return flags & B_EMIT_FROM_CONN;
    }
    bool fillEase() const {
        return flags & B_FILL_EASE;
    }
    bool randomColor() const {
        return flags & B_RANDOM_COLOR;
    }
};
