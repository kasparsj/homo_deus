#pragma once

#include <stdint.h>
#include <algorithm>
#include "Config.h"

using std::min;
using std::max;

enum EmitParam {
    P_MODEL = 0,
    P_SPEED = 1,
    P_EASE = 2,
    P_FADE = 3,
    P_FADE_THRESH = 4,
    P_FADE_EASE = 5,
    P_LENGTH = 6,
    P_TRAIL = 7,
    P_ORDER = 8,
    P_HEAD = 9,
    P_LINKED = 10,
    P_FROM = 11,
    P_LIFE = 12,
    P_COLOR = 13,
    P_NOTE_ID = 14,
    P_MIN_BRI = 15,
    P_MAX_BRI = 16,
    P_BEHAVIOUR = 17,
    P_EMIT_GROUPS = 18,
    P_EMIT_OFFSET = 19,
    P_COLOR_CHANGE_GROUPS = 20,
};

class EmitParams {

  public:

    static int8_t DEFAULT_LIFE;
    static int8_t DEFAULT_MODEL;
    static uint8_t DEFAULT_BRIGHTNESS;

    int8_t model = DEFAULT_MODEL;
    float speed = DEFAULT_SPEED;
    uint8_t ease = 0;
    float fadeSpeed = 0;
    float fadeThresh = 0;
    uint8_t fadeEase = 0;
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
    uint8_t minBri = 0;
    uint8_t maxBri = DEFAULT_BRIGHTNESS;
    uint8_t behaviourFlags = 0;
    uint8_t emitGroups = 0;
    uint8_t emitOffset = 0;
    uint8_t colorChangeGroups = 0;

    uint16_t getSpeedTrail(float speed, uint16_t length) {
      uint16_t trail = 0;
      if (order == LIST_ORDER_SEQUENTIAL && linked && !(behaviourFlags & B_RENDER_SEGMENT)) {
        trail = min((int) (speed * max(1, length / 2)), EMITTER_MAX_LENGTH - 1);
      }
      return trail;
    }
};
