//
//  CloudsRGBDVideoPlayer.cpp
//  CloudsRGBDVideoPlayer
//
//  Created by Patricio Gonzalez Vivo on 4/14/13.
//
//

#include "CloudsRGBDVideoPlayer.h"

//---------------------------------------------------------------
CloudsRGBDVideoPlayer::CloudsRGBDVideoPlayer(){
    
	nearClip    = 1.0f;
	edgeClip    = 50.0f;
	farClip     = 6000.0f;
	
	fadeOutValue = fadeInValue = 0.0;

	minDepth = 400;
	maxDepth = 2000;
	maxVolume = 1.5;
	currentMaxVolume = 1.0;
    currentClipVolumeAdjustment = 1;
	nextClipIsVO = false;
	playingVO = false;

	bEventRegistered = false;
	clipPrerolled = false;
	
#ifdef AVF_PLAYER
	nextPlayer = ofPtr<ofxAVFVideoPlayer>( new ofxAVFVideoPlayer() );
	currentPlayer = ofPtr<ofxAVFVideoPlayer>( new ofxAVFVideoPlayer() );
#else
	currentPlayer = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );
	nextPlayer = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );
#endif

	currentVoiceoverPlayer = ofPtr<ofSoundPlayer>( new ofSoundPlayer() );
	nextVoiceoverPlayer = ofPtr<ofSoundPlayer>( new ofSoundPlayer() );

}

//---------------------------------------------------------------
CloudsRGBDVideoPlayer::~CloudsRGBDVideoPlayer(){
    
}

//---------------------------------------------------------------
//SURYA TODO: Add new end time
bool CloudsRGBDVideoPlayer::setup(string videoPath, string calibrationXMLPath, string subtitlesPath, float offsetTime,float clipVolume ){
	
	if(!bEventRegistered){
		ofAddListener(ofEvents().update, this, &CloudsRGBDVideoPlayer::update);
		bEventRegistered = true;
	}
	
	if(!nextPlayer->loadMovie(videoPath)){
		ofLogError("CloudsRGBDVideoPlayer::setup") << "Movie path " << videoPath << " failed to load";
		return false;
	}
	
	nextPlayer->setPosition( offsetTime / nextPlayer->getDuration() );

	nextCalibrationXML = calibrationXMLPath;
	cout << "prerolled clip " << videoPath << " to time " << offsetTime << endl;

    /* Subtitles */
    nextHaveSubtitles = loadSubtitles(subtitlesPath);
    
	clipPrerolled = true;
	nextClipIsVO = false;
    nextClipVolumeAdjustment = clipVolume;

	return true;
}

bool CloudsRGBDVideoPlayer::setupVO(string audioPath){
	
	if(!nextVoiceoverPlayer->loadSound(audioPath)){
		ofLogError("CloudsRGBDVideoPlayer::setupVO") << "Audio path " << audioPath << " failed to load";
		return false;
	}
	
	clipPrerolled = true;
	nextClipIsVO = true;
	return true;
}

