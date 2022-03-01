#include <math.h>
#include "Connection.h"
#include "Intersection.h"
#include "LPObject.h"

Connection::Connection(Intersection *from, Intersection *to, uint8_t group) : LPOwner(group) {
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
  }
}

void Connection::add(LPLight *light) {
    if (numLeds > 0) {
        LPOwner::add(light);
    }
    else {
        outgoing(light);
    }
}

void Connection::emit(LPLight* light) {
    light->setOutPort(fromPort, from->id);
    add(light);
}

void Connection::update(LPLight* const light) {
    light->resetPixels();
    Behaviour *behaviour = light->getBehaviour();
    if (light->shouldExpire() && (light->getSpeed() == 0 || (behaviour != NULL && behaviour->expireImmediately()))) {
      light->isExpired = true;
      light->owner = NULL;
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
          outgoing(light);
        }
    }
}

void Connection::outgoing(LPLight* const light) const {
    const bool dir = light->outPort->direction;
    light->position -= numLeds;
    if (dir) {
        light->setInPort(fromPort);
    }
    else {
        light->setInPort(toPort);
    }
    light->setOutPort(NULL);
    LP_LOGF("con:out %d : %2.f : %d\n", light->idx, light->position, uxTaskGetStackHighWaterMark(NULL));
    if (dir) {
        from->add(light);
    }
    else {
        to->add(light);
    }
}

uint16_t Connection::getFromPixel() const {
  return fromPort->intersection->topPixel;
}

uint16_t Connection::getToPixel() const {
  return toPort->intersection->topPixel;
}
