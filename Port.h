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
  
    Port(Connection *connection, Intersection *intersection, bool direction) {
      this->id = lastId;
      Port::lastId++;
      this->connection = connection;
      this->intersection = intersection;
      this->direction = direction;
      // todo: do this from outside
      //this->intersection->addPort(this);
    }
  
};
