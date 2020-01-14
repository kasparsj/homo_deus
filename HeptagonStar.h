#pragma once

#include "Config.h"
#include "Intersection.h"
#include "Connection.h"
#include "Model.h"

enum HeptagonStarModel { M_DEFAULT, M_STAR, M_OUTER_STAR, M_INNER_CIRCLE };

class HeptagonStar {

  public:
  
    // todo: insert pixel numbers
    Intersection outerNeurons[7] = { Intersection(2, 0), Intersection(2, 0), Intersection(2, 0), Intersection(2, 0), Intersection(2, 0), Intersection(2, 0), Intersection(2, 0) };
    Intersection middleNeurons[7] = { Intersection(4, 0), Intersection(4, 0), Intersection(4, 0), Intersection(4, 0), Intersection(4, 0), Intersection(4, 0), Intersection(4, 0) };
    Intersection innerNeurons[7] = { Intersection(4, 0), Intersection(4, 0), Intersection(4, 0), Intersection(4, 0), Intersection(4, 0), Intersection(4, 0), Intersection(4, 0) };
  
    Connection outerConnections[14];
    Connection middleConnections[14];
    Connection innerConnections[7];
  
    Model models[NUM_MODELS] = { Model(1), Model(0), Model(1), Model(0) };
  
    HeptagonStar() {
      for (int i=0; i<7; i++) {
        Intersection *outerNeuron1 = &outerNeurons[i];
        Intersection *outerNeuron2 = &outerNeurons[(i+1) % 7];
        outerConnections[i*2].setup(outerNeuron1, &middleNeurons[i], calculateNumLeds(outerNeuron1, &middleNeurons[i]));
        outerConnections[i*2+1].setup(outerNeuron2, &middleNeurons[i], calculateNumLeds(outerNeuron2, &middleNeurons[i]));
      }
  
      for (int i=0; i<7; i++) {
        Intersection *middleNeuron1 = &middleNeurons[i];
        Intersection *middleNeuron2 = &middleNeurons[(i-1+7) % 7];
        Connection *middleConnection1 = &middleConnections[i*2];
        Connection *middleConnection2 = &middleConnections[i*2+1];
        middleConnection1->setup(middleNeuron1, &innerNeurons[i], calculateNumLeds(middleNeuron1, &innerNeurons[i]));
        models[M_STAR].put(middleConnection1->fromPort, middleNeuron1->ports[1], 1);
        models[M_OUTER_STAR].put(middleConnection1, 0);
        middleConnection2->setup(middleNeuron2, &innerNeurons[i], calculateNumLeds(middleNeuron2, &innerNeurons[i]));
        models[M_STAR].put(middleConnection2->fromPort, middleNeuron2->ports[0], 1);
        models[M_OUTER_STAR].put(middleConnection2, 0);
      }
      
      for (int i=0; i<7; i++) {
        Intersection *innerNeuron1 = &innerNeurons[i];
        Intersection *innerNeuron2 = &innerNeurons[(i+1) % 7];
        Connection *innerConnection = &innerConnections[i];
        innerConnection->setup(innerNeuron1, innerNeuron2, calculateNumLeds(innerNeuron1, innerNeuron2));
        models[M_STAR].put(innerConnection->fromPort, innerNeuron1->ports[1], 1);
        models[M_STAR].put(innerConnection->toPort, innerNeuron2->ports[0], 1);
        models[M_OUTER_STAR].put(innerConnection, 0);
        models[M_INNER_CIRCLE].put(innerConnection, 1);
      }
    }
  
    int calculateNumLeds(Intersection *from, Intersection *to) const {
      int numLeds = abs(from->topPixel - to->topPixel) - 2;
      if (from->bottomPixel > -1) {
        numLeds = min(numLeds, abs(from->bottomPixel - to->topPixel));
      }
      if (to->bottomPixel > -1) {
        numLeds = min(numLeds, abs(from->topPixel - to->bottomPixel));
      }
      if (from->bottomPixel > -1 && to->bottomPixel > -1) {
        numLeds = min(numLeds, abs(from->bottomPixel - to->bottomPixel));
      }
      return numLeds;
    }
  
    void update() {
      
    }

};