void CloudsRGBDVideoPlayer::swapAndPlay(){
	
	if(!nextClipIsVO){
		
		ofxXmlSettings XML;
		if ( !XML.loadFile(nextCalibrationXML) ){
			ofLogError("CloudsRGBDVideoPlayer::setup") << "XML Path " << nextCalibrationXML << " failed to load";
			return;
		}
		
		colorPrincipalPoint.x = XML.getValue("colorIntrinsics:ppx", 971.743835449);
		colorPrincipalPoint.y = XML.getValue("colorIntrinsics:ppy", 546.945983887);
		colorFOV.x = XML.getValue("colorIntrinsics:fovx", 923.500793457);
		colorFOV.y = XML.getValue("colorIntrinsics:fovy", 921.060791016);
		colorRect.x = 0.0f;
		colorRect.y = 0.0f;
		colorRect.width = XML.getValue("colorIntrinsics:width", 1920.000000000);
		colorRect.height = XML.getValue("colorIntrinsics:height", 1080.000000000);
		
		float depthToRGBRotation[9];
		float depthToRGBTranslation[3];
		for (int i = 0; i < 9; i++) {
			depthToRGBRotation[i] = XML.getValue("extrinsics:rotation:r"+ofToString(i), 1.0f);
		}
		
		for (int i = 0; i < 3; i++) {
			depthToRGBTranslation[i] = XML.getValue("extrinsics:translation:t"+ofToString(i), 1.0f);
		}
		
		for (int i = 0; i < 3; i++) {
			distortionK[i] = XML.getValue("colorIntrinsics:dK:k"+ofToString(i), 1.0f);
		}
		
		for (int i = 0; i < 2; i++) {
			distortionP[i] = XML.getValue("colorIntrinsics:dP:p"+ofToString(i), 1.0f);
		}
		
		headPosition = ofVec3f(-XML.getValue("face:x", 0.0),
							   -XML.getValue("face:y", 0.0),
							   XML.getValue("face:z", 0.0));
		
		//cout << "head position " << headPosition << endl;
		
		float mat4x4[16] = {
			depthToRGBRotation[0],depthToRGBRotation[1],depthToRGBRotation[2],0,
			depthToRGBRotation[3],depthToRGBRotation[4],depthToRGBRotation[5],0,
			depthToRGBRotation[6],depthToRGBRotation[7],depthToRGBRotation[8],0,
			depthToRGBTranslation[0],depthToRGBTranslation[1],depthToRGBTranslation[2],1
		};
		
		extrinsics = ofMatrix4x4(mat4x4);
		
		//	cout << "extrinsic matrix: " << endl << extrinsics << endl;
		
		//adjustment
		adjustTranslate.x = XML.getValue("adjustment:translate:x", 0.0);
		adjustTranslate.y = XML.getValue("adjustment:translate:y", 0.0);
		adjustTranslate.z = XML.getValue("adjustment:translate:z", 0.0);
		
		adjustRotate.x = XML.getValue("adjustment:rotate:x", 0.0);
		adjustRotate.y = XML.getValue("adjustment:rotate:y", 0.0);
		adjustRotate.z = XML.getValue("adjustment:rotate:z", 0.0);
		
		adjustScale.x = XML.getValue("adjustment:scale:x", 1.0);
		adjustScale.y = XML.getValue("adjustment:scale:y", 1.0);
		
		depthPrincipalPoint.x = XML.getValue("depthIntrinsics:ppx", 320.0);
		depthPrincipalPoint.y = XML.getValue("depthIntrinsics:ppy", 240.0);
		depthFOV.x = XML.getValue("depthIntrinsics:fovx", 570.34);
		depthFOV.y = XML.getValue("depthIntrinsics:fovy", 570.34);
		
		depthRect.x = 0.0;      //  TODO: do this automatically
		depthRect.y = 720.0;    //
		depthRect.width = XML.getValue("depthIntrinsics:width", 640.0);
		depthRect.height = XML.getValue("depthIntrinsics:height", 480.0);
		
		normalRect.x = 640.0;       //  TODO: do this automatically
		normalRect.y = 720.0;       //
		normalRect.width = 640.0;
		normalRect.height = 480.0;
		
		nearClip = minDepth = XML.getValue("adjustment:depth:min", 1.0f);
		farClip = maxDepth = XML.getValue("adjustment:depth:max", 6000.0f);
		
		////////-----NO LONGER USED
		//this describes the face features: eyes, mouth, and skin
		faceFeatureRect = ofRectangle(depthRect.x, depthRect.getMaxY(), 640, 360);
		//this describes the change each frame
		deltaChangeRect = ofRectangle(normalRect.x, normalRect.getMaxY(), 640, 360);
		////////////
		
		hasSkinSettings = XML.tagExists("skin");
		skinSampleColor.r = XML.getValue("skin:targetR", 0.);
		skinSampleColor.g = XML.getValue("skin:targetG", 0.);
		skinSampleColor.b = XML.getValue("skin:targetB", 0.);
		skinWeights.x = XML.getValue("skin:hueWeight", 0.);
		skinWeights.y = XML.getValue("skin:satWeight", 0.);
		skinWeights.z = XML.getValue("skin:brightWeight", 0.);
		skinThreshold.min = XML.getValue("skin:lowerThreshold", .0);
		skinThreshold.max = XML.getValue("skin:upperThreshold", 1.0);
		
//		cout << "HAS SKIN? " << hasSkinSettings << endl;

		//	float colorWidth  = getPlayer().getWidth();
		//	float colorHeight = getPlayer().getHeight();
		float colorWidth  = 1280;
		float colorHeight = 1560;
		colorScale.x = colorWidth / colorRect.width;
		colorScale.y = float(colorHeight - (depthRect.height + faceFeatureRect.height) ) / float(colorRect.height);
		useFaces = true;
		
	}

	currentVoiceoverPlayer->stop();
	currentPlayer->stop();
	currentClipVolumeAdjustment = nextClipVolumeAdjustment;
    
//    cout<<"Current Max Vol: "<<currentMaxVolume<<endl;
	swap(currentPlayer,nextPlayer);
	swap(currentVoiceoverPlayer, nextVoiceoverPlayer);
#ifdef SHOW_SUBTITLES
    swap(currentSubtitles, nextSubtitles);
	swap(currentHaveSubtitles, nextHaveSubtitles);
#endif
	if(nextClipIsVO){
		currentVoiceoverPlayer->play();
		currentVoiceoverPlayer->setLoop(false);
	}
	else{
		currentPlayer->play();
	}
	
	playingVO = nextClipIsVO;
	clipPrerolled = false;

//	cout << "swapped and played clip " << endl;

}

