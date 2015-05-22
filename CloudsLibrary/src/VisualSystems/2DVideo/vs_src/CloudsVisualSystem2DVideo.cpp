//
//  CloudsVisualSystem2DVideo.cpp
//

#include "CloudsVisualSystem2DVideo.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfSetupGui()
{
    vidCam = ofCamera();
	playerGui = new ofxUISuperCanvas("VideoPlayer", gui);
	playerGui->copyCanvasStyle(gui);
	playerGui->copyCanvasProperties(gui);
	playerGui->setName("VideoPlayer");
	playerGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    playerGui->addLabel("VIDEOS");
    playerGui->addSpacer();
    playerGui->addRadio("MOVIE FILES",movieStrings);
    playerGui->addSpacer();
    playerGui->addButton("SET IN TIME", false);
    playerGui->addButton("SET OUT TIME", false);
    playerGui->addSlider("INTIME", 0, 1000, &inTime);
    playerGui->addSlider("OUTTIME", 0, 1000, &outTime);
    
	ofAddListener(playerGui->newGUIEvent, this, &CloudsVisualSystem2DVideo::selfGuiEvent);
	guis.push_back(playerGui);
	guimap[playerGui->getName()] = playerGui;
}

void CloudsVisualSystem2DVideo:: selfSetDefaults(){
    primaryCursorMode = CURSOR_MODE_CAMERA;
    secondaryCursorMode =  CURSOR_MODE_INACTIVE;
	rotationRange = ofVec2f(5,5);
    inTime = 0;
    outTime = 0;
    bWaitForVHX = false;
    bFileLoadCompleted = false;
    
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.getKind() == OFX_UI_WIDGET_TOGGLE){

		if( ((ofxUIToggle*)e.widget)->getValue()){
			loadMovieWithName( e.widget->getName() );
		}

    }
    if (e.getKind() == OFX_UI_WIDGET_BUTTON){
        
        if(e.getName() == "SET IN TIME"){
			inTime = player->getPosition()*player->getDuration();
        }
        else if (e.getName() == "SET OUT TIME"){
            outTime = player->getPosition()*player->getDuration();
        }
    }
    
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystem2DVideo::selfSetupSystemGui(){
    
}

void CloudsVisualSystem2DVideo::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystem2DVideo::selfSetupRenderGui(){
    
}

