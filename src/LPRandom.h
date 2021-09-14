#pragma once

#include "Config.h"

class LPRandom
{
  public:
    static uint32_t randomDuration();

    static uint32_t MIN_DURATION;
    static uint32_t MAX_DURATION;
};
