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
  
    HeptagonStar() {
      for (int i=0; i<7; i++) {
        zeroConnections[i].setup(&outerNeurons[i*2], &outerNeurons[i*2+1]);
      }
      for (int i=0; i<7; i++) {
        Intersection *outerNeuron1 = &outerNeurons[i*2+1];
        Intersection *outerNeuron2 = &outerNeurons[(i+1)*2];
        outerConnections[i*2].setup(outerNeuron1, &middleNeurons[i]);
        outerConnections[i*2+1].setup(outerNeuron2, &middleNeurons[i]);
      }
  
      for (int i=0; i<7; i++) {
        Intersection *middleNeuron1 = &middleNeurons[i];
        Intersection *middleNeuron2 = &middleNeurons[(i-1+7) % 7];
        Connection *middleConnection1 = &middleConnections[i*2];
        Connection *middleConnection2 = &middleConnections[i*2+1];
        middleConnection1->setup(middleNeuron1, &innerNeurons[i]);
        models[M_STAR].put(middleConnection1->fromPort, middleNeuron1->ports[1], 1);
        models[M_OUTER_STAR].put(middleConnection1, 0);
        middleConnection2->setup(middleNeuron2, &innerNeurons[i]);
        models[M_STAR].put(middleConnection2->fromPort, middleNeuron2->ports[0], 1);
        models[M_OUTER_STAR].put(middleConnection2, 0);
      }
      
      for (int i=0; i<7; i++) {
        Intersection *innerNeuron1 = &innerNeurons[i];
        Intersection *innerNeuron2 = &innerNeurons[(i+1) % 7];
        Connection *innerConnection = &innerConnections[i];
        innerConnection->setup(innerNeuron1, innerNeuron2);
        models[M_STAR].put(innerConnection->fromPort, innerNeuron1->ports[1], 1);
        models[M_STAR].put(innerConnection->toPort, innerNeuron2->ports[0], 1);
        models[M_OUTER_STAR].put(innerConnection, 0);
        models[M_INNER_CIRCLE].put(innerConnection, 1);
      }
    }
  
    void update() {
      for (int i=0; i<14; i++) {
        if (i < 7) {
          zeroConnections[i].update();
          innerConnections[i].update();
        }
        outerConnections[i].update();
        middleConnections[i].update();
      }
      for (int i=0; i<14; i++) {
        if (i < 7) {
          middleNeurons[i].update();
          innerNeurons[i].update();          
        }
        outerNeurons[i].update();
      }
    }

};
