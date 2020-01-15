#include "Connection.h"
#include "Intersection.h"

void Connection::setup(Intersection *from, Intersection *to) {
  this->from = from;
  this->to = to;
  
  fromPort = new Port(this, from, false);
  toPort = new Port(this, to, true);    
    
  pixelDir = to->topPixel > from->topPixel ? 1 : -1;
  fromPixel = from->topPixel + pixelDir;
  toPixel = to->topPixel - pixelDir;
  numLeds = abs(fromPixel - toPixel) > 3 ? abs(fromPixel - toPixel) + 1 : 0;
  if (from->bottomPixel > -1) {
    if (abs(from->bottomPixel - to->topPixel) < numLeds) {
      pixelDir = to->topPixel > from->bottomPixel ? 1 : -1;
      fromPixel = from->bottomPixel + pixelDir;
      toPixel = to->topPixel - pixelDir;
      numLeds = abs(fromPixel - toPixel) + 1;
    }
  }
  if (to->bottomPixel > -1) {
    if (abs(from->topPixel - to->bottomPixel) < numLeds) {
      pixelDir = to->bottomPixel > from->topPixel ? 1 : -1;
      fromPixel = from->topPixel + pixelDir;
      toPixel =  to->bottomPixel + pixelDir;
      numLeds = abs(fromPixel - toPixel) + 1;
    }
  }
  if (from->bottomPixel > -1 && to->bottomPixel > -1) {
    if (abs(from->bottomPixel - to->bottomPixel) < numLeds) {
      pixelDir = to->bottomPixel > from->bottomPixel ? 1 : -1;
      fromPixel = from->bottomPixel + pixelDir;
      toPixel = to->bottomPixel - pixelDir;
      numLeds = abs(fromPixel - toPixel) + 1;
    }
  }
}

void Connection::addLight(Light *light) {
  if (numLeds > 0) {
    for (int i=0; i<MAX_LIGHTS; i++) {
      if (lights[i] == NULL) {
        lights[i] = light;
        break;
      }
    }    
  }
  else {
    outgoing(light);
  }
}

void Connection::update() {
  if (numLeds == 0) return;
  for (int i=0; i<MAX_LIGHTS; i++) {
    Light *light = lights[i];
    if (light != NULL) {
      light->resetPixels();
      // hack: fix rounding error
      float pos = round(light->position * 1000) / 1000.0;
      if (pos < 0) {
        light->update();
      }
      else if (pos < numLeds) {
        int ledIdx = light->outPort->direction ? ceil((float) numLeds - pos - 1.0) : floor(pos);
        light->pixel1 = fromPixel + (floor(pos) * pixelDir);
        light->pixel1Bri = light->brightness;
        //light->pixel2 = 
        light->update();
      }
      else {
        outgoing(lights[i]);
        lights[i] = NULL;
      }
    }
  }
}

void Connection::outgoing(Light *light) {
  // connections are updated before neurons
  Intersection *neuron = light->outPort->direction ? from : to;
  Port *port = light->outPort->direction ? fromPort : toPort;
  light->position -= numLeds;
  light->setInPort(port);
  neuron->addLight(light);
}
