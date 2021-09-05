#pragma once

#include "Config.h"
#include "LPObject.h"
#include "Emitter.h"

class HeptagonStar : public LPObject {

  public:
  
    Model models[NUM_MODELS] = { 
      Model(M_DEFAULT, 1, GROUP1), 
      Model(M_STAR, 0, GROUP1), 
      Model(M_OUTER_STAR, 1, GROUP1), 
      Model(M_INNER_CIRCLE, 0, GROUP2),
      Model(M_HORNS, 0, GROUP1),
      Model(M_HALO, 0, GROUP2),
      Model(M_SPLATTER, 1, GROUP2 | GROUP3), 
    };

    HeptagonStar(uint16_t pixelCount) : LPObject(pixelCount) {
    }

    void setup();
    Model* getModel(int i) {
      return &models[i];
    }

};
