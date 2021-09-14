#pragma once

#include "Config.h"
#include "EmitParams.h"
#include "LPObject.h"

class State {

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
    
    State(LPObject &obj) : object(obj) {
      pixelValuesR = new uint16_t[obj.pixelCount]{0};
      pixelValuesG = new uint16_t[obj.pixelCount]{0};
      pixelValuesB = new uint16_t[obj.pixelCount]{0};
      pixelDiv = new uint8_t[obj.pixelCount]{0};
    }
    ~State() {
      delete[] pixelValuesR;
      delete[] pixelValuesG;
      delete[] pixelValuesB;
      delete[] pixelDiv;
    }
    
    float randomSpeed();
    uint8_t randomModel();
    uint16_t randomLength();
    uint8_t randomBrightness();
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
      params.fadeSpeed = 1;
      params.fadeThresh = 127;
      params.order = LIST_ORDER_RANDOM;
      params.behaviourFlags |= B_POS_CHANGE_FADE;
      return emit(params);
    }
    int8_t emitSplatter(uint8_t model) {
      EmitParams params;
      params.model = model;
      params.speed = randomSpeed();
      params.linked = false;
      params.duration = max(1, (int) (1.f/params.speed) + 1) * (1000 / EmitParams::DURATION_FPS);
      return emit(params);
    }
    LPOwner* getEmitter(Model* model, Behaviour* behaviour, EmitParams& params);
    void update();
    void colorAll();
    void splitAll();
    void stopAll();
    void stopNote(uint8_t i);
    ColorRGB getPixel(uint16_t i, uint8_t maxBrightness = FULL_BRI);

    #ifdef LP_DEBUG
    void debug();
    #endif
    
  private:
    void doEmit(LPOwner* from, LightList *lightList, EmitParams& params);
    void setPixel(uint16_t pixel, ColorRGB &color);

};
