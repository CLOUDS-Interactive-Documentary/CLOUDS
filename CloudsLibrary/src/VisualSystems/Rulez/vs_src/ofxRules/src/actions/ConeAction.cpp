/*
 *  ConeAction.cpp
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
#include "ConeAction.h"

namespace itg
{
    void ConeAction::setParameters(float radius, float height, unsigned resolution)
    {
        coneMesh = Action::cone(radius, height, resolution + 1);
        ofVec3f direction = ((ofVec3f() * getTransform()) - ofVec3f()).normalized();
        ofMatrix4x4 r;
        r.makeRotationMatrix(ofVec3f(0, 1, 0), direction);
        r.rotate(180, 1, 0, 0);
        for (unsigned i = 0; i < coneMesh.getNumVertices(); ++i)
        {
            coneMesh.setVertex(i, coneMesh.getVertex(i) * r);
            coneMesh.setNormal(i, coneMesh.getNormal(i) * r);
        }
    }
    
    Branch::Ptr ConeAction::step(Branch::Ptr branch, ofMesh& mesh)
    {
        Branch::Ptr newBranch = TransformAction::step(branch, mesh);
        ofMatrix4x4 normalMatrix = inverseTranspose(newBranch->getTransform());
        for (unsigned i = 0; i < coneMesh.getNumIndices(); ++i)
        {
            mesh.addIndex(coneMesh.getIndex(i) + mesh.getNumVertices());
        }
        for (unsigned i = 0; i < coneMesh.getNumVertices(); ++i)
        {
            mesh.addVertex(coneMesh.getVertex(i) * newBranch->getTransform());
            mesh.addNormal(coneMesh.getNormal(i) * normalMatrix);
            mesh.addTexCoord(ofVec2f(0.f, branch->getDepth()));
            mesh.addColor(colour);
        }
        //newBranch->setVertexIndex(mesh.getNumVertices());
        return newBranch;
    }
    
    void ConeAction::load(ofxXmlSettings& xml, const string& tagName, unsigned tagIdx)
    {
        TransformAction::load(xml, tagName, tagIdx);
        float radius = xml.getAttribute(tagName, "radius", 1.f, tagIdx);
        float height = xml.getAttribute(tagName, "height", 1.f, tagIdx);
        unsigned resolution = xml.getAttribute(tagName, "resolution", 12, tagIdx);
        setParameters(radius, height, resolution);
        colour = Action::parseColour(xml.getAttribute(tagName, "colour", "", tagIdx));
    }
}
