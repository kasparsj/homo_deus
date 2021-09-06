#pragma once

#include "Config.h"
#include "LightList.h"

class LPEmitter {

  public:
    LightList *lightLists[EMITTER_MAX_LIGHT_LISTS] = {0};

    void add(LightList *lightList) {
        for (uint8_t j=0; j<EMITTER_MAX_LIGHT_LISTS; j++) {
          if (lightLists[j] == 0) {
            lightLists[j] = lightList;
            return;
          }
        }
        LP_LOGF("LPBase addLightList no free slot\n");
    }
    void updateLightLists() {
        for (uint8_t i=0; i<EMITTER_MAX_LIGHT_LISTS; i++) {
          if (lightLists[i] != 0) {
            emit(i);
          }
        }
    }
    void emit(uint8_t k) {
        LightList *lightList = lightLists[k];
        emitLightList(lightList);
        if (lightList->numEmitted >= lightList->numLights) {
          lightLists[k] = 0;
        }
    }
    virtual void emitLightList(LightList *lightList) = 0;

};
