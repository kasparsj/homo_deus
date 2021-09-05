#pragma once

#include "Config.h"
#include "LPObject.h"
#include "Emitter.h"

class HeptagonStar : public LPObject {

  public:
  
    HeptagonStar(uint16_t pixelCount) : LPObject(pixelCount) {
        setup();
    }

    void setup();

};
