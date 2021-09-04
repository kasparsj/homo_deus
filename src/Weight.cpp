#include "Weight.h"

void Weight::add(Port *incoming, float w) {
  #if defined(ARDUINO) && defined(LP_DEBUG)
  if (conditional.willOverflow()) {
    LP_LOG("HashMap overflow");
  }
  #endif
  conditional[incoming->id] = w;
}

float Weight::get(Port *incoming) {
   if (incoming != 0 && conditional.contains(incoming->id)) {
    return conditional[incoming->id];
   }
  return w;
}
