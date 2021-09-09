#include "Port.h"
#include "Intersection.h"
#include "Config.h"

Port::Port(Connection *connection, Intersection *intersection, bool direction, uint8_t group) {
  this->id = getNextId();
  this->connection = connection;
  this->intersection = intersection;
  this->direction = direction;
  this->group = group;
  this->intersection->addPort(this);
}
