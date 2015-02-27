//
//  CloudsRGBDVideoPlayer.cpp
//  CloudsRGBDVideoPlayer
//
//  Created by Patricio Gonzalez Vivo on 4/14/13.
//
//

#include "CloudsRGBDVideoPlayer.h"
#include "CloudsVisualSystem.h"
#include "CloudsLocalization.h"

//---------------------------------------------------------------
CloudsRGBDVideoPlayer::CloudsRGBDVideoPlayer(){
    
	nearClip    = 1.0f;
	edgeClip    = 50.0f;
	farClip     = 6000.0f;
	playingVideo = false;

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

    nextVideoPath = "";
    nextCalibrationXML = "";
    nextSubtitlesPath = "";
    nextOffsetTime = 0;
    nextClipVolume = 0;
    bLoadResult = false;
	bPlayWhenReady = false;
	showingLowerThirds = false;
	
	fontLoadWidth = 0;

	subtitle2DOffsetLowerThirds = .65;
	subtitle2DOffsetVisualSystem = .85;

	englishSubtitleKerning = .5;
	japaneseSubtitleKerning = .4;

	currentPlayer = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );
	nextPlayer    = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );

	currentVoiceoverPlayer = ofPtr<ofSoundPlayer>( new ofSoundPlayer() );
	nextVoiceoverPlayer    = ofPtr<ofSoundPlayer>( new ofSoundPlayer() );
	currentSubtitles = ofPtr<ofxSubtitles>( new ofxSubtitles() );
	nextSubtitles    = ofPtr<ofxSubtitles>( new ofxSubtitles() );

	currentClipHasSubtitles = nextClipHasSubtitles = false;
}

//---------------------------------------------------------------
CloudsRGBDVideoPlayer::~CloudsRGBDVideoPlayer(){
    
}

//---------------------------------------------------------------
bool CloudsRGBDVideoPlayer::setup(string videoPath, string calibrationXMLPath, string subtitlesPath, float offsetTime,float clipVolume){

	cout << "*** SETTING UP CLIP " << calibrationXMLPath << endl;
    if (!ofFile::doesFileExist(videoPath)){
    	ofLogError("CloudsRGBDVideoPlayer::setup") << "Movie path " << videoPath << " failed to load";
        return false;
    }

	if(!ofFile::doesFileExist(calibrationXMLPath)){
    	ofLogError("CloudsRGBDVideoPlayer::setup") << "XML path " << calibrationXMLPath << " failed to load";
		return false;
	}

	cout << "*** SETTING UP CLIP FILES ARE PRESENT " << endl;

    if(!bEventRegistered){
		ofAddListener(ofEvents().update, this, &CloudsRGBDVideoPlayer::update);
		bEventRegistered = true;
	}
    
    nextVideoPath = videoPath;
    nextCalibrationXML = calibrationXMLPath;
    nextSubtitlesPath = subtitlesPath;
    nextOffsetTime = offsetTime;
    nextClipVolume = clipVolume;

	clipPrerolled = true;

    nextClipHasSubtitles = loadSubtitles(nextSubtitlesPath);

#ifdef TARGET_WIN32
    nextPlayer->setUseTexture(false);
	bLoadResult = false;
	
	cout << "*** SETTING UP CLIP STARTING THREAD" << endl;
    startThread(true);

    return true;
#else
    // No need to use a thread, just call this function directly.
    threadedFunction();

    return bLoadResult;
#endif
}

//---------------------------------------------------------------
void CloudsRGBDVideoPlayer::threadedFunction(){
	
	cout << "*** SETTING UP CLIP ENTERED THREAD" << endl;

    if(!nextPlayer->loadMovie(nextVideoPath)){
		ofLogError("CloudsRGBDVideoPlayer::setup") << "Movie path " << nextVideoPath << " failed to load";
        bLoadResult = false;
		clipPrerolled = false;
        return;
    }

	nextPlayer->setPosition( nextOffsetTime / nextPlayer->getDuration() );

	cout << "prerolled clip " << nextVideoPath << " to time " << (nextOffsetTime / nextPlayer->getDuration()) << endl;

	nextClipIsVO = false;
    nextClipVolumeAdjustment = nextClipVolume;
	
	cout << "*** SETTING UP CLIP FINISHED SETUP" << endl;

    bLoadResult = true;
}

bool CloudsRGBDVideoPlayer::setupVO(string audioPath, string subtitlesPath){
	
	if(!nextVoiceoverPlayer->loadSound(audioPath)){
		ofLogError("CloudsRGBDVideoPlayer::setupVO") << "Audio path " << audioPath << " failed to load";
		bLoadResult = false;
		clipPrerolled = false;
		return false;
	}

   if(!bEventRegistered){
		ofAddListener(ofEvents().update, this, &CloudsRGBDVideoPlayer::update);
		bEventRegistered = true;
	}

	nextSubtitlesPath = subtitlesPath;
    nextClipHasSubtitles = loadSubtitles(nextSubtitlesPath);

	clipPrerolled = true;
	nextClipIsVO = true;
	bLoadResult = true;
	return true;
}

