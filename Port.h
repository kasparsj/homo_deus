#pragma once

class Connection;
class Intersection;

class Port {

  public:

    static int lastId;

    int id;
    Connection *connection;
    Intersection *intersection;
    bool direction;
  
    Port(Connection *connection, Intersection *intersection, bool direction);
  
};
