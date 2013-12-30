/*
 *  CubeAction.cpp
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
#include "CubeAction.h"

namespace itg
{
    const ofVec3f CubeAction::NORMALS[] = {
        ofVec3f(+1,0,0), ofVec3f(+1,0,0), ofVec3f(+1,0,0), ofVec3f(+1,0,0),
        ofVec3f(0,+1,0), ofVec3f(0,+1,0), ofVec3f(0,+1,0), ofVec3f(0,+1,0),
        ofVec3f(0,0,+1), ofVec3f(0,0,+1), ofVec3f(0,0,+1), ofVec3f(0,0,+1),
        ofVec3f(-1,0,0), ofVec3f(-1,0,0), ofVec3f(-1,0,0), ofVec3f(-1,0,0),
        ofVec3f(0,-1,0), ofVec3f(0,-1,0), ofVec3f(0,-1,0), ofVec3f(0,-1,0),
        ofVec3f(0,0,-1), ofVec3f(0,0,-1), ofVec3f(0,0,-1), ofVec3f(0,0,-1)
    };

    const ofIndexType CubeAction::INDICES[] = {
        0,1,2, // right top left
        0,2,3, // right bottom right
        4,5,6, // bottom top right
        4,6,7, // bottom bottom left
        8,9,10, // back bottom right
        8,10,11, // back top left
        12,13,14, // left bottom right
        12,14,15, // left top left
        16,17,18, // ... etc
        16,18,19,
        20,21,22,
        20,22,23
    };
    
    void CubeAction::setDimensions(float w, float h, float d)
    {
        w *= .5f;
        h *= .5f;
        d *= .5f;
        ofVec3f vertices[] = {
            ofVec3f(+w,-h,+d), ofVec3f(+w,-h,-d), ofVec3f(+w,+h,-d), ofVec3f(+w,+h,+d),
            ofVec3f(+w,+h,+d), ofVec3f(+w,+h,-d), ofVec3f(-w,+h,-d), ofVec3f(-w,+h,+d),
            ofVec3f(+w,+h,+d), ofVec3f(-w,+h,+d), ofVec3f(-w,-h,+d), ofVec3f(+w,-h,+d),
            ofVec3f(-w,-h,+d), ofVec3f(-w,+h,+d), ofVec3f(-w,+h,-d), ofVec3f(-w,-h,-d),
            ofVec3f(-w,-h,+d), ofVec3f(-w,-h,-d), ofVec3f(+w,-h,-d), ofVec3f(+w,-h,+d),
            ofVec3f(-w,-h,-d), ofVec3f(-w,+h,-d), ofVec3f(+w,+h,-d), ofVec3f(+w,-h,-d)
        };
        this->vertices.clear();
        this->vertices.assign(vertices, vertices + NUM_VERTICES);
    }
    
    void CubeAction::setSize(float size)
    {
        setDimensions(size, size, size);
    }
    
    Branch::Ptr CubeAction::step(Branch::Ptr branch, ofMesh& mesh)
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
            mesh.addNormal(NORMALS[i] * normalMatrix);
            mesh.addTexCoord(ofVec2f(0.f, branch->getDepth()));
            mesh.addColor(colour);
        }
        return newBranch;
    }
    
    void CubeAction::load(ofxXmlSettings& xml, const string& tagName, unsigned tagIdx)
    {
        TransformAction::load(xml, tagName, tagIdx);
        setSize(xml.getAttribute(tagName, "size", 0.1, tagIdx));
        colour = Action::parseColour(xml.getAttribute(tagName, "colour", "", tagIdx));
    }
}
