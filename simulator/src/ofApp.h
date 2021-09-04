#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "HeptagonStar.h"

#define PIXEL_COUNT1 524
#define PIXEL_COUNT2 395
#define PIXEL_COUNT (PIXEL_COUNT1 + PIXEL_COUNT2)
#define OSC_PORT 54321

class ofApp : public ofBaseApp{

public:
    HeptagonStar *heptagon;
    Emitter *emitter;

    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void updateOsc();
    void onCommand(const ofxOscMessage& m);
    void onEmit(const ofxOscMessage& m);
    void onNoteOn(const ofxOscMessage& m);
    void onNoteOff(const ofxOscMessage& m);
    void onAuto(const ofxOscMessage& m);
    void parseParams(EmitParams &p, const ofxOscMessage &m);
    void doCommand(char command);
    ofColor getColor(uint16_t i);

    ofxOscReceiver receiver;
    bool showIntersections = false;
    bool showConnections = false;
    bool showPalette = false;
    bool showAll = false;
    bool showFps = false;
    bool showHeap = false;

};
