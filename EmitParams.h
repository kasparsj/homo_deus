#pragma once

#include "Config.h"
#include <Arduino.h>

enum EmitParam {
  P_MODEL = 0,
  P_SPEED = 1,
  P_FADE = 2,
  P_LENGTH = 3,
  P_ORDER = 4,
  P_LINKED = 5,
  P_FROM = 6,
  P_LIFE = 7,
  P_COLOR = 8,
  P_NOTE_ID = 9,
  P_BRIGHTNESS = 10,
  P_BEHAVIOUR = 11,
  P_EMIT_GROUPS = 12,
  P_COLOR_CHANGE_GROUPS = 13,
};

class EmitParams {

  public:

    int8_t model = DEFAULT_MODEL;
    float speed = DEFAULT_SPEED;
    float fadeSpeed = 0;
    float fadeThresh = 0;
    uint16_t length = 0;
    ListOrder order = LIST_SEQUENTIAL;
    bool linked = true;
    int8_t from = -1;
    int16_t life = DEFAULT_LIFE;
    int8_t color = -1;
    uint16_t noteId = 0;
    float brightness = DEFAULT_BRIGHTNESS;
    uint8_t behaviourFlags = 0;
    uint8_t emitGroups = 0;
    uint8_t colorChangeGroups = 0;

    uint16_t getTrail(float speed, uint16_t length) {
      uint16_t trail = 0;
      if (order == LIST_SEQUENTIAL && linked && !(behaviourFlags & B_RENDER_SEGMENT)) {
        trail = min((int) (speed * max(1, length / 2)), max(EMITTER_MAX_LENGTH, EMITTER_MAX_LIGHTS) - 1);
      }
      return trail;
    }
};