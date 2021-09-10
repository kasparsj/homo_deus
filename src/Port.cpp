#include "Port.h"
#include "Intersection.h"

#ifdef LP_TEST
Port* Port::pool[255] = {0};
#endif

Port::Port(Connection *connection, Intersection *intersection, bool direction, uint8_t group) {
    this->id = getNextId();
    this->connection = connection;
    this->intersection = intersection;
    this->direction = direction;
    this->group = group;
    this->intersection->addPort(this);
    #ifdef LP_TEST
    pool[this->id] = this;
    #endif
}
