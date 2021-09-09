#pragma once

#include <stdint.h>

class Connection;
class Intersection;

class Port {

  public:

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
