#pragma once

#include "Config.h"
#include "EmitParams.h"
#include "LPObject.h"

class Emitter {

  public:
  
    LPObject &object;
    LightList *lightLists[MAX_LIGHT_LISTS] = {0};
    uint16_t totalLights = 0;
    uint8_t totalLightLists = 0;
    unsigned long nextEmit = 0;
    uint16_t *pixelValuesR;
    uint16_t *pixelValuesG;
    uint16_t *pixelValuesB;
    uint8_t *pixelDiv;
    bool autoEnabled = false;
    uint8_t currentPalette = 0;
    
    Emitter(LPObject &obj) : object(obj) {
      pixelValuesR = new uint16_t[obj.pixelCount]{0};
      pixelValuesG = new uint16_t[obj.pixelCount]{0};
      pixelValuesB = new uint16_t[obj.pixelCount]{0};
      pixelDiv = new uint8_t[obj.pixelCount]{0};
    }
    ~Emitter() {
      delete[] pixelValuesR;
      delete[] pixelValuesG;
      delete[] pixelValuesB;
      delete[] pixelDiv;
    }
    
    float randomSpeed();
    uint16_t randomLife();
    uint8_t randomModel();
    uint16_t randomLength();
    float randomBrightness();
    uint16_t randomNextEmit();
    ColorRGB randomColor();
    ColorRGB paletteColor(uint8_t color);
    void autoEmit(unsigned long millis);
    int8_t emit(EmitParams &params);
    int8_t emit(uint8_t model) {
      EmitParams params;
      params.model = model;
      params.speed = randomSpeed();
      return emit(params);
    }
    int8_t emit() {
      return emit(randomModel());
    }
    int8_t emitRandom() {
      EmitParams params;
      params.speed = 0;
      params.fadeSpeed = 0.01f;
      params.fadeThresh = 0.5f;
      params.order = LIST_ORDER_RANDOM;
      params.behaviourFlags |= B_POS_CHANGE_FADE;
      return emit(params);
    }
    int8_t emitSplatter() {
      EmitParams params;
      params.model = M_SPLATTER;
      params.speed = randomSpeed();
      params.linked = false;
      params.life = max(1, (int) (1.f/params.speed) + 1);
      return emit(params);
    }
    void update();
    void colorAll();
    void splitAll();
    void stopAll();
    void stopNote(uint8_t i);
    #ifdef LP_TEST
    uint16_t numLights();
    void debug();
    #endif

    ColorRGB getPixel(uint16_t i);
    
  private:
    void doEmit(Intersection* from, LightList *lightList);
    void setPixel(uint16_t pixel, ColorRGB &color);

};
