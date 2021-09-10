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

    LPObject(uint16_t pixelCount);
    ~LPObject();

    void initInter(uint8_t inter1Count, uint8_t inter2Count=0, uint8_t inter3Count=0, uint8_t inter4Count=0, uint8_t inter5Count=0);
    void initConn(uint8_t conn1Count, uint8_t conn2Count=0, uint8_t conn3Count=0, uint8_t conn4Count=0, uint8_t conn5Count=0);
    void initModels(uint8_t modelCount);
    Model* addModel(Model *model);
    Intersection* addIntersection(Intersection *intersection);
    Connection* addConnection(Connection *connection);
    Connection* addBridge(uint16_t fromPixel, uint16_t toPixel, uint8_t group);
    void update();
    Model* getModel(int i) {
      return models[i];
    }
    Intersection* getIntersection(uint8_t i, uint8_t groups);
    Intersection* getFreeIntersection(uint8_t groups);
    Connection* getConnection(uint8_t i, uint8_t groups);
    uint8_t countConnections() {
        uint8_t count = 0;
        for (uint8_t i=0; i<MAX_GROUPS; i++) {
            count += connCount[i];
        }
        return count;
    }

  private:
    uint8_t nextInter[MAX_GROUPS];
    uint8_t nextConn[MAX_GROUPS];

};
