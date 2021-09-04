#include "HeptagonStar.h"

void HeptagonStar::setup() {
  for (uint8_t i=0; i<7; i++) {
    zeroConnections[i].setup(&outerNeurons[i*2], &outerNeurons[i*2+1], GROUP1);
  }
  for (uint8_t i=0; i<7; i++) {
    Intersection *outerNeuron1 = &outerNeurons[i*2+1];
    Intersection *outerNeuron2 = &outerNeurons[((i+1)*2) % 14];
    outerConnections[i*2].setup(outerNeuron1, &middleNeurons[i], GROUP2);
    outerConnections[i*2+1].setup(outerNeuron2, &middleNeurons[i], GROUP2);
  }

  for (uint8_t i=0; i<7; i++) {
    Intersection *middleNeuron1 = &middleNeurons[i];
    Intersection *middleNeuron2 = &middleNeurons[(i-1+7) % 7];
    Connection *middleConnection1 = &middleConnections[i*2];
    Connection *middleConnection2 = &middleConnections[i*2+1];
    middleConnection1->setup(middleNeuron1, &innerNeurons[i], GROUP3);
    models[M_STAR].put(middleConnection1->fromPort, middleNeuron1->ports[1], 1);
    models[M_OUTER_STAR].put(middleConnection1, 0);
    middleConnection2->setup(middleNeuron2, &innerNeurons[i], GROUP3);
    models[M_STAR].put(middleConnection2->fromPort, middleNeuron2->ports[0], 1);
    models[M_OUTER_STAR].put(middleConnection2, 0);
    models[M_INNER_CIRCLE].put(middleConnection1, 1);
    models[M_INNER_CIRCLE].put(middleConnection2, 1);
  }
  
  for (uint8_t i=0; i<7; i++) {
    Intersection *innerNeuron1 = &innerNeurons[i];
    Intersection *innerNeuron2 = &innerNeurons[(i+1) % 7];
    Connection *innerConnection = &innerConnections[i];
    innerConnection->setup(innerNeuron1, innerNeuron2, GROUP4);
    models[M_STAR].put(innerConnection->fromPort, innerNeuron1->ports[1], 1);
    models[M_STAR].put(innerConnection->toPort, innerNeuron2->ports[0], 1);
    models[M_OUTER_STAR].put(innerConnection, 0);
    models[M_INNER_CIRCLE].put(innerConnection, 1);
  }

  // for (uint8_t i=0; i<14; i++) {
  //   models[M_DEFAULT].addEmitter(&outerNeurons[i]);
  //   models[M_STAR].addEmitter(&outerNeurons[i]);
  //   models[M_OUTER_STAR].addEmitter(&outerNeurons[i]);
  //   if (i<7) {
  //     models[M_INNER_CIRCLE].addEmitter(&middleNeurons[i]);
  //     models[M_INNER_CIRCLE].addEmitter(&innerNeurons[i]);
  //     models[M_SPLATTER].addEmitter(&middleNeurons[i]);
  //     models[M_SPLATTER].addEmitter(&innerNeurons[i]);
  //   }    
  // }

  models[M_HORNS].put(&outerConnections[5], 1);
  models[M_HORNS].put(&outerConnections[6], 1);
  models[M_HORNS].put(&outerConnections[7], 1);
  models[M_HORNS].put(&outerConnections[8], 1);
  models[M_HORNS].put(&middleConnections[6], 1);
  models[M_HORNS].put(&middleConnections[7], 1);
  models[M_HORNS].put(&middleConnections[8], 1);
  models[M_HORNS].put(&middleConnections[9], 1);
  //models[M_HORNS].put(&innerConnections[3], 1);
  // models[M_HORNS].addEmitter(&outerNeurons[6]);
  // models[M_HORNS].addEmitter(&outerNeurons[7]);
  // models[M_HORNS].addEmitter(&outerNeurons[8]);
  // models[M_HORNS].addEmitter(&outerNeurons[9]);
  // models[M_HORNS].addEmitter(&middleNeurons[2]);
  // models[M_HORNS].addEmitter(&middleNeurons[3]);
  // models[M_HORNS].addEmitter(&middleNeurons[4]);
  // models[M_HORNS].addEmitter(&innerNeurons[3]);
  // models[M_HORNS].addEmitter(&innerNeurons[4]);

  models[M_HALO].put(&innerConnections[3], 1);
  models[M_HALO].put(&middleConnections[6], 1);
  models[M_HALO].put(&middleConnections[9], 1);
  // models[M_HALO].addEmitter(&middleNeurons[3]);
  // models[M_HALO].addEmitter(&innerNeurons[3]);
  // models[M_HALO].addEmitter(&innerNeurons[4]);
  
  #ifdef LP_TEST
  for (uint8_t i=0; i<14; i++) {
    if (i < 7) {
      intersections[middleNeurons[i].topPixel] = true;
      intersections[middleNeurons[i].bottomPixel] = true;
      intersections[innerNeurons[i].topPixel] = true;
      intersections[innerNeurons[i].bottomPixel] = true;
    }
    intersections[outerNeurons[i].topPixel] = true;
  }
  for (uint8_t i=0; i<14; i++) {
    if (i < 7) {
      connections[innerConnections[i].fromPixel] = true;
      connections[innerConnections[i].toPixel] = true;
    }
    connections[outerConnections[i].fromPixel] = true;
    connections[outerConnections[i].toPixel] = true;
    connections[middleConnections[i].fromPixel] = true;
    connections[middleConnections[i].toPixel] = true;
  }
  #endif
  
  #ifdef LP_DEBUG
  LP_LOGF("HeptagonStar setup complete");
  #endif
}

