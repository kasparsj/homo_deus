#include "Weight.h"
#include <Arduino.h>

void Weight::add(Port *incoming, float w) {
  #ifdef HD_DEBUG
  if (conditional.willOverflow()) {
    Serial.println("HashMap overflow");
  }
  #endif
  conditional[incoming->id] = w;
}

float Weight::get(Port *incoming) {
   if (incoming != NULL && conditional.contains(incoming->id)) {
    return conditional[incoming->id];
   }
  return w;
}
