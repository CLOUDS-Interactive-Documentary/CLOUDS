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
	
	bEventRegistered = false;
	clipPrerolled = false;
}

//---------------------------------------------------------------
CloudsRGBDVideoPlayer::~CloudsRGBDVideoPlayer(){
    
}

//---------------------------------------------------------------
//SURYA TODO: Add new end time
bool CloudsRGBDVideoPlayer::setup(string videoPath, string calibrationXMLPath, float offsetTime){
	
	if(!bEventRegistered){
		ofAddListener(ofEvents().update, this, &CloudsRGBDVideoPlayer::update);
		bEventRegistered = true;
	}
	
	if(!nextPlayer.loadMovie(videoPath)){
		ofLogError() << "CloudsRGBDVideoPlayer::setup -- Movie path " << videoPath << " failed to load";
		return false;
	}
#ifdef AVF_PLAYER
	nextPlayer.setPositionInSeconds( offsetTime );
#else
	nextPlayer.setPosition( offsetTime / nextPlayer.getDuration() );
#endif
	nextCalibrationXML = calibrationXMLPath;
	cout << "prerolled clip " << videoPath << " to time " << offsetTime << endl;
	clipPrerolled = true;
	return true;
}

void CloudsRGBDVideoPlayer::swapAndPlay(){
	
	ofxXmlSettings XML;
	if ( !XML.loadFile(nextCalibrationXML) ){
		ofLogError() << "CloudsRGBDVideoPlayer::setup -- XML Path " << nextCalibrationXML << " failed to load";
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
	
	//TODO automatically
	//this describes the face features: eyes, mouth, and skin
	faceFeatureRect = ofRectangle(depthRect.x, depthRect.getMaxY(), 640, 360);
	//this describes the change each frame
	deltaChangeRect = ofRectangle(normalRect.x, normalRect.getMaxY(), 640, 360);
	
	//	float colorWidth  = getPlayer().getWidth();
	//	float colorHeight = getPlayer().getHeight();
	float colorWidth  = 1280;
	float colorHeight = 1560;
	
    colorScale.x = float(colorWidth) / float(colorRect.width);
	if(colorHeight > 1200){
		useFaces = true;
		colorScale.y = float(colorHeight - (depthRect.height + faceFeatureRect.height) ) / float(colorRect.height);
	}
	else{
		useFaces = false;
		colorScale.y = float(colorHeight - (depthRect.height) ) / float(colorRect.height);
	}

	currentPlayer.stop();
	nextPlayer.play();
	swap(currentPlayer,nextPlayer);
	clipPrerolled = false;
	
//	cout << "swapped and played clip " << endl;

}

//--------------------------------------------------------------- ACTIONS
void CloudsRGBDVideoPlayer::setupProjectionUniforms(ofShader& shader){
    
	if(!getPlayer().isLoaded()){
		ofLogWarning() << " CloudsRGBDVideoPlayer::setupProjectionUniforms -- player is not ready";
		return;
	}
	
    shader.setUniformTexture("rgbdTexture", getPlayer().getTextureReference(), 0);
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

	shader.setUniform1i("useFaces", useFaces ? 1 : 0);
	shader.setUniform1f("flowPosition", flowPosition);
	
	shader.setUniform1f("farClip", farClip);
    shader.setUniform1f("nearClip", nearClip);
	shader.setUniform1f("edgeClip", edgeClip);

	shader.setUniform1f("minDepth", minDepth);
    shader.setUniform1f("maxDepth", maxDepth);

}

//--------------------------------------------------------------- ACTIONS
#ifdef AVF_PLAYER
ofxAVFVideoPlayer& CloudsRGBDVideoPlayer::getPlayer(){
#else
ofVideoPlayer& CloudsRGBDVideoPlayer::getPlayer(){
#endif
	return currentPlayer;
}

//--------------------------------------------------------------- ACTIONS
void CloudsRGBDVideoPlayer::update(ofEventArgs& args){
	
	currentPlayer.update();
	if(clipPrerolled){
		nextPlayer.update();
	}
	
	float audioVolume = maxVolume;
#ifdef AVF_PLAYER
	float position = getPlayer().getPositionInSeconds();
	float duration = getPlayer().getDuration();
#else
	float position = getPlayer().getPosition()*getPlayer().getDuration();
	float duration = getPlayer().getDuration();
#endif
	
	//cout << "position is " << position << " " << duration << " duration " << endl;
	fadeInValue = MIN(position, 1.0);
	fadeOutValue = ofMap(position, duration - 1.0, duration, 1.0, 0.0, true);
	if(position < 1.0){
		audioVolume = ofMap(position, 1.0, 1.2, 0., maxVolume, true);
	}
	else if(position > duration - 1.0){
		audioVolume = ofMap(position, duration - 1.2, duration - 1.0, maxVolume, 0.0, true);
	}

	getPlayer().setVolume(audioVolume);

	if(position > duration - .04){
		getPlayer().stop();
	}
}

bool CloudsRGBDVideoPlayer::isPlaying(){
	return getPlayer().isLoaded() && getPlayer().isPlaying();
}

bool CloudsRGBDVideoPlayer::isDone(){
	return getPlayer().isLoaded() && !getPlayer().isPlaying();
}