void HeptagonStar::update() {
  for (uint8_t k=0; k<UPDATES_PER_FRAME; k++) {
    for (uint8_t i=0; i<14; i++) {
      if (i < 7) {
        zeroConnections[i].update();
        innerConnections[i].update();
      }
      outerConnections[i].update();
      middleConnections[i].update();
    }
    for (uint8_t i=0; i<14; i++) {
      if (i < 7) {
        middleNeurons[i].update();
        innerNeurons[i].update();          
      }
      outerNeurons[i].update();
    }
  }
}

Intersection* HeptagonStar::getIntersection(uint8_t i, uint8_t groups) {
  if (groups & GROUP1) {
    if (i < 14) {
      return &outerNeurons[i];
    } 
    i -= 14;   
  }
  if (groups & GROUP2) {
    if (i < 7) {
      return &middleNeurons[i];
    }
    i -= 7;
  }
  if (groups & GROUP3) {
    if (i < 7) {
      return &innerNeurons[i];
    }
    i -= 7;
  }
  return NULL;
}

Intersection* HeptagonStar::getFreeIntersection(uint8_t groups) {
  if (groups & GROUP1) {
    uint8_t r = LP_RANDOM(14);
    for (uint8_t i=0; i<14; i++) {
      uint8_t j = (r + i) % 14;
      if (outerNeurons[j].freeLight == 0) {
        return &outerNeurons[j];
      }
    }
  }
  if (groups & GROUP2) {
    uint8_t r = LP_RANDOM(7);
    for (uint8_t i=0; i<7; i++) {
      uint8_t j = (r + i) % 7;
      if (middleNeurons[j].freeLight == 0) {
        return &middleNeurons[j];
      }
    }          
  }
  if (groups & GROUP3) {
    uint8_t r = LP_RANDOM(7);
    for (uint8_t i=0; i<7; i++) {
      uint8_t j = (r + i) % 7;
      if (innerNeurons[j].freeLight == 0) {
        return &innerNeurons[j];
      }
    }
  }
  return NULL;
}

#ifdef LP_TEST
bool HeptagonStar::isIntersection(uint16_t i) {
  return intersections[i];
}
bool HeptagonStar::isConnection(uint16_t i) {
  return connections[i];
}
void HeptagonStar::debugConnections() {
  LP_LOGLN("--- CONNECTIONS ---");
  for (uint8_t i=0; i<14; i++) {
    if (i < 7) {
      LP_LOGF("Zero %d - %d: %d / %d\n", zeroConnections[i].fromPixel, zeroConnections[i].toPixel, zeroConnections[i].freeLight, zeroConnections[i].maxLights);
      LP_LOGF("Inner %d - %d: %d / %d\n", innerConnections[i].fromPixel, innerConnections[i].toPixel, innerConnections[i].freeLight, innerConnections[i].maxLights);
    } 
    LP_LOGF("Outer %d - %d: %d / %d\n", outerConnections[i].fromPixel, outerConnections[i].toPixel, outerConnections[i].freeLight, outerConnections[i].maxLights);
    LP_LOGF("Middle %d - %d: %d / %d\n", middleConnections[i].fromPixel, middleConnections[i].toPixel, middleConnections[i].freeLight, middleConnections[i].maxLights);
  }
}
void HeptagonStar::debugIntersections() {
  LP_LOGLN("--- INTERSECTIONS ---");
  for (uint8_t i=0; i<14; i++) {
    if (i < 7) {
      LP_LOGF("Middle %d: %d\n", middleNeurons[i].id, middleNeurons[i].freeLight);
      LP_LOGF("Inner %d: %d\n", innerNeurons[i].id, innerNeurons[i].freeLight);
    } 
    LP_LOGF("Outer %d: %d\n", outerNeurons[i].id, outerNeurons[i].freeLight);
  }
}
#endif
