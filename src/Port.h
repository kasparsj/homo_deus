#pragma once

#include <stdint.h>

class Connection;
class Intersection;

class Port {

  public:

    static uint8_t lastId;

    uint8_t id;
    Connection *connection;
    Intersection *intersection;
    bool direction;
    uint8_t group;
  
    Port(Connection *connection, Intersection *intersection, bool direction, uint8_t group);
  
};
