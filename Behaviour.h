#pragma once

#include "Config.h"
#include "EmitParams.h"
#include <NeoPixelBus.h>

class Light;

class Behaviour {

  public:
    PosBehaviour posChangeBe = B_CHANGE_POS_SPEED;
    RenderBehaviour renderBe = B_RENDER_LIGHT;
    RandomPortBehaviour randomPortBe = B_RND_PORT_THROUGH;
    uint8_t colorChangeGroups = 0;

    Behaviour(EmitParams &params) {
      posChangeBe = params.posChangeBe;
      renderBe = params.renderBe;
      randomPortBe = params.randomPortBe;
      colorChangeGroups = params.colorChangeGroups;
    }

    float getPosition(Light *light);

    RgbColor getColor(Light *light, uint8_t group);
};