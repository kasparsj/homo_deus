#pragma once

#include "Config.h"
#include <Arduino.h>

enum EmitParam {
  P_MODEL,
  P_SPEED,
  P_FADE,
  P_LENGTH,
  P_ORDER,
  P_LINKED,
  P_FROM,
  P_LIFE,
  P_COLOR,
  P_NOTE_ID,
  P_BRIGHTNESS,
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
    PosBehaviour posChangeBe = B_CHANGE_POS_SPEED;
    RenderBehaviour renderBe = B_RENDER_LIGHT;
    uint8_t colorChangeGroups = 0;

    uint16_t getTrail(float speed, uint16_t length) {
      uint16_t trail = 0;
      if (order == LIST_SEQUENTIAL && linked && renderBe != B_RENDER_SEGMENT) {
        trail = min((int) (speed * max(1, length / 2)), max(EMITTER_MAX_LENGTH, EMITTER_MAX_LIGHTS) - 1);
      }
      return trail;
    }
};