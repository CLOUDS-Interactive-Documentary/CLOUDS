//
//  CloudsVisualSystemVisuals.cpp
//

#include "CloudsVisualSystemVisuals.h"
#include "CloudsLocalization.h"

CloudsVisualSystemVisuals::CloudsVisualSystemVisuals(){
    skipCameraSweep = false;
}
//This is called whenever a new preset is loaded, before selfSetup()
//use it to ensure all your simple variables are initialized to an
//acceptable default state
void CloudsVisualSystemVisuals::selfSetDefaults(){
    cylinderCellsWide = 5;
    cylinderCellsTall = 5;
    cylRadius = 0;
    rowHeight = 0;
    imageScale = 1.0;
    bFreeCam = false;
    cameraBackupDistance = 10;
    bSelectionChanged = false;
    bSelectionConfirmed = false;
    fontSize = 20;
    typeScale = .5;
    bMouseEventCanceled = false;

}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemVisuals::selfSetupGui(){

	cylinderGui = new ofxUISuperCanvas("CYLINDER", gui);
	cylinderGui->copyCanvasStyle(gui);
	cylinderGui->copyCanvasProperties(gui);
	cylinderGui->setName("Custom");
	cylinderGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

    cylinderGui->addToggle("FREE CAM", &bFreeCam);
	cylinderGui->addIntSlider("CELLS WIDE", 1, 15, &cylinderCellsWide);
	cylinderGui->addIntSlider("CELLS TALL", 1, 15, &cylinderCellsTall);
	cylinderGui->addSlider("RADIUS", 0, 500, &cylRadius);
	cylinderGui->addSlider("ROW HEIGHT", 0, 500, &rowHeight);
	cylinderGui->addSlider("IMAGE SCALE", 0, 1.0, &imageScale);
    
    cylinderGui->addSlider("CAMERA BACKUP DIST", 0, 500, &cameraBackupDistance);
    cylinderGui->addIntSlider("FONT SIZE", 4, 30, &fontSize);
    cylinderGui->addSlider("FONT SCALE", 0, 1.0, &typeScale);
    cylinderGui->addSlider("TYPE OFFSET X", 0, 250.0, &typeOffset.x);
    cylinderGui->addSlider("TYPE OFFSET Y", 0, 250.0, &typeOffset.y);
    
	ofAddListener(cylinderGui->newGUIEvent, this, &CloudsVisualSystemVisuals::selfGuiEvent);
	guis.push_back(cylinderGui);
	guimap[cylinderGui->getName()] = cylinderGui;
}

void CloudsVisualSystemVisuals::selfGuiEvent(ofxUIEventArgs &e){

}

