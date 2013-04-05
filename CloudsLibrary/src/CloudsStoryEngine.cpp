//
//  CloudsStoryEngine.cpp
//  CloudsFCPParser
//
//  Created by James George on 3/17/13.
//
//

#include "CloudsStoryEngine.h"

CloudsStoryEngine::CloudsStoryEngine(){
	visualizer = NULL;
}

CloudsStoryEngine::~CloudsStoryEngine(){
	
}

void CloudsStoryEngine::setup(){
	
}

void CloudsStoryEngine::selectNewClip(){
	
	if(visualizer == NULL){
		return;
	}
	
	if(visualizer->currentOptionClips.size() > 0){
		int randomOption = ofRandom( visualizer->currentOptionClips.size()-1 );
		cout << "picking random of " << randomOption << "/" << visualizer->currentOptionClips.size() << endl;
		ClipMarker m = visualizer->currentOptionClips[ randomOption ];
		
		visualizer->addLinksToPhysics( m );
	}

}