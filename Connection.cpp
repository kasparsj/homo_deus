#include "Connection.h"
#include "Intersection.h"

void Connection::setup(Intersection *from, Intersection *to, int numLeds) {
  this->from = from;
  this->to = to;
  this->numLeds = numLeds;
  
  fromPort = new Port(this, from, false);
  toPort = new Port(this, to, true);    

  brightness = new float[numLeds];
}

void Connection::addLight(Light *light) {
  for (int i=0; i<MAX_LIGHTS; i++) {
    if (lights[i] == NULL) {
      lights[i] = light;
      break;
    }
  }
}

void Connection::update() {
  for (int i=0; i<numLeds; i++) {
    brightness[i] = 0;
  }
  for (int i=0; i<MAX_LIGHTS; i++) {
    Light *light = lights[i];
    if (light) {
      // hack: fix rounding error
      float pos = round(light->position * 1000) / 1000.0;
      if (pos < 0) {
        light->update();
      }
      else if (pos < numLeds) {
        int ledIdx = light->outPort->direction ? ceil((float) numLeds - pos - 1.0) : floor(pos);
        brightness[ledIdx] += light->brightness;
        light->update();
      }
      else {
        // connections are updated before neurons
        Intersection *neuron = light->outPort->direction ? from : to;
        Port *port = light->outPort->direction ? fromPort : toPort;
        light->position -= numLeds;
        light->setInPort(port);
        neuron->addLight(lights[i]);
        lights[i] = NULL;
      }
    }
  }
}
