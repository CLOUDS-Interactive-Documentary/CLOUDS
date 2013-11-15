/*
 *  ModelCreature.h
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

#include "ofxAssimpModelLoader.h"
#ifdef TARGET_WIN32
#include <memory>
#else
#include <tr1/memory>
#endif
#include "Creature.h"

namespace itg
{
    using namespace tr1;
    
    class ModelCreature : public Creature
    {
        friend class Creatures;
        
    public:
        typedef shared_ptr<ModelCreature> Ptr;
        
        ModelCreature(unsigned modelIdx, const ofFloatColor& colour);
        
        void setSize(float size) { this->size = size; }
        
        void integrate();
        
        void customDraw();
        
    private:
        static vector<ofxAssimpModelLoader> fishModels;
        //static vector<ofImage> fishTextures;
        static ofShader fishShader;
        static vector<ofVec3f> deformAxes;
        static vector<float> bends;
        
        float frequency;
        float size;
        unsigned modelIdx;
        ofVec3f deformAxis;
        ofFloatColor colour;
    };
}