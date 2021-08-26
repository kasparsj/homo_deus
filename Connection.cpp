#include "Connection.h"
#include "Intersection.h"
#include <Arduino.h>

void Connection::setup(Intersection *from, Intersection *to, uint8_t group) {
  this->from = from;
  this->to = to;
  this->group = group;
  
  fromPort = new Port(this, from, false, group);
  toPort = new Port(this, to, true, group);    
    
  pixelDir = to->topPixel > from->topPixel;
  fromPixel = from->topPixel + (pixelDir ? 1 : -1);
  toPixel = to->topPixel - (pixelDir ? 1 : -1);
  uint16_t diff = abs(fromPixel - toPixel);
  uint16_t leds = diff > 4 && diff < (PIXEL_COUNT - 4) ? diff + 1 : 0;
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
    maxLights = min(numLeds * CONNECTION_MAX_MULT, CONNECTION_MAX_LIGHTS);
    lights = new Light*[maxLights]();
    for (uint16_t i=0; i<maxLights; i++) {
      lights[i] = NULL;
    }    
  }
}

void Connection::addLight(Light *light) {
  if (numLeds > 0) {
    if (freeLight < maxLights) {
      lights[freeLight] = light;
      uint8_t i;
      for (i=freeLight+1; i<maxLights; i++) {
        if (lights[i] == NULL) {
          break;
        }
      }
      freeLight = i;
    }
    else {
      Serial.printf("Connection addLight no free slot: %d - %d\n", fromPixel, toPixel);
    }
  }
  else {
    outgoing(light);
  }
}

void Connection::update() {
  if (numLeds == 0) return;
  for (uint16_t i=0; i<maxLights; i++) {
    updateLight(i);
  }
}

void Connection::updateLight(uint16_t i) {
  Light *light = lights[i];
  if (light != NULL) {
    light->resetPixels();
    // hack: fix rounding error
    float pos = round(light->position * 1000) / 1000.0;
    if (light->speed == 0 && light->shouldExpire()) {
      light->isExpired = true;
      removeLight(i);
    }
    else if (pos < numLeds) {      
      if (light->outPort == NULL) {
        #ifdef HD_DEBUG
        Serial.printf("light: %d outport NULL, conn: %d - %d\n", light->id, fromPixel, toPixel);
        #endif
      }
      uint16_t ledIdx = light->outPort->direction ? ceil((float) numLeds - pos - 1.0) : floor(pos);
      light->pixel1 = getPixel(ledIdx);
    }
    else {
      outgoing(light, i);
    }
  }
}

void Connection::removeLight(uint16_t i) {
  lights[i] = NULL;
  if (i < freeLight) {
    freeLight = i;
  }  
}

void Connection::outgoing(Light* light, int16_t i) {
  Intersection *neuron = light->outPort->direction ? from : to;
  Port *port = light->outPort->direction ? fromPort : toPort;
  light->position -= numLeds;
  light->setInPort(port);
  light->setOutPort(NULL);
  //#ifdef HD_DEBUG
  //Serial.printf("Conn %d - %d outgoing %d\n", fromPixel, toPixel, light->id);
  //#endif
  if (i >= 0) {
    removeLight(i);    
  }
  neuron->addLight(light);
}
