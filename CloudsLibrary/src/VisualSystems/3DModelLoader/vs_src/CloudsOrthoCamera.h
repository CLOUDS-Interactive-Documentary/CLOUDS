//
//  CloudsOrthoCamera.h
//  3DModelVisualSystem
//
//  Created by laserstorm on 10/7/13.
//
//

#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"

class CloudsOrthoCamera : public ofCamera {
public:
	
	CloudsOrthoCamera();
	~CloudsOrthoCamera();
	
	// TODO: this should be ofGetViewRect() eventually
	virtual void begin(ofRectangle viewport = ofGetCurrentViewport() );
	void reset();
	
	//----------------------------------------
	// advanced functions
	
	void setTarget(const ofVec3f& target);
	void setTarget(ofNode& target);
	ofNode& getTarget();
	
	void setDistance(float distance);
	float getDistance() const;
	
	// drag is how quickly the camera picks up and slows down
	// it is a normalized value between 0-1
	void setDrag(float drag);
	float getDrag() const;
	// the translation key is the key used to switch between rotation and translation.
	// translation happens only when the key is pressed.
	void setTranslationKey(char key);
    char getTranslationKey();
	// enable or disable mouse input to navigate
	void enableMouseInput();
	void disableMouseInput();
	bool getMouseInputEnabled();
	
	void enableMouseMiddleButton();
	void disableMouseMiddleButton();
	bool getMouseMiddleButtonEnabled();
	
    void setAutoDistance(bool bAutoDistance);
	
	//----------------------------------------
	ofMatrix4x4 getProjectionMatrix(ofRectangle viewport = ofGetCurrentViewport()) const;
	//----------------------------------------
	ofMatrix4x4 getModelViewMatrix(ofRectangle viewport = ofGetCurrentViewport()) const;
	
	
	void setMaxOrbitDistance( float dist ){
		maxOrbitDistance = dist;
	}
	void setMinOrbitDistance( float dist ){
		minOrbitDistance = dist;
	}
	float getMaxOrbitDistance( float dist ){
		return maxOrbitDistance;
	}
	void getMinOrbitDistance( float dist ){
		return minOrbitDistance;
	}
	
	void addSlidersToGui( ofxUISuperCanvas* gui, string label );
	
	bool bExploreMode;
	bool bOrbitMode;
	bool bDisableEasyCamControls;
	
	void setToStartPosition(ofVec3f targetPos = ofVec3f());
	
private:
	void setDistance(float distance, bool save);
	
	ofNode target;
	
	
	bool bEnableMouseMiddleButton;
	bool bApplyInertia;
	bool bDoTranslate;
	bool bDoRotate;
	bool bValidClick;
	bool bInsideArcball;
	bool bMouseInputEnabled;
	bool bDistanceSet;
    bool bAutoDistance;
	float lastDistance;
	
	float drag;
	
	float xRot;
	float yRot;
	float zRot;
	
	float moveX;
	float moveY;
	float moveZ;
	
	float sensitivityXY;
	float sensitivityZ;
	float sensitivityRot;
	
	float rotationFactor;
	
	ofVec2f mouse;
	ofVec2f lastMouse;
	ofVec2f mouseVel;
	
	void updateRotation();
	void updateTranslation();
	void update(ofEventArgs & args);
	void updateMouse();
	
	char doTranslationKey;
	
	unsigned long lastTap;
	
	ofQuaternion curRot;
    
	ofRectangle viewport;// having the  viewport saved localy will make it easier for all the needed maths dealing with viewport.
	
	float orthoViewScale;
	
	ofVec2f orbitVel;
	float maxOrbitDistance, minOrbitDistance, orbitZoomFrequency;
	
	
	ofxUISuperCanvas* gui;
	float mouseScl, deadZone, cameraSpeed, pitchScale, tiltLimit, orbitVelAttenuation, minTilt, maxTilt;
};