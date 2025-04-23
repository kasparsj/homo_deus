#include "ofApp.h"
#include "Globals.h"
#include "LightList.h"
#include "LPRandom.h"

//--------------------------------------------------------------
LPObject* ofApp::createObject(ObjectType type, uint16_t pixelCount) {
    currentObjectType = type;
    switch (type) {
        case OBJ_HEPTAGON_STAR:
            return new HeptagonStar(pixelCount);
        case OBJ_LINE:
            return new Line(pixelCount);
        case OBJ_CROSS:
            return new Cross(pixelCount);
        default:
            return new HeptagonStar(pixelCount);
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(62);
    // Default to HeptagonStar for now but can be changed via key commands
    object = createObject(OBJ_HEPTAGON_STAR, HEPTAGON_PIXEL_COUNT);
    state = new State(*object);
    debugger = new LPDebugger(*object);
    receiver.setup(OSC_PORT);
}

//--------------------------------------------------------------
void ofApp::update(){
    updateOsc();

    state->autoEmit(ofGetElapsedTimeMillis());
    gMillis = ofGetElapsedTimeMillis();
    state->update();
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
        else if (m.getAddress() == "/notes_set"){
            onNotesSet(m);
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
  doEmit(params);
}

void ofApp::onNoteOn(const ofxOscMessage& m) {
  EmitParams params;
  params.duration = INFINITE_DURATION;
  parseParams(params, m);
  doEmit(params);
}

void ofApp::onNoteOff(const ofxOscMessage& m) {
  if (m.getNumArgs() > 0) {
    uint16_t noteId = m.getArgAsInt(0);
    state->stopNote(noteId);
  }
  else {
    state->stopAll();
  }
}

void ofApp::onNotesSet(const ofxOscMessage& m) {
    EmitParams notesSet[MAX_NOTES_SET];
    for (uint8_t i=0; i<m.getNumArgs() / 3; i++) {
        uint16_t noteId = m.getArgAsInt(i*3);
        uint8_t k = 0;
        for (k; k<MAX_NOTES_SET; k++) {
          if (notesSet[k].noteId == 0 || notesSet[k].noteId == noteId) {
            notesSet[k].noteId = noteId;
            break;
          }
        }
        EmitParam param = static_cast<EmitParam>(m.getArgAsInt(i*3+1));
        uint8_t j = i*3+2;
        parseParam(notesSet[k], m, param, j);
    }
    for (uint8_t i=0; i<MAX_NOTES_SET; i++) {
        if (notesSet[i].noteId > 0) {
            doEmit(notesSet[i]);
        }
    }
}

void ofApp::onAuto(const ofxOscMessage &m) {
  state->autoEnabled = !state->autoEnabled;
}

void ofApp::parseParams(EmitParams &p, const ofxOscMessage &m) {
    for (uint8_t i=0; i<m.getNumArgs() / 2; i++) {
        EmitParam param = static_cast<EmitParam>(m.getArgAsInt(i*2));
        uint8_t j = i*2+1;
        parseParam(p, m, param, j);
    }
}

void ofApp::parseParam(EmitParams &p, const ofxOscMessage &m, EmitParam &param, uint8_t j) {
    switch (param) {
        case P_MODEL:
            p.model = m.getArgAsInt(j);
            break;
        case P_SPEED:
            p.speed = m.getArgAsFloat(j);
            break;
        case P_EASE:
            p.ease = m.getArgAsInt(j);
            break;
        case P_LENGTH:
            p.setLength(m.getArgAsInt(j));
            break;
        case P_TRAIL:
            p.trail = m.getArgAsInt(j);
            break;
        case P_FADE:
            p.fadeSpeed = m.getArgAsInt(j);
            break;
        case P_FADE_THRESH:
            p.fadeThresh = m.getArgAsInt(j);
            break;
        case P_FADE_EASE:
            p.fadeEase = m.getArgAsInt(j);
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
        case P_DURATION_MS:
            p.duration = m.getArgAsInt(j);
            break;
        case P_DURATION_FRAMES:
            p.duration = m.getArgAsInt(j) * EmitParams::frameMs();
            break;
        case P_COLOR:
            p.color = ColorRGB(m.getArgAsInt(j));
            break;
        case P_COLOR_INDEX: {
            int16_t index = m.getArgAsInt(j);
            if (index == RANDOM_COLOR) {
              p.color.setRandom();
            }
            else {
              p.color = state->paletteColor(index);
            }
            break;
        }
        case P_NOTE_ID:
            p.noteId = m.getArgAsInt(j);
            break;
        case P_MIN_BRI:
            p.minBri = m.getArgAsInt(j);
            break;
        case P_MAX_BRI:
            p.maxBri = m.getArgAsInt(j);
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

void ofApp::doCommand(char command) {
  switch (command) {
    case 'e':
      state->autoEnabled = !state->autoEnabled;
      ofLog(OF_LOG_NOTICE, "AutoEmitter is %s", state->autoEnabled ? "enabled" : "disabled");
      break;
    case '.':
      state->stopAll();
      break;
    case '!':
      state->colorAll();
      break;
    case 's':
      state->splitAll();
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
      if (showModel >= object->modelCount) {
          showModel = 0;
      }
      break;
    case 'o':
      {
          // Cycle through object types
          ObjectType newType;
          switch (currentObjectType) {
              case OBJ_HEPTAGON_STAR:
                  newType = OBJ_LINE;
                  break;
              case OBJ_LINE:
                  newType = OBJ_CROSS;
                  break;
              case OBJ_CROSS:
              default:
                  newType = OBJ_HEPTAGON_STAR;
                  break;
          }
          
          delete state;
          delete debugger;
          delete object;
          
          // Create appropriate object type with corresponding pixel count
          uint16_t pixelCount;
          switch (newType) {
              case OBJ_HEPTAGON_STAR:
                  pixelCount = HEPTAGON_PIXEL_COUNT;
                  break;
              case OBJ_LINE:
                  pixelCount = LINE_PIXEL_COUNT;
                  break;
              case OBJ_CROSS:
                  pixelCount = CROSS_PIXEL_COUNT;
                  break;
              default:
                  pixelCount = HEPTAGON_PIXEL_COUNT;
                  break;
          }
          
          object = createObject(newType, pixelCount);
          state = new State(*object);
          debugger = new LPDebugger(*object);
          const char* objName;
          switch (newType) {
              case OBJ_HEPTAGON_STAR: objName = "HeptagonStar"; break;
              case OBJ_LINE: objName = "Line"; break;
              case OBJ_CROSS: objName = "Cross"; break;
              default: objName = "Unknown"; break;
          }
          ofLog(OF_LOG_NOTICE, "Switched to %s", objName);
      }
      break;
    case 'p':
      showPixels = !showPixels;
      break;
    case '>':
      if (state->currentPalette < 32)
      state->currentPalette++;
      break;
    case '<':
      if (state->currentPalette > 0) {
        state->currentPalette--;
      }
      break;
    case 'l':
      ofLog(OF_LOG_NOTICE, "Total %d lights\n", state->totalLights);
      break;
    case 'L':
      state->debug();
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
    case '6': {
      EmitParams params(command - '1', LPRandom::randomSpeed());
      doEmit(params);
      break;
    }
    case '7': {
      EmitParams params(M_STAR);
      params.colorChangeGroups |= GROUP1;
      doEmit(params);
      break;
    }
    case '+': {
      EmitParams params(M_SPLATTER, LPRandom::randomSpeed());
      params.linked = false;
      params.duration = max(1, (int) (LPRandom::MAX_SPEED/params.speed) + 1) * EmitParams::frameMs();
      doEmit(params);
      break;
    }
    case '*': {
      // works reliably with M_STAR, other models might or might not work
      EmitParams params(M_STAR);
      params.speed = 0;
      params.fadeSpeed = 1;
      params.fadeThresh = 127;
      params.order = LIST_ORDER_RANDOM;
      params.behaviourFlags |= B_POS_CHANGE_FADE;
      doEmit(params);
      break;
    }
    case '/': { // emitSegment
      EmitParams params;
      params.behaviourFlags |= B_RENDER_SEGMENT;
      params.setLength(1);
      state->emit(params);
      break;
    }
    case '-': { // emitBounce
      EmitParams params(M_STAR);
      params.behaviourFlags |= B_FORCE_BOUNCE;
      state->emit(params);
      break;
    }
    case '?': { // emitNoise
      EmitParams params;
      //params.order = LIST_NOISE;
      params.behaviourFlags |= B_BRI_CONST_NOISE;
      state->emit(params);
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
        if (object->interCount[i] > 0) {
            for (uint8_t j=0; j<object->interCount[i]; j++) {
                ofSetColor(getColor(object->inter[i][j]->topPixel));
                glm::vec2 point = intersectionPos(object->inter[i][j], j);
                ofDrawCircle(point, size);
                if (showPixels) {
                    ofSetColor(255);
                    ofDrawBitmapString(ofToString(object->inter[i][j]->topPixel), point + glm::vec2(20, 20));
                }
            }
        }
        if (object->connCount[i] > 0) {
            for (uint8_t j=0; j<object->connCount[i]; j++) {
                Connection* conn = object->conn[i][j];
                glm::vec2 fromPos = intersectionPos(conn->from);
                glm::vec2 toPos = intersectionPos(conn->to);
                float dist = glm::distance(fromPos, toPos);
                for (uint16_t k=0; k<conn->numLeds; k++) {
                    glm::vec2 point = glm::mix(fromPos, toPos, (float) (k+1)/(conn->numLeds+1));
                    ofSetColor(getColor(object->conn[i][j]->getPixel(k)));
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

    if (lastList >= 0 && state->lightLists[lastList] != NULL) {
        ofSetColor(255);
        uint32_t life = gMillis < state->lightLists[lastList]->lifeMillis ? state->lightLists[lastList]->lifeMillis - gMillis : 0;
        ofDrawBitmapString("numLights: " + ofToString(state->lightLists[lastList]->numLights), ofGetWidth() - 200, 35);
        //ofDrawBitmapString("linked: " + (state->lightLists[lastList]->linked ? "linked" : "random"), ofGetWidth() - 200, 50);
        ofDrawBitmapString("model: " + ofToString((uint16_t) state->lightLists[lastList]->model->id), ofGetWidth() - 200, 65);
        ofDrawBitmapString("speed: " + ofToString(state->lightLists[lastList]->speed), ofGetWidth() - 200, 80);
        ofDrawBitmapString("length: " + ofToString(state->lightLists[lastList]->numLights), ofGetWidth() - 200, 95);
        ofDrawBitmapString("life: " + ofToString(life), ofGetWidth() - 200, 110);
        //ofDrawBitmapString("maxBri: " + ofToString(state->lightLists[lastList]->maxBri), ofGetWidth() - 200, 125);
        ofDrawBitmapString("fadeSpeed: " + ofToString(state->lightLists[lastList]->fadeSpeed), ofGetWidth() - 200, 140);
        ofDrawBitmapString("totalLights: " + ofToString(state->totalLights + state->lightLists[lastList]->numLights), ofGetWidth() - 200, 155);
        ofDrawBitmapString("totalLightLists: " + ofToString((uint16_t) state->totalLightLists), ofGetWidth() - 200, 170);
    }

}

glm::vec2 ofApp::intersectionPos(Intersection* intersection, int8_t j) {
    uint16_t groupDiam[MAX_GROUPS] = {0};
    
    if (currentObjectType == OBJ_HEPTAGON_STAR) {
        // HeptagonStar visualization
        groupDiam[0] = ofGetHeight()*0.9;
        groupDiam[1] = ofGetHeight()*0.4;
        groupDiam[2] = ofGetHeight()*0.25;
    } else if (currentObjectType == OBJ_LINE) {
        // Line visualization - use a linear layout
        groupDiam[0] = ofGetWidth()*0.8;
    } else if (currentObjectType == OBJ_CROSS) {
        // Cross visualization - use dimensions for the cross
        groupDiam[0] = ofGetHeight()*0.9;
    }

    uint8_t i = log2(intersection->group);
    if (j<0) {
        for (j=0; j<object->interCount[i]; j++) {
            if (object->inter[i][j] == intersection) {
                break;
            }
        }
    }

    if (currentObjectType == OBJ_HEPTAGON_STAR) {
        // Heptagon star positioning - circular arrangement
        float offset = TWO_PI/4.f+(i%2*TWO_PI/7.f/2.f);
        float delta = 0;
        if (object->interCount[i] == 14) {
            float k = showPixels ? 0.1 : 0.01;
            delta = (k/2)-((j+1)%2)*k;
            j = j / 2;
        }
        return pointOnEllipse(TWO_PI/7.f*j+offset+delta, groupDiam[i], groupDiam[i]);
    } else if (currentObjectType == OBJ_LINE) {
        // Line positioning - linear arrangement
        float x = -groupDiam[0]/2 + groupDiam[0] * j / (float)max(1, object->interCount[i]-1);
        return glm::vec2(x, 0);
    } else if (currentObjectType == OBJ_CROSS) {
        if (j < 4) {
            int k = j == 0 ? 1 : j == 1 ? 3 : j == 3 ? 0 : j;
            return pointOnEllipse(TWO_PI/4.f * k, groupDiam[i], groupDiam[i]);
        }
        return glm::vec2(0, 0);
    }
    
    // Default fallback
    return glm::vec2(0, 0);
}

glm::vec2 pointOnEllipse(float rad, float w, float h) {
  return glm::vec2(cos(rad) * w / 2.f, sin(rad) * h/2.f);
}

ofColor ofApp::getColor(uint16_t i) {
  ColorRGB pixel = state->getPixel(i, MAX_BRIGHTNESS);
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
//    pixel = state->paletteColor(i);
//    color = ofColor(pixel.R, pixel.G, pixel.B);
//  }
  return color;
  //return colorGamma.Correct(color);
}

void ofApp::doEmit(EmitParams &params) {
    lastList = state->emit(params);
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
