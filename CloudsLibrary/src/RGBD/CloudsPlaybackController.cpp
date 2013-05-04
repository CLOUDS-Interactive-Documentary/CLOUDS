
#include "CloudsPlaybackController.h"

CloudsPlaybackController::CloudsPlaybackController(){
	eventsRegistered = false;
	playingCombinedVideo = false;
}

CloudsPlaybackController::~CloudsPlaybackController(){
	if(eventsRegistered){
		eventsRegistered = false;
//		ofRemoveListener(storyEngine->getEvents().storyBegan, this, &CloudsPlaybackController::storyBegan);
//		ofRemoveListener(storyEngine->getEvents().clipChanged, this, &CloudsPlaybackController::clipChanged);
	}
}

void CloudsPlaybackController::setup(CloudsStoryEngine& storyEngine){
	if(!eventsRegistered){
		this->storyEngine = &storyEngine;
		ofAddListener(storyEngine.getEvents().storyBegan, this, &CloudsPlaybackController::storyBegan);
		ofAddListener(storyEngine.getEvents().clipChanged, this, &CloudsPlaybackController::clipChanged);
		eventsRegistered = true;
	}
}

void CloudsPlaybackController::storyBegan(CloudsStoryEventArgs& args){
	playClip(args.chosenClip);
}

void CloudsPlaybackController::clipChanged(CloudsStoryEventArgs& args){
	playClip(args.chosenClip);	
}

void CloudsPlaybackController::playClip(CloudsClip& clip){
	
	if(clip.hasCombinedVideo){
		if(player.loadMovie(relinkMovieFilepath(clip.combinedVideoFilePath))){
			playingCombinedVideo = true;
			renderer.setup(relinkMovieFilepath(clip.combinedVideoCalibrationXml));
			renderer.setTexture(player);
		}
	}
	else{
		playingCombinedVideo = false;
		player.loadMovie( relinkMovieFilepath(clip.sourceVideoFilePath) );
	}
	
	player.setFrame(clip.startFrame);
	player.play();
	
	currentClip = clip;
}

void CloudsPlaybackController::update(){
	player.update();
	if(player.isPlaying()){
		if(player.getCurrentFrame() >= currentClip.endFrame){
			storyEngine->selectNewClip();
		}
	}
}

void CloudsPlaybackController::draw(){
	if(player.isPlaying()){
		if(playingCombinedVideo){
			renderer.drawPointCloud();
		}
		else {
			player.draw(0,0,960,540);
		}
	}
}

string CloudsPlaybackController::relinkMovieFilepath(string filePath){
	
	if( !ofFile(filePath).exists() ){
		//		cout << "Switched clip from " << clipFilePath;
		ofStringReplace(filePath, "Nebula_backup", "Seance");
		ofStringReplace(filePath, "Nebula", "Seance");
		//		cout << " to " << clipFilePath << endl;
	}
	return filePath;
}
