#include "Intersection.h"
#include "Connection.h"
#include "Model.h"
#include <Arduino.h>

uint8_t Intersection::nextId = 0;

Intersection::Intersection(uint8_t numPorts, uint16_t topPixel, int16_t bottomPixel) {
  this->id = nextId++;
  this->numPorts = numPorts;
  this->topPixel = topPixel;
  this->bottomPixel = bottomPixel;
  this->ports = new Port*[numPorts]();
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

void Intersection::add(LightList *lightList) {
  for (uint8_t j=0; j<EMITTER_MAX_LIGHT_LISTS; j++) {
    if (lightLists[j] == NULL) {
      lightLists[j] = lightList;
      return;
    }
  }
  Serial.printf("Intersection %d addLightList no free slot\n", topPixel);
}

void Intersection::emit(uint8_t k) {
  LightList *lightList = lightLists[k];
  if (lightList->numEmitted < lightList->numLights) {
    uint8_t batchSize = min(lightList->numLights - lightList->numEmitted, EMITTER_MAX_LIGHTS - numLights);
    uint8_t j = lightList->numEmitted;      
    for (uint8_t i=0; i<batchSize; i++) {
      Light *light = (*lightList)[i+j];
      if (light->position < 0) {
        break;
      }
      lightList->numEmitted++;
      // go straight out of zeroConnection
      Behaviour *behaviour = lightList->behaviour;
      if (numPorts == 2) {
        for (uint8_t i=0; i<2; i++) {
          if (behaviour->randomPortBe == B_RND_PORT_THROUGH ? ports[i]->connection->numLeds == 0 : ports[i]->connection->numLeds > 0) {
            light->setInPort(ports[i]);
            break;
          }
        }
      }
      addLight(light);
    }
  }
  if (lightList->numEmitted >= lightList->numLights) {
    lightLists[k] = NULL;
  }
}

void Intersection::addLight(Light *light) {
  if (freeLight < EMITTER_MAX_LIGHTS) {
    lights[freeLight] = light;
    numLights++;
    uint8_t i;
    for (i=freeLight+1; i<EMITTER_MAX_LIGHTS; i++) {
      if (lights[i] == NULL) {
        break;
      }
    }
    freeLight = i;
  }
  else {
    Serial.printf("Intersection %d addLight no free slot\n", topPixel);
  }
}

void Intersection::update() {
  for (uint8_t i=0; i<EMITTER_MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] != NULL) {
      emit(i);
    }
  }
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

void Intersection::removeLight(uint8_t i) {
  numLights--;
  lights[i] = NULL;
  if (i < freeLight) {
    freeLight = i;
  }
}

void Intersection::queueOutgoing(uint8_t i) {
  if (freeOutgoing < EMITTER_MAX_LIGHTS) {
    outgoingLights[freeOutgoing] = i;
    freeOutgoing++;  
  }
  else {
    Serial.println("Intersection queueOutgoing no free slot");
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
  if (port == NULL) {
    port = choosePort(model, light);
  }
  #ifdef HD_DEBUG
  if (port == NULL) {
    Serial.print("Intersection ");
    Serial.print(topPixel);
    Serial.print(" choosePort returned NULL ");
    Serial.print(" for model ");
    Serial.print(model->id);
    Serial.print(" with linkedPrev ");
    Serial.println(light->linkedPrev != NULL);
  }
  #endif
  light->setOutPort(port, id);
  light->setInPort(NULL);
  light->position -= 1.f;
  removeLight(outgoingLights[i]);
  outgoingLights[i] = -1;
  if (port != NULL) { 
    Behaviour *behaviour = light->getBehaviour();
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
    #ifdef HD_DEBUG
    if (port == NULL) {
      Serial.print("Intersection ");
      Serial.print(topPixel);
      Serial.print("port ");
      Serial.print(i);
      Serial.println("is NULL");
    }
    #endif
    sum += model->get(port, incoming);
  }
  return sum;
}

Port *Intersection::randomPort(Port *incoming, Behaviour *behaviour) {
  Port *port;
  do {
    port = ports[random(numPorts)];
  } while (behaviour->randomPortBe == B_RND_PORT_THROUGH ? port == incoming : port != incoming);
  return port;
}

Port *Intersection::choosePort(Model *model, Light *light) {
    Port *incoming = light->inPort;
    float sum = sumW(model, incoming);
    if (sum <= 0.f) {
      return randomPort(incoming, light->getBehaviour());
    }
    float rnd = random(sum * 1000) / 1000.f;
    for (uint8_t i=0; i<numPorts; i++) {
       Port *port = ports[i];
       #ifdef HD_DEBUG
       if (port == NULL) {
        Serial.print("Intersection ");
        Serial.print(topPixel);
        Serial.print(" choosePort port is NULL ");
        Serial.println(i);
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
