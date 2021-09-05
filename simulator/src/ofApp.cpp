#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    heptagon = new HeptagonStar(PIXEL_COUNT);
    emitter = new Emitter(*heptagon);
    receiver.setup( OSC_PORT );
}

//--------------------------------------------------------------
void ofApp::update(){
    updateOsc();
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
      case P_FADE:
        p.fadeSpeed = m.getArgAsFloat(j);
        break;
      case P_ORDER:
        p.order = static_cast<ListOrder>(m.getArgAsInt(j));
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
    #ifdef LP_TEST
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
    case 'p':
      showPalette = !showPalette;
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
      heptagon->dumpConnections();
      break;
    case 'I':
      heptagon->dumpIntersections();
      break;
    #endif
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
      emitter->emitSplatter();
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
      params.behaviourFlags |= B_RND_PORT_BOUNCE;
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

    for (uint16_t i=0; i<PIXEL_COUNT; i++) {
        //strip.SetPixelColor(i, getColor(i));
    }

}

ofColor ofApp::getColor(uint16_t i) {
  ColorRGB pixel = emitter->getPixel(i);
  ofColor color = ofColor(pixel.R, pixel.G, pixel.B);
  #ifdef LP_TEST
  if (showAll) {
    color.r = MAX_BRIGHTNESS / 2;
  }
  if (showConnections) {
    color.g = (heptagon->isConnection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showIntersections) {
    color.b = (heptagon->isIntersection(i) ? 1.f : 0.f) * MAX_BRIGHTNESS;
  }
  if (showPalette && i < 256) {
    pixel = emitter->paletteColor(i);
    color = ofColor(pixel.R, pixel.G, pixel.B);
  }
  #endif
  return color;
  //return colorGamma.Correct(color);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
