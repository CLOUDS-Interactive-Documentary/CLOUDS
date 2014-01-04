/*
 *  Path.h
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
#include "ofxPtf.h"
#include "PathPoint.h"
#include "Path.h"

namespace itg
{
    class Path
    {
    public:
        // floats only so can add them to gui
        static float maxLineLength;
        static float maxMeshLength;
        static float lineFadeLength;
        static float lineWidth;
        static float hueMin, hueMax;
        
        Path(float radius);
        
        void addVertex(const ofVec3f& vertex);
        void addVertex(float x, float y, float z);
        
        void drawMesh();
        void drawLine();
        void drawNormals(float size);
        void drawInflections();
        void drawAcc();
        
        PathPoint back() const { return points.back(); }
		unsigned size() const { return points.size(); }
		PathPoint operator[](unsigned idx) const { return points[idx]; }
        
        deque<ofVec3f>& getVerticesRef() { return vertices; }
        
        ofxPtf& getPtfRef() { return ptf; }
        
        ofVboMesh& getLineMeshRef() { return lineMesh; }
        
		void setMeshRadius(float meshRadius);
        
		void setResolution(unsigned resolution);
        
        void clear();
        
    private:
        void updateSlice();
        
        deque<PathPoint> points;
        deque<ofVec3f> inflections;
        
        // line stuff
        ofVboMesh lineMesh;
        unsigned resolution;
        ofxPtf ptf;        
        
        // mesh stuff
        vector<ofVec3f> sliceVertices;
        vector<ofVec3f> sliceNormals;
        deque<ofVec3f> vertices;
        deque<ofVec3f> normals;
        deque<unsigned> indices;
		ofVboMesh mesh;
        float meshRadius;
    };
}
