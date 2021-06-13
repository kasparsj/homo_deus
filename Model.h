#pragma once

#include "Weight.h"
#include "Port.h"
#include "Connection.h"
#include "Intersection.h"
#include <NeoPixelBus.h>

class Light;

class Model {

  public:

    uint8_t id;
    float defaultW;
    // todo: perhaps better use HashMap
    Weight *weights[MAX_PORTS] = {0};
    uint8_t numEmitters;
    Intersection **emitters;
    uint8_t numColorPorts;
    Port **colorPorts;
    
    Model(uint8_t id, float defaultW, uint8_t numEmitters, uint8_t numColorPorts = 0) {
      this->id = id;
      this->defaultW = defaultW;
      this->numEmitters = numEmitters;
      this->emitters = new Intersection*[numEmitters]();
      for (uint8_t i=0; i<numEmitters; i++) {
        this->emitters[i] = NULL;
      }
      this->numColorPorts = numColorPorts;
      if (numColorPorts > 0) {
        this->colorPorts = new Port*[numColorPorts]();
        for (uint8_t i=0; i<numColorPorts; i++) {
          this->colorPorts[i] = NULL;
        }
      }
    }
  
    ~Model() {
      delete[] weights;
    }
    
    void put(Port *outgoing, Port *incoming, float weight) {
      _getOrCreate(outgoing, defaultW)->add(incoming, weight);
      _getOrCreate(incoming, defaultW)->add(outgoing, weight);
    }
    
    void put(Port *outgoing, float w) {
      _getOrCreate(outgoing, w);
    }
    
    void put(Connection *con, float w1, float w2) {
      put(con->fromPort, w1);
      put(con->toPort, w2);
    }
    
    void put(Connection *con, float w) {
      put(con, w, w);
    }
    
    float get(Port *outgoing, Port *incoming) {
      if (outgoing == incoming) {
        return 0;
      }
      Weight *weight = weights[outgoing->id];
      if (weight) {
        return weight->get(incoming);      
      }
      return defaultW;
    }
    
    Weight *_getOrCreate(Port *outgoing, float w) {
      Weight *weight = weights[outgoing->id];
      if (!weight) {
        weight = new Weight(w);
        weights[outgoing->id] = weight;
      }
      return weight;
    }

    uint16_t getMaxLength() {
      // todo: implement      
      return PIXEL_COUNT;
    }

    void addEmitter(Intersection *emitter) {
      for (uint8_t i=0; i<numEmitters; i++) {
        if (emitters[i] == NULL) {
          this->emitters[i] = emitter;
          return;
        }
      }
      Serial.printf("Model %d cannot add emitter\n", id);
    }

    int8_t getFreeEmitter() {
      uint8_t r = random(numEmitters);
      for (uint8_t i=0; i<numEmitters; i++) {
        uint8_t j = (r + i) % numEmitters;
        // todo: maybe better check light lists?
        if (emitters[j]->freeLight == 0) {
          return j;
        }
      }
      return -1;
    }

    void emit(uint8_t i, LightList *lightList) {
      lightList->initEmit();
      emitters[i]->add(lightList);
    }

    void addColorPort(Port *port) {
      for (uint8_t i=0; i<numColorPorts; i++) {
        if (colorPorts[i] == NULL) {
          colorPorts[i] = port;
          break;
        }
      }
    }

    bool checkColorPort(Port *port) {
      for (uint8_t i=0; i<numColorPorts; i++) {
        if (colorPorts[i] == port) {
          return true;
        }
      }
      return false;
    }

    RgbColor changeColor(Light *light) {
      if (light->linkedPrev != NULL) {
        return light->linkedPrev->color;
      }
      return RgbColor(random(255), random(255), random(255));
    }
};
