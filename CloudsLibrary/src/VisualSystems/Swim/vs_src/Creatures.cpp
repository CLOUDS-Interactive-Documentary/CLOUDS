/*
 *  CreatureController.cpp
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
#include "Creatures.h"

//#define _DEBUG

namespace itg
{
    void Creatures::init(const string& dataPath)
    {
        numGreyFish = 125;
        numYellowFish = 75;
        numJellyOne = 75;
        numJellyTwo = 75;
        zoneRadius = 500.f;
        alignmentLower = 0.33;
        alignmentUpper = 0.66;
        repelStrength = 0.2;
        attractStrength = 0.2;
        alignStrength = 0.2;
        maxDistFromCentre = 2000.f;
        
        // load model creature stuff
        ModelCreature::fishModels.push_back(ofxAssimpModelLoader());
        ModelCreature::fishModels.back().loadModel(dataPath + "models/plotva.3ds");
        ModelCreature::fishModels.back().setRotation(0, 180, 0, 0, 1);
        ModelCreature::fishModels.back().setRotation(1, 90, 0, 1, 0);
        ModelCreature::fishModels.back().setScale(0.3, 0.3, 0.3);
        ModelCreature::deformAxes.push_back(ofVec3f());
        ModelCreature::deformAxes.back().set(0, 0, 1);
        ModelCreature::bends.push_back(1.f);
        
        ModelCreature::fishModels.push_back(ofxAssimpModelLoader());
        ModelCreature::fishModels.back().loadModel(dataPath + "models/TropicalFish05.obj");
        ModelCreature::fishModels.back().setRotation(0, 180, 0, 0, 1);
        ModelCreature::fishModels.back().setScale(0.1, 0.1, 0.1);
        ModelCreature::deformAxes.push_back(ofVec3f());
        ModelCreature::deformAxes.back().set(1, 0, 0);
        ModelCreature::bends.push_back(1.f);
        
        ModelCreature::fishShader.load(dataPath + "shaders/fish");
        
        // load jelly stuff
        ofDisableArbTex();
        JellyCreature::tex.setCompression(OF_COMPRESS_ARB);
        JellyCreature::tex.loadImage(dataPath + "images/wf.png");
        JellyCreature::tex.bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        JellyCreature::tex.unbind();
        ofEnableArbTex();
        JellyCreature::shader.load(dataPath + "shaders/jelly");
        
        //generate();
    }
    
    void Creatures::generate()
    {
        creatures.clear();
        
        const float startArea = 1200.f;
        
        creaturesByType.push_back(vector<Creature::Ptr>());
        for (int i = 0; i < round(numGreyFish); ++i)
        {
            ModelCreature::Ptr modelCreature = ModelCreature::Ptr(new ModelCreature(0, ofFloatColor(0.7, 0.7, 0.7)));
            modelCreature->setSize(ofRandom(0.5, 1.5));
            creatures.push_back(modelCreature);
            creatures.back()->setVelocity(ofRandom(-100, 100), ofRandom(-100, 100), ofRandom(-100, 100));
            creaturesByType.back().push_back(creatures.back());
        }
        
        creaturesByType.push_back(vector<Creature::Ptr>());
        for (int i = 0; i < round(numYellowFish); ++i)
        {
            ModelCreature::Ptr modelCreature = ModelCreature::Ptr(new ModelCreature(1, ofFloatColor(ofRandom(0.6, 0.7), ofRandom(0.6, 0.7), 0.0)));
            modelCreature->setSize(ofRandom(0.5, 1.5));
            creatures.push_back(modelCreature);
            creatures.back()->setVelocity(ofRandom(-100, 100), ofRandom(-100, 100), ofRandom(-100, 100));
            creaturesByType.back().push_back(creatures.back());
        }
        
        // NB add jellies last as they are drawn with glDepthMask(GL_FALSE) to make additive blending
        // work so depth isn't stored
        creaturesByType.push_back(vector<Creature::Ptr>());
        for (int i = 0; i < round(numJellyOne); ++i)
        {
            creatures.push_back(JellyCreature::Ptr(new JellyCreature(jellyOneParams)));
            creatures.back()->setVelocity(ofRandom(-50, 50), ofRandom(-50, 50), ofRandom(-50, 50));
            creaturesByType.back().push_back(creatures.back());
        }
        
        creaturesByType.push_back(vector<Creature::Ptr>());
        for (int i = 0; i < round(numJellyTwo); ++i)
        {
            creatures.push_back(JellyCreature::Ptr(new JellyCreature(jellyTwoParams)));
            creatures.back()->setVelocity(ofRandom(-50, 50), ofRandom(-50, 50), ofRandom(-50, 50));
            creaturesByType.back().push_back(creatures.back());
        }
        
        for (unsigned i = 0; i < creatures.size(); ++i)
        {
            creatures[i]->setPosition(ofRandom(-startArea, startArea), ofRandom(-startArea, startArea), ofRandom(-startArea, 0));
            creatures[i]->setOrientation(ofVec3f(ofRandom(-180.f, 180.f), ofRandom(-180.f, 180.f), ofRandom(-180.f, 180.f)));
        }
    }
    
    void Creatures::update()
    {
        const float zoneRadiusSq = zoneRadius * zoneRadius;
        
        for (unsigned k = 0; k < creaturesByType.size(); ++k)
        {
            vector<ofVec3f> posns;
            for (int i = 0; i < creaturesByType[k].size(); ++i)
            {
                creaturesByType[k][i]->zeroAccumulated();
                creaturesByType[k][i]->setMagic(false);
                // make nn search so that it has an interface that can be implemented
                posns.push_back(creaturesByType[k][i]->getPosition());
            }
            nn.buildIndex(posns);
            
            // zero accumulated forces and then add forces
            // from fish fish interaction
            for (int i = 0; i < creaturesByType[k].size(); ++i)
            {
                const int numClosest = 8;
                vector<float> dists2;
                vector<NNIndex> indices;
                nn.findNClosestPoints(creaturesByType[k][i]->getPosition(), numClosest, indices, dists2);
                
                for (int j = 0; j < indices.size(); ++j)
                {
                    if (indices[j] != i)
                    {
                        Creature::Ptr creature = creaturesByType[k][indices[j]];
                        //if (creature->getType() == creaturesByType[k][i]->getType())
                        {
                            // towards creature i
                            ofVec3f dir = creaturesByType[k][i]->getPosition() - creature->getPosition();
                            
                            if( dists2[j] < zoneRadiusSq )
                            {
                                // Neighbor is in the zone
                                float percent = dists2[j]/zoneRadiusSq;
                                
                                if( percent < alignmentLower )
                                {
                                    // Separation
                                    float F = ( alignmentLower/percent - 1.0f ) * repelStrength;//repelStrength;
                                    dir = dir.normalized() * F;
                                    creaturesByType[k][i]->accumulate(dir);
                                }
                                else if( percent < alignmentUpper )
                                {	
                                    // Alignment
                                    float threshDelta		= alignmentUpper - alignmentLower;
                                    float adjustedPercent	= ( percent - alignmentLower )/threshDelta;
                                    float F					= ( 1.0 - ( cos( adjustedPercent * TWO_PI ) * -0.5f + 0.5f ) ) * alignStrength;// * alignStrength;
                                    ofVec3f force = creature->getNormalisedVelocity() * F;
                                    creaturesByType[k][i]->accumulate(force);
                                    
                                }
                                else
                                {								
                                    // Cohesion
                                    float threshDelta		= 1.0f - alignmentUpper;
                                    float adjustedPercent	= ( percent - alignmentUpper )/threshDelta;
                                    float F					= ( 1.0 - ( cos( adjustedPercent * TWO_PI ) * -0.5f + 0.5f ) ) * attractStrength;// * attractStrength;
                                    
                                    dir.normalize();
                                    dir *= F;
                                    ofVec3f force = -dir;
                                    creaturesByType[k][i]->accumulate(force);
                                }
                            }
                        }
                    }
                }
            }
        }
        

        for (int i = 0; i < creatures.size(); ++i)
        {
            // keep them in the centre
            ofVec3f dirToCenter = creatures[i]->getPosition();
            float distToCenter = dirToCenter.length();
            //static const float maxDistance = 3000.f;
            
            if( distToCenter > maxDistFromCentre )
            {
                dirToCenter.normalize();
                float pullStrength = 0.004f;
                ofVec3f force = -dirToCenter * ( ( distToCenter - maxDistFromCentre ) * pullStrength );
                creatures[i]->accumulate(force);
            }
            
            ofVec3f drag = -0.002 * creatures[i]->getVelocity();
            creatures[i]->accumulate(drag);
            
            creatures[i]->accumulateOtherForces();
            
            creatures[i]->integrate();
            creatures[i]->updateNormalisedVelocity();
            ofVec3f toMove = creatures[i]->getVelocity() * ofGetLastFrameTime();
#ifndef _DEBUG
            creatures[i]->move(toMove + sin(ofGetElapsedTimef() * creatures[i]->getFrequency()) * toMove * 0.4);
#endif
            /*ofQuaternion quat;
            quat.makeRotate(ofVec3f(0, 0, 1), creatures[i]->getNormalisedVelocity());
            creatures[i]->setOrientation(quat);
            */
            creatures[i]->slerp(creatures[i]->getPosition() - creatures[i]->getNormalisedVelocity(), ofVec3f(0, 1, 0));

			creatures[i]->update();
            
        }
    }
    
    void Creatures::draw()
    {
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_DEPTH_TEST);
        for (int i = 0; i < creatures.size(); ++i)
        {
            creatures[i]->draw();
        }
        glPopAttrib();
    }
}