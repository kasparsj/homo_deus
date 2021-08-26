#include "Light.h"
#include "LightList.h"
#include "Connection.h"
#include <Arduino.h>

Light::Light(float maxBri, float speed, int16_t life, LightList *parent, Light *linkedPrev) {
  this->maxBri = maxBri;
  this->speed = speed;
  this->life = life;
  this->parent = parent;
  this->linkedPrev = linkedPrev;
  this->color = RgbColor(255, 255, 255);
  position = -1;
}

float Light::getBrightness() {
  float value = fmod(bri, 2.f);
  value = (value > 1.f ? 2.f - value : value);
  value = (value - parent->fadeThresh) / (1.f - parent->fadeThresh);
  return value * maxBri;
}

void Light::update() {
  bri = parent->getBri(this);
  brightness = max(0.f, getBrightness());
  if (parent == NULL) {
    position += speed;
  }
  else {
    position = parent->getPosition(this);
  }
  age++;
}

bool Light::shouldExpire() {
  if (life == 0) {
    return false;
  }
  return age >= life && (parent->fadeSpeed == 0 || brightness == 0);
}

void Light::resetPixels() {
  pixel1 = -1;
  // pixel2 = -1;
}

uint16_t* Light::getPixels() {
  if (pixel1 >= 0) {
    Behaviour *behaviour = getBehaviour();
    if (behaviour != NULL && outPort != NULL) {
      switch (behaviour->renderBe) {
        case B_RENDER_SEGMENT: {
          uint8_t numPixels = outPort->connection->numLeds;
          uint16_t* pixels = new uint16_t[numPixels+3];
          pixels[0] = numPixels+2;
          pixels[1] = outPort->connection->getFromPixel();
          pixels[2] = outPort->connection->getToPixel();
          for (uint8_t i=3; i<numPixels+3; i++) {
            pixels[i] = outPort->connection->getPixel(i-3);
          }
          return pixels;
        }
      }
    }
    return new uint16_t[2]{1, pixel1};
  }
  return NULL;
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

Model* Light::getModel() {
  if (parent != NULL) {
    return parent->model;
  }
  return NULL;
}

Behaviour* Light::getBehaviour() {
  if (parent != NULL) {
    return parent->behaviour;
  }
  return NULL;
}