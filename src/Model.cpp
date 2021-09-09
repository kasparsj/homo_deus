#include "Model.h"
#include "LPObject.h"

// override from your Sculpture Object
uint8_t Model::maxWeights = 1;

uint16_t Model::getMaxLength() {
    // todo: implement
    return LPObject::instance->pixelCount;
}
