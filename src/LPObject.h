#pragma once

#include "Intersection.h"
#include "Connection.h"
#include "Model.h"

class LPObject {

  public:
    static LPObject* instance;
    uint16_t pixelCount;
    uint8_t interCount[5] = {0};
    uint8_t connCount[5] = {0};
    Intersection **inter[5];
    Connection **conn[5];

    LPObject(uint16_t pixelCount) : pixelCount(pixelCount) {
        instance = this;
        #ifdef LP_TEST
        intersections = new bool[pixelCount]{false};
        connections = new bool[pixelCount]{false};
        #endif
    }
    ~LPObject() {
        for (uint8_t i=0; i<5; i++) {
            if (interCount[i] > 0) {
                delete[] inter[i];
            }
            if (connCount[i] > 0) {
                delete[] conn[i];
            }
        }
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
        for (uint8_t i=0; i<5; i++) {
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
        for (uint8_t i=0; i<5; i++) {
            if (connCount[i] > 0) {
                conn[i] = new Connection*[connCount[i]]{0};
            }
        }
    }
    Intersection* addIntersection(Intersection *intersection, uint8_t group) {
        if (group & GROUP1) {
            inter[0][nextInter[0]++] = intersection;
        }
        else if (group & GROUP2) {
            inter[1][nextInter[1]++] = intersection;
        }
        else if (group & GROUP3) {
            inter[2][nextInter[2]++] = intersection;
        }
        else if (group & GROUP4) {
            inter[3][nextInter[3]++] = intersection;
        }
        #ifdef LP_TEST
        intersections[intersection->topPixel] = true;
        if (intersection->bottomPixel >= 0) {
            intersections[intersection->bottomPixel] = true;
        }
        #endif
        return intersection;
    }
    Connection* addConnection(Connection *connection) {
        if (connection->group & GROUP1) {
            conn[0][nextConn[0]++] = connection;
        }
        else if (connection->group & GROUP2) {
            conn[1][nextConn[1]++] = connection;
        }
        else if (connection->group & GROUP3) {
            conn[2][nextConn[2]++] = connection;
        }
        else if (connection->group & GROUP4) {
            conn[3][nextConn[3]++] = connection;
        }
        #ifdef LP_TEST
        connections[connection->fromPixel] = true;
        connections[connection->toPixel] = true;
        #endif
        return connection;
    }
    void addBridge(uint16_t fromPixel, uint16_t toPixel, uint8_t group) {
        Intersection *from = new Intersection(2, fromPixel);
        Intersection *to = new Intersection(2, toPixel);
        addIntersection(from, group);
        addIntersection(to, group);
        addConnection(new Connection(from, to, group));
    }
    void update();
    virtual Model* getModel(int i) = 0;
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
    uint8_t nextInter[5] = {0};
    uint8_t nextConn[5] = {0};

};
