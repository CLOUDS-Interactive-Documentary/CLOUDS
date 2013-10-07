//
//  CloudsVisualSystemEmpty.cpp
//

#include "CloudsVisualSystemExampleVectorMath.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemExampleVectorMath::selfSetupGui(){

	customGui = new ofxUISuperCanvas("EXAMPLE VECTOR MATH", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("EXAMPLE VECTOR MATH");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
    customGui->addSlider("CAM Z", 300, 1000, &camPos.z);
	customGui->addSlider("X ROTATION", -10, 10, &rotateAmount.x);
	customGui->addSlider("Y ROTATION", -10, 10, &rotateAmount.y);
    customGui->addSlider("Z ROTATION", -10, 10, &rotateAmount.z);
    customGui->addSlider("ROTATION SPEED", -2, 2, &speedOfRotation);
    customGui->addSlider("LINE WIDTH", 1, 20, &lineWidth);
    customGui->addSlider("FADE SPEED", 0.05, 2, &fadeSpeed);
    customGui->addToggle("AUTO COLOR SHIFT", &bShiftColor);
	customGui->addToggle("SHOW AXIS", &bShowAxis);
    customGui->addToggle("AUTO DRAW", &bAutoDraw);
    customGui->addSlider("AUTO DRAW SPEED", 0.1, 3, &autoDrawSpeed);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemExampleVectorMath::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemExampleVectorMath::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemExampleVectorMath::selfSetupSystemGui(){
	
}

void CloudsVisualSystemExampleVectorMath::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemExampleVectorMath::selfSetupRenderGui(){

}

void CloudsVisualSystemExampleVectorMath::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemExampleVectorMath::selfSetup(){
    
    cam.setPosition(0, 0, 750);
    speedOfRotation = 0.5f;
    lineWidth = 1;
    maxDrawPointAge = 10;
    fadeSpeed = 10;
    autoDrawSpeed = 2;
    bShiftColor = true;
    colorShift = 0;
    bAutoDraw = false;
    colorShiftSpeed = 0.5;
    lineColor = ofFloatColor::white;
    autoDraw.x = ofRandomf();
    autoDraw.y = ofRandomf();
    xAxisMin.set(-100,0,0);
	xAxisMax.set(100,0,0);
	yAxisMin.set(0,-100,0);
	yAxisMax.set(0,100,0);
	zAxisMin.set(0,0,-100);
	zAxisMax.set(0,0,100);
    
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemExampleVectorMath::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemExampleVectorMath::selfBegin(){
    pts.clear();
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemExampleVectorMath::selfSceneTransformation(){
	
}


bool IsInvisible (DrawPoint& pt) {
    return pt.color.a < 0.2;
}

//normal update call
void CloudsVisualSystemExampleVectorMath::selfUpdate(){

    cam.setPosition(camPos);
    

    // rotate the points
    for (int i = 0; i < pts.size(); i++){
        pts[i].pos = pts[i].pos.rotated(speedOfRotation, rotateAmount);
        pts[i].color.a -= ofGetLastFrameTime() * fadeSpeed;
    }
    pts.erase(std::remove_if(pts.begin(),pts.end(), IsInvisible), pts.end());

    xAxisMin.rotate(speedOfRotation, rotateAmount);
    xAxisMax.rotate(speedOfRotation, rotateAmount);
    yAxisMin.rotate(speedOfRotation, rotateAmount);
    yAxisMax.rotate(speedOfRotation, rotateAmount);
    zAxisMin.rotate(speedOfRotation, rotateAmount);
    zAxisMax.rotate(speedOfRotation, rotateAmount);

    if(bAutoDraw) {
        
        DrawPoint pt;
        pt.color = lineColor;
        pt.createdAt = ofGetElapsedTimef();
        pt.pos.x = 400 - ofNoise(autoDraw.x, ofGetElapsedTimef()*autoDrawSpeed)*800;
        pt.pos.y = 300 - ofNoise(autoDraw.y, ofGetElapsedTimef()*autoDrawSpeed)*600;
        pts.push_back( pt );
    }
    
    if(bShiftColor) {
        colorShift += ofGetLastFrameTime()*colorShiftSpeed;
        lineColor.setHsb(ofNoise(colorShift), 1, 1);
    }
    
//    while(pts.size() > MAX_N_PTS) {
//        pts.erase(pts.begin());
//    }
    
//    if(pts.size()>0 && pts[0].color.a<0.3){
//        ofLogNotice() << "removing element";
//        pts.erase(pts.begin());
//    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemExampleVectorMath::selfDraw(){
	
    cam.begin();
    ofPushMatrix();
    ofScale(1, -1, 1);
    
    
    ofSetLineWidth(lineWidth);
    ofSetHexColor(0xFFFFFF);
    
    ofNoFill();
    ofEnableAlphaBlending();
    //ofBeginShape();
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < pts.size(); i++){
        if(i>0 && (pts[i].createdAt - pts[i-1].createdAt) > 0.5) {
            //float dist = ofDist(pts[i].pos.x, pts[i].pos.y, pts[i-1].pos.x, pts[i-1].pos.y)> 100
            ofEndShape();
            ofBeginShape();
        } else {
            ofSetColor(pts[i].color.r*255, pts[i].color.g*255, pts[i].color.b*255, pts[i].color.a*255);
            glVertex3f(pts[i].pos.x, pts[i].pos.y, pts[i].pos.z);
            /*
            if(bCurveVertices) {
                ofCurveVertex(pts[i].pos);
            } else {
                ofVertex(pts[i].pos);
            }
             */
            //ofVertex(pts[i].pos);
        }
    }
    ofDisableAlphaBlending();
    //ofEndShape();
    glEnd();
    
    if(bShowAxis)
    {
        ofSetColor(50,50,255);
        ofSetLineWidth(1);
        glBegin(GL_LINES);
            glVertex3f(xAxisMin.x, xAxisMin.y, xAxisMin.z);
            glVertex3f(xAxisMax.x, xAxisMax.y, xAxisMax.z);
            glVertex3f(yAxisMin.x, yAxisMin.y, yAxisMin.z);
            glVertex3f(yAxisMax.x, yAxisMax.y, yAxisMax.z);
            glVertex3f(zAxisMin.x, zAxisMin.y, zAxisMin.z);
            glVertex3f(zAxisMax.x, zAxisMax.y, zAxisMax.z);
        glEnd();
    }
    
    ofPopMatrix();
    cam.end();
}

// draw any debug stuff here
void CloudsVisualSystemExampleVectorMath::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemExampleVectorMath::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemExampleVectorMath::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemExampleVectorMath::selfExit(){
	pts.clear();
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemExampleVectorMath::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemExampleVectorMath::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemExampleVectorMath::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemExampleVectorMath::selfMouseMoved(ofMouseEventArgs& data){

    float x = ofMap(data.x, 0, ofGetWidth(), -(ofGetWidth()/2.0), (ofGetWidth()/2.0));
    float y = ofMap(data.y, 0, ofGetHeight(), -(ofGetHeight()/2.0), (ofGetHeight()/2.0));

    if(!bAutoDraw) {
        DrawPoint pt;
        pt.pos.set(x, y);
        pt.color = lineColor;
        pt.createdAt = ofGetElapsedTimef();
        pts.push_back( pt );
    }

}

void CloudsVisualSystemExampleVectorMath::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleVectorMath::selfMouseReleased(ofMouseEventArgs& data){
	
}