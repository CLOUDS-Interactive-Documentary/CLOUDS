/*
 *  JellyCreature.h
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

namespace itg
{
    struct JellyParams
    {
        JellyParams() : widthMin(80.f), widthMax(100.f), lengthMin(80.f), lengthMax(100.f) {}
        
        float tentaclesHMin, tentaclesHMax, tentaclesS, tentaclesB;
        float bodyHMin, bodyHMax, bodyS, bodyB;
        
        float bodyAlpha;
        
        float widthMin, widthMax;
        float lengthMin, lengthMax;
        
        float segmentMin, segmentMax;
        float m1Min, m1Max, m2Min, m2Max;
        float pulseAmtMin, pulseAmtMax;
    };
    
    class JellyCreature : public Creature
    {
        friend class Creatures;
        
    public:
        typedef shared_ptr<JellyCreature> Ptr;
        
        static float undulationAmt;
        static float undulationFreqMin;
        static float undulationFreqMax;
        
        JellyCreature(const JellyParams& params);
        
        virtual void update();
        
        // draw function called after node transform has been done
        virtual void customDraw();
        
        void setShaderUniforms();
        
        ofFloatColor getColour() const { return bodyColour; }
        
        unsigned getNumTentacles() const { return numTentacles; }
        
        vector<ofVec3f>& getTentaclePosnsRef() { return tentaclePosns; }
        
        //void setFrequency(const float frequency) { this->frequency = frequency; }
        
    private:
        static ofImage tex;
        static ofShader shader;
        
        unsigned numTentacles;
        
        void genMeshes();
        ofVec3f superVertex(float phi, float theta);
        
        ofVboMesh outerMesh;
        ofVboMesh innerMesh;
        ofVboMesh lineMesh;
        
        vector<ofVec3f> tentaclePosns;
       
        int texRepeatS;
        
        ofVec3f size;
        float segment;
        // superformula stuff
        float m1, m2;
        float n11, n21, n31, a1, b1;
        float n12, n22, n32, a2, b2;
        float bodyAlpha;
        ofFloatColor bodyColour;
        ofFloatColor tentacleColour;
        //float deformAmount;
        float pulseAmt;
        bool drawInner;
    };
}