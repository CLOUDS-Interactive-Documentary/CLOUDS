/*
 *  SphericalGenerator.h
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
#pragma once

#include "ofMesh.h"
#include "CircularFormula.h"

namespace itg
{
    namespace mesh
    {
        class SphericalGenerator
        {
        public:
            SphericalGenerator(CircularFormula::Ptr f1, CircularFormula::Ptr f2, const ofVec3f& scale = ofVec3f(1.f, 1.f, 1.f));
            
            void setScale(const ofVec3f& scale) { this->scale = scale; }
            ofVec3f getScale() const { return scale; }
            
            void genStrip(ofMesh& mesh, const float phi0, const float phi1, const unsigned segments = 50) const;
            void genSpiralMesh(ofMesh& mesh, float u1 = 1.f, float v1 = 1.f, const unsigned loops = 50, const unsigned segmentsPerLoop = 50, float maxTheta = PI) const;
            //void genStackedMesh(ofMesh& mesh, const unsigned stacks = 50, unsigned slices = 50) const;
            void genStackedMesh(ofMesh& mesh, float u1, float v1, unsigned stacks, unsigned slices, float maxTheta) const;
            
        private:
            CircularFormula::Ptr f1;
            CircularFormula::Ptr f2;
            ofVec3f scale;
        };
    }
}