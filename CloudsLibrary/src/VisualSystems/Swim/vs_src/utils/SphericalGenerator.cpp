/*
 *  SphericalGenerator.cpp
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
#include "SphericalGenerator.h"

namespace itg
{
    namespace mesh
    {
        SphericalGenerator::SphericalGenerator(CircularFormula::Ptr f1, CircularFormula::Ptr f2, const ofVec3f& scale) :
            f1(f1), f2(f2), scale(scale)
        {
        }

        void SphericalGenerator::genStrip(ofMesh& mesh, const float phi0, const float phi1, const unsigned segments) const
        {
            mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            for (unsigned i = 0; i < segments; ++i)
            {
                for (unsigned j = 0; j < 2; ++j)
                {
                    float phi = j?phi0:phi1;
                    float theta = i * PI / segments;
                    float sinTheta = std::sin(theta);
                    float sinPhi = std::sin(phi);
                    float cosTheta = std::cos(theta);
                    float cosPhi = std::cos(phi);
                    float f1Phi = f1->radius(phi);
                    float f2Theta = f2->radius(theta);
                    mesh.addVertex(ofVec3f(scale.x * f1Phi * cosPhi * f2Theta * sinTheta,
                                           scale.y * f1Phi * sinPhi * f2Theta * sinTheta,
                                           scale.z * f2Theta * cosTheta));
                }
            }
        }

        // Mesh code from here http://www.gamedev.net/topic/350823-rendering-a-sphere-using-triangle-strips/
        void SphericalGenerator::genSpiralMesh(ofMesh& mesh, float u1, float v1, const unsigned loops, const unsigned segmentsPerLoop, float maxTheta) const
        {
            mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber)
            {
                float theta = 0;
                float phi = loopSegmentNumber * TWO_PI / segmentsPerLoop;
                float sinTheta = std::sin(theta);
                float sinPhi = std::sin(phi);
                float cosTheta = std::cos(theta);
                float cosPhi = std::cos(phi);
                float f1Phi = f1->radius(phi);
                float f2Theta = f2->radius(theta);
                mesh.addVertex(ofVec3f(scale.x * f1Phi * cosPhi * f2Theta * sinTheta,
                                       scale.y * f1Phi * sinPhi * f2Theta * sinTheta,
                                       scale.z * f2Theta * cosTheta));
                //if (angularTexCoords) mesh.addTexCoord(ofVec2f(phi, theta));
                //else
                mesh.addTexCoord(ofVec2f(u1 * phi / TWO_PI, v1 * theta / maxTheta));
            }
            for (unsigned int loopNumber = 0; loopNumber <= loops; ++loopNumber)
            {
                for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber)
                {
                    float theta = (loopNumber * maxTheta / loops) + ((maxTheta * loopSegmentNumber) / (segmentsPerLoop * loops));
                    if (loopNumber == loops)
                    {
                        theta = maxTheta;
                    }
                    float phi = loopSegmentNumber * TWO_PI / segmentsPerLoop;
                    float sinTheta = std::sin(theta);
                    float sinPhi = std::sin(phi);
                    float cosTheta = std::cos(theta);
                    float cosPhi = std::cos(phi);
                    float f1Phi = f1->radius(phi);
                    float f2Theta = f2->radius(theta);
                    mesh.addVertex(ofVec3f(scale.x * f1Phi * cosPhi * f2Theta * sinTheta,
                                           scale.y * f1Phi * sinPhi * f2Theta * sinTheta,
                                           scale.z * f2Theta * cosTheta));
                    //if (angularTexCoords) mesh.addTexCoord(ofVec2f(phi, theta));
                    //else
                    mesh.addTexCoord(ofVec2f(u1 * phi / TWO_PI, v1 * theta / maxTheta));
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
        }

        void SphericalGenerator::genStackedMesh(ofMesh& mesh, float u1, float v1, unsigned stacks, unsigned slices, float maxTheta) const
        {
            mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            for (unsigned int stackNumber = 0; stackNumber <= stacks; ++stackNumber)
            {
                for (unsigned int sliceNumber = 0; sliceNumber <= slices; ++sliceNumber)
                {
                    float theta = stackNumber * maxTheta / stacks;
                    float phi = sliceNumber * TWO_PI / slices;
                    float sinTheta = std::sin(theta);
                    float sinPhi = std::sin(phi);
                    float cosTheta = std::cos(theta);
                    float cosPhi = std::cos(phi);
                    float f1Phi = f1->radius(phi);
                    float f2Theta = f2->radius(theta);
                    mesh.addVertex(ofVec3f(scale.x * f1Phi * cosPhi * f2Theta * sinTheta,
                                           scale.y * f1Phi * sinPhi * f2Theta * sinTheta,
                                           scale.z * f2Theta * cosTheta));
                    mesh.addTexCoord(ofVec2f(u1 * phi / TWO_PI, v1 * theta / maxTheta));
                }
            }
            
            for (unsigned int stackNumber = 0; stackNumber < stacks; ++stackNumber)
            {
                for (unsigned int sliceNumber = 0; sliceNumber <= slices; ++sliceNumber)
                {			
                    mesh.addIndex((stackNumber * (slices + 1)) + (sliceNumber % (slices + 1)));
                    mesh.addIndex(((stackNumber + 1) * (slices + 1)) + (sliceNumber % (slices + 1)));
                }
            }
        }
    }
}