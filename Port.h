#pragma once

#include <Arduino.h>

class Connection;
class Intersection;

class Port {

  public:

    static uint8_t lastId;

    uint8_t id;
    Connection *connection;
    Intersection *intersection;
    bool direction;
  
    Port(Connection *connection, Intersection *intersection, bool direction);
  
};
