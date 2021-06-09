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
       Intersection(EMITTER_MAX_LIGHTS, 2, 918), Intersection(EMITTER_MAX_LIGHTS, 2, 1),  // bottom
       Intersection(EMITTER_MAX_LIGHTS, 2, 653), Intersection(EMITTER_MAX_LIGHTS, 2, 654),  // bottom left
       Intersection(EMITTER_MAX_LIGHTS, 2, 389), Intersection(EMITTER_MAX_LIGHTS, 2, 390),  // left
       Intersection(EMITTER_MAX_LIGHTS, 2, 125), Intersection(EMITTER_MAX_LIGHTS, 2, 126), // top left
       Intersection(EMITTER_MAX_LIGHTS, 2, 789), Intersection(EMITTER_MAX_LIGHTS, 2, 791), // top right
       Intersection(EMITTER_MAX_LIGHTS, 2, 522), Intersection(EMITTER_MAX_LIGHTS, 2, 525),  // right
       Intersection(EMITTER_MAX_LIGHTS, 2, 260), Intersection(EMITTER_MAX_LIGHTS, 2, 263), // bottom right
     };
    Intersection middleNeurons[7] = {
      Intersection(INTER_MAX_LIGHTS, 4, 612, 42),  // bottom left
      Intersection(INTER_MAX_LIGHTS, 4, 696, 347), 
      Intersection(INTER_MAX_LIGHTS, 4, 434, 81),  // upper left
      Intersection(INTER_MAX_LIGHTS, 4, 742, 172), // top
      Intersection(INTER_MAX_LIGHTS, 4, 836, 478), // upper right
      Intersection(INTER_MAX_LIGHTS, 4, 568, 218),  // right
      Intersection(INTER_MAX_LIGHTS, 4, 876, 304) }; // bottom right
    Intersection innerNeurons[7] = {
      Intersection(INTER_MAX_LIGHTS, 4, 597, 320), 
      Intersection(INTER_MAX_LIGHTS, 4, 55, 334), 
      Intersection(INTER_MAX_LIGHTS, 4, 708, 69), 
      Intersection(INTER_MAX_LIGHTS, 4, 722, 448),  // top left
      Intersection(INTER_MAX_LIGHTS, 4, 462, 191), // top right
      Intersection(INTER_MAX_LIGHTS, 4, 848, 206),  // right 
      Intersection(INTER_MAX_LIGHTS, 4, 863, 582) }; // bottom right

    Connection zeroConnections[7];
    Connection outerConnections[14];
    Connection middleConnections[14];
    Connection innerConnections[7];
  
    Model models[NUM_MODELS] = { Model(M_DEFAULT, 1), Model(M_STAR, 0), Model(M_OUTER_STAR, 1), Model(M_INNER_CIRCLE, 0) };
  
    HeptagonStar() {}
    void setup();
    void update();

    #ifdef HD_TEST
    bool intersections[PIXEL_COUNT] = {false};
    bool isIntersection(uint16_t i);
    bool connections[PIXEL_COUNT] = {false};
    bool isConnection(uint16_t i);
    void debugConnections();
    void debugIntersections();
    #endif

};
