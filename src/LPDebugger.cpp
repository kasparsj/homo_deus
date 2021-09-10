#include "LPDebugger.h"

LPDebugger::LPDebugger(LPObject &object) : object(object) {
    interPixels = new bool[object.pixelCount]{false};
    connPixels = new bool[object.pixelCount]{false};
    Port* ports[255] = {};
    for (uint8_t i=0; i<MAX_GROUPS; i++) {
        for (uint j=0; j<object.interCount[i]; j++) {
            Intersection* intersection = object.inter[i][j];
            interPixels[intersection->topPixel] = true;
            if (intersection->bottomPixel >= 0) {
                interPixels[intersection->bottomPixel] = true;
            }
            for (uint8_t k=0; k<intersection->numPorts; k++) {
                Port *port = intersection->ports[k];
                ports[port->id] = port;
            }
        }
        for (uint j=0; j<object.connCount[i]; j++) {
            Connection* connection = object.conn[i][j];
            connPixels[connection->fromPixel] = true;
            connPixels[connection->toPixel] = true;
        }
    }
    weightPixels = new bool*[object.modelCount];
    for (uint8_t i=0; i<object.modelCount; i++) {
        weightPixels[i] = new bool[object.pixelCount]{false};
        Model* model = object.models[i];
        for (uint8_t j=0; j<model->weights->size(); j++) {
            uint8_t portId = model->weights->keyAt(j);
            Port* port = ports[portId];
            //Weight* weight = model->weights->valueAt(j);
            weightPixels[model->id][port->intersection->topPixel] = true;
        }
    }
}

LPDebugger::~LPDebugger() {
    delete[] interPixels;
    delete[] connPixels;
    for (uint8_t i=0; i<object.modelCount; i++) {
        delete[] weightPixels[i];
        weightPixels[i] = NULL;
    }
    delete[] weightPixels;
}

bool LPDebugger::isModelWeight(uint8_t id, uint16_t i) {
  return weightPixels[id][i];
}

bool LPDebugger::isIntersection(uint16_t i) {
  return interPixels[i];
}

bool LPDebugger::isConnection(uint16_t i) {
  return connPixels[i];
}

void LPDebugger::dumpConnections() {
  LP_LOGLN("--- CONNECTIONS ---");
  for (uint8_t i=0; i<MAX_GROUPS; i++) {
      for (uint8_t j=0; j<object.connCount[i]; j++) {
        LP_LOGF("Connection%d %d - %d: %d / %d\n", i, object.conn[i][j]->fromPixel, object.conn[i][j]->toPixel, object.conn[i][j]->freeLight, object.conn[i][j]->maxLights);
      }
  }
}

void LPDebugger::dumpIntersections() {
  LP_LOGLN("--- INTERSECTIONS ---");
  for (uint8_t i=0; i<MAX_GROUPS; i++) {
      for (uint8_t j=0; j<object.interCount[i]; i++) {
        LP_LOGF("Intersection%d %d: %d\n", i, object.inter[i][j]->id, object.inter[i][j]->freeLight);
      }
  }
}
