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
       Intersection(2, 921), Intersection(2, 0), Intersection(2, 656), Intersection(2, 657), Intersection(2, 392), Intersection(2, 393), Intersection(2, 126),
       Intersection(2, 127), Intersection(2, 792), Intersection(2, 795), Intersection(2, 525), Intersection(2, 528), Intersection(2, 262), Intersection(2, 265),
     };
    Intersection middleNeurons[7] = { Intersection(4, 614, 42), Intersection(4, 699, 350), Intersection(4, 437, 81), Intersection(4, 745, 174), Intersection(4, 840, 481), Intersection(4, 570, 220), Intersection(4, 880, 306) };
    Intersection innerNeurons[7] = { Intersection(4, 600, 322), Intersection(4, 55, 335), Intersection(4, 711, 69), Intersection(4, 725, 451), Intersection(4, 465, 193), Intersection(4, 852, 208), Intersection(4, 867, 585) };

    Connection zeroConnections[7];
    Connection outerConnections[14];
    Connection middleConnections[14];
    Connection innerConnections[7];
  
    Model models[NUM_MODELS] = { Model(1), Model(0), Model(1), Model(0) };
  
    HeptagonStar() {}
    void setup();
    void update();

};
