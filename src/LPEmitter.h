#pragma once

#include "Config.h"
#include "LightList.h"

class LPEmitter {

  public:

    LPEmitter() {
        for (uint8_t i=0; i<EMITTER_MAX_LIGHT_LISTS; i++) {
            lightLists[i] = NULL;
        }
    }
    void add(LightList *lightList) {
        for (uint8_t i=0; i<EMITTER_MAX_LIGHT_LISTS; i++) {
          if (lightLists[i] == NULL) {
            lightLists[i] = lightList;
            return;
          }
        }
        LP_LOGF("LPBase addLightList no free slot\n");
    }
    void updateLightLists() {
        for (uint8_t i=0; i<EMITTER_MAX_LIGHT_LISTS; i++) {
          if (lightLists[i] != NULL) {
            emit(i);
          }
        }
    }
    void emit(uint8_t k) {
        LightList *lightList = lightLists[k];
        emitLightList(lightList);
        if (lightList->numEmitted >= lightList->numLights) {
          lightLists[k] = NULL;
        }
    }
    virtual void emitLightList(LightList *lightList) = 0;
    
  protected:
    
    LightList *lightLists[EMITTER_MAX_LIGHT_LISTS];

};
