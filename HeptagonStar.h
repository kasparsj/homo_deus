#pragma once

#include "Config.h"
#include "Intersection.h"
#include "Connection.h"
#include "Model.h"

enum HeptagonStarModel { M_DEFAULT, M_STAR, M_OUTER_STAR, M_INNER_CIRCLE };

class HeptagonStar {

  public:
  
    // todo: insert pixel numbers
    Intersection outerNeurons[14] = { 
       Intersection(EMITTER_MAX_LIGHTS, 2, 921), Intersection(EMITTER_MAX_LIGHTS, 2, 0), Intersection(EMITTER_MAX_LIGHTS, 2, 656), Intersection(EMITTER_MAX_LIGHTS, 2, 657), 
       Intersection(EMITTER_MAX_LIGHTS, 2, 392), Intersection(EMITTER_MAX_LIGHTS, 2, 393), Intersection(EMITTER_MAX_LIGHTS, 2, 126), Intersection(EMITTER_MAX_LIGHTS, 2, 127), 
       Intersection(EMITTER_MAX_LIGHTS, 2, 792), Intersection(EMITTER_MAX_LIGHTS, 2, 795), Intersection(EMITTER_MAX_LIGHTS, 2, 525), Intersection(EMITTER_MAX_LIGHTS, 2, 528), 
       Intersection(EMITTER_MAX_LIGHTS, 2, 262), Intersection(EMITTER_MAX_LIGHTS, 2, 265),
     };
    Intersection middleNeurons[7] = {
      Intersection(INTER_MAX_LIGHTS, 4, 614, 42), Intersection(INTER_MAX_LIGHTS, 4, 699, 350), Intersection(INTER_MAX_LIGHTS, 4, 437, 81),  Intersection(INTER_MAX_LIGHTS, 4, 745, 174), 
      Intersection(INTER_MAX_LIGHTS, 4, 840, 481), Intersection(INTER_MAX_LIGHTS, 4, 570, 220), Intersection(INTER_MAX_LIGHTS, 4, 880, 306) };
    Intersection innerNeurons[7] = {
      Intersection(INTER_MAX_LIGHTS, 4, 600, 322), Intersection(INTER_MAX_LIGHTS, 4, 55, 335), Intersection(INTER_MAX_LIGHTS, 4, 711, 69), Intersection(INTER_MAX_LIGHTS, 4, 725, 451), 
      Intersection(INTER_MAX_LIGHTS, 4, 465, 193), Intersection(INTER_MAX_LIGHTS, 4, 852, 208), Intersection(INTER_MAX_LIGHTS, 4, 867, 585) };

    Connection zeroConnections[7];
    Connection outerConnections[14];
    Connection middleConnections[14];
    Connection innerConnections[7];
  
    Model models[NUM_MODELS] = { Model(M_DEFAULT, 1), Model(M_STAR, 0), Model(M_OUTER_STAR, 1), Model(M_INNER_CIRCLE, 0) };
  
    HeptagonStar() {}
    void setup();
    void update();

};
