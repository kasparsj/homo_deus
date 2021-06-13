#include "Connection.h"
#include "Intersection.h"
#include <Arduino.h>

void Connection::setup(Intersection *from, Intersection *to) {
  this->from = from;
  this->to = to;
  
  fromPort = new Port(this, from, false);
  toPort = new Port(this, to, true);    
    
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
      freeLight++;
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
  for (uint16_t i=0; i<freeLight; i++) {
    updateLight(i);
  }
  for (int16_t i=(freeRemove-1); i>=0; i--) {
    if (removeLights[i] >= 0) {
      removeLight(removeLights[i]);
      removeLights[i] = -1;
    }
  }
  freeLight -= freeRemove;
  freeRemove = 0;
}

void Connection::updateLight(uint16_t i) {
  Light *light = lights[i];
  if (light != NULL) {
    light->resetPixels();
    // hack: fix rounding error
    float pos = round(light->position * 1000) / 1000.0;
    if (light->speed == 0 && light->shouldExpire()) {
      light->isExpired = true;
      queueRemove(i);
    }
    else if (pos < numLeds) {      
      if (light->outPort == NULL) {
        #ifdef HD_DEBUG
        Serial.printf("light: %d outport NULL, conn: %d - %d\n", light->id, fromPixel, toPixel);
        #endif
      }
      uint16_t ledIdx = light->outPort->direction ? ceil((float) numLeds - pos - 1.0) : floor(pos);
      light->pixel1 = fromPixel + (ledIdx * (pixelDir ? 1 : -1));
      light->pixel1Bri = light->getBrightness();
    }
    else {
      outgoing(light);
      queueRemove(i);
    }
  }
}

void Connection::queueRemove(uint16_t i) {
  if (freeRemove < EMITTER_MAX_LIGHTS) {
    removeLights[freeRemove] = i;
    freeRemove++;    
  }
  else {
    Serial.println("Connection queueRemove no free slot");
  }
}

void Connection::removeLight(uint16_t i) {
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

void Connection::outgoing(Light *light) {
  Intersection *neuron = light->outPort->direction ? from : to;
  Port *port = light->outPort->direction ? fromPort : toPort;
  light->position -= numLeds;
  light->setInPort(port);
  light->setOutPort(NULL);
  //#ifdef HD_DEBUG
  //Serial.printf("Conn %d - %d outgoing %d\n", fromPixel, toPixel, light->id);
  //#endif
  neuron->addLight(light);
}
