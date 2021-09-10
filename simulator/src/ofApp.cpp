#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(62);
    heptagon = new HeptagonStar(PIXEL_COUNT);
    emitter = new State(*heptagon);
    debugger = new LPDebugger(*heptagon);
    receiver.setup( OSC_PORT );
}

//--------------------------------------------------------------
void ofApp::update(){
    updateOsc();

    emitter->autoEmit(ofGetElapsedTimeMillis());
    heptagon->update();
    emitter->update();
}

void ofApp::updateOsc(){
    while( receiver.hasWaitingMessages() )
    {
        ofxOscMessage m;
        receiver.getNextMessage(m);
        if (m.getAddress() == "/emit") {
            onEmit(m);
        }
        else if (m.getAddress() == "/note_on"){
            onNoteOn(m);
        }
        else if (m.getAddress() == "/note_off"){
            onNoteOff(m);
        }
        else if (m.getAddress() == "/auto"){
            onAuto(m);
        }
        else if (m.getAddress() == "/command") {
            onCommand(m);
        }

    }
}

void ofApp::onCommand(const ofxOscMessage& m) {
  string command = m.getArgAsString(0);
  for (uint8_t i=0; i<command.length(); i++) {
    doCommand(command[i]);
  }
}

void ofApp::onEmit(const ofxOscMessage& m) {
  EmitParams params;
  parseParams(params, m);
  emitter->emit(params);
}

void ofApp::onNoteOn(const ofxOscMessage& m) {
  EmitParams params;
  params.life = INFINITE_LIFE;
  parseParams(params, m);
  emitter->emit(params);
}

void ofApp::onNoteOff(const ofxOscMessage& m) {
  if (m.getNumArgs() > 0) {
    uint16_t noteId = m.getArgAsInt(0);
    emitter->stopNote(noteId);
  }
  else {
    emitter->stopAll();
  }
}

void ofApp::onAuto(const ofxOscMessage &m) {
  emitter->autoEnabled = !emitter->autoEnabled;
}

void ofApp::parseParams(EmitParams &p, const ofxOscMessage &m) {
    for (uint8_t i=0; i<m.getNumArgs() / 2; i++) {
        EmitParam param = static_cast<EmitParam>(m.getArgAsInt(i*2));
        uint8_t j = i*2+1;
        switch (param) {
            case P_MODEL:
                p.model = m.getArgAsInt(j);
                break;
            case P_SPEED:
                p.speed = m.getArgAsFloat(j);
                break;
            case P_LENGTH:
                p.length = m.getArgAsInt(j);
                break;
            case P_TRAIL:
                p.trail = m.getArgAsInt(j);
                break;
            case P_FADE:
                p.fadeSpeed = m.getArgAsFloat(j);
                break;
            case P_ORDER:
                p.order = static_cast<ListOrder>(m.getArgAsInt(j));
                break;
            case P_HEAD:
                p.head = static_cast<ListHead>(m.getArgAsInt(j));
                break;
            case P_LINKED:
                p.linked = m.getArgAsInt(j) > 0;
                break;
            case P_FROM:
                p.from = m.getArgAsInt(j);
                break;
            case P_LIFE:
                p.life = m.getArgAsInt(j);
                break;
            case P_COLOR:
                p.color = m.getArgAsInt(j);
                break;
            case P_NOTE_ID:
                p.noteId = m.getArgAsInt(j);
                break;
            case P_BRIGHTNESS:
                p.brightness = m.getArgAsFloat(j);
                break;
            case P_BEHAVIOUR:
                p.behaviourFlags = m.getArgAsInt(j);
                break;
            case P_EMIT_GROUPS:
                p.emitGroups = m.getArgAsInt(j);
                break;
            case P_EMIT_OFFSET:
                p.emitOffset = m.getArgAsInt(j);
                break;
            case P_COLOR_CHANGE_GROUPS:
                p.colorChangeGroups = m.getArgAsInt(j);
                break;
        }
    }
}

