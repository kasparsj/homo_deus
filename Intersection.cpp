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
      for (uint16_t i=0; i<lightList->numLights; i++) {
        Light *light = (*lightList)[i];
        light->position = i * -1;
        light->life += ceil(1.0 / light->speed * i);
      }
      lightLists[j] = lightList;
      return;
    }
  }
  Serial.printf("Intersection %d addLightList no free slot\n", topPixel);
}

void Intersection::emit(uint8_t k) {
  LightList *lightList = lightLists[k];
  if (lightList->numEmitted < lightList->numLights) {
    for (uint8_t i=lightList->numEmitted; i<lightList->numLights; i++) {
      Light *light = (*lightList)[i];
      if (light->position < 0) {
        break;
      }
      lightList->numEmitted++;
      addLight(light);
    }
  }
  else {
    lightLists[k] = NULL;
  }
}

void Intersection::addLight(Light *light) {
  if (freeLight < EMITTER_MAX_LIGHTS) {
    lights[freeLight] = light;
    freeLight++;
  }
  else {
    Serial.printf("Intersection %d addLight no free slot\n", topPixel);
  }
}

void Intersection::update() {
  for (uint16_t i=0; i<EMITTER_MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] != NULL) {
      emit(i);
    }
  }
  for (uint16_t i=0; i<freeLight; i++) {
    updateLight(i);
  }
  for (uint16_t i=0; i<freeOutgoing; i++) {
    if (outgoingLights[i] != NULL) {
      sendOut(i); 
    }
  }
  freeOutgoing = 0;
  for (int16_t i=(freeRemove-1); i>=0; i--) {
    if (removeLights[i] >= 0) {
      removeLight(removeLights[i]);
      removeLights[i] = -1;
    }
  }
  freeLight -= freeRemove;
  freeRemove = 0;
}

void Intersection::updateLight(uint16_t i) {
  Light *light = lights[i];
  if (light != NULL && !light->isExpired) {
    light->resetPixels();
    if (light->position >= 0.f) {
      light->pixel1 = topPixel;
      light->pixel1Bri = light->brightness;
    }
    if (light->shouldExpire()) {
      if (light->position >= 1.f) {
        light->isExpired = true;
        queueRemove(i);
      }
    }
    else if (light->position >= 1.f) {
      // neurons are updated after connections
      queueOutgoing(light);
      queueRemove(i);
    }        
  }
}

void Intersection::queueOutgoing(Light *light) {
  if (freeOutgoing < MAX_OUTGOING_LIGHTS) {
    outgoingLights[freeOutgoing] = light;
    freeOutgoing++;  
  }
  else {
    Serial.println("Intersection queueOutgoing no free slot");
  }
}

void Intersection::queueRemove(uint16_t i) {
  if (freeRemove < MAX_OUTGOING_LIGHTS) {
    removeLights[freeRemove] = i;
    freeRemove++;    
  }
  else {
    Serial.println("Intersection queueRemove no free slot");
  }
}

Port* Intersection::sendOut(uint16_t i) {
  Light *light = outgoingLights[i];
  Port *port = NULL;
  if (light->linkedPrev != NULL) {
    uint16_t maxOutgoing = freeOutgoing;
    for (uint16_t k=0; k<maxOutgoing; k++) {
      if (outgoingLights[k] == light->linkedPrev) {
        port = sendOut(k);
        break;
      }
    }
    if (port == NULL) {    
      port = light->linkedPrev->getOutPort(id);
    }
  }
  if (port == NULL) {
    port = choosePort(light->model, light->inPort);
  }
  #ifdef HD_DEBUG
  if (port == NULL) {
    Serial.print("Intersection ");
    Serial.print(topPixel);
    Serial.print(" choosePort returned NULL ");
    Serial.print(" for model ");
    Serial.print(light->model->id);
    Serial.print(" with linkedPrev ");
    Serial.println(light->linkedPrev != NULL);
  }
  #endif
  light->setOutPort(port, id);
  light->setInPort(NULL);
  light->position -= 1.f;
  outgoingLights[i] = NULL;
  if (port != NULL) { 
    if (light->model->numColorPorts > 0 && light->model->checkColorPort(port)) {
      light->color = light->model->changeColor(light);
    }
    port->connection->addLight(light);
  }
  return port;
}

void Intersection::removeLight(uint16_t i) {
  if (i < (freeLight - freeRemove)) {
    for (uint16_t j=1; j <= freeRemove; j++) {
      if (lights[(freeLight - j)] != NULL) {
        lights[i] = lights[(freeLight - j)];
        lights[(freeLight - j)] = NULL;
        break;
      }
    }
  }
  else {
    lights[i] = NULL;
  }
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

Port *Intersection::randomPort(Port *incoming) {
  Port *port;
  do {
    port = ports[random(numPorts)];
  } while (port == incoming);
  return port;
}

Port *Intersection::choosePort(Model *model, Port *incoming) {
    float sum = sumW(model, incoming);
    if (sum <= 0.f) {
      return randomPort(incoming);
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
