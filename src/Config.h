#pragma once

#include <stdint.h>

#if defined (ARDUINO)

#include "Arduino.h"
#define LP_LOG(...) Serial.print(__VA_ARGS__)
#define LP_LOGF(...) Serial.printf(__VA_ARGS__)
#define LP_LOGLN(...) Serial.println(__VA_ARGS__)
#define LP_RANDOM(...) random(__VA_ARGS__)
#define LP_STRING String

#else
//#elif defined (OPENFRAMEWORKS)

#include "ofMain.h"
#define LP_LOG(...) ofLog(OF_LOG_WARNING, __VA_ARGS__)
#define LP_LOGF(...) ofLog(OF_LOG_WARNING, __VA_ARGS__)
#define LP_LOGLN ofLogWarning
#define LP_RANDOM ofRandom
#define LP_STRING std::string

#endif

struct ColorRGB {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    ColorRGB(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) {}
    ColorRGB(uint32_t rgb) {
        set(rgb);
    }
    ColorRGB() : ColorRGB(0) {}
    ColorRGB Dim(uint8_t ratio) const {
        // specifically avoids float math
        return ColorRGB(_elementDim(R, ratio), _elementDim(G, ratio), _elementDim(B, ratio));
    }
    void set(uint32_t rgb) {
        R = (rgb >> 16) & 0xFF;
        G = (rgb >> 8) & 0xFF;
        B = rgb & 0xFF;
    }
    void setRandom() {
        R = LP_RANDOM(255);
        G = LP_RANDOM(255);
        B = LP_RANDOM(255);
    }
    inline static uint8_t _elementDim(uint8_t value, uint8_t ratio) {
        return (static_cast<uint16_t>(value) * (static_cast<uint16_t>(ratio) + 1)) >> 8;
    }
};

enum Groups {
    GROUP1 = 1,
    GROUP2 = 2,
    GROUP3 = 4,
    GROUP4 = 8,
    GROUP5 = 16,
    GROUP6 = 32,
    GROUP7 = 64,
    GROUP8 = 128,
};

enum BehaviourFlags {
    B_POS_CHANGE_FADE = 1,
    B_BRI_CONST_NOISE = 2,
    B_RENDER_SEGMENT = 4,
    B_ALLOW_BOUNCE = 8,
    B_FORCE_BOUNCE = 16,
    B_EXPIRE_IMMEDIATE = 32,
    B_EMIT_FROM_CONN = 64,
    B_FILL_EASE = 128,
    B_RANDOM_COLOR = 256,
    B_MIRROR_FLIP = 512,
    B_MIRROR_ROTATE = 1024,
    B_SMOOTH_CHANGES = 2048,
};

enum ListOrder { 
    LIST_ORDER_SEQUENTIAL,
    LIST_ORDER_RANDOM,
    LIST_ORDER_NOISE,
    LIST_ORDER_OFFSET,
    LO_FIRST = LIST_ORDER_SEQUENTIAL,
    LO_LAST = LIST_ORDER_OFFSET,
};

enum ListHead {
    LIST_HEAD_FRONT,
    LIST_HEAD_MIDDLE,
    LIST_HEAD_BACK,
};

enum Ease {
    EASE_NONE = 0,
    EASE_LINEAR_IN = 1,
    EASE_LINEAR_OUT = 2,
    EASE_LINEAR_INOUT = 3,
    EASE_SINE_IN = 4,
    EASE_SINE_OUT = 5,
    EASE_SINE_INOUT = 6,
    EASE_CIRCULAR_IN = 7,
    EASE_CIRCULAR_OUT = 8,
    EASE_CIRCULAR_INOUT = 9,
    EASE_QUADRATIC_IN = 10,
    EASE_QUADRATIC_OUT = 11,
    EASE_QUADRATIC_INOUT = 12,
    EASE_CUBIC_IN = 13,
    EASE_CUBIC_OUT = 14,
    EASE_CUBIC_INOUT = 15,
    EASE_QUARTIC_IN = 16,
    EASE_QUARTIC_OUT = 17,
    EASE_QUARTIC_INOUT = 18,
    EASE_QUINTIC_IN = 19,
    EASE_QUINTIC_OUT = 20,
    EASE_QUINTIC_INOUT = 21,
    EASE_EXPONENTIAL_IN = 22,
    EASE_EXPONENTIAL_OUT = 23,
    EASE_EXPONENTIAL_INOUT = 24,
    EASE_BACK_IN = 25,
    EASE_BACK_OUT = 26,
    EASE_BACK_INOUT = 27,
    EASE_BOUNCE_IN = 28,
    EASE_BOUNCE_OUT = 29,
    EASE_BOUNCE_INOUT = 30,
    EASE_ELASTIC_IN = 31,
    EASE_ELASTIC_OUT = 32,
    EASE_ELASTIC_INOUT = 33,
};

#ifndef UINT32_MAX
#define UINT32_MAX 4294967295
#endif

#define EMITTER_MIN_LENGTH 1
#define EMITTER_MAX_LENGTH 100
#define EMITTER_MIN_BRI 0.75f
#define EMITTER_MAX_BRI 1.0f
#define EMITTER_MIN_NEXT 2000 // ms, ~125 frames (avg fps is 62.5)
#define EMITTER_MAX_NEXT 20000 // ms, ~1250 frames (avg fps is 62.5)
#define MAX_GROUPS 5
#define MAX_LIGHT_LISTS 20
#define MAX_CONDITIONAL_WEIGHTS 10
#define MAX_TOTAL_LIGHTS 1500
#define CONNECTION_MAX_MULT 10
#define CONNECTION_MAX_LIGHTS 340
#define CONNECTION_MAX_LEDS 48
#define OUT_PORTS_MEMORY 3
//#define LP_OSC_REPLY(I) OscWiFi.publish(SC_HOST, SC_PORT, "/emit", (I));
#define RANDOM_MODEL -1
#define RANDOM_DURATION 0
#define RANDOM_COLOR -1
#define INFINITE_DURATION UINT32_MAX
#define DEFAULT_SPEED 1.0
#define FULL_BRIGHTNESS 255
