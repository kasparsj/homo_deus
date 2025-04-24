#pragma once

#include "../Config.h"
#include "../LPObject.h"
#include "../State.h"

#define CROSS_PIXEL_COUNT 288  // 2 lines with 144 pixels each

enum CrossModel {
    C_DEFAULT = 0,
    C_HORIZONTAL = 1,
    C_VERTICAL = 2,
    C_DIAGONAL = 3,
    C_FIRST = C_DEFAULT,
    C_LAST = C_DIAGONAL,
};

class Cross : public LPObject {

  public:
  
    Cross(uint16_t pixelCount) : LPObject(pixelCount) {
        horizontalLineStart = 0;
        horizontalLineEnd = pixelCount / 2 - 1;
        verticalLineStart = pixelCount / 2;
        verticalLineEnd = pixelCount - 1;
        horizontalCross = pixelCount / 4;
        verticalCross = pixelCount / 4 * 3;
        
        setup();
    }
    
    virtual ~Cross() {}
    
    bool isMirrorSupported() { return true; }
    uint16_t* getMirroredPixels(uint16_t pixel, LPOwner* mirrorFlipEmitter, bool mirrorRotate);
    float getProgressOnLine(uint16_t pixel, bool isVertical) const;
    uint16_t getPixelOnLine(float perc, bool isVertical) const;
    
    EmitParams* getModelParams(int model) override {
        EmitParams* params = new EmitParams(model % CrossModel::C_LAST, LPRandom::randomSpeed());
        return params;
    }

  private:
    void setup();
    
    uint16_t mirrorPixels[2];
    uint16_t horizontalLineStart;
    uint16_t horizontalLineEnd;
    uint16_t verticalLineStart;
    uint16_t verticalLineEnd;
    uint16_t horizontalCross;
    uint16_t verticalCross;
};
