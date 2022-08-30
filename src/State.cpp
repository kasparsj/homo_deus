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

uint8_t State::randomModel() {
  return floor(LP_RANDOM(object.modelCount));
}

uint16_t State::randomLength() {
  return (uint16_t) (EMITTER_MIN_LENGTH + LP_RANDOM(max(EMITTER_MAX_LENGTH - EMITTER_MIN_LENGTH, 0)));
}

uint16_t State::randomNextEmit() {
  return EMITTER_MIN_NEXT + LP_RANDOM(max(EMITTER_MAX_NEXT - EMITTER_MIN_NEXT, 0));
}

ColorRGB State::paletteColor(uint8_t color) {
  #ifdef ARDUINO
  const CRGBPalette16& palette = gGradientPalettes[currentPalette];
  CRGB crgb = ColorFromPalette(palette, color);
  return ColorRGB(crgb.r, crgb.g, crgb.b);  
  #else
  uint8_t g = (color >> 4) & 0xF;
  uint8_t b = (color & 0xF);
  return ColorRGB(255, g * 16, b * 16);
  #endif
}

void State::autoEmit(unsigned long ms) {
    if (autoEnabled && nextEmit <= ms) {
        EmitParams params(EmitParams::DEFAULT_MODEL, LPRandom::randomSpeed());
        emit(params);
        nextEmit = ms + randomNextEmit();
    }
}

int8_t State::emit(EmitParams &params) {
    uint8_t which = params.model >= 0 ? params.model : randomModel();
    Model *model = object.getModel(which);
    Behaviour *behaviour = new Behaviour(params);
    int8_t index = prepareFrom(params);
    if (index > -1) {
        lightLists[index]->model = model;
        lightLists[index]->behaviour = behaviour;
        LPOwner *emitter = getEmitter(model, behaviour, params);
        if (emitter == NULL) {
            LP_LOGF("emit failed, no free emitter.");
            return -1;
        }
        doEmit(emitter, lightLists[index], params);
        #ifdef LP_OSC_REPLY
        LP_OSC_REPLY(i);
        #endif
    }
}

int8_t State::prepareFrom(EmitParams &params) {
    uint16_t length = params.length > 0 ? params.length : randomLength();
    if (totalLights + length > MAX_TOTAL_LIGHTS) {
      LP_LOGF("emit failed, %d is over max %d lights\n", totalLights + length, MAX_TOTAL_LIGHTS);
      return -1;
    }
    if (params.noteId > 0) {
        int8_t noteIndex = findNote(params.noteId);
        if (noteIndex > -1) {
            totalLights -= lightLists[noteIndex]->numLights;
            totalLightLists--;
            lightLists[noteIndex]->setupWith(length, params);
            return noteIndex;
        }
    }
    for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
      if (lightLists[i] == NULL) {
        lightLists[i] = new LightList();
        lightLists[i]->setupWith(length, params);
        return i;
      }
    }
    LP_LOGF("emit failed: no free light lists");
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
                        setPixels(pixels[k], color, lightList);
                    }
                }
            }
            else if (light->pixel1 >= 0) {
                setPixels(light->pixel1, color, lightList);
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

void State::setPixels(uint16_t pixel, ColorRGB &color, const LightList* const lightList) {
    setPixel(pixel, color);
    if (lightList->behaviour != NULL && (lightList->behaviour->mirrorFlip() || lightList->behaviour->mirrorRotate())) {
        uint16_t* mirrorPixels = object.getMirroredPixels(pixel, lightList->behaviour->mirrorFlip() ? lightList->emitter : 0, lightList->behaviour->mirrorRotate());
        if (mirrorPixels != NULL) {
            // first value is length
            uint16_t numPixels = mirrorPixels[0];
            for (uint16_t k=1; k<numPixels+1; k++) {
                setPixel(mirrorPixels[k], color);
            }
        }
    }
}

void State::setPixel(uint16_t pixel, ColorRGB &color) {
    pixelValuesR[pixel] += color.R;
    pixelValuesG[pixel] += color.G;
    pixelValuesB[pixel] += color.B;
    pixelDiv[pixel]++;
}

void State::colorAll() {
    ColorRGB color;
    color.setRandom();
    for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
        if (lightLists[i] == NULL) continue;
        lightLists[i]->setColor(color);
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

int8_t State::findNote(uint8_t noteId) {
    for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
      if (lightLists[i] == NULL) continue;
      if (lightLists[i]->noteId == noteId) {
        return i;
      }
    }
    return -1;
}

void State::stopNote(uint8_t noteId) {
    int8_t index = findNote(noteId);
    if (index > -1) {
        lightLists[index]->setDuration(0);
    }
}

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
