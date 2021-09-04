#include "Port.h"
#include "Intersection.h"
#include "Config.h"

uint8_t Port::lastId = 0;

Port::Port(Connection *connection, Intersection *intersection, bool direction, uint8_t group) {
  this->id = lastId;
  lastId++;
  this->connection = connection;
  this->intersection = intersection;
  this->direction = direction;
  this->group = group;
  this->intersection->addPort(this);
}
