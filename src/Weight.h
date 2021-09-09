#pragma once

#include "Config.h"
#include "Port.h"
#include "HashMap.h"

class Weight {

  public:
    
    Weight(float w) : w(w) {
        conditional = new HashMap<uint8_t, float>(MAX_CONDITIONAL_WEIGHTS);
    }
    ~Weight() {
        delete conditional;
    }
    
    void add(Port *incoming, float w);
    float get(Port *incoming);
    
  private:
    float w;
    HashMap<uint8_t, float> *conditional;
  
};
