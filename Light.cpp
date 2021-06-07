#include "Light.h"
#include "Config.h"
#include <Arduino.h>

const float Light::SPEED = 1.0;
const int Light::LIFE = 60;

Light::Light(float brightness, float speed, int life, Model *model, Light *linkedPrev) {
  this->brightness = brightness;
  this->speed = speed;
  this->life = life;
  this->model = model;
  this->linkedPrev = linkedPrev;
  position = -1;
}

void Light::update() {
  position += speed;
  age++;
}

void Light::resetPixels() {
  pixel1 = -1;
  pixel1Bri = 0;
  pixel2 = -1;
  pixel2Bri = 0;
}

void Light::setOutPort(Port *port, int intersectionId) {
  outPort = port;
  if (intersectionId > -1) {
    outPorts[intersectionId] = port;
  }
}