void ofApp::doCommand(char command) {
  switch (command) {
    case 'e':
      emitter->autoEnabled = !emitter->autoEnabled;
      ofLog(OF_LOG_NOTICE, "AutoEmitter is %s", emitter->autoEnabled ? "enabled" : "disabled");
      break;
    case '.':
      emitter->stopAll();
      break;
    case '!':
      emitter->colorAll();
      break;
    case 's':
      emitter->splitAll();
      break;
    case 'f':
      showFps = !showFps;
      break;
    case 'h':
      showHeap = !showHeap;
      break;
    case 'a':
      showAll = !showAll;
      break;
    case 'i':
      showIntersections = !showIntersections;
      break;
    case 'c':
      showConnections = !showConnections;
      break;
    case 'm':
      showModel++;
      if (showModel >= heptagon->modelCount) {
          showModel = 0;
      }
      break;
    case 'p':
      showPixels = !showPixels;
      break;
    case '>':
      if (emitter->currentPalette < 32)
      emitter->currentPalette++;
      break;
    case '<':
      if (emitter->currentPalette > 0) {
        emitter->currentPalette--;
      }
      break;
    case 'l':
      ofLog(OF_LOG_NOTICE, "Total %d lights\n", emitter->totalLights);
      break;
    case 'L':
      emitter->debug();
      break;
    case 'C':
      debugger->dumpConnections();
      break;
    case 'I':
      debugger->dumpIntersections();
      break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
      emitter->emit(command - '1');
      break;
    case '7': {
      EmitParams p;
      p.model = M_STAR;
      p.colorChangeGroups |= GROUP1;
      emitter->emit(p);
      break;
    }
    case '+':
      emitter->emitSplatter(M_SPLATTER);
      break;
    case '*':
      emitter->emitRandom();
      break;
    case '/': { // emitSegment
      EmitParams params;
      params.behaviourFlags |= B_RENDER_SEGMENT;
      params.length = 1;
      emitter->emit(params);
      break;
    }
    case '-': { // emitBounce
      EmitParams params;
      params.model = M_STAR;
      params.behaviourFlags |= B_FORCE_BOUNCE;
      emitter->emit(params);
      break;
    }
    case '?': { // emitNoise
      EmitParams params;
      //params.order = LIST_NOISE;
      params.behaviourFlags |= B_BRI_CONST_NOISE;
      emitter->emit(params);
      break;
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw(){
    //ofClear(70);
    ofBackground(70);

    uint16_t size = 3;

    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        if (heptagon->interCount[i] > 0) {
            for (uint8_t j=0; j<heptagon->interCount[i]; j++) {
                ofSetColor(getColor(heptagon->inter[i][j]->topPixel));
                glm::vec2 point = intersectionPos(heptagon->inter[i][j], j);
                ofDrawCircle(point, size);
                if (showPixels) {
                    ofSetColor(255);
                    ofDrawBitmapString(ofToString(heptagon->inter[i][j]->topPixel), point + glm::vec2(20, 20));
                }
            }
        }
        if (heptagon->connCount[i] > 0) {
            for (uint8_t j=0; j<heptagon->connCount[i]; j++) {
                Connection* conn = heptagon->conn[i][j];
                glm::vec2 fromPos = intersectionPos(conn->from);
                glm::vec2 toPos = intersectionPos(conn->to);
                float dist = glm::distance(fromPos, toPos);
                for (uint8_t k=0; k<conn->numLeds; k++) {
                    glm::vec2 point = glm::mix(fromPos, toPos, (float) (k+1)/(conn->numLeds+1));
                    ofSetColor(getColor(heptagon->conn[i][j]->getPixel(k)));
                    ofDrawCircle(point, size);
                }
            }
        }
    }
    ofPopMatrix();

    if (showFps) {
        ofSetColor(255);
        ofDrawBitmapString(ofToString(ofGetFrameRate()), ofGetWidth() - 100, 20);
    }

}

glm::vec2 ofApp::intersectionPos(Intersection* intersection, int8_t j) {
    uint16_t groupDiam[MAX_GROUPS] = {0};
    groupDiam[0] = ofGetHeight();
    groupDiam[1] = ofGetHeight()*0.4;
    groupDiam[2] = ofGetHeight()*0.25;

    uint8_t i = log2(intersection->group);
    if (j<0) {
        for (j=0; j<heptagon->interCount[i]; j++) {
            if (heptagon->inter[i][j] == intersection) {
                break;
            }
        }
    }

    float offset = TWO_PI/4.f+(i%2*TWO_PI/7.f/2.f);
    float delta = 0;
    if (heptagon->interCount[i] == 14) {
        float k = showPixels ? 0.1 : 0.01;
        delta = (k/2)-((j+1)%2)*k;
        j = j / 2;
    }
    return pointOnEllipse(TWO_PI/7.f*j+offset+delta, groupDiam[i], groupDiam[i]);
}

glm::vec2 pointOnEllipse(float rad, float w, float h) {
  return glm::vec2(cos(rad) * w / 2.f, sin(rad) * h/2.f);
}

ofColor ofApp::getColor(uint16_t i) {
  ColorRGB pixel = emitter->getPixel(i);
  ofColor color = ofColor(pixel.R, pixel.G, pixel.B);
  if (showAll) {
    color.r = MAX_BRIGHTNESS / 2;
  }
  if (showConnections) {
    color.g = (debugger->isConnection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showIntersections) {
    color.b = (debugger->isIntersection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showModel > 0) {
    color.g = (debugger->isModelWeight(showModel - 1, i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
//  if (showPalette && i < 256) {
//    pixel = emitter->paletteColor(i);
//    color = ofColor(pixel.R, pixel.G, pixel.B);
//  }
  return color;
  //return colorGamma.Correct(color);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    doCommand(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
