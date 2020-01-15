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
  return conditional.contains(incoming->id) ? conditional[incoming->id] : w;
}
