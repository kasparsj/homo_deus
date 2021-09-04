#pragma once

#include "Config.h"
#include "Port.h"
#include "HashMap.h"

class Weight {

  public:
    float w;
    HashMap<uint8_t, float, MAX_CONDITIONAL_WEIGHTS> conditional;
    
    Weight(float w) {
      this->w = w;
    }
    
    void add(Port *incoming, float w);
    float get(Port *incoming);
  
};
