#include "Intersection.h"
#include "Connection.h"
#include "Model.h"
#include "Light.h"

uint8_t Intersection::nextId = 0;

Intersection::Intersection(uint8_t numPorts, uint16_t topPixel, int16_t bottomPixel, uint8_t group) : LPOwner(group) {
  this->id = nextId++;
  this->numPorts = numPorts;
  this->topPixel = topPixel;
  this->bottomPixel = bottomPixel;
  ports = new Port*[numPorts];
  for (uint8_t i=0; i<numPorts; i++) {
    ports[i] = NULL;
  }
}

void Intersection::addPort(Port *p) {
  for (uint8_t i=0; i<numPorts; i++) {
    if (ports[i] == NULL) {
      ports[i] = p;
      p->intersection = this;
      break;
    }
  }
}

void Intersection::emit(LPLight* light) {
    // go straight out of zeroConnection
    Behaviour *behaviour = light->getBehaviour();
    if (numPorts == 2) {
      for (uint8_t i=0; i<2; i++) {
        if (behaviour->forceBounce() ? ports[i]->connection->numLeds > 0 : ports[i]->connection->numLeds == 0) {
          light->setInPort(ports[i]);
          break;
        }
      }
    }
    light->owner = this;
}

void Intersection::update(LPLight* const light) {
  if (!light->isExpired) {
    light->resetPixels();
    if (light->shouldExpire()) {
      if (light->getSpeed() == 0 || light->position >= 1.f) {
        light->isExpired = true;
        light->owner = NULL;
      }
    }
    else if (light->position >= 1.f) {
      // neurons are updated after connections
      sendOut(light);
    }       
    else if (light->position >= 0.f) {
      light->pixel1 = topPixel;
    }
  }
}

//Port* Intersection::sendOut(LPLight *light) {
void Intersection::sendOut(LPLight* const light) {
  Port *port = NULL;
  LPLight* linkedPrev = light->getPrev();
  if (linkedPrev != NULL) {
    port = linkedPrev->getOutPort(id);
  }
  Model *model = light->getModel();
  Behaviour *behaviour = light->getBehaviour();
  if (port == NULL) {
    port = choosePort(model, light);
  }
  light->setOutPort(port, id);
  light->setInPort(NULL);
  light->position -= 1.f;
  light->owner = NULL;
  if (port != NULL) { 
    if (behaviour->colorChangeGroups & port->group) {
      (light)->setColor(behaviour->getColor(light, port->group));
    }
    port->connection->add(light);
  }
  //return port;
}

uint16_t Intersection::sumW(Model *model, Port *incoming) {
  uint16_t sum = 0;
  for (uint8_t i=0; i<numPorts; i++) {
    Port *port = ports[i];
    sum += model->get(port, incoming);
  }
  return sum;
}

Port *Intersection::randomPort(Port *incoming, Behaviour *behaviour) {
  Port *port;
  do {
    port = ports[(uint8_t) LP_RANDOM(numPorts)];
  } while (!behaviour->allowBounce() && behaviour->forceBounce() ? port != incoming : port == incoming);
  return port;
}

Port *Intersection::choosePort(Model *model, LPLight *light) {
    Port *incoming = light->inPort;
    uint16_t sum = sumW(model, incoming);
    if (sum == 0) {
      return randomPort(incoming, light->getBehaviour());
    }
    uint16_t rnd = LP_RANDOM(sum);
    for (uint8_t i=0; i<numPorts; i++) {
       Port *port = ports[i];
       uint8_t w = model->get(port, incoming);
       if (port == incoming || w == 0) continue;
       if (rnd < w) {
         return port;
       }
       rnd -= w;
    }
    return NULL;
  }
