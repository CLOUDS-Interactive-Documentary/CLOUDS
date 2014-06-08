//
//  lightManager.cpp
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-02-14.
//
//

#include "lightManager.h"

void lightManager::setup(colorSchemeDesigner * _cs) {
    cs = _cs;
    
    isShaderDirty = true;
    
	mLigDirectional.setup();
	mLigDirectional.setDirectional();
    
    
	mLigDirectional.setAmbientColor(ofColor::fromHsb(100, 0, 100));
	mLigDirectional.setDiffuseColor(ofColor::fromHsb(20, 120, 128));
	mLigDirectional.setSpecularColor(ofColor(255,255,255));
	
	mMatMainMaterial.setDiffuseColor(ofColor(0,0,0));
	mMatMainMaterial.setSpecularColor(ofColor(200,200,200));
	mMatMainMaterial.setShininess(64.0f);
	
    shouldRenderNormals = false;
	shouldUseFlatShading = false;
}

void lightManager::update() {

    if (isShaderDirty){
		
		GLuint err = glGetError();	// we need this to clear out the error buffer.
		
		if (mShdPhong != NULL ) delete mShdPhong;
		mShdPhong = new ofShader();
		mShdPhong->load("shaders/phong");
		err = glGetError();	// we need this to clear out the error buffer.
		ofLogNotice() << "Loaded Shader: " << err;
        
		
		isShaderDirty = false;
	}
}

void lightManager::begin() {

    glShadeModel(GL_SMOOTH);
    glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
    
    ofEnableLighting();
    
//    mLigDirectional.setAmbientColor( cs->colorScheme[0][2]);
//    mLigDirectional.setDiffuseColor( cs->colorScheme[0][1]);
//    mLigDirectional.setSpecularColor(255);
    mLigDirectional.setGlobalPosition(0, 150, 600);
    mLigDirectional.lookAt(ofVec3f(0,0,-600));
    
    ofEnableSeparateSpecularLight();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    mLigDirectional.enable();
    ofSetColor(mLigDirectional.getDiffuseColor());
    mMatMainMaterial.begin();
    
    mShdPhong->begin();
    
    if (shouldRenderNormals){
        mShdPhong->setUniform1f("shouldRenderNormals", 1.0);
    } else {
        mShdPhong->setUniform1f("shouldRenderNormals", 0.0);
    }
    
    glPushAttrib(GL_SHADE_MODEL);
    if (shouldUseFlatShading){
        mShdPhong->setUniform1f("shouldUseFlatShading", 1.0);
        glShadeModel(GL_FLAT);
        glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);		// OpenGL default is GL_LAST_VERTEX_CONVENTION
    } else {
        mShdPhong->setUniform1f("shouldUseFlatShading", 0.0);
        glShadeModel(GL_SMOOTH);
        glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
    }

}

void lightManager::end() {
//    material.end();
//    ofDisableLighting();
    
    // restores shade model
    glPopAttrib();
    // restores vertex convention defaults.
    glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
    
    mShdPhong->end();
    
    // we revert to default values, to not end up
    
    
    mMatMainMaterial.end();
    mLigDirectional.disable();
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
    ofDisableLighting();

}

