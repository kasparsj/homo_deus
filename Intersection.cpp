#include "Intersection.h"
#include "Connection.h"
#include "Model.h"

void Intersection::addPort(Port *p) {
  for (int i=0; i<numPorts; i++) {
    if (ports[i] == NULL) {
      ports[i] = p;
      p->intersection = this;
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
  for (int i=0; i<MAX_LIGHTS; i++) {
    if (lights[i] == NULL) {
      lights[i] = light;
      break;
    }
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
  light->setOutPort(port);
  light->position -= 1.0;
  port->connection->addLight(light);
}

void Intersection::update() {
  for (int i=0; i<MAX_LIGHTS; i++) {
    Light *light = lights[i];
    if (light != NULL) {
      light->resetPixels();
      if (light->position >= 0) {
        light->pixel1 = topPixel;
        light->pixel1Bri = light->brightness;
      }
      light->update();
      if (light->expired()) {
        if (light->position >= 1.0) {
          delete lights[i];
          lights[i] = NULL;
        }
      }
      else if (light->position >= 1.0) {
        // neurons are updated after connections
        outgoing(lights[i]);
        lights[i] = NULL;
      }        
    }
  }
}

float Intersection::sumW(Model *model, Port *incoming) {
  float sum = 0;
  for (int i=0; i<numPorts; i++) {
    Port *port = ports[i];
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
    float rnd = random(sum);
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
