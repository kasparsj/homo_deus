#pragma once

#include "Intersection.h"
#include "Connection.h"
#include "Model.h"

class LPObject {

  public:
    static LPObject* instance;
    uint16_t pixelCount;
    uint8_t interCount[MAX_GROUPS] = {0};
    uint8_t connCount[MAX_GROUPS] = {0};
    Intersection **inter[MAX_GROUPS];
    Connection **conn[MAX_GROUPS];
    uint8_t modelCount;
    Model **models;

    LPObject(uint16_t pixelCount) : pixelCount(pixelCount) {
        instance = this;
        #ifdef LP_TEST
        intersections = new bool[pixelCount]{false};
        connections = new bool[pixelCount]{false};
        #endif
    }
    ~LPObject() {
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
        delete[] intersections;
        delete[] connections;
        #endif
    }
    void initInter(uint8_t inter1Count, uint8_t inter2Count=0, uint8_t inter3Count=0, uint8_t inter4Count=0, uint8_t inter5Count=0) {
        interCount[0] = inter1Count;
        interCount[1] = inter2Count;
        interCount[2] = inter3Count;
        interCount[3] = inter4Count;
        interCount[4] = inter5Count;
        for (uint8_t i=0; i<MAX_GROUPS; i++) {
            if (interCount[i] > 0) {
                inter[i] = new Intersection*[interCount[i]]{0};
            }
        }
    }
    void initConn(uint8_t conn1Count, uint8_t conn2Count=0, uint8_t conn3Count=0, uint8_t conn4Count=0, uint8_t conn5Count=0) {
        connCount[0] = conn1Count;
        connCount[1] = conn2Count;
        connCount[2] = conn3Count;
        connCount[3] = conn4Count;
        connCount[4] = conn5Count;
        for (uint8_t i=0; i<MAX_GROUPS; i++) {
            if (connCount[i] > 0) {
                conn[i] = new Connection*[connCount[i]]{0};
            }
        }
    }
    void initModels(uint8_t modelCount) {
        this->modelCount = modelCount;
        models = new Model*[modelCount]{0};
    }
    Model* addModel(Model *model) {
        models[model->id] = model;
        return model;
    }
    Intersection* addIntersection(Intersection *intersection);
    Connection* addConnection(Connection *connection);
    Connection* addBridge(uint16_t fromPixel, uint16_t toPixel, uint8_t group) {
        Intersection *from = new Intersection(fromPixel, group);
        Intersection *to = new Intersection(toPixel, group);
        addIntersection(from);
        addIntersection(to);
        return addConnection(new Connection(from, to, group));
    }
    void update();
    Model* getModel(int i) {
      return models[i];
    }
    Intersection* getIntersection(uint8_t i, uint8_t groups);
    Intersection* getFreeIntersection(uint8_t groups);

    #ifdef LP_TEST
    bool *intersections;
    bool isIntersection(uint16_t i);
    bool *connections;
    bool isConnection(uint16_t i);
    void dumpConnections();
    void dumpIntersections();
    #endif

  private:
    uint8_t nextInter[MAX_GROUPS] = {0};
    uint8_t nextConn[MAX_GROUPS] = {0};

};
