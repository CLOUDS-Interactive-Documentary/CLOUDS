/*
 *  PlaneAction.cpp
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
#include "PlaneAction.h"

namespace itg
{
    const ofVec3f PlaneAction::NORMAL = ofVec3f(0.f, 1.f, 0.f);
    const ofIndexType PlaneAction::INDICES[] = { 0, 1, 3, 3, 1, 2 };
    
    void PlaneAction::setParameters(float w, float d)
    {
        this->w = w;
        this->d = d;
        vertices.clear();
        vertices.push_back(ofVec3f(-0.5 * w, 0.f, 0.5 * d));
        vertices.push_back(ofVec3f(-0.5 * w, 0.f, -0.5 * d));
        vertices.push_back(ofVec3f(0.5 * w, 0.f, -0.5 * d));
        vertices.push_back(ofVec3f(0.5 * w, 0.f, 0.5 * d));
    }
    
    Branch::Ptr PlaneAction::step(Branch::Ptr branch, ofMesh& mesh)
    {
        Branch::Ptr newBranch = TransformAction::step(branch, mesh);
        ofMatrix4x4 normalMatrix = inverseTranspose(newBranch->getTransform());
        for (unsigned i = 0; i < NUM_INDICES; ++i)
        {
            mesh.addIndex(INDICES[i] + mesh.getNumVertices());
        }
        for (unsigned i = 0; i < vertices.size(); ++i)
        {
            mesh.addVertex(vertices[i] * newBranch->getTransform());
            mesh.addNormal(NORMAL * normalMatrix);
            mesh.addTexCoord(ofVec2f(0.f, branch->getDepth()));
            mesh.addColor(colour);
        }
        return newBranch;
    }
    
    void PlaneAction::load(ofxXmlSettings& xml, const string& tagName, unsigned tagIdx)
    {
        TransformAction::load(xml, tagName, tagIdx);
        setParameters(xml.getAttribute(tagName, "width", 100, tagIdx), xml.getAttribute(tagName, "depth", 100, tagIdx));
        colour = Action::parseColour(xml.getAttribute(tagName, "colour", "", tagIdx));
    }
}
