#include "Port.h"
#include "Intersection.h"

int Port::lastId = 0;

Port::Port(Connection *connection, Intersection *intersection, bool direction) {
  this->id = lastId;
  Port::lastId++;
  this->connection = connection;
  this->intersection = intersection;
  this->direction = direction;
  this->intersection->addPort(this);
}
