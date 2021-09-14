#include "State.h"
#include "Model.h"
#include "Behaviour.h"
#include "HeptagonStar.h"
#include "LPRandom.h"

#ifdef ARDUINO
#include "Palettes.h"
#endif

#ifdef HD_OSC_REPLY
#include <ArduinoOSC.h>
#endif

float State::randomSpeed() {
  return EMITTER_MIN_SPEED + LP_RANDOM(max(EMITTER_MAX_SPEED - EMITTER_MIN_SPEED, 0.f));
}

uint8_t State::randomModel() {
  return floor(LP_RANDOM(object.modelCount));
}

uint16_t State::randomLength() {
  return (uint16_t) (EMITTER_MIN_LENGTH + LP_RANDOM(max(EMITTER_MAX_LENGTH - EMITTER_MIN_LENGTH, 0)));
}

uint8_t State::randomBrightness() {
  return LP_RANDOM(256);
}

uint16_t State::randomNextEmit() {
  return EMITTER_MIN_NEXT + LP_RANDOM(max(EMITTER_MAX_NEXT - EMITTER_MIN_NEXT, 0));
}

ColorRGB State::randomColor() {
  return ColorRGB(LP_RANDOM(255), LP_RANDOM(255), LP_RANDOM(255));
}

ColorRGB State::paletteColor(uint8_t color) {
  #ifdef ARDUINO
  const CRGBPalette16& palette = gGradientPalettes[currentPalette];
  CRGB crgb = ColorFromPalette(palette, color);
  return ColorRGB(crgb.r, crgb.g, crgb.b);  
  #else
  return randomColor();
  #endif
}

void State::autoEmit(unsigned long ms) {
  if (autoEnabled && nextEmit <= ms) {
    emit();
    nextEmit = ms + randomNextEmit();
  }
}

int8_t State::emit(EmitParams &params) {
  uint16_t length = params.length > 0 ? params.length : randomLength();
  if (totalLights + length > MAX_TOTAL_LIGHTS) {
    LP_LOGF("emit failed, %d is over max %d lights\n", totalLights + length, MAX_TOTAL_LIGHTS);
    return -1;
  }
  uint8_t which = params.model >= 0 ? params.model : randomModel();  
  Model *model = object.getModel(which);
  Behaviour *behaviour = new Behaviour(params);
  LPOwner *emitter = getEmitter(model, behaviour, params);
  if (emitter == NULL) {
    LP_LOGF("emit failed, no free emitter.");
    return -1;
  }
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) {
      float speed = params.speed >= 0 ? params.speed : randomSpeed();
      uint32_t duration = params.duration > 0 ? params.duration : LPRandom::randomDuration();
      ColorRGB color = params.color >= 0 ? paletteColor(params.color) : randomColor();
      uint8_t maxBri = params.maxBri > 0 ? params.maxBri : randomBrightness();
      uint16_t numTrail = params.speed == 0 ? params.trail : params.getSpeedTrail(speed, length);
      lightLists[i] = new LightList();
      lightLists[i]->model = model;
      lightLists[i]->behaviour = behaviour;
      lightLists[i]->order = params.order;
      lightLists[i]->head = params.head;
      lightLists[i]->color = color;
      lightLists[i]->linked = params.linked;
      lightLists[i]->minBri = params.minBri;
      lightLists[i]->setSpeed(speed, params.ease);
      lightLists[i]->setDuration(duration);
      lightLists[i]->setFade(params.fadeSpeed, params.fadeThresh, params.fadeEase);
      lightLists[i]->setLeadTrail(numTrail);
      lightLists[i]->noteId = params.noteId;
      uint16_t numFull = max(1, length - numTrail);
      #ifdef LP_DEBUG
      LP_LOGF("emitting %d %s lights (%d/%.1f/%d/%d/%d/%d), total: %d (%d)\n",
        numFull + numTrail, (params.linked ? "linked" : "random"), which, speed, length, duration, maxBri, params.fadeSpeed, totalLights + numFull + numTrail, totalLightLists + 1);
      #endif
      lightLists[i]->setup(numFull, maxBri);
      doEmit(emitter, lightLists[i], params);
      #ifdef LP_OSC_REPLY
      LP_OSC_REPLY(i);
      #endif
      return i;
    }
  }
  #ifdef LP_DEBUG
  LP_LOGF("emit failed: no free light lists");
  #endif
  return -1;
}

