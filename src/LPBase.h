#pragma once

#include "Config.h"
#include "LightList.h"
#include "Light.h"

template<typename T>
class LPBase
{
  public:
    LightList *lightLists[EMITTER_MAX_LIGHT_LISTS] = {0};
    Light **lights;
    T maxLights = 0;
    T numLights = 0;
    T freeLight = 0;
    uint8_t group;

    LPBase(uint8_t group) : group(group) {}
    ~LPBase() {
        delete[] lights;
    }

    void initLights(T maxLights) {
        this->maxLights = maxLights;
        this->lights = new Light*[maxLights]();
        for (T i=0; i<maxLights; i++) {
          lights[i] = NULL;
        }
    }
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
        if (lightList->numEmitted < lightList->numLights) {
          uint8_t batchSize = min(lightList->numLights - lightList->numEmitted, maxLights - numLights);
          uint8_t j = lightList->numEmitted;
          for (uint8_t i=0; i<batchSize; i++) {
            Light *light = (*lightList)[i+j];
            if (light->position < 0) {
              break;
            }
            lightList->numEmitted++;
            emitLight(light);
          }
        }
        if (lightList->numEmitted >= lightList->numLights) {
          lightLists[k] = 0;
        }
    }
    virtual void emitLight(Light* light) = 0;
    void addLight(Light *light) {
      if (freeLight < this->maxLights) {
        lights[freeLight] = light;
        numLights++;
        T i;
        for (i=freeLight+1; i<this->maxLights; i++) {
          if (lights[i] == NULL) {
            break;
          }
        }
        freeLight = i;
      }
      else {
        LP_LOGF("addLight no free slot\n");
      }
    }
    void removeLight(T i) {
        numLights--;
        lights[i] = NULL;
        if (i < freeLight) {
          freeLight = i;
        }
    }
};

