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

class ofxLight
{
public:    
    ofxLight()
    {
        bEnabled = true;
//        light.setup();
        lightPos.set(0.0f,0.0f,0.0f);
        lightOrientation.set(0.0f,0.0f,0.0f);
        lightAmbient = ofFloatColor(.5f,.5f,.5f,1.f);
        lightDiffuse = ofFloatColor(.5f,.5f,.5f,1.f);
        lightSpecular = ofFloatColor(.5f,.5f,.5f,1.f);
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
    ofFloatColor lightAmbient;
    ofFloatColor lightDiffuse;
    ofFloatColor lightSpecular;    
};

#endif