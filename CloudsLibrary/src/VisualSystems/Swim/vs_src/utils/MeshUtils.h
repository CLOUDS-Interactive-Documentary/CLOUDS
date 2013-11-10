/*
 *  MeshUtils.h
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

#include "Singleton.h"
#include "ofMesh.h"
#include "ofMatrix4x4.h"
#include "CircularFormula.h"

namespace itg
{
    namespace mesh
    {
        enum SphereType
        {
            STACKED,
            SPIRAL
        };
        
		void zeroNormals(ofMesh& mesh);
		void genNormals(ofMesh& mesh);
		void normalizeNormals(ofMesh& mesh);
		int getSize(ofMesh& mesh);
		int getIndex(ofMesh& mesh, const int idx);
        void drawNormals(ofMesh& mesh, float size = 1.f);
        
        void genSphere(ofMesh& mesh, float size, float u0 = 0.f, float u1 = 1.f, float v0 = 0.f, float v1 = 1.f, const ofMatrix4x4& transform = ofMatrix4x4(),
                       unsigned stacks = 20, unsigned slices = 20, const CircularFormula& f1 = CircularFormula(), const CircularFormula& f2 = CircularFormula(), SphereType type = STACKED);
        void genCurvedStrip(ofMesh& mesh, float r0, float r1, float theta0, float theta1,
                            float u0 = 0.f, float u1 = 1.f, float v0 = 0.f, float v1 = 1.f, const ofMatrix4x4& transform = ofMatrix4x4(), unsigned numRings = 10, unsigned numSlices = 20);
        
        /**
         * @deprecated
         */
        void genSphere(ofMesh& mesh, float size, const CircularFormula& f1 = CircularFormula(), const CircularFormula& f2 = CircularFormula(),
                       unsigned stacks = 20, unsigned slices = 20, bool angularTexCoords = false, SphereType type = STACKED);

	}	
}
