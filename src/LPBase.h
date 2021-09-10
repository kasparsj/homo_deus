#pragma once

#include "Config.h"
#include "LPLight.h"
#include "LightList.h"

template<typename T>
class LPBase
{
  public:
    LPLight **lights;
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
        this->lights = new LPLight*[maxLights]();
        for (T i=0; i<maxLights; i++) {
          lights[i] = NULL;
        }
    }
    virtual void emitLightList(LightList* lightList) {
        if (lightList->numEmitted < lightList->numLights) {
          uint8_t batchSize = min(lightList->numLights - lightList->numEmitted, maxLights - numLights);
          uint8_t j = lightList->numEmitted;
          for (uint8_t i=0; i<batchSize; i++) {
            LPLight *light = (*lightList)[i+j];
            if (light->position < 0) {
              break;
            }
            lightList->numEmitted++;
            emitLight(light);
          }
        }
    }
    virtual void emitLight(LPLight* light) = 0;
    void addLight(LPLight *light) {
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

