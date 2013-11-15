/*
 *  GeomUtils.cpp
 *
 *  Copyright (c) 2011, Neil Mendoza, http://www.neilmendoza.com
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
 *  * Neither the name of 16b.it nor the names of its contributors may be used 
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
#include "MeshUtils.h"
#include "ofMain.h"

namespace itg
{
    namespace mesh
    {
        void genNormals(ofMesh& mesh)
        {
            zeroNormals(mesh);
            if (mesh.getMode() == OF_PRIMITIVE_TRIANGLE_STRIP || mesh.getMode() == OF_PRIMITIVE_TRIANGLE_FAN)
            {
                ofVec3f prevEdge = mesh.getVertex(getIndex(mesh, 1)) - mesh.getVertex(getIndex(mesh, 0));	
                for (int i = 2; i < getSize(mesh); ++i)
                {
                    ofVec3f edge;
                    switch (mesh.getMode())
                    {
                        case OF_PRIMITIVE_TRIANGLE_STRIP:
                        {
                            // calculate new edge
                            edge = mesh.getVertex(getIndex(mesh, i)) - mesh.getVertex(getIndex(mesh, i - 1));
                            // calcualte normal to new triangle
                            ofVec3f normal = prevEdge.crossed(edge).normalize();
                            if (i % 2) normal = -normal;
                            // add normalised normal to every vertex in new triangle
                            for (int j = 0; j < 3; ++j)
                            {
                                mesh.setNormal(getIndex(mesh, i - j), normal + mesh.getNormal(getIndex(mesh, i - j)));
                            }
                            break;
                        }
                            
                        case OF_PRIMITIVE_TRIANGLE_FAN:
                        {
                            // calculate new edge
                            edge = mesh.getVertex(getIndex(mesh, i)) - mesh.getVertex(getIndex(mesh, 0));
                            // calcualte normal to new triangle
                            ofVec3f normal = prevEdge.crossed(edge).normalize();
                            // add normalised normal to every vertex in new triangle
                            for (int j = 0; j < 2; ++j)
                            {
                                mesh.setNormal(getIndex(mesh, i - j), normal + mesh.getNormal(getIndex(mesh, i - j)));
                            }
                            mesh.setNormal(getIndex(mesh, 0), normal + mesh.getNormal(getIndex(mesh, 0)));
                            break;
                        }
                            
                        default:
                            break;
                    }
                    prevEdge = edge;
                }
            }
            else if (mesh.getMode() == OF_PRIMITIVE_TRIANGLES)
            {
                if (getSize(mesh) % 3) ofLogError() << "Triangle mesh with number of vertices that is not divisible by 3";
                for (int i = 0; i < getSize(mesh) / 3; ++i)
                {
                    ofVec3f side1 = mesh.getVertex(getIndex(mesh, i * 3 + 1)) - mesh.getVertex(getIndex(mesh, i * 3));
                    ofVec3f side2 = mesh.getVertex(getIndex(mesh, i * 3 + 2)) - mesh.getVertex(getIndex(mesh, i * 3 + 1));
                    ofVec3f normal = side1.crossed(side2).normalize();
                    for (int j = 0; j < 3; ++j)
                    {
                        mesh.setNormal(getIndex(mesh, i * 3 + j), normal + mesh.getNormal(getIndex(mesh, i * 3 + j)));
                    }
                }
            }
            normalizeNormals(mesh);
        }
        
        int getSize(ofMesh& mesh)
        {
            if (mesh.getNumIndices()) return mesh.getNumIndices();
            return mesh.getNumVertices();
        }
        
        int getIndex(ofMesh& mesh, const int idx)
        {
            if (mesh.getNumIndices()) return mesh.getIndex(idx);
            return idx;
        }
        
        void normalizeNormals(ofMesh& mesh)
        {
            for (int i = 0; i < mesh.getNumNormals(); ++i)
            {
                mesh.setNormal(i, mesh.getNormal(i).normalize());
            }
        }
        
        void zeroNormals(ofMesh& mesh)
        {
            mesh.getNormals().clear();
            for (int i = 0; i < mesh.getNumVertices(); ++i)
            {
                mesh.addNormal(ofVec3f());
            }
        }
        
        void drawNormals(ofMesh& mesh, float size)
        {
            for (int i = 0; i < getSize(mesh); ++i)
            {
                unsigned idx = getIndex(mesh, i);
                ofVec3f vertex = mesh.getVertex(idx);
                ofVec3f normal = mesh.getNormal(idx);
                
                ofLine(vertex, vertex + size * normal);
            }
        }

        void genCurvedStrip(ofMesh& mesh, float r0, float r1, float theta0, float theta1,
                            float u0, float u1, float v0, float v1, const ofMatrix4x4& transform, unsigned numRings, unsigned numSlices)
        {
            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
            unsigned startVerts = mesh.getNumVertices();
            for (unsigned i = 0; i < numRings; ++i)
            {
                float r = ofMap(i, 0, numRings - 1, r0, r1);
                for (unsigned j = 0; j < numSlices; ++j)
                {
                    float theta = ofMap(j, 0, numSlices - 1, theta0, theta1);
                    mesh.addVertex(transform * ofVec3f(r * sin(theta), -r * cos(theta), 0.f));
                    
                    float u = ofMap(j, 0, numSlices - 1, u0, u1);
                    float v = ofMap(i, 0, numRings - 1, v0, v1);
                    
                    mesh.addTexCoord(ofVec2f(u, v));
                }
            }
            for (unsigned i = 0; i < numRings - 1; ++i)
            {
                for (unsigned j = 0; j < numSlices - 1; ++j)
                {
                    mesh.addIndex(startVerts + i * numSlices + j);
                    mesh.addIndex(startVerts + i * numSlices + j + 1);
                    mesh.addIndex(startVerts + (i + 1) * numSlices + j);
                    
                    mesh.addIndex(startVerts + i * numSlices + j + 1);
                    mesh.addIndex(startVerts + (i + 1) * numSlices + j);
                    mesh.addIndex(startVerts + (i + 1) * numSlices + j + 1);
                }
            }
        }
        
        void genSphere(ofMesh& mesh, float size, float u0, float u1, float v0, float v1, const ofMatrix4x4& transform,
                       unsigned stacks, unsigned slices, const CircularFormula& f1, const CircularFormula& f2, SphereType type)
        {
            switch (type)
            {
                case STACKED:
                {
                    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
                    for (unsigned int stackNumber = 0; stackNumber <= stacks; ++stackNumber)
                    {
                        for (unsigned int sliceNumber = 0; sliceNumber < slices; ++sliceNumber)
                        {
                            float theta = stackNumber * PI / stacks;
                            float phi = sliceNumber * TWO_PI / slices;
                            float sinTheta = std::sin(theta);
                            float sinPhi = std::sin(phi);
                            float cosTheta = std::cos(theta);
                            float cosPhi = std::cos(phi);
                            float f1Phi = f1.radius(phi);
                            float f2Theta = f2.radius(theta);
                            mesh.addVertex(ofVec3f(size * f1Phi * cosPhi * f2Theta * sinTheta,
                                                   size * f1Phi * sinPhi * f2Theta * sinTheta,
                                                   size * f2Theta * cosTheta) * transform);
                            mesh.addTexCoord(ofVec2f(ofMap(phi, 0.f, TWO_PI, u0, u1),
                                                     ofMap(theta, 0.f, PI, v0, v1)));
                        }
                    }
                    
                    for (unsigned int stackNumber = 0; stackNumber < stacks; ++stackNumber)
                    {
                        for (unsigned int sliceNumber = 0; sliceNumber <= slices; ++sliceNumber)
                        {
                            mesh.addIndex((stackNumber * slices) + (sliceNumber % slices));
                            mesh.addIndex(((stackNumber + 1) * slices) + (sliceNumber % slices));
                        }
                    }
                    break;
                }
                    
                case SPIRAL:
                {
                    unsigned loops = stacks;
                    unsigned segmentsPerLoop = slices;
                    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
                    for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber)
                    {
                        float theta = 0;
                        float phi = loopSegmentNumber * TWO_PI / segmentsPerLoop;
                        float sinTheta = std::sin(theta);
                        float sinPhi = std::sin(phi);
                        float cosTheta = std::cos(theta);
                        float cosPhi = std::cos(phi);
                        float f1Phi = f1.radius(phi);
                        float f2Theta = f2.radius(theta);
                        mesh.addVertex(ofVec3f(size * f1Phi * cosPhi * f2Theta * sinTheta,
                                               size * f1Phi * sinPhi * f2Theta * sinTheta,
                                               size * f2Theta * cosTheta) * transform);
                        mesh.addTexCoord(ofVec2f(ofMap(phi, 0.f, TWO_PI, u0, u1),
                                                 ofMap(theta, 0.f, PI, v0, v1)));
                    }
                    for (unsigned int loopNumber = 0; loopNumber <= loops; ++loopNumber)
                    {
                        for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber)
                        {
                            float theta = (loopNumber * PI / loops) + ((PI * loopSegmentNumber) / (segmentsPerLoop * loops));
                            if (loopNumber == loops)
                            {
                                theta = PI;
                            }
                            float phi = loopSegmentNumber * 2 * PI / segmentsPerLoop;
                            float sinTheta = std::sin(theta);
                            float sinPhi = std::sin(phi);
                            float cosTheta = std::cos(theta);
                            float cosPhi = std::cos(phi);
                            float f1Phi = f1.radius(phi);
                            float f2Theta = f2.radius(theta);
                            mesh.addVertex(ofVec3f(size * f1Phi * cosPhi * f2Theta * sinTheta,
                                                   size * f1Phi * sinPhi * f2Theta * sinTheta,
                                                   size * f2Theta * cosTheta) * transform);
                            mesh.addTexCoord(ofVec2f(ofMap(phi, 0.f, TWO_PI, u0, u1),
                                                     ofMap(theta, 0.f, PI, v0, v1)));
                        }
                    }
                    for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber)
                    {
                        mesh.addIndex(loopSegmentNumber);
                        mesh.addIndex(segmentsPerLoop + loopSegmentNumber);
                    }
                    for (unsigned int loopNumber = 0; loopNumber < loops; ++loopNumber)
                    {
                        for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber)
                        {
                            mesh.addIndex(((loopNumber + 1) * segmentsPerLoop) + loopSegmentNumber);
                            mesh.addIndex(((loopNumber + 2) * segmentsPerLoop) + loopSegmentNumber);
                        }
                    }
                    break;
                }
                    
                default:
                    break;
            }
        }
        
        void genSphere(ofMesh& mesh, float size, const CircularFormula& f1, const CircularFormula& f2,
                       unsigned stacks, unsigned slices, bool angularTexCoords, SphereType type)
        {
            if (angularTexCoords) genSphere(mesh, size, 0, TWO_PI, 0, TWO_PI, ofMatrix4x4(), stacks, slices, f1, f2, type);
            else genSphere(mesh, size, 0, 1.f, 0, 1.f, ofMatrix4x4(), stacks, slices, f1, f2, type);
        }
    }
}