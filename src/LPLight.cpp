#include "LPLight.h"
#include "LightList.h"
#include "Connection.h"
#include "ofxEasing.h"
#include "Globals.h"

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
        owner->update(this);
    }
    brightness = getBrightness();
}

uint8_t LPLight::getBrightness() {
    uint16_t value = bri % 511;
    value = (value > 255 ? 511 - value : value);
    value = (float) (value - list->fadeThresh) / (255 - list->fadeThresh) * 511.f;
    if (value > 0) {
        return ofxeasing::map(value, 0, 511, list->minBri, maxBri, list->fadeEase);
    }
    return 0;
}

ColorRGB LPLight::getPixelColor() {
    if (brightness == 255) {
        return list->color;
    }
    return list->color.Dim(brightness);
}

uint16_t* LPLight::getPixels() {
  if (pixel1 >= 0) {
    Behaviour *behaviour = getBehaviour();
    if (behaviour != NULL && behaviour->renderSegment()) {
        setSegmentPixels();
    }
    else if (behaviour != NULL && behaviour->fillEase()) {
        setLinkPixels();
    }
    else {
        setPixel1();
    }
    return pixels;
  }
  return NULL;
}

void LPLight::setPixel1() {
    pixels[0] = 1;
    pixels[1] = pixel1;
}

void LPLight::setSegmentPixels() {
    if (outPort != NULL) {
        uint8_t numPixels = outPort->connection->numLeds;
        pixels[0] = numPixels+2;
        pixels[1] = outPort->connection->getFromPixel();
        pixels[2] = outPort->connection->getToPixel();
        for (uint8_t i=3; i<numPixels+3; i++) {
            pixels[i] = outPort->connection->getPixel(i-3);
        }
    }
    else {
        setPixel1();
    }
}

void LPLight::setLinkPixels() {
    LPLight* prev = getPrev();
    if (prev != NULL && owner == prev->owner) {
        uint8_t numPixels = abs(pixel1 - prev->pixel1);
        pixels[0] = numPixels;
        for (uint8_t i=1; i<numPixels+1; i++) {
            pixels[i] = pixel1 + (i-1) * (pixel1 < prev->pixel1 ? 1 : -1);
        }
    }
    else {
        setPixel1();
    }
}

void LPLight::nextFrame() {
  bri = list->getBri(this);
  position = list->getPosition(this);
}

bool LPLight::shouldExpire() {
  if (list->lifeMillis == INFINITE_DURATION) {
    return false;
  }
  return gMillis >= (list->lifeMillis + lifeMillis) && (list->fadeSpeed == 0 || brightness == 0);
}

LPLight* LPLight::getPrev() {
    return idx > 0 ? (*list)[idx - 1] : NULL;
}

LPLight* LPLight::getNext() {
    return ((idx+1) < list->numLights) ? (*list)[idx+1] : NULL;
}

float LPLight::getSpeed() {
    return list->speed;
}

ofxeasing::function LPLight::getEasing() {
    return list->ease;
}

uint32_t LPLight::getLife() {
    return list->lifeMillis;
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