void CloudsRGBDVideoPlayer::swapAndPlay(){
	
	cout << "*** SWAPPING CLIP" << endl;

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

	if(clipPrerolled){
		if(bLoadResult){
			cout << "*** STARTING PLAYER FROM SWAP" << endl;
			startPlayer();
		}
		else{
			bPlayWhenReady = true;
		}
	}
}

//--------------------------------------------------------------- ACTIONS
void CloudsRGBDVideoPlayer::startPlayer(){


	currentVoiceoverPlayer->stop();
	currentPlayer->stop();
	currentClipVolumeAdjustment = nextClipVolumeAdjustment;

    nextPlayer->setUseTexture(true);
    
	swap(currentPlayer,nextPlayer);
	swap(currentVoiceoverPlayer, nextVoiceoverPlayer);
    swap(currentSubtitles, nextSubtitles);

	currentClipHasSubtitles = nextClipHasSubtitles;
	if(nextClipIsVO){
		currentVoiceoverPlayer->play();
		currentVoiceoverPlayer->setLoop(false);
	}
	else{
		currentPlayer->play();
		currentPlayer->setLoopState(OF_LOOP_NONE);
	}
	
	playingVO = nextClipIsVO;
	playingVideo = !nextClipIsVO;
	clipPrerolled = false;

//	cout << "swapped and played clip " << endl;
}