void CloudsVisualSystemVisuals::skipNextCameraSweep(){
    skipCameraSweep = true;
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemVisuals::selfSetupSystemGui(){
	
}

void CloudsVisualSystemVisuals::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemVisuals::selfSetupRenderGui(){

}

void CloudsVisualSystemVisuals::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemVisuals::selfSetup(){

}

void CloudsVisualSystemVisuals::setVisuals(map<string, CloudsVisualSystemCredit>& visuals){

    for(map<string, CloudsVisualSystemCredit>::iterator it = visuals.begin(); it != visuals.end(); it++){
        string systemName = it->first;
        string imageFileName = getVisualSystemDataPath() + "thumbs/" + systemName + ".jpg";
        if(ofFile(imageFileName).exists()){
            
            thumbs[systemName].image.setUseTexture(false);
            thumbs[systemName].image.loadImage(imageFileName);
            thumbs[systemName].title = it->second.title;
            thumbs[systemName].byLine = it->second.line1;
        }
        else{
            ofLogError("CloudsVisualSystemVisuals::selfSetup") << "Missing thumbnail for " << imageFileName;
        }
    }
    
    layoutThumbnails();
}

void CloudsVisualSystemVisuals::pushTextures(){
    map<string, VisualThumb>::iterator it;
    for(it = thumbs.begin(); it != thumbs.end(); it++){
        it->second.image.setUseTexture(true);
        it->second.image.update();
    }
}

void CloudsVisualSystemVisuals::layoutThumbnails(){
    int curInt = 0;
    float totalheight = (thumbs.size() / cylinderCellsTall) * rowHeight;
    map<string, VisualThumb>::iterator it;
    for(it = thumbs.begin(); it != thumbs.end(); it++){
        int x = curInt % cylinderCellsWide;
        int y = curInt / cylinderCellsWide;
        float arcPoint = ofMap(x, 0, cylinderCellsWide, 0, 360);
        
        ofVec3f pos(0, -y * rowHeight + totalheight/2, cylRadius);
        pos.rotate(arcPoint, ofVec3f(0,1,0));
        ofVec3f centerPos = pos;
        centerPos.z = centerPos.x = 0;
        ofNode n;
        n.setPosition(pos);
        n.lookAt(centerPos, ofVec3f(0,1,0));
        it->second.mesh.clear();
        
        it->second.mesh.addVertex(n.getPosition() + n.getSideDir() * .5 * 1920 * imageScale + n.getUpDir() * 1080 * .5 * imageScale ); //top left
        it->second.mesh.addVertex(n.getPosition() - n.getSideDir() * .5 * 1920 * imageScale + n.getUpDir() * 1080 * .5 * imageScale ); //top right
        it->second.mesh.addVertex(n.getPosition() + n.getSideDir() * .5 * 1920 * imageScale - n.getUpDir() * 1080 * .5 * imageScale ); //bottom left
        it->second.mesh.addVertex(n.getPosition() - n.getSideDir() * .5 * 1920 * imageScale - n.getUpDir() * 1080 * .5 * imageScale ); //bottom right
        
        it->second.mesh.addTexCoord(ofVec2f(0,0));
        it->second.mesh.addTexCoord(ofVec2f(1920,0));
        it->second.mesh.addTexCoord(ofVec2f(0,1080));
        it->second.mesh.addTexCoord(ofVec2f(1920,1080));
        it->second.mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        
        it->second.normal = (pos - centerPos).normalize();
        
        it->second.pos = pos;
        it->second.rotation = arcPoint;
        it->second.onScreen = false;
        it->second.hovered = false;
        curInt++;
    }
}

vector<string> CloudsVisualSystemVisuals::getAvailableSystems(){
    map<string, VisualThumb>::iterator it;
    vector<string> visuals;
    for(it = thumbs.begin(); it != thumbs.end(); it++){
        visuals.push_back(it->first);
    }
    return visuals;
}

void CloudsVisualSystemVisuals::selectSystem(string systemName){

    if(systemName == selectedSystem){
        return;
    }

    selectedSystem = systemName;
    
    if(thumbs.find(systemName) == thumbs.end()){
        ofLogError("CloudsVisualSystemVisuals::selectSystem") << "System not found: " << systemName;
        return;
    }
    
    camTargetPos = thumbs[systemName].pos - thumbs[systemName].normal * cameraBackupDistance;
    camLookPos = thumbs[systemName].pos;
    
    targetCameraSideDir = camLookPos.normalized().getCrossed( ofVec3f(0,-1,0) );
    targetCameraUpDir   = camLookPos.normalized().getCrossed( targetCameraSideDir );

    if(skipCameraSweep){
        currentCameraUpDir = targetCameraUpDir;
        currentCameraSideDir = targetCameraUpDir;
        
        selectCamera.setPosition(camTargetPos);
        selectCamera.lookAt(camLookPos);
        skipCameraSweep = false;
    }

}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemVisuals::selfPresetLoaded(string presetPath){
//	layoutThumbnails();
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemVisuals::selfBegin(){
    layoutThumbnails();	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemVisuals::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemVisuals::selfUpdate(){
    
    if(currentFontSize != fontSize || !visualFont.isLoaded()){
        visualFont.loadFont(GetFontPath(), fontSize);
        currentFontSize = fontSize;
    }
    
    ofVec2f mousePos( GetCloudsInputX(), GetCloudsInputY() );
    map<string, VisualThumb>::iterator it;
    for(it = thumbs.begin(); it != thumbs.end(); it++){
        it->second.hovered = !bMouseEventCanceled && it->second.onScreen && it->second.screenPoly.inside(mousePos);
    }

    currentCameraSideDir += (targetCameraSideDir - currentCameraSideDir) * .03;
    currentCameraUpDir   += (targetCameraUpDir - currentCameraUpDir) * .03;
    
    float distFromTarget = selectCamera.getPosition().distance(camTargetPos);
    //ofVec3f targetPos = camTargetPos;
    ofVec3f rotAxis = camLookPos.getInterpolated(camTargetPos, .75);
//    rotAxis.x = 0;
//    rotAxis.z = 0;
    ofVec3f targetPos = camTargetPos.rotated(ofMap(GetCloudsInputX(), 0, getCanvasWidth(), 50, -50,true), rotAxis, currentCameraUpDir);
    targetPos = targetPos.rotated(ofMap(GetCloudsInputY(), 0, getCanvasHeight(), -90, 90,true), rotAxis, currentCameraSideDir);
    
    ofNode n = selectCamera;
    n.lookAt(camLookPos.getInterpolated(targetPos, ofMap(distFromTarget, cameraBackupDistance, cameraBackupDistance*2, .0, 1.0, true) ), ofVec3f(0,1,0) );
    selectCamera.setPosition( selectCamera.getPosition() + (targetPos - selectCamera.getPosition())*.03 );
    
    ofQuaternion q;
    q.slerp(.04, selectCamera.getOrientationQuat(), n.getOrientationQuat());
    selectCamera.setOrientation(q);
    
    ofRectangle screenRect(0,0, getCanvasWidth(), getCanvasHeight());
    for(map<string, VisualThumb>::iterator it = thumbs.begin(); it != thumbs.end(); it++){
        it->second.screenPoly.clear();

        ofVec3f a = it->second.mesh.getVertex(0);
        ofVec3f b = it->second.mesh.getVertex(1);
        ofVec3f c = it->second.mesh.getVertex(3);
        ofVec3f d = it->second.mesh.getVertex(2);
        float az = getCameraRef().getGlobalTransformMatrix().getInverse().preMult( a ).z;
        float bz = getCameraRef().getGlobalTransformMatrix().getInverse().preMult( b ).z;
        float cz = getCameraRef().getGlobalTransformMatrix().getInverse().preMult( c ).z;
        float dz = getCameraRef().getGlobalTransformMatrix().getInverse().preMult( d ).z;
        
        it->second.onScreen = az < 0 && bz < 0 && cz < 0 && dz < 0;
        if(!it->second.onScreen){
            continue;
        }
        
        ofVec2f sa = getCameraRef().worldToScreen(a);
        ofVec2f sb = getCameraRef().worldToScreen(b);
        ofVec2f sc = getCameraRef().worldToScreen(c);
        ofVec2f sd = getCameraRef().worldToScreen(d);
        
        it->second.onScreen = screenRect.inside(sa) || screenRect.inside(sb) || screenRect.inside(sc) || screenRect.inside(sd);
        if(!it->second.onScreen){
            continue;
        }
        
        it->second.screenPoly.addVertex( sa );
        it->second.screenPoly.addVertex( sb );
        it->second.screenPoly.addVertex( sc );
        it->second.screenPoly.addVertex( sd );
        it->second.screenPoly.addVertex( sa );

    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemVisuals::selfDraw(){
    map<string, VisualThumb>::iterator it;
    for(it = thumbs.begin(); it != thumbs.end(); it++){
        it->second.image.bind();
        it->second.mesh.draw();
        it->second.image.unbind();
        if(it->first == selectedSystem){
            ofNode n;
            n.setPosition(it->second.pos);
            n.lookAt(it->second.pos + it->second.normal, ofVec3f(0,1,0));
            n.setPosition(n.getPosition() -
                          n.getSideDir()  * typeOffset.x -
                          n.getUpDir()    * typeOffset.y);
            ofPushMatrix();
            ofMultMatrix(n.getGlobalTransformMatrix());
            ofScale(typeScale, -typeScale, typeScale);
            visualFont.drawString(it->second.title + " by " + it->second.byLine, 0, 0);
            ofPopMatrix();
        }
    }
    
//    ofPushStyle();
//    ofNode n;
//    n.setPosition(camLookPos);
//    n.lookAt(camTargetPos);
//    n.draw();
//    n.setPosition(camTargetPos);
//    n.lookAt(camLookPos);
//    n.draw();
//    ofPopStyle();
}


// draw any debug stuff here
void CloudsVisualSystemVisuals::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemVisuals::selfDrawBackground(){
	
}

void CloudsVisualSystemVisuals::selfDrawOverlay(){
    
    //TODO Proper highlight
    
    map<string, VisualThumb>::iterator it;
    for(it = thumbs.begin(); it != thumbs.end(); it++){
        if(it->second.onScreen){
            if(it->second.hovered){
                ofSetColor(ofColor::red);
            }
            else{
                ofSetColor(ofColor::white);
            }
            
            it->second.screenPoly.draw();
        }
    }
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemVisuals::selfEnd(){	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemVisuals::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemVisuals::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemVisuals::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemVisuals::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemVisuals::selfMouseMoved(ofMouseEventArgs& data){
    bMouseEventCanceled = data.canceled;
}

void CloudsVisualSystemVisuals::selfMousePressed(ofMouseEventArgs& data){
    
    if(data.canceled) return;
    
    ofVec2f mousePos(data.x + bleed, data.y + bleed);
    map<string, VisualThumb>::iterator it;
    for(it = thumbs.begin(); it != thumbs.end(); it++){
        if(it->second.hovered){
            if(selectedSystem == it->first){
                bSelectionConfirmed = true;
            }
            else{
                selectSystem(it->first);
                bSelectionChanged = true;
            }
        }
    }
}

bool CloudsVisualSystemVisuals::selectionChanged(){
    bool ret = bSelectionChanged;
    bSelectionChanged = false;
    return ret;
}

bool CloudsVisualSystemVisuals::selectionConfirmed(){
    bool ret = bSelectionConfirmed;
    bSelectionConfirmed = false;
    return ret;
}

string CloudsVisualSystemVisuals::getSelectedSystem(){
    return selectedSystem;
}

void CloudsVisualSystemVisuals::selfMouseReleased(ofMouseEventArgs& data){
	
}