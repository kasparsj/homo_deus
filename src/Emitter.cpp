#include "Emitter.h"
#include "Model.h"
#include "Behaviour.h"
#include "HeptagonStar.h"

#ifdef ARDUINO
#include "Palettes.h"
#endif

#ifdef HD_OSC_REPLY
#include <ArduinoOSC.h>
#endif

float Emitter::randomSpeed() {
  return EMITTER_MIN_SPEED + LP_RANDOM(max(EMITTER_MAX_SPEED - EMITTER_MIN_SPEED, 0.f));
}

uint16_t Emitter::randomLife() {
  return EMITTER_MIN_LIFE + LP_RANDOM(max(EMITTER_MAX_LIFE - EMITTER_MIN_LIFE, 0));
}

uint8_t Emitter::randomModel() {
  return floor(LP_RANDOM(object.modelCount));
}

uint16_t Emitter::randomLength() {
  return (uint16_t) (EMITTER_MIN_LENGTH + LP_RANDOM(max(EMITTER_MAX_LENGTH - EMITTER_MIN_LENGTH, 0)));
}

float Emitter::randomBrightness() {
  return LP_RANDOM(1001) / 1000.f;
}

uint16_t Emitter::randomNextEmit() {
  return EMITTER_MIN_NEXT + LP_RANDOM(max(EMITTER_MAX_NEXT - EMITTER_MIN_NEXT, 0));
}

ColorRGB Emitter::randomColor() {
  return ColorRGB(LP_RANDOM(255), LP_RANDOM(255), LP_RANDOM(255));
}

ColorRGB Emitter::paletteColor(uint8_t color) {
  #ifdef ARDUINO
  const CRGBPalette16& palette = gGradientPalettes[currentPalette];
  CRGB crgb = ColorFromPalette(palette, color);
  return ColorRGB(crgb.r, crgb.g, crgb.b);  
  #else
  return randomColor();
  #endif
}

void Emitter::autoEmit(unsigned long ms) {
  if (autoEnabled && nextEmit <= ms) {
    emit();
    nextEmit = ms + randomNextEmit();
  }
}

