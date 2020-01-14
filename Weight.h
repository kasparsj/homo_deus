#pragma once

#include "Config.h"
#include "Port.h"

class Weight {

  public:
    float w;
    float conditional[MAX_PORTS];
    
    Weight(float w) {
      this->w = w;
    }
    
    void add(Port *incoming, float w) {
      conditional[incoming->id] = w;
    }
    
    float get(Port *incoming) {
      return conditional[incoming->id] != NULL ? conditional[incoming->id] : w;
    }
  
};
