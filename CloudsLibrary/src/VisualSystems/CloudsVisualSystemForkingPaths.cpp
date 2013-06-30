
#include "CloudsVisualSystemForkingPaths.h"

//--------------------------------------------------------------
CloudsVisualSystemForkingPaths::CloudsVisualSystemForkingPaths(){
	regenerate = false;
	speed = 10;
	forkPercent = .01;
	trackDistance = 30;
}

//--------------------------------------------------------------
string CloudsVisualSystemForkingPaths::getSystemName(){
	return "ForkingPaths";
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::selfSetup(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::selfBegin(){
	
	generateTracks();
	addPath();
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::selfSetupGuis(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::generateTracks(){
	
	tracks.clear();
	usedTracks.clear();
	
	for(float i = 0.0f; i < ofGetHeight(); i+=trackDistance){
		tracks.push_back(i);
		usedTracks.push_back( false );
	}
	
	regenerate = false;
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::selfUpdate(){
	vector<ForkingPath> newForks;
	for(int i = 0; i < paths.size(); i++){
		
		paths[i].currentPos += paths[i].currentDirection * speed;
		if(abs(paths[i].currentPos.y - paths[i].targetTrackHeight) < trackDistance ){
			paths[i].currentDirection = ofVec2f(1,0);
			
			//line it up to the track
			paths[i].currentPos.x += abs(paths[i].currentPos.y - paths[i].targetTrackHeight);
			paths[i].currentPos.y = paths[i].targetTrackHeight;

			paths[i].foundTrack = true;
		}
		
		paths[i].track.addVertex(paths[i].currentPos);
		paths[i].backset += speed;
		if(paths[i].foundTrack){
			if(ofRandomuf() < forkPercent){
				forkPath(paths[i],newForks);
			}
		}
		
		//cout << " backset  " << paths[i].backset << " and position  " << paths[i].currentPos << endl;
	}
	
	for(int i = 0; i < newForks.size(); i++){
		paths.push_back( newForks[i] );
	}
}

int CloudsVisualSystemForkingPaths::closestUnusedTrack(int trackIndex){

	int currentTrack = 0;
	while(true){
		int trackAbove = trackIndex + currentTrack;
		int trackBelow = trackIndex - currentTrack;
		if(ofRandomuf() > .5) swap(trackAbove,trackBelow);
		bool aboveInRange = trackAbove >= 0 && trackAbove < usedTracks.size();
		bool belowInRange = trackBelow >= 0 && trackBelow < usedTracks.size();
		
		//bail, no more tracks
		if(!aboveInRange && !belowInRange){
			return trackIndex;
		}
		
		if(aboveInRange && !usedTracks[trackAbove]){
			return trackAbove;
		}
		else if(belowInRange && !usedTracks[trackBelow]){
			return trackBelow;
		}
		
		currentTrack++;
	}
}

void CloudsVisualSystemForkingPaths::addPath(){
    ForkingPath path;
	
	path.trackIndex = tracks.size()/2;
	path.targetTrackHeight = tracks[path.trackIndex];
	path.forkPosition = path.currentPos = ofVec2f(ofGetWidth()/2, path.targetTrackHeight);
	
	usedTracks[path.trackIndex] = true;
	
	path.currentDirection = ofVec2f(1,0).getRotated(45);
	path.track.addVertex(path.forkPosition);
	
	paths.push_back(path);	
}


bool CloudsVisualSystemForkingPaths::forkPath(ForkingPath& path, vector<ForkingPath>& paths){
    ForkingPath fork;
	fork.forkPosition = fork.currentPos = path.currentPos - ofVec2f(path.backset,0);
	
	fork.trackIndex = closestUnusedTrack(path.trackIndex);
	if(fork.trackIndex == path.trackIndex){
		return false;
	}
	
	usedTracks[fork.trackIndex] = true;
	
	bool up = fork.trackIndex < path.trackIndex;
	fork.currentDirection = ofVec2f(1,0).getRotated( up ? -45 : 45  );
	fork.targetTrackHeight = tracks[fork.trackIndex];
	
	fork.track.addVertex(fork.currentPos);
	
	paths.push_back(fork);
}

void CloudsVisualSystemForkingPaths::selfDrawBackground(){
	for(int i = 0; i < paths.size(); i++){
		ofPushMatrix();
		ofTranslate(-paths[i].backset, 0);
		paths[i].track.draw();
		ofPopMatrix();
	}
}

void CloudsVisualSystemForkingPaths::selfDrawDebug(){

}

void CloudsVisualSystemForkingPaths::selfSceneTransformation(){
	
}

void CloudsVisualSystemForkingPaths::selfDraw(){
		
}

void CloudsVisualSystemForkingPaths::selfExit(){
	
}

void CloudsVisualSystemForkingPaths::selfPresetLoaded(string presetPath){
	
}

void CloudsVisualSystemForkingPaths::selfEnd(){
	paths.clear();
	tracks.clear();
}

void CloudsVisualSystemForkingPaths::selfKeyPressed(ofKeyEventArgs & args){

}

void CloudsVisualSystemForkingPaths::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemForkingPaths::selfMouseDragged(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::selfMouseReleased(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::selfSetupGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::selfGuiEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::selfSetupSystemGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::guiSystemEvent(ofxUIEventArgs &e){
}

void CloudsVisualSystemForkingPaths::selfSetupRenderGui(){
	rdrGui->addButton("regenerate", false);
	rdrGui->addSlider("speed", 1, 20, &speed);
	
//	rdrGui->addSlider("birthrate", 0, 100, &particlesPerFrame);
//	
//	rdrGui->addSlider("step", 5, 100, &step);
//	rdrGui->addSlider("chaos", 5, 100, &chaos);
//	rdrGui->addSlider("maxLength", 0, 100, &maxLength);
//	rdrGui->addSlider("fieldAlpha", 0, 1.0, &fieldAlpha);

	
//	rdrGui->addSlider("speed", 0., 2., &speed);
//	rdrGui->addSlider("point size", 1., 25., &pointSize);
//	rdrGui->addSlider("scale", 1., 10., &scale);
}

//--------------------------------------------------------------
void CloudsVisualSystemForkingPaths::guiRenderEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "regenerate"){
		regenerate = true;
	}
}

