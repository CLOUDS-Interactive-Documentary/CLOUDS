//
//  CastManager.h
//  OpenP5Caustics
//
//  Created by Elie Zananiri on 11/13/2013.
//
//

#pragma once

#include "ofMain.h"
#include "Caster.h"

const int kCastAlgoRandom      = 0;
const int kCastAlgoSpiral      = 1;
const int kCastAlgoSpiralClear = 2;
const int kCastAlgoCount       = 3;

//--------------------------------------------------------------
//--------------------------------------------------------------
class CastManager : public ofThread
{
    public:
        CastManager(int width, int height);
        ~CastManager();
        
        void cycleAlgo() { algo = (algo + 1) % kCastAlgoCount; };
        void start() { startThread(false, false); }
        void stop() {  stopThread(); }
        void reset() { bReset = true; }
        void reseed() { bReseed = true; }
    
        void threadedFunction();
    
        int algo;

        int width, height;
        Caster * cR, * cG, * cB;
    
        float bump;  // height of bump
        float noiseScale;
        float boxW;
        float boxH;
        float z;
        bool bReset;
        bool bReseed;
        long loopCount;
        int fpsGranularity;
        float fps;
};
