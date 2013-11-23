/*
 *  IcosphereAction.cpp
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
#include "IcosphereAction.h"

namespace itg
{
    void IcosphereAction::setParameters(float radius, unsigned iterations)
    {
        icoMesh = Action::icosphere(radius, iterations);
    }
    
    Branch::Ptr IcosphereAction::step(Branch::Ptr branch, ofMesh& mesh)
    {
        Branch::Ptr newBranch = TransformAction::step(branch, mesh);
        ofMatrix4x4 normalMatrix = inverseTranspose(newBranch->getTransform());
        for (unsigned i = 0; i < icoMesh.getNumIndices(); ++i)
        {
            mesh.addIndex(icoMesh.getIndex(i) + mesh.getNumVertices());
        }
        for (unsigned i = 0; i < icoMesh.getNumVertices(); ++i)
        {
            mesh.addVertex(icoMesh.getVertex(i) * newBranch->getTransform());
            mesh.addNormal(icoMesh.getNormal(i) * normalMatrix);
            mesh.addTexCoord(ofVec2f(0.f, branch->getDepth()));
            mesh.addColor(colour);
        }
        //newBranch->setVertexIndex(mesh.getNumVertices());
        return newBranch;
    }
    
    void IcosphereAction::load(ofxXmlSettings& xml, const string& tagName, unsigned tagIdx)
    {
        TransformAction::load(xml, tagName, tagIdx);
        float radius = xml.getAttribute(tagName, "radius", 0.1, tagIdx);
        unsigned iterations = xml.getAttribute(tagName, "iterations", 2, tagIdx);
        setParameters(radius, iterations);
        colour = Action::parseColour(xml.getAttribute(tagName, "colour", "", tagIdx));
    }
}