//--------------------------------------------------------------- ACTIONS
void CloudsRGBDVideoPlayer::setupProjectionUniforms(ofShader& shader){
    
	if(!getPlayer().isLoaded() || !getTextureReference().isAllocated()){
		//ofLogWarning() << " CloudsRGBDVideoPlayer::setupProjectionUniforms -- player is not ready";
		return;
	}
	
	if(playingVO){
		return;
	}

    shader.setUniformTexture("rgbdTexture", getTextureReference(), 1);
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

//--------------------------------------------------------------- 
ofVideoPlayer& CloudsRGBDVideoPlayer::getPlayer(){
	return *currentPlayer;
}

//--------------------------------------------------------------- 
ofTexture& CloudsRGBDVideoPlayer::getTextureReference(){
	return getPlayer().getTextureReference();
}

//--------------------------------------------------------------- 
ofPtr<ofxSubtitles> CloudsRGBDVideoPlayer::getSubtitles(){
	return currentSubtitles;
}
    
//--------------------------------------------------------------- 
float CloudsRGBDVideoPlayer::getFadeIn(){
	return fadeInValue;
}

//--------------------------------------------------------------- 
float CloudsRGBDVideoPlayer::getFadeOut(){
	return fadeOutValue;
}

//--------------------------------------------------------------- 
void CloudsRGBDVideoPlayer::stop(){
    getPlayer().stop();
    currentVoiceoverPlayer->stop();
}
    
//--------------------------------------------------------------- 
void CloudsRGBDVideoPlayer::update(ofEventArgs& args){
	
	if(!playingVO){
		currentPlayer->update();
	}
	
	if(bLoadResult && clipPrerolled && !nextClipIsVO){
		nextPlayer->update();
	}

	if(bPlayWhenReady && bLoadResult){
		cout << "*** STARTING PLAYER FROM UPDATE" << endl;
		startPlayer();
		bPlayWhenReady = false;
	}

    float audioVolume =  maxVolume * currentClipVolumeAdjustment;

	if(playingVO){
		//JG: audio volume bug 
		//currentVoiceoverPlayer->setVolume(audioVolume);
        if (currentClipHasSubtitles) {
			currentSubtitles->setTimeInSeconds(currentVoiceoverPlayer->getPositionMS()/1000.);
        }
	}
	else{
		float position = getPlayer().getPosition()*getPlayer().getDuration();
		float duration = getPlayer().getDuration();
//		float handleLength = 1.1;
		
		//sometimes NAN comes back from position.
		if(position != position){
			return;
		}
        
		//cout << "position is " << position << " " << duration << " duration " << endl;
		
		fadeInValue = MIN(position, 1.5);
		fadeOutValue = ofMap(position, duration - 1.0, duration, 1.0, 0.0, true);
        
		//remap to make it tigheter
        fadeInValue  = powf(ofMap(fadeInValue,  1.0, 1.5, 0.0, 1.0, true), 2.0);
        fadeOutValue = powf(ofMap(fadeOutValue, .5, 1.0, 0.0, 1.0, true), 2.0);
		
		float fadeInStartTime = 1.0;
		float fadeInEndTime = 1.4;
		float fadeOutStartTime = duration - 1.3 ;
		float fadeOutEndTime = duration - 1.0;
		if(position < 1.0){
			audioVolume = ofMap(position, fadeInStartTime, fadeInEndTime, 0., maxVolume, true);
		}
		else if(position > fadeOutStartTime){
			audioVolume = ofMap(position, fadeOutStartTime, fadeOutEndTime, maxVolume, 0.0, true);
		}

//		cout << "/*/*/*/*/*/***** FADIN VALUE " << fadeInValue << " FADE OUT VALUE " << fadeOutValue << " AUDIO VOLUME " << audioVolume << endl;
//		cout << "is playing? " << (isPlaying() ? "YES" : "NO") << endl;
		getPlayer().setVolume(audioVolume);

		if(forceStop && position > duration - .04){
			getPlayer().stop();
		}
        
        /* Subtitles */
        if (currentClipHasSubtitles) {
			currentSubtitles->setTimeInSeconds(getPlayer().getPosition()*getPlayer().getDuration());
        }
	}
}

//--------------------------------------------------------------- 
bool CloudsRGBDVideoPlayer::isPlaying(){
	return playingVO ? currentVoiceoverPlayer->getIsPlaying() : (getPlayer().isLoaded() && getPlayer().isPlaying());
}

//--------------------------------------------------------------- 
bool CloudsRGBDVideoPlayer::isDone(){
	return playingVO ? !currentVoiceoverPlayer->getIsPlaying() : (getPlayer().isLoaded() && !getPlayer().isPlaying());
}

//--------------------------------------------------------------- 
bool CloudsRGBDVideoPlayer::loadSubtitles(string path){
    
    if (path == "") {
		ofLogError("CloudsRGBDVideoPlayer::loadSubtitles") << "Subtitle path is blank";
        return false;
    }

    if (!ofFile(path).exists()) {
		ofLogError("CloudsRGBDVideoPlayer::loadSubtitles") << "Subtitle path doesn't exist " << path;
        return false;
    }
    
    // need to know fps (all 24 except Higa (30))
    int fps = 24;
    if (strstr(path.data(), "Higa") != NULL) {
        fps = 30;
    }
    
	if(fontLoadWidth != CloudsVisualSystem::getStaticRenderTarget().getWidth()){
		int fontSize = 50;
		string fontPath = GetFontPath();

#ifdef OCULUS_RIFT
		if(!nextSubtitles->setup(path, fontPath, fontSize, fps, TEXT_JUSTIFICATION_LEFT)) {
#else
		if(!nextSubtitles->setup(path, fontPath, fontSize, fps, TEXT_JUSTIFICATION_CENTER)) {
#endif
			ofLogError("CloudsRGBDVideoPlayer::loadSubtitles") << "Failed to set up subtitles at path " << path;
			return false;
		}
		nextSubtitles->font.setLetterSpacing(japaneseSubtitleKerning);
		nextSubtitles->lineHeight = .3;
		// find font size based on 85% canvas width and a predefined maximum string
		float requiredWidth = (float)CloudsVisualSystem::getStaticRenderTarget().getWidth() * .85;
		if(requiredWidth == 0){
			requiredWidth = 1920*.85;
		}
		string maxStr = "If I'd have to choose from something interesting. Something else";
		float curStringWidth = nextSubtitles->font.stringWidth(maxStr);
    
		// loop here until you find the right font size
		while (curStringWidth > requiredWidth && fontSize > 0) {
			nextSubtitles->font.setSize(--fontSize);
			curStringWidth = nextSubtitles->font.stringWidth(maxStr);
		}

		cout << "SUBTITLE FONT SIZE IS " << fontSize << endl;
		if(fontSize == 0){
			ofLogError("CloudsRGBDVideoPlayer::loadSubtitles") << "Font size went to 0, failed to load titles";
			return false;
		}
		fontLoadWidth = fontSize;

	    return true;
	}
	else{
		return nextSubtitles->load(path);
	}

}

//--------------------------------------------------------------- 
void CloudsRGBDVideoPlayer::drawSubtitles(){
#ifdef OCULUS_RIFT
	int x = 700;
#else
	int x = CloudsVisualSystem::getStaticRenderTarget().getWidth()/2.0;
#endif
    int y = CloudsVisualSystem::getStaticRenderTarget().getHeight();
	y *= showingLowerThirds ? subtitle2DOffsetLowerThirds : subtitle2DOffsetVisualSystem;
	drawSubtitles(x,y);
}

//--------------------------------------------------------------- 
void CloudsRGBDVideoPlayer::drawSubtitles(int x, int y){
	bool playing = isPlaying() ;
	if( hasSubtitles() && playing) {
        ofPushStyle();
		ofDisableLighting();
		ofEnableAlphaBlending();
        ofSetColor(0, 200);
        currentSubtitles->draw(x+3, y-2);
        ofSetColor(255);
        currentSubtitles->draw(x, y);
		ofDisableAlphaBlending();
        ofPopStyle();
    }
}
    
//--------------------------------------------------------------- 
bool CloudsRGBDVideoPlayer::hasSubtitles(){
    return currentClipHasSubtitles;
}
    
