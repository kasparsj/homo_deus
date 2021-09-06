#include <math.h>
#include "LPObject.h"

LPObject* LPObject::instance = 0;

Intersection* LPObject::addIntersection(Intersection *intersection) {
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        if (intersection->group & (uint8_t) pow(2, i)) {
            inter[i][nextInter[i]++] = intersection;
            break;
        }
    }
    #ifdef LP_TEST
    intersections[intersection->topPixel] = true;
    if (intersection->bottomPixel >= 0) {
        intersections[intersection->bottomPixel] = true;
    }
    #endif
    return intersection;
}

Connection* LPObject::addConnection(Connection *connection) {
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        if (connection->group & (uint8_t) pow(2, i)) {
            conn[i][nextConn[i]++] = connection;
            break;
        }
    }

    #ifdef LP_TEST
    connections[connection->fromPixel] = true;
    connections[connection->toPixel] = true;
    #endif
    return connection;
}

void LPObject::update() {
  for (uint8_t k=0; k<UPDATES_PER_FRAME; k++) {
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        for (uint8_t j=0; j<connCount[i]; j++) {
            conn[i][j]->update();
        }
    }
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        for (uint8_t j=0; j<interCount[i]; j++) {
            inter[i][j]->update();
        }
    }
  }
}

Intersection* LPObject::getIntersection(uint8_t i, uint8_t groups) {
    for (uint8_t j=0; j<MAX_GROUPS; j++) {
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
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
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

Connection* LPObject::getConnection(uint8_t i, uint8_t groups) {
    for (uint8_t j=0; j<MAX_GROUPS; j++) {
        if (groups == 0 || groups & (uint8_t) pow(2, j)) {
          if (i < connCount[i]) {
            return conn[0][i];
          }
          i -= connCount[i];
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
  for (uint8_t i=0; i<MAX_GROUPS; i++) {
      for (uint8_t j=0; j<connCount[i]; j++) {
        LP_LOGF("Connection%d %d - %d: %d / %d\n", i, conn[i][j]->fromPixel, conn[i][j]->toPixel, conn[i][j]->freeLight, conn[i][j]->maxLights);
      }
  }
}
void LPObject::dumpIntersections() {
  LP_LOGLN("--- INTERSECTIONS ---");
  for (uint8_t i=0; i<MAX_GROUPS; i++) {
      for (uint8_t j=0; j<interCount[i]; i++) {
        LP_LOGF("Intersection%d %d: %d\n", i, inter[i][j]->id, inter[i][j]->freeLight);
      }
  }
}
#endif
