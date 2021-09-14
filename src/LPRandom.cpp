#include "LPRandom.h"

float LPRandom::MIN_SPEED = 0.5f;
float LPRandom::MAX_SPEED = 10.f;
uint32_t LPRandom::MIN_DURATION = 120 * 16;
uint32_t LPRandom::MAX_DURATION = 1440 * 16;

float LPRandom::randomSpeed() {
  return MIN_SPEED + LP_RANDOM(max(MAX_SPEED - MIN_SPEED, 0.f));
}

uint32_t LPRandom::randomDuration() {
  return MIN_DURATION + LP_RANDOM(max(MAX_DURATION - MIN_DURATION, (uint32_t) 0));
}
