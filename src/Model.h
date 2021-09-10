#pragma once

#include "Weight.h"
#include "Port.h"
#include "Connection.h"
#include "Intersection.h"

class Light;

class Model {

  public:

    static uint8_t maxWeights;
     
    uint8_t id;
    float defaultW;
    uint8_t emitGroups;
    HashMap<uint8_t, Weight*> *weights;
    
    Model(uint8_t id, float defaultW, uint8_t emitGroups) : id(id), defaultW(defaultW), emitGroups(emitGroups) {
        weights = new HashMap<uint8_t, Weight*>(maxWeights);
        weights->setNullValue(NULL);
    }
  
    ~Model() {
        delete weights;
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
      Weight *weight = weights->get(outgoing->id);
      if (weight != NULL) {
        return weight->get(incoming);      
      }
      return defaultW;
    }
    
    Weight *_getOrCreate(Port *outgoing, float w) {
        Weight *weight = weights->get(outgoing->id);
        if (weight == NULL) {
            weight = new Weight(w);
            weights->set(outgoing->id, weight);
        }
        return weight;
    }

    uint16_t getMaxLength();
};