//--------------------------------------------------------------- ACTIONS
void CloudsRGBDVideoPlayer::setupProjectionUniforms(ofShader& shader){
    
	if(!getPlayer().isLoaded() || !getPlayer().getTextureReference().isAllocated()){
		//ofLogWarning() << " CloudsRGBDVideoPlayer::setupProjectionUniforms -- player is not ready";
		return;
	}
	
	if(playingVO){
		return;
	}

    shader.setUniformTexture("rgbdTexture", getPlayer().getTextureReference(), 1);
    shader.setUniform2f("textureSize",  getPlayer().getWidth(), getPlayer().getHeight());
    
    shader.setUniform4f("colorRect", colorRect.x, colorRect.y, colorRect.width, colorRect.height);
    shader.setUniform2f("colorScale", colorScale.x, colorScale.y);
    
    shader.setUniform2f("colorFOV", colorFOV.x, colorFOV.y );
    shader.setUniform2f("colorPP", colorPrincipalPoint.x, colorPrincipalPoint.y);
    shader.setUniform3f("dK", distortionK.x, distortionK.y, distortionK.z);
    shader.setUniform2f("dP", distortionP.x, distortionP.y);
    
	ofMatrix4x4 adjustmentMatrix;
	adjustmentMatrix.rotate(adjustRotate.x, 0, 1, 0);
	adjustmentMatrix.rotate(adjustRotate.y, 1, 0, 0);
	adjustmentMatrix.translate(adjustTranslate.x, adjustTranslate.y, adjustTranslate.z);

	shader.setUniformMatrix4f("extrinsics", extrinsics * adjustmentMatrix );
    
    shader.setUniform4f("depthRect", depthRect.x, depthRect.y, depthRect.width, depthRect.height);
	shader.setUniform2f("depthPP", depthPrincipalPoint.x, depthPrincipalPoint.y);
	shader.setUniform2f("depthFOV", depthFOV.x, depthFOV.y);
    
    shader.setUniform4f("normalRect", normalRect.x, normalRect.y, normalRect.width, normalRect.height);
    shader.setUniform4f("faceFeatureRect", faceFeatureRect.x, faceFeatureRect.y, faceFeatureRect.width, faceFeatureRect.height);
    shader.setUniform4f("deltaChangeRect", deltaChangeRect.x, deltaChangeRect.y, deltaChangeRect.width, deltaChangeRect.height);
	
	shader.setUniform1f("farClip", farClip);
    shader.setUniform1f("nearClip", nearClip);
	shader.setUniform1f("edgeClip", edgeClip);

	shader.setUniform1f("minDepth", minDepth);
    shader.setUniform1f("maxDepth", maxDepth);
	
	shader.setUniform3f("skinSampleColor",skinSampleColor.r,skinSampleColor.g,skinSampleColor.b);
	shader.setUniform3f("skinWeights", skinWeights.x,skinWeights.y,skinWeights.z);
	shader.setUniform2f("skinThreshold", skinThreshold.min, skinThreshold.max);

	shader.setUniform3f("headPosition",headPosition.x,-headPosition.y,headPosition.z);
	
	shader.setUniform1f("flowPosition", flowPosition);
}

