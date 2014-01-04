/*
 *  Path.cpp
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
#include "Path.h"

namespace itg
{
    float Path::maxLineLength = 100;
    float Path::maxMeshLength = 100;
    float Path::lineFadeLength = 1.f;
    float Path::lineWidth = 1.f;
    float Path::hueMin = 0.f;
    float Path::hueMax = 1.f;
    
    void Path::init(float meshRadius, const string& dataPath)// : resolution(10), meshRadius(meshRadius)
    {
        this->resolution = 10;
        this->meshRadius = meshRadius;
        
        // set max frames so we don't end up using up all memory
        ptf.setMaxFrames(1000);
        lineMesh.setUsage(GL_DYNAMIC_DRAW);
        lineMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
        
        lineParticles.init(maxLineLength, 1, OF_PRIMITIVE_LINE_STRIP, false, 1);
        lineParticles.getDrawShaderRef().load(dataPath + "shaders/line");
        
        linePosns.resize(maxLineLength);
        
        unsigned fadeIdx = lineFadeLength * maxLineLength;
        for (unsigned i = 0; i < maxLineLength; ++i)
        {
            lineMesh.addVertex(ofVec3f());
            float alpha = ofMap(i, 0, fadeIdx, 0.f, 1.f, true);
            lineMesh.addColor(ofFloatColor::fromHsb(ofMap(abs(points[i].getPos().x) + abs(points[i].getPos().y), 0, 50, hueMin, hueMax, true), 0.5, 1.0, alpha));
        }
        
        updateSlice();
    }
    
    void Path::setMeshRadius(float meshRadius)
    {
        this->meshRadius = meshRadius;
        updateSlice();
    }
    
    void Path::setResolution(unsigned resolution)
    {
        this->resolution = resolution;
        updateSlice();
    }
    
    void Path::updateSlice()
    {
        sliceVertices.clear();
        sliceNormals.clear();
        ofVec3f normal = ofVec3f(0, 0, 1);
        for (unsigned i = 0; i < resolution; ++i)
        {
            float theta = TWO_PI * (i % resolution) / (float)resolution;
            sliceVertices.push_back(ofVec3f(0, meshRadius * sin(theta), meshRadius * cos(theta)));
            sliceNormals.push_back(normal);
            normal.rotateRad(TWO_PI / resolution, ofVec3f(1, 0, 0));
        }
    }
    
    void Path::addVertex(float x, float y, float z)
    {
        addVertex(ofVec3f(x, y, z));
    }
    
    void Path::addVertex(const ofVec3f& vertex)
    {
        linePosns.push_back(vertex);
        while (linePosns.size() > maxLineLength) linePosns.erase(linePosns.begin());
        
        points.push_back(vertex);
        while (points.size() > maxLineLength) points.pop_front();
        
        // path stuff
        if (points.size() > 1)
        {
            PathPoint& curr = points[points.size() - 1];
            PathPoint& prev = points[points.size() - 2];
            float elapsed = curr.getCreated() - prev.getCreated();
            points.back().setVel((curr.getPos() - prev.getPos()) / elapsed);
            if (points.size() > 2) points.back().setAcc((curr.getVel() - prev.getVel()) / elapsed);
            
            if (curr.getAcc().getNormalized().dot(prev.getAcc().getNormalized()) < 0)
            {
                inflections.push_back(0.5f * (curr.getPos() + prev.getPos()));
                curr.setInflection(true);
            }
        }
        
        // line
        if (maxLineLength != lineParticles.getWidth()) lineParticles.init(maxLineLength, 1, OF_PRIMITIVE_LINE_STRIP, false, 1);
        lineParticles.loadDataTexture(ofxGpuParticles::POSITION, linePosns[0].getPtr());
        //lineMesh.clear();
        //unsigned lineLength = fmin(maxLineLength, points.size());
        //unsigned fadeIdx = lineFadeLength * lineLength;
        /*for (unsigned i = 0; i < lineLength; ++i)
        {
            lineMesh.addVertex(points[i].getPos());
            float alpha = ofMap(i, 0, fadeIdx, 0.f, 1.f, true);
            lineMesh.addColor(ofFloatColor::fromHsb(ofMap(abs(points[i].getPos().x) + abs(points[i].getPos().y), 0, 50, hueMin, hueMax, true), 0.5, 1.0, alpha));
        }*/
        
        // mesh
        ptf.addPoint(vertex);
        if (ptf.getFrames().size())
        {
            bool firstLayer = mesh.getVertices().empty();
            ofMatrix4x4 transform = ptf.transformMatrix();
            ofMatrix4x4 normalMatrix = ptf.normalMatrix();
            for (unsigned i = 0; i < resolution; ++i)
            {
                vertices.push_back(sliceVertices[i] * transform);
                normals.push_back(sliceNormals[i] * normalMatrix);
                mesh.addVertex(vertices.back());
                mesh.addNormal(normals.back());
            }
            
            /*
            if (!firstLayer)
            {
                unsigned prevLayerIdx = mesh.getNumVertices() - 2 * resolution;
                unsigned currLayerIdx = mesh.getNumVertices() - resolution;
                for (unsigned i = 0; i < resolution; ++i)
                {
                    indices.push_back(prevLayerIdx + i);
                    indices.push_back(prevLayerIdx + (i + 1) % resolution);
                    indices.push_back(currLayerIdx + i);
                    
                    indices.push_back(currLayerIdx + i);
                    indices.push_back(prevLayerIdx + (i + 1) % resolution);
                    indices.push_back(currLayerIdx + (i + 1) % resolution);
                }
                
                for (unsigned i = 0; i < 6 * resolution; ++i)
                {
                    mesh.addIndex(indices[indices.size() - 6 * resolution + i]);
                }
                
                unsigned removed = 0;
                while (vertices.size() > resolution * Path::maxMeshLength)
                {
                    for (unsigned i = 0; i < resolution; ++i)
                    {
                        vertices.pop_front();
                        normals.pop_front();
                        for (unsigned j = 0; j < 6; ++j) indices.pop_front();
                    }
                    removed += resolution;
                }
                if (removed != 0)
                {
                    for (unsigned i = 0; i < indices.size(); ++i) indices[i] -= removed;
                    
                    mesh.getVertices().clear();
                    for (unsigned i = 0; i < vertices.size(); ++i) mesh.addVertex(vertices[i]);
                    
                    mesh.getNormals().clear();
                    for (unsigned i = 0; i < normals.size(); ++i) mesh.addNormal(normals[i]);
                    
                    mesh.getIndices().clear();
                    for (unsigned i = 0; i < indices.size(); ++i) mesh.addIndex(indices[i]);
                }
            }*/
        }
    }
    
    void Path::drawNormals(float size)
    {
        unsigned step = 8;
        for (unsigned i = 0; i < mesh.getNumVertices() / resolution; i += step)
        {
            for (unsigned j = 0; j < resolution; ++j)
            {
                ofVec3f vertex = mesh.getVertex(i * resolution + j);
                ofVec3f normal = mesh.getNormal(i * resolution + j);
                
                ofLine(vertex, vertex + size * normal);
            }
        }
    }
    
    void Path::drawMesh()
    {
        mesh.draw();
    }
    
    void Path::drawLine(float fogStart, float fogEnd)
    {
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetLineWidth(lineWidth);
        lineParticles.getDrawShaderRef().begin();
        lineParticles.getDrawShaderRef().setUniform1f("fogStart", fogStart);
        lineParticles.getDrawShaderRef().setUniform1f("fogEnd", fogEnd);
        lineParticles.getDrawShaderRef().setUniform1f("hueMin", hueMin);
        lineParticles.getDrawShaderRef().setUniform1f("hueMax", hueMax);
        lineParticles.getDrawShaderRef().end();
        lineParticles.draw();
        //lineMesh.draw();
        ofPopStyle();
        /*
        ofPushStyle();
        ofNoFill();
        ofBeginShape();
        for (unsigned i = 0; i < points.size(); ++i)
        {
            ofVec3f v = points[i].getPos();
            ofVertex(v);
        }
        ofEndShape();
        ofPopStyle();
         */
    }
    
    void Path::drawInflections()
    {
        ofPushStyle();
        ofSetColor(255, 255, 0);
        for (unsigned i = 0; i < inflections.size(); ++i)
        {
            ofSphere(inflections[i], 2);
        }
        ofPopStyle();
    }
    
    void Path::drawAcc()
    {
        ofPushStyle();
        for (unsigned i = 0; i < points.size(); ++i)
        {
            ofFloatColor col = ofFloatColor::fromHsb(ofMap(points[i].getAccMag(), 0, 500, 0, 1), 1, 1);
            ofSetColor(col);
            ofLine(points[i].getPos(), points[i].getPos() + 0.01f * points[i].getAcc());
        }
        ofPopStyle();
    }
    
    void Path::clear()
    {
        vertices.clear();
        normals.clear();
        indices.clear();
		mesh.clear();
        lineMesh.clear();
        ptf.clear();
        points.clear();
        inflections.clear();
    }
}
