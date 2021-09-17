#pragma once

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
    P_DURATION_MS = 12,
    P_DURATION_FRAMES = 13,
    P_COLOR = 14,
    P_COLOR_INDEX = 15,
    P_NOTE_ID = 16,
    P_MIN_BRI = 17,
    P_MAX_BRI = 18,
    P_BEHAVIOUR = 19,
    P_EMIT_GROUPS = 20,
    P_EMIT_OFFSET = 21,
    P_COLOR_CHANGE_GROUPS = 22,
};

class EmitParams {

  public:

    static int8_t DEFAULT_MODEL;
    static uint32_t DEFAULT_DURATION;
    static uint8_t DEFAULT_BRIGHTNESS;
    static int64_t DEFAULT_COLOR;
    static float DURATION_FPS;

    int8_t model = DEFAULT_MODEL;
    float speed = DEFAULT_SPEED;
    uint8_t ease = 0;
    uint8_t fadeSpeed = 0;
    uint8_t fadeThresh = 0;
    uint8_t fadeEase = 0;
    uint16_t length = 0;
    uint16_t trail = 0;
    ListOrder order = LIST_ORDER_SEQUENTIAL;
    ListHead head = LIST_HEAD_FRONT;
    bool linked = true;
    int8_t from = -1;
    uint32_t duration = DEFAULT_DURATION;
    ColorRGB color = DEFAULT_COLOR;
    uint16_t noteId = 0;
    uint8_t minBri = 0;
    uint8_t maxBri = DEFAULT_BRIGHTNESS;
    uint16_t behaviourFlags = 0;
    uint8_t emitGroups = 0;
    uint8_t emitOffset = 0;
    uint8_t colorChangeGroups = 0;

    static uint16_t frameMs() {
        return (1000.f / EmitParams::DURATION_FPS);
    }

    EmitParams(int8_t model, float speed) : model(model), speed(speed) {
        if (DEFAULT_COLOR == RANDOM_COLOR) {
            color.setRandom();
        }
        else {
            color.set(DEFAULT_COLOR);
        }
    }
    EmitParams(int8_t model) : EmitParams(model, DEFAULT_SPEED) {

    }
    EmitParams() : EmitParams(DEFAULT_MODEL, DEFAULT_SPEED) {

    }

    uint16_t getSpeedTrail(float speed, uint16_t length) {
      uint16_t trail = 0;
      if (order == LIST_ORDER_SEQUENTIAL && linked && !(behaviourFlags & B_RENDER_SEGMENT)) {
        trail = min((int) (speed * max(1, length / 2)), EMITTER_MAX_LENGTH - 1);
      }
      return trail;
    }
};
