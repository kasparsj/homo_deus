#include "LightList.h"
#include "Light.h"
#include "Model.h"
#include "Globals.h"

uint16_t LightList::nextId = 0;

void LightList::init(uint16_t numLights) {
  this->numLights = numLights;
  lights = new LPLight*[numLights]();
}

void LightList::setup(uint16_t numLights, float brightness) {
    init(lead + numLights + trail);
    for (uint16_t i=0; i<this->numLights; i++) {
        createLight(i, brightness);
    }
}

LPLight* LightList::createLight(uint16_t i, float brightness) {
    float mult = getBriMult(i);
    LPLight *light;
    if (behaviour != NULL && behaviour->colorChangeGroups > 0) {
        light = new Light(this, speed, life, linked ? i : 0, brightness * mult);
    }
    else {
        light = new LPLight(this, life, linked ? i : 0, brightness * mult);
    }
    (*this)[i] = light;
    return light;
}

void LightList::setLife(int16_t numFrames) {
  this->life = numFrames;
  for (uint16_t i=0; i<numLights; i++) {
    if ((*this)[i] == 0) continue;
    (*this)[i]->life = numFrames;
  }
}

void LightList::setColor(ColorRGB color) {
    this->color = color;
    if (numLights > 0) {
        for (uint16_t i=0; i<numLights; i++) {
            if ((*this)[i] == 0) continue;
            ((*this)[i])->setColor(color);
        }
    }
}

void LightList::setLeadTrail(uint16_t trail) {
    if (head == LIST_HEAD_FRONT) {
        if (trail > 0) {
            this->lead = 1;
            trail -= 1;
        }
        this->trail = trail;
    }
    else if (head == LIST_HEAD_BACK) {
        this->lead = trail;
    }
    else {
        this->lead = (uint16_t) trail / 2;
        this->trail = (uint16_t) ceil(trail / 2.f);
    }
}

void LightList::initEmit(uint8_t posOffset) {
    for (uint16_t i=0; i<numLights; i++) {
        LPLight *light = (*this)[i];
        initPosition(i, light);
        light->position += posOffset;
        initBri(i, light);
        initLife(i, light);
    }
}

float LightList::getPosition(LPLight *light) {
  if (behaviour != NULL) {
    return behaviour->getPosition(light);
  }
  return light->position + light->getSpeed();
}

void LightList::initPosition(uint16_t i, LPLight* light) {
  float position = (speed != 0 ? i * -1.f : numLights - 1 - i * 1.f);
  if (order == LIST_ORDER_RANDOM) {
    position = LP_RANDOM(model->getMaxLength());
  }
  light->position = position;
}

void LightList::initBri(uint16_t i, LPLight* light) {
  switch (order) {
    case LIST_ORDER_RANDOM:
      if (fadeThresh > 0) {
        light->bri = (LP_RANDOM(fadeThresh * 3000.f) - (fadeThresh * 2000.f)) / 1000.f;
      }
      break;
    case LIST_ORDER_NOISE:
      light->bri = gPerlinNoise.GetValue(id * 10, i * 100);
      break;
  }
}

float LightList::getBri(LPLight *light) {
  if (behaviour != NULL) {
    return behaviour->getBri(light);
  }
  return light->bri + fadeSpeed;
}

void LightList::initLife(uint16_t i, LPLight* light) {
  int16_t life = light->life;    
  if (life > 0 && order == LIST_ORDER_SEQUENTIAL) {
    life += ceil(1.0 / light->getSpeed() * i);
  }
  light->life = life;
}

bool LightList::update() {
    if (numEmitted < numLights) {
        uint8_t batchSize = numLights - numEmitted;
        uint8_t j = numEmitted;
        for (uint8_t i=0; i<batchSize; i++) {
            LPLight *light = (*this)[i+j];
            if (light->position < 0) {
                break;
            }
            numEmitted++;
            emitter->emit(light);
        }
    }
    bool allExpired = true;
    for (uint16_t j=0; j<numLights; j++) {
        LPLight* light = lights[j];
        if (light == NULL) continue;
        if (light->isExpired) {
          LPLight *next = light->getNext();
          if (next != NULL) {
            next->idx = 0;
          }
          delete lights[j];
          lights[j] = NULL;
          continue;
        }
        allExpired = false;
        light->update();
    }
    return allExpired;
}

void LightList::nextFrame() {
    age++;
}

void LightList::split() {
    numSplits++;
    if (numSplits < numLights) {
    for (uint8_t i=0; i<numSplits; i++) {
      uint16_t split = (i+1)*(numLights/(numSplits+1));
      if ((*this)[split] == 0) continue;
      (*this)[split]->idx = 0;
    }
    // todo: modify trail
  }
}
