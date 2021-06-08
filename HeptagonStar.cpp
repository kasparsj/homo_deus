#include "HeptagonStar.h"
#include <Arduino.h>

void HeptagonStar::setup() {
  for (uint8_t i=0; i<7; i++) {
    zeroConnections[i].setup(&outerNeurons[i*2], &outerNeurons[i*2+1]);
  }
  for (uint8_t i=0; i<7; i++) {
    Intersection *outerNeuron1 = &outerNeurons[i*2+1];
    Intersection *outerNeuron2 = &outerNeurons[((i+1)*2) % 14];
    outerConnections[i*2].setup(outerNeuron1, &middleNeurons[i]);
    outerConnections[i*2+1].setup(outerNeuron2, &middleNeurons[i]);
  }

  for (uint8_t i=0; i<7; i++) {
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
    models[M_INNER_CIRCLE].put(middleConnection1, 1);
    models[M_INNER_CIRCLE].put(middleConnection2, 1);
  }
  
  for (uint8_t i=0; i<7; i++) {
    Intersection *innerNeuron1 = &innerNeurons[i];
    Intersection *innerNeuron2 = &innerNeurons[(i+1) % 7];
    Connection *innerConnection = &innerConnections[i];
    innerConnection->setup(innerNeuron1, innerNeuron2);
    models[M_STAR].put(innerConnection->fromPort, innerNeuron1->ports[1], 1);
    models[M_STAR].put(innerConnection->toPort, innerNeuron2->ports[0], 1);
    models[M_OUTER_STAR].put(innerConnection, 0);
    models[M_INNER_CIRCLE].put(innerConnection, 1);
  }
  
  #ifdef HD_TEST
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
  
  #ifdef HD_DEBUG
  Serial.println("HeptagonStar setup complete");
  #endif
}

void HeptagonStar::update() {
  for (uint8_t k=0; k<3; k++) {
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

#ifdef HD_TEST
bool HeptagonStar::isIntersection(uint16_t i) {
  return intersections[i];
}
bool HeptagonStar::isConnection(uint16_t i) {
  return connections[i];
}
void HeptagonStar::debugConnections() {
  Serial.println("--- CONNECTIONS ---");
  for (uint8_t i=0; i<14; i++) {
    if (i < 7) {
      Serial.print("Zero");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(zeroConnections[i].freeLight);
      Serial.print(" / ");
      Serial.println(zeroConnections[i].maxLights);

      Serial.print("Inner");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(innerConnections[i].freeLight);
      Serial.print(" / ");
      Serial.println(innerConnections[i].maxLights);
    } 
    Serial.print("Outer");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(outerConnections[i].freeLight);
    Serial.print(" / ");
    Serial.println(outerConnections[i].maxLights);

    Serial.print("Middle");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(middleConnections[i].freeLight);
    Serial.print(" / ");
    Serial.println(middleConnections[i].maxLights);
  }
}
void HeptagonStar::debugIntersections() {
  Serial.println("--- INTERSECTIONS ---");
  for (uint8_t i=0; i<14; i++) {
    if (i < 7) {
      Serial.print("Middle");
      Serial.print(middleNeurons[i].id);
      Serial.print(": ");
      Serial.print(middleNeurons[i].freeLight);
      Serial.print(" / ");
      Serial.println(middleNeurons[i].maxLights);

      Serial.print("Inner");
      Serial.print(innerNeurons[i].id);
      Serial.print(": ");
      Serial.print(innerNeurons[i].freeLight);
      Serial.print(" / ");
      Serial.println(innerNeurons[i].maxLights);
    } 
    Serial.print("Outer");
    Serial.print(outerNeurons[i].id);
    Serial.print(": ");
    Serial.print(outerNeurons[i].freeLight);
    Serial.print(" / ");
    Serial.println(outerNeurons[i].maxLights);
  }
}
#endif
