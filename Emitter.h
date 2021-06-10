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
    int16_t randomLife();
    uint8_t randomModel();
    uint16_t randomLength();
    float randomBriThresh();
    uint16_t randomNextEmit();
    RgbColor randomColor();
    RgbColor paletteColor(uint8_t color);
    void emit(unsigned long millis);
    int8_t emitLinked(uint8_t model, float speed, uint16_t length, RgbColor color);
    int8_t emitLinked(uint8_t model, float speed, uint16_t length, uint8_t color) {
      return emitLinked(model, speed, length, paletteColor(color));
    }
    int8_t emitLinked(uint8_t model, float speed, uint16_t length) {
      return emitLinked(model, speed, length, randomColor());
    }
    int8_t emitLinked(uint8_t model, float speed) {
      return emitLinked(model, speed, randomLength());
    }
    int8_t emitLinked(uint8_t model) {
      return emitLinked(model, randomSpeed(), randomLength());
    }
    int8_t emitLinked() {
      return emitLinked(randomModel(), randomSpeed(), randomLength());
    }
    int8_t emitSplatter(float speed, uint16_t length, RgbColor color);
    int8_t emitSplatter(float speed, uint16_t length, uint8_t color) {
      return emitSplatter(speed, length, paletteColor(color));
    }
    int8_t emitSplatter(float speed, uint16_t length) {
      return emitSplatter(speed, length, randomColor());
    }
    int8_t emitSplatter() {
      return emitSplatter(randomSpeed(), randomLength());
    }
    void update();
    void colorAll();
    void splitAll();
    void stopAll();
    void stopNote(uint8_t i);
    #ifdef HD_TEST
    uint16_t numLights();
    void debug();
    #endif

};
