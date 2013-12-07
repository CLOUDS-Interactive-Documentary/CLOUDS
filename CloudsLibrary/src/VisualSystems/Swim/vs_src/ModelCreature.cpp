/*
 *  ModelCreature.cpp
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
#include "ModelCreature.h"

namespace itg
{
    vector<ofxAssimpModelLoader> ModelCreature::fishModels;
    vector<ofVec3f> ModelCreature::deformAxes;
    vector<float> ModelCreature::bends;
    map<unsigned, ofImage> ModelCreature::textures;
    ofShader ModelCreature::fishShader;
    float ModelCreature::texAmount = 1.f;
    
    ModelCreature::ModelCreature(const ModelParams& params) : Creature()
    {
        type = MODEL;
        modelIdx = params.modelIdx;
        colour = params.colour;
        size = randomGauss(params.sizeAverage, params.sizeStdDeviation);
        
        frequency = ofRandom(2, 5);
    }
    
    void ModelCreature::integrate()
    {
        velocity += accumulated;
        // slow down y velocity to stop fish swimming upwards and downwards
        velocity.y *= 0.1f;
    }
    
    void ModelCreature::customDraw()
    {
        fishShader.begin();
        fishShader.setUniform3f("lightPos", 1000, 2000, 0);
        fishShader.setUniform1f("time", ofGetElapsedTimef());
        fishShader.setUniform1f("frequency", frequency);
        //fishShader.setUniformTexture("tex", fishTextures[modelIdx].getTextureReference(), 0);
        fishShader.setUniform1f("size", size);
        fishShader.setUniform3fv("deformAxis", deformAxes[modelIdx].getPtr());
        fishShader.setUniform1f("bend", bends[modelIdx]);
        fishShader.setUniform3f("colour", colour.r, colour.g, colour.b);
        if (textures.find(modelIdx) != textures.end())
        {
            fishShader.setUniformTexture("tex", textures[modelIdx], 0);
            fishShader.setUniform1f("texAmt", texAmount);
        }
        else fishShader.setUniform1f("texAmt", 0.f);
        fishModels[modelIdx].drawFaces();
        fishShader.end();
    }
}