LPOwner* State::getEmitter(Model* model, Behaviour* behaviour, EmitParams& params) {
    int8_t from = params.from >= 0 ? params.from : -1;
    if (behaviour->emitFromConnection()) {
        from = from >= 0 ? from : LP_RANDOM(object.countConnections(params.emitGroups));
        return object.getConnection(from, params.emitGroups);
    }
    else {
        uint8_t emitGroups = params.emitGroups > 0 ? params.emitGroups : model->emitGroups;
        from = from >= 0 ? from : LP_RANDOM(object.countIntersections(emitGroups));
        return object.getIntersection(from, emitGroups);
    }
}

void State::doEmit(LPOwner* from, LightList *lightList, EmitParams& params) {
  lightList->initEmit(params.emitOffset);
  lightList->emitter = from;
  totalLights += lightList->numLights;
  totalLightLists++;
}

void State::update() {
  memset(pixelValuesR, 0, sizeof(uint16_t) * object.pixelCount);
  memset(pixelValuesG, 0, sizeof(uint16_t) * object.pixelCount);
  memset(pixelValuesB, 0, sizeof(uint16_t) * object.pixelCount);
  memset(pixelDiv, 0, object.pixelCount);
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    LightList* lightList = lightLists[i];
    if (lightList == NULL) continue;
    bool allExpired = lightList->update();
    if (allExpired) {
      totalLights -= lightList->numLights;
      totalLightLists--;
      delete lightLists[i];
      lightLists[i] = NULL;
    }
    else {
        for (uint16_t j=0; j<lightList->numLights; j++) {
            LPLight* light = lightList->lights[j];
            if (light == NULL) continue;
            ColorRGB color = light->getPixelColor();
            // todo: perhaps it's OK to always retrieve pixels
            if (lightList->behaviour != NULL && (lightList->behaviour->renderSegment() || lightList->behaviour->fillEase())) {
                uint16_t* pixels = light->getPixels();
                if (pixels != NULL) {
                    // first value is length
                    uint16_t numPixels = pixels[0];
                    for (uint16_t k=1; k<numPixels+1; k++) {
                        setPixel(pixels[k], color);
                    }
                }
            }
            else if (light->pixel1 >= 0) {
                setPixel(light->pixel1, color);
            }
            light->nextFrame();
        }
    }
  }
}

ColorRGB State::getPixel(uint16_t i, uint8_t maxBrightness) {
  ColorRGB color = ColorRGB(0, 0, 0);
  if (pixelDiv[i]) {
    color.R = min(pixelValuesR[i] / pixelDiv[i] / 255.f, 1.f) * maxBrightness;
    color.G = min(pixelValuesG[i] / pixelDiv[i] / 255.f, 1.f) * maxBrightness;
    color.B = min(pixelValuesB[i] / pixelDiv[i] / 255.f, 1.f) * maxBrightness;
  }
  return color;
}

void State::setPixel(uint16_t pixel, ColorRGB &color) {
  pixelValuesR[pixel] += color.R;
  pixelValuesG[pixel] += color.G;
  pixelValuesB[pixel] += color.B;
  pixelDiv[pixel]++;
}

void State::colorAll() {
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    lightLists[i]->setColor(randomColor());
  }
}

void State::splitAll() {
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    lightLists[i]->split();
  }
}

void State::stopAll() {
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    lightLists[i]->setDuration(0);
  }
}

void State::stopNote(uint8_t noteId) {
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    if (lightLists[i]->noteId == noteId) {
      lightLists[i]->setDuration(0);
    }
  }
}

#ifdef LP_DEBUG
void State::debug() {
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    LP_STRING lights = "";
    for (uint16_t j=0; j<lightLists[i]->numLights; j++) {
      if (lightLists[i]->lights[j] == NULL || lightLists[i]->lights[j]->isExpired) {
        continue;
      }
      else {
        lights += j;
        lights += "(";
        lights += lightLists[i]->lights[j]->pixel1;
        lights += ")";
        lights += ", ";
      }
    }
    LP_LOGF("LightList %d (%d) active lights:", i, lightLists[i]->numLights);
    LP_LOG(lights);
    LP_LOGLN("");
  }
}
#endif
