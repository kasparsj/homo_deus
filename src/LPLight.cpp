#include "LPLight.h"
#include "LightList.h"
#include "Connection.h"
#include "ofxEasing.h"

uint16_t LPLight::pixels[CONNECTION_MAX_LEDS] = {0};

void LPLight::resetPixels() {
  pixel1 = -1;
  // pixel2 = -1;
}

Port* LPLight::getOutPort(uint8_t intersectionId) {
  for (uint8_t i=0; i<OUT_PORTS_MEMORY; i++) {
    if (outPortsInt[i] == intersectionId) {
      return outPorts[i];
    }
  }
  return NULL;
}

void LPLight::setOutPort(Port *port, int8_t intersectionId) {
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

void LPLight::update() {
    if (owner) {
        owner->updateLight(this);
    }
    brightness = getBrightness();
}

float LPLight::getBrightness() {
    float value = fmod(bri, 2.f);
    value = (value > 1.f ? 2.f - value : value);
    value = max(0.f, min(1.f, (value - list->fadeThresh) / (1.f - list->fadeThresh)));
    // handle float inprecision
    value = round(value * 10000) / 10000.f;
    if (value > 0) {
        value = ofxeasing::map(value, 0.f, 1.f, list->minBri, maxBri, list->fadeEase);
    }
    return value;
}

ColorRGB LPLight::getPixelColor() {
    if (brightness == 1.f) {
        return list->color;
    }
    return list->color.Dim(255 * brightness);
}

uint16_t* LPLight::getPixels() {
  if (pixel1 >= 0) {
    Behaviour *behaviour = getBehaviour();
    if (behaviour != NULL && behaviour->renderSegment() && outPort != NULL) {
      uint8_t numPixels = outPort->connection->numLeds;
      pixels[0] = numPixels+2;
      pixels[1] = outPort->connection->getFromPixel();
      pixels[2] = outPort->connection->getToPixel();
      for (uint8_t i=3; i<numPixels+3; i++) {
        pixels[i] = outPort->connection->getPixel(i-3);
      }
    }
    else {
      pixels[0] = 1;
      pixels[1] = pixel1;
    }
    return pixels;
  }
  return NULL;
}

void LPLight::nextFrame() {
  bri = list->getBri(this);
  position = list->getPosition(this);
}

bool LPLight::shouldExpire() {
  if (life == 0) {
    return false;
  }
  return list->age >= life && (list->fadeSpeed == 0 || brightness == 0);
}

LPLight* LPLight::getPrev() {
    return idx > 0 ? (*list)[idx - 1] : NULL;
}

float LPLight::getSpeed() {
    return list->speed;
}

ofxeasing::function LPLight::getEasing() {
    return list->ease;
}

ColorRGB LPLight::getColor() {
    return list->color;
}

Model* LPLight::getModel() {
    return list->model;
}

Behaviour* LPLight::getBehaviour() {
    return list->behaviour;
}
