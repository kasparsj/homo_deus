#include "Intersection.h"
#include "Connection.h"
#include "Model.h"
#include <Arduino.h>

Intersection::Intersection(int maxLights, int numPorts, int topPixel, int bottomPixel) {
  this->numPorts = numPorts;
  this->topPixel = topPixel;
  this->bottomPixel = bottomPixel;
  this->maxLights = maxLights;
  this->lights = new Light*[maxLights]();
  for (int i=0; i<maxLights; i++) {
    lights[i] = NULL;
  }
  this->ports = new Port*[numPorts]();
  for (int i=0; i<numPorts; i++) {
    ports[i] = NULL;
  }
}

void Intersection::addPort(Port *p) {
  for (int i=0; i<numPorts; i++) {
    if (ports[i] == NULL) {
      ports[i] = p;
      p->intersection = this;
      break;
    }
  }
}

void Intersection::emit(LightList *lightList) {
  for (int i=0; i<lightList->numLights; i++) {
    Light *light = (*lightList)[i];
    light->position = i * -1;
    light->life += ceil(1.0 / light->speed * i);
    addLight(light);
  }
}

void Intersection::addLight(Light *light) {
  if (freeLight < maxLights) {
    lights[freeLight] = light;
    freeLight++;
  }
  else {
    Serial.println("Intersection addLight no free slot");
  }
}

void Intersection::outgoing(Light *light) {
  Port *port;
  if (light->linkedPrev != NULL) {
    port = light->linkedPrev->outPort;
  }
  else {
    port = choosePort(light->model, light->inPort);
  }
  #ifdef HD_DEBUG
  if (port == NULL) {
    Serial.print("Intersection ");
    Serial.print(topPixel);
    Serial.println("choosePort returned NULL");
    return;
  }
  #endif
  light->setOutPort(port);
  light->position -= 1.f;
  port->connection->addLight(light);
}

void Intersection::update() {
  for (int i=0; i<freeLight; i++) {
    Light *light = lights[i];
    if (light && !light->isExpired) {
      light->resetPixels();
      if (light->position >= 0) {
        light->pixel1 = topPixel;
        light->pixel1Bri = light->brightness;
      }
      light->update();
      if (light->shouldExpire()) {
        if (light->position >= 1.f) {
          light->isExpired = true;
          removeLight(i);
        }
      }
      else if (light->position >= 1.0) {
        // neurons are updated after connections
        outgoing(light);
        removeLight(i);
      }        
    }
  }
}

void Intersection::removeLight(int i) {
  if (i < (freeLight - 1)) {
    lights[i] = lights[(freeLight - 1)];
    lights[(freeLight - 1)] = NULL;
  }
  else {
    lights[i] = NULL;
  }
  freeLight--;
}

float Intersection::sumW(Model *model, Port *incoming) {
  float sum = 0;
  for (int i=0; i<numPorts; i++) {
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
    if (sum == 0) {
      return randomPort(incoming);
    }
    float rnd = random(sum * 1000) / 1000.f;
    for (int i=0; i<numPorts; i++) {
       Port *port = ports[i];
       float w = model->get(port, incoming);
       if (port == incoming || w == 0) continue;
       if (rnd < w) {
         return port;
       }
       rnd -= w;
    }
    return NULL;
  }
