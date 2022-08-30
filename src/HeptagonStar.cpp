#include "HeptagonStar.h"

void HeptagonStar::setup() {
    initInter(14, 7, 7);
    initConn(7, 14, 14, 7);
    initModels(7);
    
    Model::maxWeights = 7 * 4 * 2;

    // todo: set maxLength for all models
    addModel(new Model(M_DEFAULT, 10, GROUP1));
    addModel(new Model(M_STAR, 0, GROUP1));
    addModel(new Model(M_OUTER_SUN, 10, GROUP1, 602));
    addModel(new Model(M_INNER_TRIS, 0, GROUP2));
    addModel(new Model(M_SMALL_STAR, 0, GROUP2));
    addModel(new Model(M_INNER_SUN, 0, GROUP2));
    addModel(new Model(M_SPLATTER, 10, GROUP2 | GROUP3));

    // outer (inter[0])
    addBridge(918, 1, GROUP1); // bottom
    addBridge(653, 654, GROUP1); // bottom left
    addBridge(389, 390, GROUP1); // left
    addBridge(125, 126, GROUP1); // top left
    addBridge(789, 791, GROUP1); // top right
    addBridge(522, 525, GROUP1); // right
    addBridge(260, 263, GROUP1); // bottom right

    // middle (inter[1])
    addIntersection(new Intersection(4, 612, 42, GROUP2));  // bottom left
    addIntersection(new Intersection(4, 696, 347, GROUP2)); // left
    addIntersection(new Intersection(4, 434, 81, GROUP2));  // upper left
    addIntersection(new Intersection(4, 742, 172, GROUP2));  // top
    addIntersection(new Intersection(4, 836, 478, GROUP2));  // upper right
    addIntersection(new Intersection(4, 568, 218, GROUP2));  // right
    addIntersection(new Intersection(4, 876, 304, GROUP2));  // bottom right

    for (uint8_t i=0; i<7; i++) {
      Intersection *outerNeuron1 = inter[0][i*2+1];
      Intersection *outerNeuron2 = inter[0][((i+1)*2) % 14];
      addConnection(new Connection(outerNeuron1, inter[1][i], GROUP2));
      addConnection(new Connection(outerNeuron2, inter[1][i], GROUP2));
    }

    // inner (inter[2)
    addIntersection(new Intersection(4, 597, 320, GROUP3)); //bottom
    addIntersection(new Intersection(4, 55, 334, GROUP3)); // bottom left
    addIntersection(new Intersection(4, 708, 69, GROUP3)); // left
    addIntersection(new Intersection(4, 722, 448, GROUP3)); // top left
    addIntersection(new Intersection(4, 462, 191, GROUP3)); // top right
    addIntersection(new Intersection(4, 848, 206, GROUP3)); // right
    addIntersection(new Intersection(4, 863, 582, GROUP3)); // bottom right

  for (uint8_t i=0; i<7; i++) {
    Intersection *middleNeuron1 = inter[1][i];
    Intersection *middleNeuron2 = inter[1][(i-1+7) % 7];
    Connection *middleConnection1 = addConnection(new Connection(middleNeuron1, inter[2][i], GROUP3));
    Connection *middleConnection2 = addConnection(new Connection(middleNeuron2, inter[2][i], GROUP3));
    models[M_STAR]->put(middleConnection1->fromPort, middleNeuron1->ports[1], 10);
    models[M_STAR]->put(middleConnection2->fromPort, middleNeuron2->ports[0], 10);
    models[M_OUTER_SUN]->put(middleConnection1, 0);
    models[M_OUTER_SUN]->put(middleConnection2, 0);
    models[M_INNER_TRIS]->put(middleConnection1, 10);
    models[M_INNER_TRIS]->put(middleConnection2, 10);
    models[M_SMALL_STAR]->put(middleConnection1->fromPort, 10);
    models[M_SMALL_STAR]->put(middleConnection2->fromPort, 10);
    models[M_INNER_SUN]->put(middleConnection1->fromPort, 10);
    models[M_INNER_SUN]->put(middleConnection2->fromPort, 10);
    models[M_INNER_SUN]->put(middleConnection1->toPort, 10);
    models[M_INNER_SUN]->put(middleConnection2->toPort, 10);
  }
  
  for (uint8_t i=0; i<7; i++) {
    Intersection *innerNeuron1 = inter[2][i];
    Intersection *innerNeuron2 = inter[2][(i+1) % 7];
    Connection *innerConnection = addConnection(new Connection(innerNeuron1, innerNeuron2, GROUP4));
    models[M_STAR]->put(innerConnection->fromPort, innerNeuron1->ports[1], 10);
    models[M_STAR]->put(innerConnection->toPort, innerNeuron2->ports[0], 10);
    models[M_SMALL_STAR]->put(innerConnection->fromPort, innerNeuron1->ports[1], 10);
    models[M_SMALL_STAR]->put(innerConnection->toPort, innerNeuron2->ports[0], 10);
    models[M_OUTER_SUN]->put(innerConnection, 0);
    models[M_INNER_TRIS]->put(innerConnection, 10);
  }
}

uint16_t* HeptagonStar::getMirroredPixels(uint16_t pixel, LPOwner* mirrorFlipEmitter, bool mirrorRotate) {
    uint8_t pathIndex = getStarSegmentIndex(pixel);
    float progress = getProgressOnStarSegment(pathIndex, pixel);
    uint8_t i = 1;
    mirrorPixels[0] = 0;
    if (mirrorFlipEmitter != NULL) {
        try {
            uint8_t emitterIndex = static_cast<Intersection*>(mirrorFlipEmitter)->id / 2;
            if (emitterIndex < 7) {
                uint8_t mirrorIndex = ((emitterIndex + (emitterIndex - pathIndex) + 11) % 7);
                mirrorPixels[i++] = getPixelOnStarSegment(mirrorIndex, 1.0 - progress);
                mirrorPixels[0] += 1;
            }
        }
        catch (...) {
            
        }
    }
    if (mirrorRotate) {
        uint8_t mirrorIndex = (pathIndex + 4) % 7;
        mirrorPixels[i++] = getPixelOnStarSegment(mirrorIndex, progress);
        mirrorPixels[0] += 1;
    }
    return mirrorPixels;
}

uint8_t HeptagonStar::getStarSegmentIndex(uint16_t pixel) const {
    uint8_t stripIndex = 0;
    for (int i=0; i<connCount[0]; i++) {
        if (pixel >= conn[0][i]->to->topPixel && pixel <= conn[0][(i + 3) % 7]->from->topPixel) {
            stripIndex = i;
        }
    }
    return stripIndex;
}

float HeptagonStar::getProgressOnStarSegment(uint8_t pathIndex, uint16_t pixel) const {
    Intersection *from = conn[0][pathIndex]->to;
    uint8_t toIndex = (pathIndex + 3) % 7;
    Intersection *to = conn[0][toIndex]->from;
    return (float) (pixel - from->topPixel) / (to->topPixel - from->topPixel);
}

uint16_t HeptagonStar::getPixelOnStarSegment(uint8_t pathIndex, float perc) const {
    Intersection *from = conn[0][pathIndex]->to;
    uint8_t toIndex = (pathIndex + 3) % 7;
    Intersection *to = conn[0][toIndex]->from;
    return from->topPixel + round((to->topPixel - from->topPixel) * perc);
}
