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
    
    uint8_t randomModel();
    uint16_t randomLength();
    uint8_t randomBrightness();
    uint16_t randomNextEmit();
    ColorRGB paletteColor(uint8_t color);
    void autoEmit(unsigned long millis);
    int8_t emit(EmitParams &params);
    LPOwner* getEmitter(Model* model, Behaviour* behaviour, EmitParams& params);
    void update();
    void colorAll();
    void splitAll();
    void stopAll();
    void stopNote(uint8_t i);
    ColorRGB getPixel(uint16_t i, uint8_t maxBrightness = FULL_BRIGHTNESS);
    void debug();
    
  private:
    void doEmit(LPOwner* from, LightList *lightList, EmitParams& params);
    void setPixel(uint16_t pixel, ColorRGB &color);

};
