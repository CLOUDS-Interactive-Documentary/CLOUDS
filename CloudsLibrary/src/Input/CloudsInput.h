//
//  CloudsInput.h
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#pragma once

#include "ofMain.h"

class CloudsInputEvents;
class CloudsInput
{
  public:
	CloudsInput();
	
	virtual void enable() = 0;
	virtual void disable() = 0;
	
	void interactionMoved(ofVec3f pos, int actionType = 0, int playerId = 0);
	void interactionStarted(ofVec3f pos, int actionType = 0, int playerId = 0);
	void interactionDragged(ofVec3f pos, int actionType = 0, int playerId = 0);
	void interactionEnded(ofVec3f pos, int actionType = 0, int playerId = 0);

	bool isDragging();
	float getPositionX();
	float getPositionY();
	float getPositionZ();
	ofVec3f getPosition();
	
	CloudsInputEvents& getEvents();
	
  protected:

	ofVec3f currentPosition;
	bool dragging;

	bool enabled;
	CloudsInputEvents* events;
	
};

void SetCloudsInput(ofPtr<CloudsInput> input);
ofPtr<CloudsInput> GetCloudsInput();

//global accessors, replacement for ofGetMouseX(), etc
bool GetCloudsInputPressed();
float GetCloudsInputX();
float GetCloudsInputY();
float GetCloudsInputZ();
ofVec3f GetCloudsInputPosition();
