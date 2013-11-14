/*
 *  Bubbles.cpp
 *
 *  Copyright (c) 2012, Neil Mendoza, http://www.neilmendoza.com
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
#include "Bubbles.h"
#include "SphericalGenerator.h"
#include "CircularFormula.h"
#include "MeshUtils.h"

namespace itg
{
    using namespace itg;
    using namespace mesh;
    
    void Bubbles::init(const string& dataPath)
    {
        string prefix = dataPath + "images/cubemap_night/night_";
        string suffix = "png";
        cubeMap.loadImages(prefix + "positive_x." + suffix, prefix + "positive_y." + suffix, prefix + "positive_z." + suffix, 
                           prefix + "negative_x." + suffix, prefix + "negative_y." + suffix, prefix + "negative_z." + suffix);
        shader.load(dataPath + "shaders/bubble");
        CircularFormula::Ptr f(new CircularFormula());
        SphericalGenerator gen(f, f);
        gen.genSpiralMesh(mesh);
        itg::mesh::genNormals(mesh);
        
        nextBubbleBlast = 0;
    }
    
    void Bubbles::update()
    {
        if (ofGetElapsedTimeMillis() > nextBubbleBlast)
        {
            int numBubbles = ofRandom(2, 30);
            float centre = ofRandom(-1000, 1000);
            for (int i = 0; i < numBubbles; ++i)
            {
                bubbles.push_back(Bubble());
                bubbles.back().pos = ofVec3f(centre + ofRandom(-2000, 2000), -3000 + ofRandom(-300, 300), 0);
                //bubbles.back().vel = ofVec3f(0, ofRandom(0.1, 1.0), 0);
                bubbles.back().size = ofRandom(3, 10);
                bubbles.back().vel = ofVec3f(0.f, ofMap(bubbles.back().size, 3, 10, 0.8, 0.1), 0.f);
            }
            nextBubbleBlast = ofGetElapsedTimeMillis() + ofRandom(4000, 10000);
        }
        for (list<Bubble>::iterator it = bubbles.begin(); it != bubbles.end(); ++it)
        {
            it->pos += it->vel * ofGetElapsedTimef();
            if (it->pos.y > 4000) it = bubbles.erase(it);
        }
    }
    
    void Bubbles::draw()
    {
        ofEnableAlphaBlending();
        shader.begin();
        cubeMap.enableFixedMapping();
        cubeMap.bindMulti(0);
        shader.setUniform1i("cubeMap", 0);
        
        for (list<Bubble>::iterator it = bubbles.begin(); it != bubbles.end(); ++it)
        {
            shader.setUniform1f("size", it->size);
            shader.setUniform4f("trans", it->pos.x, it->pos.y, it->pos.z, 0.0);
            mesh.draw();
        }
        cubeMap.unbind();
        cubeMap.disableFixedMapping();
        shader.end();
        ofDisableAlphaBlending();
    }
}