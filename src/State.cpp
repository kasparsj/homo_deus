#include "State.h"
#include "Model.h"
#include "Behaviour.h"
#include "HeptagonStar.h"
#include "LightList.h"

#ifdef ARDUINO
#include "Palettes.h"
#endif

#ifdef HD_OSC_REPLY
#include <ArduinoOSC.h>
#endif

uint8_t State::randomModel() {
  return floor(LP_RANDOM(object.modelCount));
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
        EmitParams params(EmitParams::DEFAULT_MODEL, RANDOM_SPEED);
        emit(params);
        nextEmit = ms + randomNextEmit();
    }
}

int8_t State::emit(EmitParams &params) {
    uint8_t which = params.model >= 0 ? params.model : randomModel();
    Model *model = object.getModel(which);
    int8_t index = getOrCreateList(params);
    if (index > -1) {
        lightLists[index]->model = model;
        LPOwner *emitter = getEmitter(model, lightLists[index]->behaviour, params);
        if (emitter == NULL) {
            LP_LOGF("emit failed, no free emitter %d %d.\n", params.getEmit(), params.getEmitGroups(model->emitGroups));
            return -1;
        }
        doEmit(emitter, lightLists[index], params);
        #ifdef LP_OSC_REPLY
        LP_OSC_REPLY(i);
        #endif
    }
    return index;
}

int8_t State::getOrCreateList(EmitParams &params) {
    if (params.noteId > 0) {
        int8_t listIndex = findList(params.noteId);
        if (listIndex > -1) {
            uint16_t prevLights = lightLists[listIndex]->numLights;
            return setupListFrom(listIndex, params);
        }
    }
    for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
        if (lightLists[i] == NULL) {
            return setupListFrom(i, params);
        }
    }
    LP_LOGF("emit failed: no free light lists (%d)\n", MAX_LIGHT_LISTS);
    return -1;
}

int8_t State::setupListFrom(uint8_t i, EmitParams &params) {
    LightList* lightList = lightLists[i];
    uint16_t oldLen = (lightList != NULL ? lightList->length : 0);
    uint16_t oldLights = (lightList != NULL ? lightList->numLights : 0);
    uint16_t newLen = params.getLength();
    Behaviour* newBehaviour = new Behaviour(params);
    if (oldLen > 0 && newBehaviour->smoothChanges()) {
        newLen = oldLen + (int) round((float)(newLen - oldLen) * 0.1f);
    }
    if (totalLights - oldLights + newLen > MAX_TOTAL_LIGHTS) {
        // todo: if it's a change, maybe emit max possible?
        LP_LOGF("emit failed, %d is over max %d lights\n", totalLights + newLen, MAX_TOTAL_LIGHTS);
        return -1;
    }
    if (lightList == NULL) {
        lightList = new LightList();
        lightLists[i] = lightList;
    }
    lightList->length = newLen;
    if (lightList->behaviour != NULL) {
        delete lightList->behaviour;
    }
    lightList->behaviour = newBehaviour;
    lightList->setupFrom(params);
    if (oldLights > 0) {
        totalLights -= oldLights;
        totalLightLists--;
    }
    return i;
}

LPOwner* State::getEmitter(Model* model, Behaviour* behaviour, EmitParams& params) {
    int8_t from = params.getEmit();
    if (behaviour->emitFromConnection()) {
        from = from >= 0 ? from : LP_RANDOM(object.countConnections(params.emitGroups));
        return object.getConnection(from, params.emitGroups);
    }
    else {
        uint8_t emitGroups = params.getEmitGroups(model->emitGroups);
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

int8_t State::findList(uint8_t noteId) const {
    for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
      if (lightLists[i] == NULL) continue;
      if (lightLists[i]->noteId == noteId) {
        return i;
      }
    }
    return -1;
}

void State::stopNote(uint8_t noteId) {
    int8_t index = findList(noteId);
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
