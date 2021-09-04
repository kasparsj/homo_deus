#pragma once

#include "Config.h"
#include "LPObject.h"
#include "Emitter.h"

class HeptagonStar : public LPObject {

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
      Model(M_DEFAULT, 1, GROUP1), 
      Model(M_STAR, 0, GROUP1), 
      Model(M_OUTER_STAR, 1, GROUP1), 
      Model(M_INNER_CIRCLE, 0, GROUP2),
      Model(M_HORNS, 0, GROUP1),
      Model(M_HALO, 0, GROUP2),
      Model(M_SPLATTER, 1, GROUP2 | GROUP3), 
    };

    HeptagonStar(uint16_t pixelCount) : LPObject(pixelCount) {
      intersections = new bool[pixelCount]{false};
      connections = new bool[pixelCount]{false};
    }
    ~HeptagonStar() {
      delete[] intersections;
      delete[] connections;
    }

    void setup();
    void update();
    Model* getModel(int i) {
      return &models[i];
    }
    Intersection* getIntersection(uint8_t i, uint8_t groups);
    Intersection* getFreeIntersection(uint8_t groups);

    #ifdef LP_TEST
    bool *intersections;
    bool isIntersection(uint16_t i);
    bool *connections;
    bool isConnection(uint16_t i);
    void debugConnections();
    void debugIntersections();
    #endif

};
