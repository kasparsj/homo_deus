#pragma once

#include "Weight.h"
#include "Port.h"
#include "Connection.h"

class Model {

  public:
  
    float defaultW;
    Weight *weights[MAX_PORTS];
    
    Model(float defaultW) {
      this->defaultW = defaultW;
    }
  
    ~Model() {
      delete[] weights;
    }
    
    Model *put(Port *outgoing, Port *incoming, float weight) {
      _getOrCreate(outgoing, defaultW)->add(incoming, weight);
      _getOrCreate(incoming, defaultW)->add(outgoing, weight);
      return this;
    }
    
    Model *put(Port *outgoing, float w) {
      _getOrCreate(outgoing, w);
      return this;
    }
    
    Model *put(Connection *con, float w1, float w2) {
      put(con->fromPort, w1);
      put(con->toPort, w2);
      return this;
    }
    
    Model *put(Connection *con, float w) {
      put(con, w, w);
      return this;
    }
    
    float get(Port *outgoing, Port *incoming) {
      if (outgoing == incoming) {
        return 0;
      }
      Weight *weight = weights[outgoing->id];
      if (weight != NULL) {
        return weight->get(incoming);      
      }
      return defaultW;
    }
    
    Weight *_getOrCreate(Port *outgoing, float w) {
      Weight *weight = weights[outgoing->id];
      if (weight == NULL) {
        weight = new Weight(w);
        weights[outgoing->id] = weight;
      }
      return weight;
    }
};
