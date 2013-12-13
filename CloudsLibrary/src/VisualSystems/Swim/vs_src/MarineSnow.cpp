/*
 *  MarineSnow.cpp
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
#include "MarineSnow.h"
#include "Creature.h"

namespace itg
{
    void MarineSnow::init(const string& dataPath, unsigned numParticles)
    {
        this->numParticles = numParticles;
        mesh.setMode(OF_PRIMITIVE_POINTS);
        shader.load(dataPath + "shaders/snow");
        ofDisableArbTex();
        tex.setCompression(OF_COMPRESS_ARB);
        tex.loadImage(dataPath + "images/snow.png");
        tex.bind();
        glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
        tex.unbind();
        ofEnableArbTex();
    }
    
    void MarineSnow::generate()
    {
        mesh.clear();
        for (unsigned i = 0; i < numParticles; ++i)
        {
            mesh.addVertex(ofVec3f(ofRandom(-Creature::fogEnd, Creature::fogEnd),
                                   ofRandom(-Creature::fogEnd, Creature::fogEnd),
                                   ofRandom(-Creature::fogEnd, 0)));
            
            mesh.addColor(ofFloatColor::fromHsb(ofRandom(0.1f, 0.3f), ofRandom(0.f, 0.8f), 1.f, ofRandom(0.2f, 0.8f)));
            
            // stick texture offset and size into normal
            ofVec3f data;
            switch (rand() % 4)
            {
                case 0:
                    data.x = 0.f;
                    data.y = 0.f;
                    break;
                
                case 1:
                    data.x = .5f;
                    data.y = 0.f;
                    break;
                    
                case 2:
                    data.x = 0.f;
                    data.y = .5f;
                    break;
                
                case 3:
                    data.x = .5f;
                    data.y = .5f;
                    break;
            }
            data.z = ofRandom(0.1f, 100.f);
            mesh.addNormal(data);
        }
    }
    
    void MarineSnow::draw(const ofCamera& cam)
    {
        ofPushStyle();
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);
        ofEnableAlphaBlending();
        shader.begin();
        shader.setUniform1f("fogEnd", Creature::fogEnd);
        shader.setUniform1f("nearClip", cam.getNearClip());
        shader.setUniform1f("camZ", cam.getPosition().z);
        shader.setUniformTexture("tex", tex, 1);
        mesh.draw();
        shader.end();
        glPopAttrib();
        ofPopStyle();
    }
}
