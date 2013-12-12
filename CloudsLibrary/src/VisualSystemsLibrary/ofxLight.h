/**********************************************************************************
 
 Copyright (C) 2012 Syed Reza Ali (www.syedrezaali.com)
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 **********************************************************************************/

#ifndef OFX_LIGHT
#define OFX_LIGHT

#include "ofMath.h"
#include "ofColor.h"
#include "ofLight.h"
#include "ofMaterial.h"

class ofxLight
{
public:    
    ofxLight()
    {
        bEnabled = true;
//        light.setup();
        lightPos.set(0.0f,0.0f,0.0f);
        lightOrientation.set(0.0f,0.0f,0.0f);
//		ofFloatColor(.5f,.5f,.5f,1.f);
        lightAmbientHSV  = ofFloatColor(1.0f, .0f, .5f, 1.f);
        lightDiffuseHSV  = ofFloatColor(1.0f, .0f, .5f, 1.f);
        lightSpecularHSV = ofFloatColor(1.0f, .0f, .5f, 1.f);
		
        lightSpotCutOff = 45.0f;
        lightExponent = 0.0f;
		
//		bEnabled = false;
    }
    
    ~ofxLight()
    {
        
    }
    
    void enable()
    {
        if(bEnabled)
        {
            switch(light.getType())
            {
                case OF_LIGHT_POINT:
                {
                    light.setPosition(lightPos);
                }
                    break;
                case OF_LIGHT_SPOT:
                {
                    light.setPosition(lightPos);
                    light.setOrientation(lightOrientation);
                    light.setSpotlightCutOff(lightSpotCutOff);
                    light.setSpotConcentration(lightExponent);
                }
                    break;
                    
                case OF_LIGHT_DIRECTIONAL:
                {                    
                    light.setOrientation(lightOrientation);
                }
                    break;
            }

			ofFloatColor lightAmbient  = ofFloatColor::fromHsb(lightAmbientHSV.r, lightAmbientHSV.g, lightAmbientHSV.b);
			ofFloatColor lightDiffuse  = ofFloatColor::fromHsb(lightDiffuseHSV.r, lightDiffuseHSV.g, lightDiffuseHSV.b);
			ofFloatColor lightSpecular = ofFloatColor::fromHsb(lightSpecularHSV.r, lightSpecularHSV.g, lightSpecularHSV.b);
			
            light.setAmbientColor(lightAmbient);
            light.setDiffuseColor(lightDiffuse);
            light.setSpecularColor(lightSpecular);
            light.enable();
        }
        else
        {
            light.disable();
        }
        
    }
    
    void disable()
    {    
        light.disable();
    }
    
    bool bEnabled; 
    
    float lightSpotCutOff;
    float lightExponent;
    ofLight light;
    ofVec3f lightPos;
    ofVec3f lightOrientation;
    ofFloatColor lightAmbientHSV;
    ofFloatColor lightDiffuseHSV;
    ofFloatColor lightSpecularHSV;
};


class ofxMaterial
{
  public:
	
//    ofxMaterial(){}
//    ~ofxMaterial(){}
    
    void begin()
    {
		ofFloatColor matAmbient  = ofFloatColor::fromHsb(matAmbientHSV.r,  matAmbientHSV.g,  matAmbientHSV.b);
		ofFloatColor matDiffuse  = ofFloatColor::fromHsb(matDiffuseHSV.r,  matDiffuseHSV.g,  matDiffuseHSV.b);
		ofFloatColor matEmissive = ofFloatColor::fromHsb(matEmissiveHSV.r, matEmissiveHSV.g, matEmissiveHSV.b);
		ofFloatColor matSpecular = ofFloatColor::fromHsb(matSpecularHSV.r, matSpecularHSV.g, matSpecularHSV.b);
		
		mat.setAmbientColor(matAmbient);
		mat.setDiffuseColor(matDiffuse);
		mat.setSpecularColor(matSpecular);
		mat.setEmissiveColor(matEmissive);
		mat.setShininess(matShininess);
		
		mat.begin();
    }
    
    void end(){
        mat.end();
    }
    
//    bool bEnabled;
    
    ofMaterial mat;
	
	float matShininess;
	ofFloatColor matDiffuseHSV;
	ofFloatColor matSpecularHSV;
	ofFloatColor matEmissiveHSV;
	ofFloatColor matAmbientHSV;
	
};

#endif