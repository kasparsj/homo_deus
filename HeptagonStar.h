#pragma once

#include "Config.h"
#include "Intersection.h"
#include "Connection.h"
#include "Model.h"

enum HeptagonStarModel { 
  M_DEFAULT, 
  M_STAR, 
  M_OUTER_STAR, 
  M_INNER_CIRCLE,
  M_STAR_COLOR,
  M_HORNS,
  M_HALO,
  M_SPLATTER,
};

class HeptagonStar {

  public:
  
    // todo: insert pixel numbers
    Intersection outerNeurons[14] = { 
       Intersection(2, 918), Intersection(2, 1),  // bottom
       Intersection(2, 653), Intersection(2, 654),  // bottom left
       Intersection(2, 389), Intersection(2, 390),  // left
       Intersection(2, 125), Intersection(2, 126), // top left
       Intersection(2, 789), Intersection(2, 791), // top right
       Intersection(2, 522), Intersection(2, 525),  // right
       Intersection(2, 260), Intersection(2, 263), // bottom right
     };
    Intersection middleNeurons[7] = {
      Intersection(4, 612, 42),  // bottom left
      Intersection(4, 696, 347), 
      Intersection(4, 434, 81),  // upper left
      Intersection(4, 742, 172), // top
      Intersection(4, 836, 478), // upper right
      Intersection(4, 568, 218),  // right
      Intersection(4, 876, 304) }; // bottom right
    Intersection innerNeurons[7] = {
      Intersection(4, 597, 320), 
      Intersection(4, 55, 334), 
      Intersection(4, 708, 69), 
      Intersection(4, 722, 448),  // top left
      Intersection(4, 462, 191), // top right
      Intersection(4, 848, 206),  // right 
      Intersection(4, 863, 582) }; // bottom right

    Connection zeroConnections[7];
    Connection outerConnections[14];
    Connection middleConnections[14];
    Connection innerConnections[7];
  
    Model models[NUM_MODELS] = { 
      Model(M_DEFAULT, 1, 14), 
      Model(M_STAR, 0, 14), 
      Model(M_OUTER_STAR, 1, 14), 
      Model(M_INNER_CIRCLE, 0, 14),
      Model(M_STAR_COLOR, 0, 14, 14),
      Model(M_HORNS, 0, 9),
      Model(M_HALO, 0, 3),
      Model(M_SPLATTER, 1, 14), 
    };
  
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
