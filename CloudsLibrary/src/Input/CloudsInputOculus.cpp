//
//  CloudsInputOculus.cpp
//  PointCloudPeople
//
//  Created by Elie Zananiri on 1/8/2014.
//
//

#include "CloudsInputOculus.h"
#include "CloudsVisualSystem.h"

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
    if (currentPosition.distance(lastPosition) > 0) {
        interactionMoved(currentPosition, true, 0);
    }
}

void SetCloudsInputOculus()
{
    SetCloudsInput(ofPtr<CloudsInput>(new CloudsInputOculus()));
}
