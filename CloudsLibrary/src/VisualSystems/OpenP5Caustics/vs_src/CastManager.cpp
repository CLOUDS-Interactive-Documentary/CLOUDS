//
//  CastManager.cpp
//  OpenP5Caustics
//
//  Created by Elie Zananiri on 11/13/2013.
//
//

#include "CastManager.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
CastManager::CastManager(int width, int height)
{
    this->width = width;
    this->height = height;

    cR = new Caster(width, height);
    cG = new Caster(width, height);
    cB = new Caster(width, height);

    cR->n = 1.4;
    cG->n = 1.42;
    cB->n = 1.44;
    
    cR->intensity = 20;
    cG->intensity = 20;
    cB->intensity = 22;

    bump = 0.5;
    noiseScale = 0.008;
    boxW = 300;
    boxH = 300;
    z = 0;
    bReset = false;
    bReseed = false;
    loopCount;
    fpsGranularity = 10000;
    algo = kCastAlgoSpiral;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
CastManager::~CastManager()
{
    delete cR;
    delete cG;
    delete cB;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void CastManager::threadedFunction()
{
    long timeStart = ofGetElapsedTimeMillis();
    
    float r = 10;
    float t = 0;
    int rDir = 1;
    
    while (true) {
        if (loopCount % fpsGranularity == 0) {
            long delta = ofGetElapsedTimeMillis() - timeStart;
            if (delta != 0) {
                fps = fpsGranularity * 1000.0f / delta;
            }
            timeStart = ofGetElapsedTimeMillis();
        }
        
        if (bReseed) {
            ofSeedRandom(ofGetElapsedTimeMillis());
            bReseed = false;
        }
        
        if (bReset) {
            cR->reset();
            cG->reset();
            cB->reset();
            bReset = false;
        }
        
        float x, y;
        switch (algo) {
            case kCastAlgoRandom:
                x = ofRandom(boxW) + width  / 2 - boxW / 2;
                y = ofRandom(boxH) + height / 2 - boxH / 2;
                break;
                
            case kCastAlgoSpiral:
            case kCastAlgoSpiralClear:
                r = (r + 0.0001f * rDir);
                if (ABS(r) > boxW / 2) {
                    rDir *= -1;
                }
                t = fmodf(t + 0.001f, TWO_PI);
                x = r * cosf(t) + width  / 2;
                y = r * sinf(t) + height / 2;
                break;
                
            default:
                x = 0;
                y = 0;
                ofLogError("CastManager::threadedFunction()") << "Unknown algo: " << algo;
        }
        
        // Calculation of gradient.
        float dzdx = (ofNoise(x * noiseScale + noiseScale, y * noiseScale, z * noiseScale) - ofNoise(x * noiseScale - noiseScale, y * noiseScale, z * noiseScale)) * bump / (noiseScale * 2);
        float dzdy = (ofNoise(x * noiseScale, y * noiseScale + noiseScale, z * noiseScale) - ofNoise(x * noiseScale, y * noiseScale - noiseScale, z * noiseScale)) * bump / (noiseScale * 2);
        
        // More accurate calculation of gradient.
        //float dzdx = (ofNoise(x * noiseScale - 2 * noiseScale, y * noiseScale, z * noiseScale) * -0.5
        //            + ofNoise(x * noiseScale - noiseScale, y * noiseScale, z * noiseScale) * -1
        //            + ofNoise(x * noiseScale + noiseScale, y * noiseScale, z * noiseScale) *  1
        //            + ofNoise(x * noiseScale + 2 * noiseScale, y * noiseScale, z * noiseScale) *  0.5) * bump / (noiseScale * 2);
        
        //float dzdy = (ofNoise(x * noiseScale, y * noiseScale - 2 * noiseScale, z * noiseScale) * -0.5
        //            + ofNoise(x * noiseScale, y * noiseScale - noiseScale, z * noiseScale) * -1
        //            + ofNoise(x * noiseScale, y * noiseScale + noiseScale, z * noiseScale) *  1
        //            + ofNoise(x * noiseScale, y * noiseScale + 2 * noiseScale, z * noiseScale) *  0.5) * bump / (noiseScale * 2);
        
        float m = sqrtf(dzdx * dzdx + dzdy * dzdy + 1);
        
        // Normal to surface.
        float nX = dzdx / m;
        float nY = dzdy / m;
        float nZ = 1 / m;

        float thetaI = acosf(nZ);
        
        cR->cast(x, y, nX, nY, nZ, thetaI);
        cG->cast(x, y, nX, nY, nZ, thetaI);
        cB->cast(x, y, nX, nY, nZ, thetaI);
        
        loopCount++;
//        ofSleepMillis(20);
    }
}
