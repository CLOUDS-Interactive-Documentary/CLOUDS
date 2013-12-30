//
//  ofxMaterial.cpp
//  CloudsVisualSystemEditor
//
//  Created by James George on 12/30/13.
//
//

#include "ofxMaterial.h"

void ofxMaterial::begin(){
	
	ofFloatColor matAmbient  = ofFloatColor::fromHsb(matAmbientHSV.r,  matAmbientHSV.g,  matAmbientHSV.b);
	ofFloatColor matDiffuse  = ofFloatColor::fromHsb(matDiffuseHSV.r,  matDiffuseHSV.g,  matDiffuseHSV.b);
	ofFloatColor matEmissive = ofFloatColor::fromHsb(matEmissiveHSV.r, matEmissiveHSV.g, matEmissiveHSV.b);
	ofFloatColor matSpecular = ofFloatColor::fromHsb(matSpecularHSV.r, matSpecularHSV.g, matSpecularHSV.b);
		
	glGetMaterialfv(GL_FRONT,GL_DIFFUSE, &prev_diffuse_front.r);
	glGetMaterialfv(GL_FRONT,GL_SPECULAR, &prev_specular_front.r);
	glGetMaterialfv(GL_FRONT,GL_AMBIENT, &prev_ambient_front.r);
	glGetMaterialfv(GL_FRONT,GL_EMISSION, &prev_emissive_front.r);
	glGetMaterialfv(GL_FRONT, GL_SHININESS, &prev_shininess_front);
	
	glGetMaterialfv(GL_BACK,GL_DIFFUSE,&prev_diffuse_back.r);
	glGetMaterialfv(GL_BACK,GL_SPECULAR,&prev_specular_back.r);
	glGetMaterialfv(GL_BACK,GL_AMBIENT,&prev_ambient_front.r);
	glGetMaterialfv(GL_BACK,GL_EMISSION,&prev_emissive_back.r);
	glGetMaterialfv(GL_BACK, GL_SHININESS, &prev_shininess_back);

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &matDiffuse.r);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &matAmbient.r);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &matSpecular.r);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &matEmissive.r);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &matShininess);
	
}

void ofxMaterial::end(){
	
	glMaterialfv(GL_FRONT,GL_DIFFUSE,&prev_diffuse_front.r);
	glMaterialfv(GL_FRONT,GL_SPECULAR,&prev_specular_front.r);
	glMaterialfv(GL_FRONT,GL_AMBIENT,&prev_ambient_front.r);
	glMaterialfv(GL_FRONT,GL_EMISSION,&prev_emissive_front.r);
	glMaterialfv(GL_FRONT,GL_SHININESS, &prev_shininess_front);

	glMaterialfv(GL_BACK,GL_DIFFUSE,&prev_diffuse_back.r);
	glMaterialfv(GL_BACK,GL_EMISSION,&prev_emissive_back.r);
	glMaterialfv(GL_BACK,GL_SPECULAR,&prev_specular_back.r);
	glMaterialfv(GL_BACK,GL_AMBIENT,&prev_ambient_back.r);
	glMaterialfv(GL_BACK,GL_SHININESS, &prev_shininess_back);
	
}