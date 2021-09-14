#include "Weight.h"

void Weight::add(Port *incoming, uint8_t w) {
    if (conditional->willOverflow()) {
        LP_LOGLN("HashMap overflow");
    }
    conditional->set(incoming->id, w);
}

uint8_t Weight::get(Port *incoming) {
    if (incoming != NULL && conditional->contains(incoming->id)) {
        return conditional->get(incoming->id);
    }
    return w;
}
