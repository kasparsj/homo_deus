#include "Emitter.h"
#include "Model.h"
#include "HeptagonStar.h"
#include <Arduino.h>
#include "Palettes.h"

// #ifdef HD_OSC
// #include <ArduinoOSC.h>
// #endif

float Emitter::randomSpeed() {
  return EMITTER_MIN_SPEED + random(max(EMITTER_MAX_SPEED - EMITTER_MIN_SPEED, 0.f));
}

uint16_t Emitter::randomLife() {
  return EMITTER_MIN_LIFE + (uint16_t) random(max(EMITTER_MAX_LIFE - EMITTER_MIN_LIFE, 0));
}

uint8_t Emitter::randomModel() {
  return floor(random(NUM_MODELS));
}

uint16_t Emitter::randomLength() {
  return (uint16_t) (EMITTER_MIN_LENGTH + random(max(EMITTER_MAX_LENGTH - EMITTER_MIN_LENGTH, 0)));
}

float Emitter::randomBriThresh() {
  return EMITTER_MIN_BRI + random(max(EMITTER_MAX_BRI - EMITTER_MIN_BRI, 0.f));
}

uint16_t Emitter::randomNextEmit() {
  return EMITTER_MIN_NEXT + random(max(EMITTER_MAX_NEXT - EMITTER_MIN_NEXT, 0));
}

RgbColor Emitter::randomColor() {
  return RgbColor(random(255), random(255), random(255));
}

RgbColor Emitter::paletteColor(uint8_t color) {
  const CRGBPalette16& palette = gGradientPalettes[currentPalette];
  CRGB crgb = ColorFromPalette(palette, color);
  return RgbColor(crgb.r, crgb.g, crgb.b);  
}

void Emitter::emit(unsigned long ms) {
  if (enabled && nextEmit <= ms) {
    emitLinked();
    nextEmit = ms + randomNextEmit();
  }
}

void Emitter::emitLinked(uint8_t which, float speed, uint16_t life, uint16_t length, RgbColor color) {
  Model *model = &models[which];
  uint8_t k = model->getFreeEmitter();
  if (k == -1) {
    Serial.println("emitLinked failed, no free emitter.");
    return;
  }
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) {
      lightLists[i] = new LightList();
      lightLists[i]->setSpeed(speed);
      lightLists[i]->setModel(model);
      lightLists[i]->setLinked(true);
      lightLists[i]->setLife(life);
      //lightLists[i]->setupNoise(length, randomBriThresh());
      uint16_t trail = min((int) (speed * max(1, length / 2)), max(EMITTER_MAX_LENGTH, EMITTER_MAX_LIGHTS) - 1);
      lightLists[i]->setTrail(trail);
      lightLists[i]->setupFull(max(1, length - trail), color);
      #ifdef HD_DEBUG
      Serial.printf("emitLinked %d lights, bringing total to %d\n", lightLists[i]->numLights, totalLights + lightLists[i]->numLights);      
      #endif
      model->emit(k, lightLists[i]);
      totalLights += lightLists[i]->numLights;
      //#ifdef HD_OSC
      //OscWiFi.publish(SC_HOST, SC_PORT, "/linked", i);
      //#endif
      return;  
    }
  }
  #ifdef HD_DEBUG
  Serial.println("No free light lists");
  #endif
}

void Emitter::emitSplatter(float speed, uint16_t length, RgbColor color) {
  Model *model = &models[0];
  uint8_t k = model->getFreeEmitter();
  if (k == -1) {
    Serial.println("emitSplatter failed, no free emitter.");
    return;
  }
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) {
      lightLists[i] = new LightList();
      lightLists[i]->setSpeed(speed);
      lightLists[i]->setModel(model);
      lightLists[i]->setLinked(false);
      lightLists[i]->setLife(4);
      //uint16_t trail = min((int) (speed * max(1, length / 2)), max(EMITTER_MAX_LENGTH, EMITTER_MAX_LIGHTS) - 1);
      //lightLists[i]->setTrail(trail);
      //lightLists[i]->setupFull(max(1, length - trail), color);
      lightLists[i]->setupFull(length, color);
      #ifdef HD_DEBUG
      Serial.printf("emitSplatter %d lights, bringing total to %d\n", lightLists[i]->numLights, totalLights + lightLists[i]->numLights);      
      #endif
      model->emit(k, lightLists[i]);
      totalLights += lightLists[i]->numLights;
      //#ifdef HD_OSC
      //OscWiFi.publish(SC_HOST, SC_PORT, "/splatter", i);
      //#endif
      return;  
    }
  }
  #ifdef HD_DEBUG
  Serial.println("No free light lists");
  #endif
}

void Emitter::update() {
  memset(pixelValuesR, 0, sizeof(pixelValuesR));
  memset(pixelValuesG, 0, sizeof(pixelValuesG));
  memset(pixelValuesB, 0, sizeof(pixelValuesB));
  memset(pixelDiv, 0, sizeof(pixelDiv));
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
      allExpired = false;
      if (lightLists[i]->lights[j]->pixel1 >= 0) {
        RgbColor color = lightLists[i]->lights[j]->getColor(lightLists[i]->lights[j]->pixel1Bri);
        pixelValuesR[lightLists[i]->lights[j]->pixel1] += color.R;
        pixelValuesG[lightLists[i]->lights[j]->pixel1] += color.G;
        pixelValuesB[lightLists[i]->lights[j]->pixel1] += color.B;
        pixelDiv[lightLists[i]->lights[j]->pixel1]++;
      }
      // if (lightLists[i]->lights[j]->pixel2 >= 0) {
      //   RgbColor color = lightLists[i]->lights[j]->getColor(lightLists[i]->lights[j]->pixel2Bri);
      //   pixelValuesR[lightLists[i]->lights[j]->pixel2] += color.R;
      //   pixelValuesG[lightLists[i]->lights[j]->pixel2] += color.G;
      //   pixelValuesB[lightLists[i]->lights[j]->pixel2] += color.B;
      //   pixelDiv[lightLists[i]->lights[j]->pixel2]++;
      // }
      lightLists[i]->lights[j]->update();
    }
    if (allExpired) {
      totalLights -= lightLists[i]->numLights;
      delete lightLists[i];
      lightLists[i] = NULL;
      #ifdef HD_DEBUG
      Serial.print("Deleted lightList");
      Serial.println(i);
      #endif
    }
  }
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
    for (uint16_t j=0; j<lightLists[i]->numLights; j++) {
      if (lightLists[i]->lights[j] == NULL || lightLists[i]->lights[j]->isExpired) {
        continue;
      }
      lightLists[i]->lights[j]->life = 0;
    }
  }
}

#ifdef HD_TEST
void Emitter::debug() {
  for (uint8_t i=0; i<MAX_LIGHT_LISTS; i++) {
    if (lightLists[i] == NULL) continue;
    String lights = "";
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
    Serial.print("LightList");
    Serial.print(i);
    Serial.print("(");
    Serial.print(lightLists[i]->numLights);
    Serial.print(")");
    Serial.print(" active lights: ");
    Serial.println(lights);
  }
}
#endif