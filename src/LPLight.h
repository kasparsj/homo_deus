#pragma once

#include "Config.h"
#include "Port.h"
#include "ofxEasing.h"

class LightList;
class Model;
class Behaviour;
class LPOwner;

class LPLight
{

  public:

    uint16_t idx;
    uint8_t maxBri;
    LightList *list;
    Port *inPort = 0;
    Port *outPort = 0;
    Port *outPorts[OUT_PORTS_MEMORY] = {0}; // 4 bytes * 7
    int8_t outPortsInt[OUT_PORTS_MEMORY] = {-1};
    int16_t pixel1 = -1;
    int16_t life = INFINITE_LIFE;
    bool isExpired = false;
    float position;
    float bri = 1.f;
    uint8_t brightness = 0;
    LPOwner *owner = 0;

    LPLight(LightList *list, int16_t life, uint16_t idx = 0, uint8_t maxBri = 255) : list(list), life(life), idx(idx), maxBri(maxBri) {
        position = -1;
    }

    void setInPort(Port *port) {
      inPort = port;
    }
    Port* getOutPort(uint8_t intersectionId);
    void setOutPort(Port *port, int8_t intersectionId = -1);
    void resetPixels();
    void update();
    virtual void nextFrame();
    virtual bool shouldExpire();

    LPLight* getPrev();
    LPLight* getNext();
    virtual Model* getModel();
    virtual Behaviour* getBehaviour();
    virtual float getSpeed();
    virtual ofxeasing::function getEasing();
    virtual ColorRGB getColor();
    virtual void setColor(ColorRGB color) {}
    virtual uint8_t getBrightness();
    virtual ColorRGB getPixelColor();
    uint16_t* getPixels();

  private:
    void setPixel1();
    void setSegmentPixels();
    void setLinkPixels();

    static uint16_t pixels[CONNECTION_MAX_LEDS];
};
