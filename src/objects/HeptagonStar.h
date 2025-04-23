#pragma once

#include "../Config.h"
#include "../LPObject.h"
#include "../State.h"

#define HEPTAGON_PIXEL_COUNT1 524
#define HEPTAGON_PIXEL_COUNT2 395
#define HEPTAGON_PIXEL_COUNT (HEPTAGON_PIXEL_COUNT1 + HEPTAGON_PIXEL_COUNT2) // 919

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
    
    virtual ~HeptagonStar() {}
    
    bool isMirrorSupported() { return true; }
    uint16_t* getMirroredPixels(uint16_t pixel, LPOwner* mirrorFlipEmitter, bool mirrorRotate);
    uint8_t getStarSegmentIndex(uint16_t pixel) const;
    float getProgressOnStarSegment(uint8_t pathIndex, uint16_t pixel) const;
    uint16_t getPixelOnStarSegment(uint8_t pathIndex, float perc) const;
    // todo: implement getXYZ

  private:
    void setup();
    
    uint16_t mirrorPixels[3];

};
