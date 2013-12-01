/*
 *  Rule.h
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

#include "ofMain.h"
#include "Action.h"

namespace itg
{
    // change this to transform action
    class TransformAction : public Action
    {
    public:
        typedef shared_ptr<TransformAction> Ptr;
        
        TransformAction(const string& nextRuleName = "", const ofMatrix4x4& transform = ofMatrix4x4());
        
        virtual Branch::Ptr step(Branch::Ptr branch, ofMesh& mesh);
        
        // transformations should be added to rule in correct order
        void translate(const ofVec3f& translation);
        void translate(float x, float y, float z);
        
        void rotate(const ofQuaternion& rotation);
        void rotate(const ofVec3f& euler);
        void rotate(float x, float y, float z);
        
        void scale(const ofVec3f& scale);
        void scale(float scale);
        
        ofMatrix4x4 getTransform() const { return transform; }
        void setTransform(const ofMatrix4x4& transform) { this->transform = transform; }
        
        void load(ofxXmlSettings& xml, const string& tagName, unsigned tagIdx);
        //void save(ofxXmlSettings& xml, const string& tagName);
        
    private:
        void parseTransforms(const string& transforms);
        ofMatrix4x4 transform;
    };
}
