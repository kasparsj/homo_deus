#pragma once

#include "Config.h"
#include "LPObject.h"
#include "State.h"

enum HeptagonStarModel {
    M_DEFAULT = 0,
    M_STAR = 1,
    M_OUTER_SUN = 2,
    M_INNER_TRIS = 3,
    M_SMALL_STAR = 4,
    M_INNER_SUN = 5,
    M_SPLATTER = 6,
    M_FIRST = M_DEFAULT,
    M_LAST = M_SPLATTER,
};

class HeptagonStar : public LPObject {

  public:
  
    HeptagonStar(uint16_t pixelCount) : LPObject(pixelCount) {
        setup();
    }

  private:
    void setup();

};