int8_t Emitter::emit(EmitParams &params) {
  uint16_t length = params.length > 0 ? params.length : randomLength();
  if (totalLights + length > MAX_TOTAL_LIGHTS) {
    LP_LOGF("emit failed, %d is over max %d lights\n", totalLights + length, MAX_TOTAL_LIGHTS);
    return -1;
  }
  uint8_t which = params.model >= 0 ? params.model : randomModel();  
  Model *model = object.getModel(which);
  Behaviour *behaviour = new Behaviour(params);
  Intersection *intersection = NULL;
  uint8_t emitGroups = behaviour->emitGroups > 0 ? behaviour->emitGroups : model->emitGroups;
  int8_t from = params.from >= 0 ? params.from : -1;
  if (from >= 0) {
    intersection = object.getIntersection(from, emitGroups);
  }
  else {
    intersection = object.getFreeIntersection(emitGroups);
  }
  if (intersection == NULL) {
    LP_LOGF("emit failed, no free emitter.");
    return -1;
  }
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) {
      float speed = params.speed >= 0 ? params.speed : randomSpeed();
      uint16_t life = params.life >= 0 ? params.life : randomLife();
      ColorRGB color = params.color >= 0 ? paletteColor(params.color) : randomColor();
      float brightness = params.brightness >= 0 ? params.brightness : randomBrightness();
      lightLists[i] = new LightList();
      lightLists[i]->setOrder(params.order);
      lightLists[i]->setSpeed(speed);
      lightLists[i]->model = model;
      lightLists[i]->behaviour = behaviour;
      lightLists[i]->setLinked(params.linked);
      lightLists[i]->setLife(life); 
      lightLists[i]->setNoteId(params.noteId);
      uint16_t numTrail = params.getTrail(speed, length);
      lightLists[i]->setTrail(numTrail);
      uint16_t numFull = max(1, length - numTrail);
      #ifdef LP_DEBUG
      LP_LOGF("emitting %d %s lights (%d/%.1f/%d/%d/%.1f/%.3f), total: %d (%d)\n",
        numFull + numTrail, (params.linked ? "linked" : "random"), which, speed, length, life, brightness, params.fadeSpeed, totalLights + numFull + numTrail, totalLightLists + 1);      
      #endif
      lightLists[i]->setup(numFull, color, brightness, params.fadeSpeed, params.fadeThresh);
      doEmit(intersection, lightLists[i]);
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

void Emitter::doEmit(Intersection* from, LightList *lightList) {
  lightList->initEmit();
  from->add(lightList);
  totalLights += lightList->numLights;
  totalLightLists++;
}

void Emitter::update() {
  memset(pixelValuesR, 0, sizeof(uint16_t) * object.pixelCount);
  memset(pixelValuesG, 0, sizeof(uint16_t) * object.pixelCount);
  memset(pixelValuesB, 0, sizeof(uint16_t) * object.pixelCount);
  memset(pixelDiv, 0, object.pixelCount);
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    bool allExpired = true;
    for (uint16_t j=0; j<lightLists[i]->numLights; j++) {
      if (lightLists[i]->lights[j] == NULL) {
        continue;
      }
      if (lightLists[i]->lights[j]->isExpired) {
        if (((j+1) < lightLists[i]->numLights) && (lightLists[i]->lights[(j+1)] != NULL)) {
          lightLists[i]->lights[(j+1)]->linkedPrev = NULL;
        }
        delete lightLists[i]->lights[j];
        lightLists[i]->lights[j] = NULL;
        continue;
      }
      Light* light = lightLists[i]->lights[j];
      ColorRGB color = light->getColor();
      allExpired = false;
      // todo: perhaps it's OK to always retrieve pixels
      if (lightLists[i]->behaviour != NULL && lightLists[i]->behaviour->isRenderSegment()) {
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
      // if (light->pixel2 >= 0) {
      //   ColorRGB color = light->getColor(light->pixel2Bri);
      //   pixelValuesR[light->pixel2] += color.R;
      //   pixelValuesG[light->pixel2] += color.G;
      //   pixelValuesB[light->pixel2] += color.B;
      //   pixelDiv[light->pixel2]++;
      // }
      light->update();
    }
    if (allExpired) {
      totalLights -= lightLists[i]->numLights;
      totalLightLists--;
      delete lightLists[i];
      lightLists[i] = NULL;
      //#ifdef LP_DEBUG
      //Serial.printf("Deleted lightList %d\n", i);
      //#endif
    }
  }
}

ColorRGB Emitter::getPixel(uint16_t i) {
  ColorRGB color = ColorRGB(0, 0, 0);
  if (pixelDiv[i]) {
    color.R = min(pixelValuesR[i] / pixelDiv[i] / 255.f, 1.f) * MAX_BRIGHTNESS;
    color.G = min(pixelValuesG[i] / pixelDiv[i] / 255.f, 1.f) * MAX_BRIGHTNESS;
    color.B = min(pixelValuesB[i] / pixelDiv[i] / 255.f, 1.f) * MAX_BRIGHTNESS;
  }
  return color;
}

void Emitter::setPixel(uint16_t pixel, ColorRGB &color) {
  pixelValuesR[pixel] += color.R;
  pixelValuesG[pixel] += color.G;
  pixelValuesB[pixel] += color.B;
  pixelDiv[pixel]++;
}

void Emitter::colorAll() {
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    lightLists[i]->setColor(randomColor());
  }
}

void Emitter::splitAll() {
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    lightLists[i]->split();
  }
}

void Emitter::stopAll() {
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    lightLists[i]->setLife(-1);
  }
}

void Emitter::stopNote(uint8_t noteId) {
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    if (lightLists[i]->noteId == noteId) {
      lightLists[i]->setLife(-1);
    }
  }
}

#ifdef LP_TEST
void Emitter::debug() {
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
