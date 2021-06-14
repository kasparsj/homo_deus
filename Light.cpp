#include "Light.h"
#include "Config.h"
#include <Arduino.h>

Light::Light(float brightness, float speed, int16_t life, Model *model, Light *linkedPrev) {
  this->maxBri = brightness;
  this->speed = speed;
  this->life = life;
  this->model = model;
  this->linkedPrev = linkedPrev;
  this->color = RgbColor(255, 255, 255);
  position = -1;
}

void Light::update() {
  position += speed;
  bri += fade;
  age++;
}

void Light::resetPixels() {
  pixel1 = -1;
  // pixel2 = -1;
}

Port* Light::getOutPort(uint8_t intersectionId) {
  for (uint8_t i=0; i<OUT_PORTS_MEMORY; i++) {
    if (outPortsInt[i] == intersectionId) {
      return outPorts[i];
    }
  }
  return NULL;
}

void Light::setOutPort(Port *port, int8_t intersectionId) {
  outPort = port;
  if (intersectionId > -1) {
    for (uint8_t i=(OUT_PORTS_MEMORY-1); i>0; i--) {
      outPorts[i] = outPorts[i-1];
      outPortsInt[i] = outPortsInt[i-1];
    }
    outPorts[0] = port;
    outPortsInt[0] = intersectionId;
  }
}
