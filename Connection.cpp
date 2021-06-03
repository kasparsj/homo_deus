#include "Connection.h"
#include "Intersection.h"
#include <Arduino.h>

void Connection::setup(Intersection *from, Intersection *to) {
  this->from = from;
  this->to = to;
  
  fromPort = new Port(this, from, false);
  toPort = new Port(this, to, true);    
    
  pixelDir = to->topPixel > from->topPixel ? 1 : -1;
  fromPixel = from->topPixel + pixelDir;
  toPixel = to->topPixel - pixelDir;
  numLeds = abs(fromPixel - toPixel) > 4 ? abs(fromPixel - toPixel) + 1 : 0;
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
  if (numLeds > 0) {
    maxLights = numLeds * 5;
    lights = new Light*[maxLights]();
    for (int i=0; i<maxLights; i++) {
      lights[i] = NULL;
    }    
  }
}

void Connection::addLight(Light *light) {
  if (numLeds > 0) {
    if (freeLight < maxLights) {
      lights[freeLight] = light;
      freeLight++;
    }
    else {
      Serial.println("Connection addLight no free slot");
    }
  }
  else {
    outgoing(light);
  }
}

void Connection::update() {
  if (numLeds == 0) return;
  for (int i=0; i<freeLight; i++) {
    Light *light = lights[i];
    if (light != NULL) {
      light->resetPixels();
      // hack: fix rounding error
      float pos = round(light->position * 1000) / 1000.0;
      if (pos < numLeds) {
        int ledIdx = light->outPort->direction ? ceil((float) numLeds - pos - 1.0) : floor(pos);
        light->pixel1 = fromPixel + (ledIdx * pixelDir);
        light->pixel1Bri = light->brightness;
        //light->pixel2 = 
      }
      else {
        outgoing(light);
        queueRemove(i);
      }
    }
  }
  for (int i=(freeRemove-1); i>=0; i--) {
    if (removeLights[i] >= 0) {
      removeLight(removeLights[i]);
      removeLights[i] = -1;
    }
  }
  freeLight -= freeRemove;
  freeRemove = 0;
}

void Connection::queueRemove(int i) {
  if (freeRemove < MAX_OUTGOING_LIGHTS) {
    removeLights[freeRemove] = i;
    freeRemove++;    
  }
  else {
    Serial.println("Connection queueRemove no free slot");
  }
}

void Connection::removeLight(int i) {
  if (i < (freeLight - freeRemove)) {
    for (int j=1; j <= freeRemove; j++) {
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

void Connection::outgoing(Light *light) {
  // connections are updated before neurons
  Intersection *neuron = light->outPort->direction ? from : to;
  Port *port = light->outPort->direction ? fromPort : toPort;
  light->position -= numLeds;
  light->setInPort(port);
  light->setOutPort(NULL);
  neuron->addLight(light);
}
