#include <math.h>
#include "LPObject.h"
#include "Port.h"

LPObject* LPObject::instance = 0;

LPObject::LPObject(uint16_t pixelCount) : pixelCount(pixelCount) {
    instance = this;
    #ifdef LP_TEST
    interPixels = new bool[pixelCount]{false};
    connPixels = new bool[pixelCount]{false};
    #endif
}

LPObject::~LPObject() {
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        if (interCount[i] > 0) {
            delete[] inter[i];
        }
        if (connCount[i] > 0) {
            delete[] conn[i];
        }
    }
    delete[] models;
    #ifdef LP_TEST
    delete[] interPixels;
    delete[] connPixels;
    for (uint8_t i=0; i<modelCount; i++) {
        delete[] weightPixels[i];
        weightPixels[i] = NULL;
    }
    delete[] weightPixels;
    #endif
}

void LPObject::initInter(uint8_t inter1Count, uint8_t inter2Count, uint8_t inter3Count, uint8_t inter4Count, uint8_t inter5Count) {
    interCount[0] = inter1Count;
    interCount[1] = inter2Count;
    interCount[2] = inter3Count;
    interCount[3] = inter4Count;
    interCount[4] = inter5Count;
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        if (interCount[i] > 0) {
            inter[i] = new Intersection*[interCount[i]];
            for (uint8_t j=0; j<interCount[i]; j++) {
                inter[i][j] = NULL;
            }
        }
        nextInter[i] = 0;
    }
}

void LPObject::initConn(uint8_t conn1Count, uint8_t conn2Count, uint8_t conn3Count, uint8_t conn4Count, uint8_t conn5Count) {
    connCount[0] = conn1Count;
    connCount[1] = conn2Count;
    connCount[2] = conn3Count;
    connCount[3] = conn4Count;
    connCount[4] = conn5Count;
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        if (connCount[i] > 0) {
            conn[i] = new Connection*[connCount[i]];
            for (uint8_t j=0; j<connCount[i]; j++) {
                conn[i][j] = NULL;
            }
        }
        nextConn[i] = 0;
    }
}

void LPObject::initModels(uint8_t modelCount) {
    this->modelCount = modelCount;
    models = new Model*[modelCount];
    #ifdef LP_TEST
    weightPixels = new bool*[modelCount];
    #endif
    for (uint8_t i=0; i<modelCount; i++) {
        models[i] = NULL;
        #ifdef LP_TEST
        weightPixels[i] = new bool[pixelCount]{false};
        #endif
    }
}

void LPObject::setupWeightPixels() {
    #ifdef LP_TEST
    for (uint8_t i=0; i<modelCount; i++) {
        Model* model = models[i];
        for (uint8_t j=0; j<model->weights->size(); j++) {
            uint8_t portId = model->weights->keyAt(j);
            Port* port = Port::pool[portId];
            //Weight* weight = model->weights->valueAt(j);
            weightPixels[model->id][port->intersection->topPixel] = true;
        }
        #endif
    }
}

Model* LPObject::addModel(Model *model) {
    models[model->id] = model;
    return model;
}

Intersection* LPObject::addIntersection(Intersection *intersection) {
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        if (intersection->group & (uint8_t) pow(2, i)) {
            inter[i][nextInter[i]++] = intersection;
            break;
        }
    }
    #ifdef LP_TEST
    interPixels[intersection->topPixel] = true;
    if (intersection->bottomPixel >= 0) {
        interPixels[intersection->bottomPixel] = true;
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
    connPixels[connection->fromPixel] = true;
    connPixels[connection->toPixel] = true;
    #endif
    return connection;
}

Connection* LPObject::addBridge(uint16_t fromPixel, uint16_t toPixel, uint8_t group) {
    Intersection *from = new Intersection(fromPixel, group);
    Intersection *to = new Intersection(toPixel, group);
    addIntersection(from);
    addIntersection(to);
    return addConnection(new Connection(from, to, group));
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
bool LPObject::isModelWeight(uint8_t id, uint16_t i) {
  return weightPixels[id][i];
}
bool LPObject::isIntersection(uint16_t i) {
  return interPixels[i];
}
bool LPObject::isConnection(uint16_t i) {
  return connPixels[i];
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
