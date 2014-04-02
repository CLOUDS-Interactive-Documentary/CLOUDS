//
//  cameraManager.cpp
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-02-14.
//
//

#include "cameraManager.h"

void cameraManager::setup(terrainManager * _tm) {
    tm = _tm;
    
    camX = 0;
    camY = 150;
    camZ = 600;
    
    
    lookatX = 0;
    lookatY = 0;
    lookatZ = -600;
    
    camYDecayed.setDecay(0.99);
    camYSmoothed.setNumPValues(25);
    camXSmoothed.setNumPValues(45);
    
    camXLookAhead = 45;
    camYLookAhead = 45;
    camXRange = 50;
    center = 256;
    
//    cam.setScale(1,-1,1);
}

void cameraManager::update() {
    lastCamX = camX;
    lastCamY = camY;
    
    
//    float minH = 1.0;
//    int index = 0;
//    for (int i = center - camXRange; i < center + camXRange; i++) {
//        if ( tm->waveHistory[camXLookAhead][i] < minH ) {
//            minH = tm->waveHistory[camXLookAhead][i];
//            index = i;
//        }
//    }
//    
//    if (tm->waveHistory[camXLookAhead][index] * 20 * 180 + 150 > camY) {
//        index -= 256;
//        camXSmoothed.addValue(index * 7);
//    }
//    else {
//        camXSmoothed.addValue(0);
//    }
//
//
//    camX = camXSmoothed.getMean();
    
    camYDecayed.addValue(tm->waveHistory[45][camX + 256] * 20 * 180 + 150);
    camYDecayed.update();
    camYSmoothed.addValue(camYDecayed.getValue());
    camY = camYSmoothed.getMean();
    
//    if ( tm->waveHistory[45][camX + 256] * 20 * 180 + 100 > camY) {
//        camYSmoothed.addValue(tm->waveHistory[45][camX + 256] * 20 * 180 + 100);
//        camY = camYSmoothed.getMean();
//    }
//    else {
//        camY *= 0.99;
//    }
    lookatX = camX - lastCamX;
    lookatY = camY - lastCamY;
    
    cam.setPosition(camX, camY, camZ);
    cam.lookAt(ofVec3f(lookatX, lookatY, lookatZ));
}
