#pragma once

#include "Config.h"

class Connection;
class Intersection;

class Port {

  public:

    #ifdef LP_TEST
    static Port* pool[255];
    #endif

    uint8_t id;
    Connection *connection;
    Intersection *intersection;
    bool direction;
    uint8_t group;
  
    Port(Connection *connection, Intersection *intersection, bool direction, uint8_t group);
    
  private:
    
    uint8_t getNextId() {
        static uint8_t lastId = 0;
        return lastId++;
    }
  
};
