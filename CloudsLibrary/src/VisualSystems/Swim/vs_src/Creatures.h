/*
 *  CreatureController.h
 *
 *  Copyright (c) 2012, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */
#pragma once

#include "Creature.h"
#include "ModelCreature.h"
#include "ofxNearestNeighbour.h"
#include "ModelCreature.h"
#include "JellyCreature.h"
#include "PointCreature.h"
#include "ofxGpuParticles.h"

namespace itg
{
    class Creatures
    {
    public:
        void init(const string& dataPath);
        void update();
        void draw(const ofCamera& cam);

        void onGui(ofxUIEventArgs& args);
        
        // GUI
        float zoneRadius;
        float alignmentLower;
        float alignmentUpper;
        float repelStrength, attractStrength, alignStrength;
        float maxDistFromCentre;
        
        // jelly gui
        int numJellyOne, numJellyTwo;
        JellyParams jellyOneParams, jellyTwoParams;
        
        // fish gui
        int numGreyFish, numYellowFish;
        ModelParams fishOneParams, fishTwoParams;
        
        // point gui
        int numPointOne, numPointTwo, numPointThree;
        float huePointOne, huePointTwo, huePointThree;
        
        void generate();
        
        void loadSeed(const string& path);
        void saveSeed(const string& path);
        
    private:
        void addPointFish(unsigned number, float hue);
        void addModelFish(unsigned number, const ModelParams& params);
        void addJellyFish(unsigned number, const JellyParams& params);
        
        static const unsigned TENTACLE_NUM_SECTIONS = 20;
        static const unsigned TENTACLE_SECTION_LENGTH = 5;
        
        vector<Creature::Ptr> creatures;
        vector<vector<Creature::Ptr> > creaturesByType;
        vector<unsigned> jellyIndices;
        vector<ofVec4f> tentaclePosns;
        ofxNearestNeighbour3D nn;
        ofVboMesh pointCreatureMesh;
        ofxGpuParticles tentacles;
    };
}