#pragma once

#include "Config.h"
#include "Model.h"

class Emitter {

  public:
  
    Model *models;
    LightList *lightLists[MAX_LIGHT_LISTS] = {0};
    uint16_t totalLights = 0;
    unsigned long nextEmit = 0;
    uint16_t pixelValuesR[PIXEL_COUNT];
    uint16_t pixelValuesG[PIXEL_COUNT];
    uint16_t pixelValuesB[PIXEL_COUNT];
    uint8_t pixelDiv[PIXEL_COUNT];
    bool enabled = false;
    uint8_t currentPalette = 0;
    
    Emitter(Model (&models)[NUM_MODELS]) {
      this->models = models;
    }
    
    float randomSpeed();
    uint16_t randomLife();
    uint8_t randomModel();
    uint16_t randomLength();
    float randomBriThresh();
    uint16_t randomNextEmit();
    RgbColor randomColor();
    RgbColor paletteColor(uint8_t color);
    void emit(unsigned long millis);
    void emitLinked(uint8_t model, float speed, uint16_t life, uint16_t length, RgbColor color);
    void emitLinked(uint8_t model, float speed, uint16_t life, uint16_t length, uint8_t color) {
      emitLinked(model, speed, life, length, paletteColor(color));
    }
    void emitLinked(uint8_t model, float speed, uint16_t life, uint16_t length) {
      emitLinked(model, speed, life, length, randomColor());
    }
    void emitLinked(uint8_t model, float speed, uint16_t life) {
      emitLinked(model, speed, life, randomLength());
    }
    void emitLinked(uint8_t model, float speed) {
      emitLinked(model, speed, randomLife(), randomLength());
    }
    void emitLinked(uint8_t model) {
      emitLinked(model, randomSpeed(), randomLife(), randomLength());
    }
    void emitLinked() {
      emitLinked(randomModel(), randomSpeed(), randomLife(), randomLength());
    }
    void emitSplatter(float speed, uint16_t length, RgbColor color);
    void emitSplatter(float speed, uint16_t length, uint8_t color) {
      emitSplatter(speed, length, paletteColor(color));
    }
    void emitSplatter(float speed, uint16_t length) {
      emitSplatter(speed, length, randomColor());
    }
    void emitSplatter() {
      emitSplatter(randomSpeed(), randomLength());
    }
    void update();
    void colorAll();
    void splitAll();
    void stopAll();
    #ifdef HD_TEST
    uint16_t numLights();
    void debug();
    #endif

};
