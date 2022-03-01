#include "Weight.h"

void Weight::add(const Port *incoming, uint8_t w) {
    if (conditional->willOverflow()) {
        LP_LOGLN("HashMap overflow");
    }
    conditional->set(incoming->id, w);
}

uint8_t Weight::get(const Port *incoming) const {
    if (incoming != NULL && conditional->contains(incoming->id)) {
        return conditional->get(incoming->id);
    }
    return w;
}
