#pragma once

#include "Config.h"
#include "LPObject.h"
#include "State.h"

enum HeptagonStarModel {
    M_DEFAULT = 0,
    M_STAR = 1,
    M_OUTER_STAR = 2,
    M_INNER_CIRCLE = 3,
    M_HORNS = 4,
    M_HALO = 5,
    M_SPLATTER = 6,
};

class HeptagonStar : public LPObject {

  public:
  
    HeptagonStar(uint16_t pixelCount) : LPObject(pixelCount) {
        setup();
    }

  private:
    void setup();

};
