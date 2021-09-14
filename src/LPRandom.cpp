#include "LPRandom.h"

uint32_t LPRandom::MIN_DURATION = 120;
uint32_t LPRandom::MAX_DURATION = 1440;

uint32_t LPRandom::randomDuration() {
  return MIN_DURATION + LP_RANDOM(max(MAX_DURATION - MIN_DURATION, (uint32_t) 0));
}
