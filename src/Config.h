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
  ColorRGB(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0) : R(r), G(g), B(b) {}
  ColorRGB Dim(uint8_t ratio) const {
    // specifically avoids float math
    return ColorRGB(_elementDim(R, ratio), _elementDim(G, ratio), _elementDim(B, ratio));
  }
  inline static uint8_t _elementDim(uint8_t value, uint8_t ratio) {
    return (static_cast<uint16_t>(value) * (static_cast<uint16_t>(ratio) + 1)) >> 8;
  }
};

enum HeptagonStarModel { 
    M_DEFAULT,
    M_STAR,
    M_OUTER_STAR,
    M_INNER_CIRCLE,
    M_HORNS,
    M_HALO,
    M_SPLATTER,
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
    B_EMIT_FROM_RANDOM = 64,
};

enum ListOrder { 
    LIST_ORDER_SEQUENTIAL,
    LIST_ORDER_RANDOM,
    LIST_ORDER_NOISE,
};

enum ListHead {
    LIST_HEAD_FRONT,
    LIST_HEAD_MIDDLE,
    LIST_HEAD_BACK,
};

#define LP_DEBUG
#define LP_TEST
#define EMITTER_MAX_LIGHT_LISTS 3 // max 127 (int8_t)
#define EMITTER_MAX_LIGHTS 110 // max 127 (int8_t)
#define EMITTER_MIN_SPEED 0.5f
#define EMITTER_MAX_SPEED 10.f
#define EMITTER_MIN_LENGTH 1
#define EMITTER_MAX_LENGTH 100
#define EMITTER_MIN_LIFE 120 // frames
#define EMITTER_MAX_LIFE 1440 // frames
#define EMITTER_MIN_BRI 0.75f
#define EMITTER_MAX_BRI 1.0f
#define EMITTER_MIN_NEXT 2000 // ms, ~125 frames (avg fps is 62.5)
#define EMITTER_MAX_NEXT 20000 // ms, ~1250 frames (avg fps is 62.5)
#define MAX_GROUPS 5
#define MAX_PORTS 90 // 85
#define MAX_LIGHT_LISTS 20
#define MAX_BRIGHTNESS 255
#define MAX_CONDITIONAL_WEIGHTS 10
#define MAX_TOTAL_LIGHTS 1100
#define CONNECTION_MAX_MULT 10
#define CONNECTION_MAX_LIGHTS 340
#define CONNECTION_MAX_LEDS 48
#define OUT_PORTS_MEMORY 3
#define UPDATES_PER_FRAME 3
//#define LP_OSC_REPLY(I) OscWiFi.publish(SC_HOST, SC_PORT, "/emit", (I));
#define INFINITE_LIFE 0
#define DEFAULT_MODEL -1
#define DEFAULT_SPEED 1.0
#define DEFAULT_LIFE -1 // -1 = random, 0 = infinite
#define DEFAULT_BRIGHTNESS 1.0
#define AVG_FPS_FRAMES 120
