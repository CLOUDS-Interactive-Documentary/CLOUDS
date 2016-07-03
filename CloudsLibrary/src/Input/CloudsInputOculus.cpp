//
//  CloudsInputOculus.cpp
//  PointCloudPeople
//
//  Created by Elie Zananiri on 1/8/2014.
//
//

#ifdef OCULUS_RIFT

#include "CloudsInputOculus.h"
#include "CloudsVisualSystem.h"

CloudsInputOculus::CloudsInputOculus()
: cursorSize(1)
{}

void CloudsInputOculus::enable(){
	if(!enabled){
        ofAddListener(ofEvents().update, this, &CloudsInputOculus::update);
		enabled = true;
	}
}

void CloudsInputOculus::disable(){
	if(enabled){
        ofRemoveListener(ofEvents().update, this, &CloudsInputOculus::update);
		enabled = false;
	}
}

void CloudsInputOculus::update(ofEventArgs& data){
    lastPosition = currentPosition;
    currentPosition = (CloudsVisualSystem::getOculusRift().gazePosition2D());
	//HACK to remove camera drift
	//currentPosition = ofVec3f(0,0,0);
    if (currentPosition.distance(lastPosition) > 0) {
        interactionMoved(currentPosition, true, false, 0);
    }
}

void CloudsInputOculus::drawCursorDefault(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus, float fadeOut){
    selfDrawCursorDefault(mode, pos, bDragged, focus * (bDragged? 1.0f:0.35f), fadeOut, cursorSize);
}

void SetCloudsInputOculus()
{
    SetCloudsInput(ofPtr<CloudsInput>(new CloudsInputOculus()));
}

#endif
