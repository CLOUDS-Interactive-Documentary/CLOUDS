/*
 *  Action.h
 *
 *  Copyright (c) 2013, Neil Mendoza, http://www.neilmendoza.com
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

#include <tr1/memory>
#include "ofxXmlSettings.h"
#include "Branch.h"

namespace itg
{
    using namespace tr1;
    
    class Action
    {
    public:
        typedef shared_ptr<Action> Ptr;
        
        Action(const string& nextRuleName = "");
        virtual ~Action() {}
        
        virtual Branch::Ptr step(Branch::Ptr branch, ofMesh& mesh) = 0;
        
        void setNextRuleName(const string& nextRuleName) { this->nextRuleName = nextRuleName; }
        string getNextRuleName() const { return nextRuleName; }
        
        void setRuleName(const string& ruleName) { this->ruleName = ruleName; }
        string getRuleName() const { return ruleName; }
        
        void setRepeat(unsigned repeat) { this->repeat = repeat; }
        unsigned getRepeat() const { return repeat; }
        
        virtual void load(ofxXmlSettings& xml, const string& tagName, unsigned tagIdx);
        virtual void save(ofxXmlSettings& xml);
        
        string getName() const { return name; }
        void setName(const string& name) { this->name = name; }
        
        static ofFloatColor parseColour(const string& colourString);
        
    protected:
        // get the normal matrix
        static ofMatrix4x4 inverseTranspose(const ofMatrix4x4& transform);
        // these functions are part of ofMesh in 0.8
        static ofMesh icosahedron(float radius);
        static ofMesh icosphere(float radius, int iterations);
        
    private:
        string name;
        string ruleName;
        string nextRuleName;
        unsigned repeat;
    };
}
