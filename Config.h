#pragma once

enum HeptagonStarModel { 
  M_DEFAULT, 
  M_STAR, 
  M_OUTER_STAR, 
  M_INNER_CIRCLE,
  M_HORNS,
  M_HALO,
  M_SPLATTER,
};

enum GroupFlags {
  GROUP1 = 1,
  GROUP2 = 2,
  GROUP3 = 4,
  GROUP4 = 8,
  GROUP5 = 16,
};

enum ListOrder { 
  LIST_SEQUENTIAL, 
  LIST_RANDOM, 
  LIST_NOISE,
};

enum PosBehaviour {
  B_POS_CHANGE_SPEED,
  B_POS_CHANGE_FADE,
};

enum BriBehaviour {
  B_BRI_CHANGE_FADE,
  B_BRI_CONST_NOISE,
};

enum RenderBehaviour {
  B_RENDER_LIGHT,
  B_RENDER_SEGMENT,
};

enum RandomPortBehaviour {
  B_RND_PORT_THROUGH,
  B_RND_PORT_BOUNCE,
};

#define HD_DEBUG
#define HD_TEST
#define HD_OPTIMIZED
#define PIXEL_COUNT1 524
#define PIXEL_COUNT2 395
#define PIXEL_COUNT (PIXEL_COUNT1 + PIXEL_COUNT2)
#define PIXEL_PIN1 14
#define PIXEL_PIN2 26
#define BUTTON_PIN 25
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
#define MAX_PORTS 90 // 85
#define NUM_MODELS 7
#define MAX_LIGHT_LISTS 20
#define MAX_BRIGHTNESS 255
#define MAX_CONDITIONAL_WEIGHTS 10
#define MAX_TOTAL_LIGHTS 1100
#define CONNECTION_MAX_MULT 10
#define CONNECTION_MAX_LIGHTS 340
#define OUT_PORTS_MEMORY 3
#define UPDATES_PER_FRAME 3
#define HD_WIFI
#define HD_OSC
//#define HD_OSC_REPLY // todo: not working ATM
#define OSC_PORT 54321
#define SC_HOST "192.168.43.101"
#define SC_PORT 57120
#define INFINITE_LIFE 0
#define DEFAULT_MODEL M_DEFAULT
#define DEFAULT_SPEED 1.0
#define DEFAULT_LIFE -1 // -1 = random, 0 = infinite
#define DEFAULT_BRIGHTNESS 1.0
#define AVG_FPS_FRAMES 120