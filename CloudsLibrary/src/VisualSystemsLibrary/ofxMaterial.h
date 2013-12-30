
#pragma once

#include "ofMaterial.h"

class ofxMaterial : public ofMaterial
{
  public:
	
    virtual void begin();
	virtual void end();
	
	ofFloatColor matDiffuseHSV;
	ofFloatColor matSpecularHSV;
	ofFloatColor matEmissiveHSV;
	ofFloatColor matAmbientHSV;
	float matShininess;
	
  protected:
    
	ofFloatColor prev_diffuse_front, prev_diffuse_back;
	ofFloatColor prev_ambient_front, prev_ambient_back;
	ofFloatColor prev_specular_front, prev_specular_back;
	ofFloatColor prev_emissive_front, prev_emissive_back;
	float prev_shininess_front, prev_shininess_back;
	
};
