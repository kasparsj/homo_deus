#include "HeptagonStar.h"
#include <Arduino.h>

void HeptagonStar::setup() {
  for (int i=0; i<7; i++) {
    zeroConnections[i].setup(&outerNeurons[i*2], &outerNeurons[i*2+1]);
  }
  for (int i=0; i<7; i++) {
    Intersection *outerNeuron1 = &outerNeurons[i*2+1];
    Intersection *outerNeuron2 = &outerNeurons[((i+1)*2) % 14];
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
  #ifdef HD_DEBUG
  Serial.println("HeptagonStar setup complete");
  #endif
}

void HeptagonStar::update() {
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
