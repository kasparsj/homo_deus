#pragma once

#include <Arduino.h>

class Connection;
class Intersection;

class Port {

  public:

    static uint16_t lastId;

    uint16_t id;
    Connection *connection;
    Intersection *intersection;
    bool direction;
  
    Port(Connection *connection, Intersection *intersection, bool direction);
  
};
