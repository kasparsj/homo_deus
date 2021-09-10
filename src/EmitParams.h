#pragma once

#include <stdint.h>
#include <algorithm>
#include "Config.h"

using std::min;
using std::max;

enum EmitParam {
    P_MODEL = 0,
    P_SPEED = 1,
    P_FADE = 2,
    P_LENGTH = 3,
    P_TRAIL = 4,
    P_ORDER = 5,
    P_HEAD = 6,
    P_LINKED = 7,
    P_FROM = 8,
    P_LIFE = 9,
    P_COLOR = 10,
    P_NOTE_ID = 11,
    P_BRIGHTNESS = 12,
    P_BEHAVIOUR = 13,
    P_EMIT_GROUPS = 14,
    P_EMIT_OFFSET = 15,
    P_COLOR_CHANGE_GROUPS = 16,
};

class EmitParams {

  public:

    int8_t model = DEFAULT_MODEL;
    float speed = DEFAULT_SPEED;
    float fadeSpeed = 0;
    float fadeThresh = 0;
    uint16_t length = 0;
    uint16_t trail = 0;
    float lengthChange = 0;
    ListOrder order = LIST_ORDER_SEQUENTIAL;
    ListHead head = LIST_HEAD_FRONT;
    bool linked = true;
    int8_t from = -1;
    int16_t life = DEFAULT_LIFE;
    int8_t color = -1;
    uint16_t noteId = 0;
    float brightness = DEFAULT_BRIGHTNESS;
    uint8_t behaviourFlags = 0;
    uint8_t emitGroups = 0;
    uint8_t emitOffset = 0;
    uint8_t colorChangeGroups = 0;

    uint16_t getSpeedTrail(float speed, uint16_t length) {
      uint16_t trail = 0;
      if (order == LIST_ORDER_SEQUENTIAL && linked && !(behaviourFlags & B_RENDER_SEGMENT)) {
        trail = min((int) (speed * max(1, length / 2)), max(EMITTER_MAX_LENGTH, EMITTER_MAX_LIGHTS) - 1);
      }
      return trail;
    }
};
