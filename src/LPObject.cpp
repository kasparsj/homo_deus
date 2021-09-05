#include <math.h>
#include "LPObject.h"

LPObject* LPObject::instance = 0;

void LPObject::update() {
  for (uint8_t k=0; k<UPDATES_PER_FRAME; k++) {
    for (uint8_t i=0; i<5; i++) {
        for (uint8_t j=0; j<connCount[i]; j++) {
            conn[i][j]->update();
        }
    }
    for (uint8_t i=0; i<5; i++) {
        for (uint8_t j=0; j<interCount[i]; j++) {
            inter[i][j]->update();
        }
    }
  }
}

Intersection* LPObject::getIntersection(uint8_t i, uint8_t groups) {
    for (uint8_t j=0; i<5; i++) {
        if (groups & (uint8_t) pow(2, j)) {
          if (i < interCount[i]) {
            return inter[0][i];
          }
          i -= interCount[i];
        }
    }
    return NULL;
}

Intersection* LPObject::getFreeIntersection(uint8_t groups) {
    for (uint8_t i=0; i<5; i++) {
        if (groups & (uint8_t) pow(2, i)) {
          uint8_t r = LP_RANDOM(interCount[i]);
          for (uint8_t i=0; i<interCount[i]; i++) {
            uint8_t j = (r + i) % interCount[i];
            if (inter[i][j]->freeLight == 0) {
              return inter[i][j];
            }
          }
        }
    }
    return NULL;
}

#ifdef LP_TEST
bool LPObject::isIntersection(uint16_t i) {
  return intersections[i];
}
bool LPObject::isConnection(uint16_t i) {
  return connections[i];
}
void LPObject::dumpConnections() {
  LP_LOGLN("--- CONNECTIONS ---");
  for (uint8_t i=0; i<5; i++) {
      for (uint8_t j=0; j<connCount[i]; j++) {
        LP_LOGF("Connection%d %d - %d: %d / %d\n", i, conn[i][j]->fromPixel, conn[i][j]->toPixel, conn[i][j]->freeLight, conn[i][j]->maxLights);
      }
  }
}
void LPObject::dumpIntersections() {
  LP_LOGLN("--- INTERSECTIONS ---");
  for (uint8_t i=0; i<5; i++) {
      for (uint8_t j=0; j<interCount[i]; i++) {
        LP_LOGF("Intersection%d %d: %d\n", i, inter[i][j]->id, inter[i][j]->freeLight);
      }
  }
}
#endif
