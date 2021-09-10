#include "Intersection.h"
#include "Connection.h"
#include "Model.h"

uint8_t Intersection::nextId = 0;

Intersection::Intersection(uint8_t numPorts, uint16_t topPixel, int16_t bottomPixel, uint8_t group) : LPEmitter(EMITTER_MAX_LIGHT_LISTS), LPBase(group) {
  this->id = nextId++;
  this->numPorts = numPorts;
  this->topPixel = topPixel;
  this->bottomPixel = bottomPixel;
  this->ports = new Port*[numPorts]{0};
  for (uint8_t i=0; i<numPorts; i++) {
    ports[i] = NULL;
  }
  initLights(EMITTER_MAX_LIGHTS);
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

void Intersection::emitLight(Light* light) {
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
    addLight(light);
}

void Intersection::update() {
  updateLightLists();
  for (uint8_t i=0; i<EMITTER_MAX_LIGHTS; i++) {
    updateLight(i);
  }
  for (uint8_t i=0; i<freeOutgoing; i++) {
    if (outgoingLights[i] >= 0) {
      sendOut(i); 
    }
  }
  freeOutgoing = 0;
}

void Intersection::updateLight(uint8_t i) {
  Light *light = lights[i];
  if (light != NULL && !light->isExpired) {
    light->resetPixels();
    if (light->shouldExpire()) {
      if (light->speed == 0 || light->position >= 1.f) {
        light->isExpired = true;
        removeLight(i);
      }
    }
    else if (light->position >= 1.f) {
      // neurons are updated after connections
      queueOutgoing(i);
    }       
    if (light->position >= 0.f) {
      light->pixel1 = topPixel;
    }
  }
}

void Intersection::queueOutgoing(uint8_t i) {
  if (freeOutgoing < EMITTER_MAX_LIGHTS) {
    outgoingLights[freeOutgoing] = i;
    freeOutgoing++;  
  }
  else {
    LP_LOGLN("Intersection queueOutgoing no free slot");
  }
}

Port* Intersection::sendOut(uint8_t i) {
  Light *light = lights[outgoingLights[i]];
  Port *port = NULL;
  if (light->linkedPrev != NULL) {
    uint8_t maxOutgoing = freeOutgoing;
    for (uint8_t k=0; k<maxOutgoing; k++) {
      if (outgoingLights[k] >= 0 && lights[outgoingLights[k]] == light->linkedPrev) {
        port = sendOut(k);
        break;
      }
    }
    if (port == NULL) {    
      port = light->linkedPrev->getOutPort(id);
    }
  }
  Model *model = light->getModel();
  Behaviour *behaviour = light->getBehaviour();
  if (port == NULL) {
    port = choosePort(model, light);
  }
  light->setOutPort(port, id);
  light->setInPort(NULL);
  light->position -= 1.f;
  removeLight(outgoingLights[i]);
  outgoingLights[i] = -1;
  if (port != NULL) { 
    if (behaviour->colorChangeGroups & port->group) {
      light->color = behaviour->getColor(light, port->group);
    }
    port->connection->addLight(light);
  }
  return port;
}

float Intersection::sumW(Model *model, Port *incoming) {
  float sum = 0;
  for (uint8_t i=0; i<numPorts; i++) {
    Port *port = ports[i];
    #ifdef LP_DEBUG
    if (port == NULL) {
      LP_LOGF("Intersection %d port %d is NULL", topPixel, i);
    }
    #endif
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

Port *Intersection::choosePort(Model *model, Light *light) {
    Port *incoming = light->inPort;
    float sum = sumW(model, incoming);
    if (sum <= 0.f) {
      return randomPort(incoming, light->getBehaviour());
    }
    float rnd = LP_RANDOM(sum * 1000) / 1000.f;
    for (uint8_t i=0; i<numPorts; i++) {
       Port *port = ports[i];
       #ifdef LP_DEBUG
       if (port == NULL) {
        LP_LOGF("Intersection %d choosePort port is NULL %d", topPixel, i);
       }
       #endif
       float w = model->get(port, incoming);
       if (port == incoming || w == 0) continue;
       if (rnd < w) {
         return port;
       }
       rnd -= w;
    }
    return NULL;
  }