void CloudsVisualSystem2DVideo::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfSetup()
{
    screenRect = ofRectangle(0,0, getCanvasWidth(), getCanvasHeight());
    videoRect  = ofRectangle(0,0, getCanvasWidth(), getCanvasHeight());
    
    movieIndex = 0;
    
    movieStrings.push_back("Alice.mov");
    movieStrings.push_back("D3_AAPL.mov");
    movieStrings.push_back("D3_Dial.mov");
    movieStrings.push_back("D3_Radial.mov");
    movieStrings.push_back("OpenPaths.mov");
    movieStrings.push_back("Exoplanets.mp4");
    movieStrings.push_back("Cascade.mov");
    movieStrings.push_back("DataTimeline.mov");
    movieStrings.push_back("CordDiagram_videogames.mov");
    movieStrings.push_back("zipcode.mov");
    movieStrings.push_back("USMap.mov");

    movieStrings.push_back("GitHub2.mov");
    movieStrings.push_back("Gource.mov");

    movieStrings.push_back("PeopleStaring.mp4");
    movieStrings.push_back("FaceShift_noise.mov");
    movieStrings.push_back("FaceSub_lowSat.mov");
    movieStrings.push_back("JapaneseSlow2.mov");
    movieStrings.push_back("TeensSlow.mov");
    
    movieStrings.push_back("Kittenstep.mov");
    movieStrings.push_back("Code_Screenflow3.mov");
    
    movieStrings.push_back("networkFlow2.mov");
    movieStrings.push_back("DesignByNumbers.mov");
    movieStrings.push_back("TweakMode.mov");
    movieStrings.push_back("Logo1.mov");
    movieStrings.push_back("Logo2.mov");
    movieStrings.push_back("Flow_1.mov");
    movieStrings.push_back("Flow2_Blue.mov");
    movieStrings.push_back("Flow5_Blue.mov");
    
    movieStrings.push_back("Reas_network1.mov");
    movieStrings.push_back("Reas_Process13.mov");
    
    //loadMovieAtIndex(movieIndex, true);
    inTime = 0;
    outTime = 0;

    
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::restart()
{
    
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfPresetLoaded(string presetPath)
{   
    
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::loadMovieWithName(string name){
	for(int i = 0; i < movieStrings.size(); i++){
		if (movieStrings[i] == name) {
			loadMovieAtIndex(i);
			break;
		}
	}
}


//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::loadMovieAtIndex(int index){
    
    if( player != NULL && player->isPlaying() ){
        player->stop();
    }
    player = ofPtr<ofVideoPlayer>(new ofVideoPlayer());
    
#ifdef VHX_MEDIA
    if(waitingMedia != NULL){
        ofRemoveListener(waitingMedia->completeEvent, this, &CloudsVisualSystem2DVideo::vhxRequestComplete);
    }
    waitingMedia = getVHXMedia(movieStrings[index]);
    if(waitingMedia == NULL){
        ofLogError("CloudsVisualSystem2DVideo::loadMovieAtIndex") << "Couldn't find VHX Media ID for clip " << movieStrings[index];
        return;
    }
    ofAddListener(waitingMedia->completeEvent, this, &CloudsVisualSystem2DVideo::vhxRequestComplete);
    waitingMedia->fetchVhxSourceUrl();
#else
	loadedMoviePath = GetCloudsMediaPath() + "visualsystems/" + getSystemName() + "/videos/" + movieStrings[index];
    finishLoad();
#endif
    
}

#ifdef VHX_MEDIA

void CloudsVisualSystem2DVideo::vhxRequestComplete(CloudsVHXEventArgs& args){
    if(!args.success){
        ofLogError("CloudsVisualSystem2DVideo::vhxRequestComplete") << "VHX request failed " << args.result;
    }
    
    loadedMoviePath = args.result;
    finishLoad();
    
    ofRemoveListener(waitingMedia->completeEvent, this, &CloudsVisualSystem2DVideo::vhxRequestComplete);
    waitingMedia = NULL;
}
#endif

void CloudsVisualSystem2DVideo::finishLoad() {
    
    if( player->loadMovie(loadedMoviePath) ){
        player->play();
        bFileLoadCompleted = false;
    }
    else{
        ofLogError("CloudsVisualSystem2DVideo::loadMovieAtIndex") << "couldn't load the movie";
    }
	receivedFrame = false;
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystem2DVideo::selfBegin(){
	player->play();
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystem2DVideo::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfUpdate()
{
    screenRect.width = getCanvasWidth();
    screenRect.height = getCanvasHeight();
    
    if(player->getWidth() > 0){
        videoRect.x = 0;
        videoRect.y = 0;
        videoRect.width = player->getWidth();
        videoRect.height = player->getHeight();
        videoRect.setFromCenter(screenRect.width/2, screenRect.height/2, videoRect.width, videoRect.height);
    }
    
    if (!bFileLoadCompleted && player->getWidth() > 0){
        
        //this is to set the intime once the video has loaded
        player->setPosition(inTime / player->getDuration());
        bFileLoadCompleted = true;
    }
    
    player->update();
    receivedFrame |= player->isFrameNew();
	
    if(timeline->getDurationInSeconds() != (outTime - inTime)){
        if(outTime-inTime > 0){
            timeline->setDurationInSeconds(outTime - inTime);
        }
	}
}

//--------------------------------------------------------------
ofCamera& CloudsVisualSystem2DVideo::getCameraRef(){
    return vidCam;
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfDraw()
{

    if(player->isLoaded() && receivedFrame){

        
        ofVec3f topLeft = vidCam.screenToWorld(videoRect.getTopLeft());
        ofVec3f bottomLeft = vidCam.screenToWorld(videoRect.getBottomLeft());
        ofVec3f topRight =vidCam.screenToWorld(videoRect.getTopRight());
        ofVec3f bottomRight =vidCam.screenToWorld(videoRect.getBottomRight());
        
        //create a mesh
        ofMesh mesh;
        //TOP LEFT
        //texture coordinates are in the image space
        mesh.addTexCoord(ofVec3f(0,0,0));
        //vertices are in the screen space
        mesh.addVertex(topLeft);

        //BOTTOM LEFT
        mesh.addTexCoord(ofVec3f(0,videoRect.height,0));
        mesh.addVertex(bottomLeft);

        //TOP RIGHT
        mesh.addTexCoord(ofVec3f(videoRect.width,0,0));
        mesh.addVertex(topRight);
        
        //BOTTOM RIGHT
        mesh.addTexCoord(ofVec3f(videoRect.width,videoRect.height,0));
        mesh.addVertex(bottomRight);
        
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    		
        float xRotationPercent = ofMap(GetCloudsInputX(), 0, getCanvasWidth(), -rotationRange.x, rotationRange.x,true);
		currentRotation.x += (xRotationPercent - currentRotation.x) * .05;
		
        float yRotationPercent = ofMap(GetCloudsInputY(), 0, getCanvasHeight(), -rotationRange.y, rotationRange.y,true);
		currentRotation.y += (yRotationPercent - currentRotation.y) * .05;
        ofPushMatrix();
		ofTranslate(mesh.getCentroid());
		ofRotate(currentRotation.x, 0, 1, 0);
		ofRotate(currentRotation.y, 1, 0, 0);
		ofTranslate(-mesh.getCentroid());

        //Extract the rotation from the current rotation
        ofVec3f axis;
        float angle;
        curRot.getRotate(angle, axis);

        player->getTextureReference().bind();
        mesh.draw();
        player->getTextureReference().unbind();
        
        ofPopMatrix();
        
	}
    
    
}

// draw any debug stuff here
void CloudsVisualSystem2DVideo::selfDrawDebug(){
    
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfDrawBackground()
{

}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::render()
{
    
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystem2DVideo::selfEnd(){
	player->stop();
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystem2DVideo::selfExit(){
 
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystem2DVideo::selfKeyPressed(ofKeyEventArgs & args){
	
    if(args.key == 'i' ){
//		cout<<"in time :"<<player->getPosition()*player->getDuration()<<endl;
        inTime = player->getPosition()*player->getDuration();
	}
    else if (args.key == 'o'){
//        cout<< "out time :"<<player->getPosition()*player->getDuration()<<endl;
        outTime = player->getPosition()*player->getDuration();
    }
    else if (args.key == 'a'){
        cout<<timeline->getDurationInSeconds()<<endl;
    }
}
void CloudsVisualSystem2DVideo::selfKeyReleased(ofKeyEventArgs & args){

}

void CloudsVisualSystem2DVideo::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystem2DVideo::selfMouseMoved(ofMouseEventArgs& data){


}

void CloudsVisualSystem2DVideo::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystem2DVideo::selfMouseReleased(ofMouseEventArgs& data){
	
}