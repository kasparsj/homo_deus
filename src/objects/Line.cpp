#include "Line.h"

void Line::setup() {
    initInter(2);
    initConn(2);
    initModels(2);
    
    Model::maxWeights = 2;  // One for bridge, one for physical connection

    // Add a default model
    addModel(new Model(L_DEFAULT, 10, GROUP1));
    addModel(new Model(L_BOUNCE, 10, GROUP1));

    // Add a bridge from last pixel to first
    Connection* bridge = addBridge(LINE_PIXEL_COUNT-1, 0, GROUP1);
    
    // Add a physical connection with explicit numLeds to cover the full line
    Connection* lineConn = addConnection(new Connection(inter[0][1], inter[0][0], GROUP1, LINE_PIXEL_COUNT-3));
    
    // Add weights to the model for both connections
    models[L_BOUNCE]->put(bridge, 0);
    models[L_BOUNCE]->put(lineConn, 10);
}

float Line::getProgressOnLine(uint16_t pixel) const {
    // Assuming a sequential range of pixels from 0 to pixelCount-1
    return (float)pixel / (pixelCount - 1);
}

uint16_t Line::getPixelOnLine(float perc) const {
    return round(perc * (pixelCount - 1));
}

uint16_t* Line::getMirroredPixels(uint16_t pixel, LPOwner* mirrorFlipEmitter, bool mirrorRotate) {
    float progress = getProgressOnLine(pixel);
    mirrorPixels[0] = 0;
    
    // Mirror by reflecting across the middle of the line
    if (mirrorRotate) {
        mirrorPixels[1] = getPixelOnLine(1.0 - progress);
        mirrorPixels[0] = 1;
    }
    
    return mirrorPixels;
}
