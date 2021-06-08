#include "Port.h"
#include "Intersection.h"
#include "Config.h"
#include <Arduino.h>

uint16_t Port::lastId = 0;

Port::Port(Connection *connection, Intersection *intersection, bool direction) {
  this->id = lastId;
  lastId++;
//  #ifdef HD_DEBUG
//  Serial.print("Port lastId ");
//  Serial.println(lastId);
//  #endif
  this->connection = connection;
  this->intersection = intersection;
  this->direction = direction;
  this->intersection->addPort(this);
}
