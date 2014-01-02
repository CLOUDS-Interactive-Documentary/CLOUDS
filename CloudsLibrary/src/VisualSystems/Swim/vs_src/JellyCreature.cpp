/*
 *  JellyCreature.cpp
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
#include "JellyCreature.h"
#include "SuperFormula.h"
#include "SphericalGenerator.h"
#include "MeshUtils.h"
#include "CircularFormula.h"

namespace itg
{
    using namespace itg;
    using namespace mesh;
    
    ofShader JellyCreature::shader;
    ofImage JellyCreature::tex;
    
    JellyCreature::JellyCreature(const JellyParams& params) : Creature()
    {
        type = JELLY;
        
        bodyColour = ofFloatColor::fromHsb(ofRandom(params.bodyHMin, params.bodyHMax), params.bodyS, params.bodyB);
        tentacleColour = ofFloatColor::fromHsb(ofRandom(params.tentaclesHMin, params.tentaclesHMax), params.tentaclesS, params.tentaclesB);
        
        bodyAlpha = params.bodyAlpha;
        
        m1 = ofRandom(params.m1Min, params.m1Max);
        m2 = ofRandom(params.m2Min, params.m2Max);
        
        pulseAmt = ofRandom(params.pulseAmtMin, params.pulseAmtMax);
        
        segment = ofRandom(params.segmentMin, params.segmentMax);
        
        n11 = .5;
        n21 = 1.7;
        n31 = 1.7;
        a1 = 1;
        b1 = 1;
        
        n12 = .5;
        n22 = 1.7;
        n32 = 1.7;
        a2 = 1;
        b2 = 1;
        
        drawInner = true;
        
        float w = ofRandom(params.widthMin, params.widthMax);//randomGauss(params.widthAverage, params.widthStdDeviation);
        float l = ofRandom(params.lengthMin, params.lengthMax);//randomGauss(params.lengthAverage, params.lengthStdDeviation);
        
        size = ofVec3f(w, w, l);
        
        deformAmount = 0.5 * w;
        
        frequency = ofRandom(0.3, 1.0);
        
        texRepeatS = 4;
        
        genMeshes();
    }
    
    void JellyCreature::genMeshes()
    {
        const unsigned resolution = 30;
        
        outerMesh.clear();
        SuperFormula::Ptr sf1(new SuperFormula(round(m1), n11, n21, n31, a1, b1));
        SuperFormula::Ptr sf2(new SuperFormula(round(m2), n12, n22, n32, a2, b2));
        SphericalGenerator gen(sf1, sf2, size);
        gen.genStackedMesh(outerMesh, 1.f, 1.f, resolution, resolution, segment);
        itg::mesh::genNormals(outerMesh);
        
        innerMesh.clear();
        SphericalGenerator gen2(sf1, sf2, size * ofVec3f(.9f, .9f, 1.f));
        gen2.genStackedMesh(innerMesh, 1.f, 1.f, resolution, resolution, 1.2 * segment);
        itg::mesh::genNormals(innerMesh);
        
        lineMesh.clear();
        lineMesh.setMode(OF_PRIMITIVE_LINES);
        const unsigned numLines = 4;
        for (unsigned k = 0; k < numLines; ++k)
        {
            for (unsigned i = 0; i < 0.5 * round(m1); ++i)
            {
                float phi = 2 * (i + 0.5) * TWO_PI / (float)round(m1);
                for (unsigned j = 0; j < resolution; ++j)
                {
                    float theta = 1.4 * j * segment / (float)resolution;
                    lineMesh.addVertex(size * superVertex(phi + 0.08 * ofNoise(2.f * theta, phi) * k / (float)numLines, theta));
                    lineMesh.addTexCoord(ofVec2f(phi / TWO_PI, theta / segment));
                    if (j != 0)
                    {
                        lineMesh.addIndex(lineMesh.getNumVertices() - 2);
                        lineMesh.addIndex(lineMesh.getNumVertices() - 1);
                    }
                }
            }
        }
        
        for (unsigned i = 0; i < resolution; ++i)
        {
            float phi = TWO_PI * i / (float)resolution;
            lineMesh.addVertex(size * superVertex(phi, segment));
            lineMesh.addTexCoord(ofVec2f(phi / TWO_PI, 1.f));
            if (i != 0)
            {
                lineMesh.addIndex(lineMesh.getNumVertices() - 2);
                lineMesh.addIndex(lineMesh.getNumVertices() - 1);
            }
        }
        lineMesh.addIndex(lineMesh.getNumVertices() - 1);
        lineMesh.addIndex(lineMesh.getNumVertices() - resolution);
    }
    
    void JellyCreature::update()
    {
    }
    
    ofVec3f JellyCreature::superVertex(float phi, float theta)
    {
        SuperFormula f1(round(m1), n11, n21, n31, a1, b1);
        SuperFormula f2(round(m2), n12, n22, n32, a2, b2);
        float sinTheta = std::sin(theta);
        float sinPhi = std::sin(phi);
        float cosTheta = std::cos(theta);
        float cosPhi = std::cos(phi);
        float f1Phi = f1.radius(phi);
        float f2Theta = f2.radius(theta);
        return ofVec3f(f1Phi * cosPhi * f2Theta * sinTheta,
                       f1Phi * sinPhi * f2Theta * sinTheta,
                       f2Theta * cosTheta);
    }
    
    void JellyCreature::setShaderUniforms()
    {
        const float DEFORM_AMOUNT = 40.f;
        shader.setUniform1f("deformAmount", deformAmount);
        shader.setUniform1f("time", ofGetElapsedTimef());
        shader.setUniform1f("frequency", frequency);
        shader.setUniformTexture("tex", tex, 0);
        shader.setUniform1f("texRepeatS", texRepeatS);
        shader.setUniform1f("fogStart", fogStart);
        shader.setUniform1f("fogEnd", fogEnd);
#warning take light pos from visual system
        shader.setUniform3f("lightPos", 1000, 0, 1000);
    }

    void JellyCreature::customDraw()
    {
        shader.begin();
        setShaderUniforms();
        
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        glDepthMask(GL_FALSE);
        
        shader.setUniform1f("texAmount", 1.f);
        shader.setUniform1f("lightingAmount", 1.f);
        shader.setUniform4f("colour", bodyColour.r, bodyColour.g, bodyColour.b, 255 / 255.f);
        outerMesh.draw();
        
        shader.setUniform1f("texAmount", 0.f);
        shader.setUniform1f("lightingAmount", 1.f);
        shader.setUniform4f("colour", bodyColour.r, bodyColour.g, bodyColour.b, bodyAlpha + pulseAmt * sin(ofGetElapsedTimef() * frequency));
        if (drawInner) innerMesh.draw();
        
        shader.setUniform1f("texAmount", 0.f);
        shader.setUniform1f("lightingAmount", 0.f);
        shader.setUniform4f("colour", tentacleColour.r, tentacleColour.g, tentacleColour.b, 40 / 255.f);
        
        glLineWidth(.2f);
        lineMesh.draw();
        glLineWidth(1.f);
        
        glDepthMask(GL_TRUE);
        ofDisableBlendMode();
        ofPopStyle();
        
        shader.end();
    }
}
