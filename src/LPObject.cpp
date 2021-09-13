#include <math.h>
#include "LPObject.h"
#include "Port.h"

LPObject* LPObject::instance = 0;

LPObject::LPObject(uint16_t pixelCount) : pixelCount(pixelCount) {
    instance = this;
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
    for (uint8_t i=0; i<modelCount; i++) {
        models[i] = NULL;
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
    return intersection;
}

Connection* LPObject::addConnection(Connection *connection) {
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        if (connection->group & (uint8_t) pow(2, i)) {
            conn[i][nextConn[i]++] = connection;
            break;
        }
    }
    return connection;
}

Connection* LPObject::addBridge(uint16_t fromPixel, uint16_t toPixel, uint8_t group) {
    Intersection *from = new Intersection(fromPixel, group);
    Intersection *to = new Intersection(toPixel, group);
    addIntersection(from);
    addIntersection(to);
    return addConnection(new Connection(from, to, group));
}

Intersection* LPObject::getIntersection(uint8_t i, uint8_t groups) {
    for (uint8_t j=0; j<MAX_GROUPS; j++) {
        if (groups == 0 || groups & (uint8_t) pow(2, j)) {
          if (i < interCount[j]) {
            return inter[j][i];
          }
          i -= interCount[j];
        }
    }
    return NULL;
}

Connection* LPObject::getConnection(uint8_t i, uint8_t groups) {
    for (uint8_t j=0; j<MAX_GROUPS; j++) {
        if (groups == 0 || groups & (uint8_t) pow(2, j)) {
          if (i < connCount[j]) {
            return conn[j][i];
          }
          i -= connCount[j];
        }
    }
    return NULL;
}
