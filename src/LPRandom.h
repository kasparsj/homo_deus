#pragma once

#include "Config.h"

class LPRandom
{
  public:
    static float randomSpeed();
    static uint32_t randomDuration();

    static float MIN_SPEED;
    static float MAX_SPEED;
    static uint32_t MIN_DURATION;
    static uint32_t MAX_DURATION;
};