//--------------------------------------------------------------- ACTIONS
#ifdef AVF_PLAYER
ofxAVFVideoPlayer& CloudsRGBDVideoPlayer::getPlayer(){
#else
ofVideoPlayer& CloudsRGBDVideoPlayer::getPlayer(){
#endif
	return *currentPlayer;
}

//--------------------------------------------------------------- ACTIONS
void CloudsRGBDVideoPlayer::update(ofEventArgs& args){
	
	//TODO: Optimize these!
	if(!playingVO){
		currentPlayer->update();
	}
	
	if(clipPrerolled && !nextClipIsVO){
		nextPlayer->update();
	}
	


    float  audioVolume =  maxVolume * currentClipVolumeAdjustment;

	if(playingVO){
		currentVoiceoverPlayer->setVolume(audioVolume);
	}
	else{
		float position = getPlayer().getPosition()*getPlayer().getDuration();
		float duration = getPlayer().getDuration();
		float handleLength = 1.1;
		
		//sometimes NAN comes back from position.
		if(position != position){
			return;
		}
		//cout << "position is " << position << " " << duration << " duration " << endl;
		
		fadeInValue = MIN(position, 1.0);
		fadeOutValue = ofMap(position, duration - 1.0, duration, 1.0, 0.0, true);
		if(position < 1.0){
			audioVolume = ofMap(position, 1.0, 1.1, 0., maxVolume, true);
		}
		else if(position > duration - 1.0){
			audioVolume = ofMap(position, duration - 1.1, duration - 1.0, maxVolume, 0.0, true);
		}
		
		getPlayer().setVolume(audioVolume);

		if(forceStop && position > duration - .04){
			getPlayer().stop();
		}
        
#ifdef SHOW_SUBTITLES
        /* Subtitles */
        if (currentHaveSubtitles) {
            currentSubtitles.setTimeInSeconds(getPlayer().getCurrentTime());
        }
#endif
	}
}

bool CloudsRGBDVideoPlayer::isPlaying(){
	return playingVO ? currentVoiceoverPlayer->getIsPlaying() : (getPlayer().isLoaded() && getPlayer().isPlaying());
}

bool CloudsRGBDVideoPlayer::isDone(){
	return playingVO ? !currentVoiceoverPlayer->getIsPlaying() : (getPlayer().isLoaded() && !getPlayer().isPlaying());
}

#ifdef SHOW_SUBTITLES

bool CloudsRGBDVideoPlayer::loadSubtitles(string path){
    
    if (path == "") {
        return false;
    }

    if (!ofFile(path).exists()) {
        return false;
    }
    
    // need to know fps (all 24 except Higa (30))
    int fps = 24;
    if (strstr(path.data(), "Higa") != NULL) {
        fps = 30;
    }
    
    int fontSize = 36;
    if(!nextSubtitles.setup(path, GetCloudsDataPath() + "/font/Blender-BOOK.ttf", fontSize/2, fps, TEXT_JUSTIFICATION_CENTER)) {
        return false;
    }
    
    // find font size based on 85% canvas width and a predefined maximum string
    float requiredWidth = (float)CloudsVisualSystem::getStaticRenderTarget().getWidth()*0.85;
    string maxStr = "If I'd have to choose from something interesting, something beautiful or something useful,";
    ofRectangle bounds = nextSubtitles.font.getStringBoundingBox(maxStr, 0, 0);
    // loop here until you find the right font size
    while (bounds.width > requiredWidth) {
        nextSubtitles.font.setSize(--fontSize);
        bounds = nextSubtitles.font.getStringBoundingBox(maxStr, 0, 0);
    }
    
    return true;
}
#else
bool CloudsRGBDVideoPlayer::loadSubtitles(string path){
    return false;
}
#endif

void CloudsRGBDVideoPlayer::drawSubtitles(float x, float y)
{
#ifdef SHOW_SUBTITLES
    if (haveSubtitles()) {
        ofPushStyle();
        ofSetColor(0, 200);
        currentSubtitles.draw(x+3, y-2);
        ofSetColor(255);
        currentSubtitles.draw(x, y);
        ofPopStyle();
    }
#endif
}
    
bool CloudsRGBDVideoPlayer::haveSubtitles()
{
    return currentHaveSubtitles;
}
    
