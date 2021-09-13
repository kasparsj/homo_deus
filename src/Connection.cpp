#include <math.h>
#include "Connection.h"
#include "Intersection.h"
#include "LPObject.h"

Connection::Connection(Intersection *from, Intersection *to, uint8_t group) : LPEmitter(1), LPBase(group) {
  this->from = from;
  this->to = to;
  
  fromPort = new Port(this, from, false, group);
  toPort = new Port(this, to, true, group);    
    
  pixelDir = to->topPixel > from->topPixel;
  fromPixel = from->topPixel + (pixelDir ? 1 : -1);
  toPixel = to->topPixel - (pixelDir ? 1 : -1);
  uint16_t diff = abs(fromPixel - toPixel);
  uint16_t leds = diff > 4 && diff < (LPObject::instance->pixelCount - 4) ? diff + 1 : 0;
  if (from->bottomPixel > -1) {
    if (abs(from->bottomPixel - to->topPixel) < leds) {
      pixelDir = to->topPixel > from->bottomPixel;
      fromPixel = from->bottomPixel + (pixelDir ? 1 : -1);
      toPixel = to->topPixel - (pixelDir ? 1 : -1);
      leds = abs(fromPixel - toPixel) + 1;
    }
  }
  if (to->bottomPixel > -1) {
    if (abs(from->topPixel - to->bottomPixel) < leds) {
      pixelDir = to->bottomPixel > from->topPixel;
      fromPixel = from->topPixel + (pixelDir ? 1 : -1);
      toPixel = to->bottomPixel - (pixelDir ? 1 : -1);
      leds = abs(fromPixel - toPixel) + 1;
    }
  }
  if (from->bottomPixel > -1 && to->bottomPixel > -1) {
    if (abs(from->bottomPixel - to->bottomPixel) < leds) {
      pixelDir = to->bottomPixel > from->bottomPixel;
      fromPixel = from->bottomPixel + (pixelDir ? 1 : -1);
      toPixel = to->bottomPixel - (pixelDir ? 1 : -1);
      leds = abs(fromPixel - toPixel) + 1;
    }
  }
  if (leds > 0) {
    numLeds = leds;
    initLights(min(numLeds * CONNECTION_MAX_MULT, CONNECTION_MAX_LIGHTS));
  }
}

void Connection::addLight(LPLight *light) {
  if (numLeds > 0) {
    LPBase::addLight(light);
  }
  else {
    outgoing(light);
  }
}

void Connection::emitLight(LPLight* light) {
    light->setOutPort(fromPort, from->id);
    addLight(light);
}

void Connection::update() {
  if (numLeds == 0) return;
  updateLightLists();
  for (uint16_t i=0; i<maxLights; i++) {
    updateLight(i);
  }
}

void Connection::updateLight(uint16_t i) {
  LPLight *light = lights[i];
  if (light != NULL) {
    light->resetPixels();
    Behaviour *behaviour = light->getBehaviour();
    if (light->shouldExpire() && (light->getSpeed() == 0 || (behaviour != NULL && behaviour->expireImmediately()))) {
      light->isExpired = true;
      removeLight(i);
    }
    else {
        // handle float inprecision
        float pos = round(light->position * 1000) / 1000.0;
        if (pos < numLeds) {
          pos = ofxeasing::map(light->position, 0, numLeds, 0, numLeds, light->getEasing());
          uint16_t ledIdx = light->outPort->direction ? ceil((float) numLeds - pos - 1.0) : floor(pos);
          light->pixel1 = getPixel(ledIdx);
        }
        else {
          outgoing(light, i);
        }
    }
  }
}

void Connection::outgoing(LPLight* light, int16_t i) {
  Intersection *neuron = light->outPort->direction ? from : to;
  Port *port = light->outPort->direction ? fromPort : toPort;
  light->position -= numLeds;
  light->setInPort(port);
  light->setOutPort(NULL);
  if (i >= 0) {
    removeLight(i);    
  }
  neuron->addLight(light);
}

uint16_t Connection::getFromPixel() {
  return fromPort->intersection->topPixel;
}

uint16_t Connection::getToPixel() {
  return toPort->intersection->topPixel;
}
