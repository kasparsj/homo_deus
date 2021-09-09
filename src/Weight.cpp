#include "Weight.h"

void Weight::add(Port *incoming, float w) {
    #if defined(ARDUINO) && defined(LP_DEBUG)
    if (conditional->willOverflow()) {
        LP_LOGLN("HashMap overflow");
    }
    #endif
    conditional->set(incoming->id, w);
}

float Weight::get(Port *incoming) {
    if (incoming != NULL && conditional->contains(incoming->id)) {
        return conditional->get(incoming->id);
    }
    return w;
}
