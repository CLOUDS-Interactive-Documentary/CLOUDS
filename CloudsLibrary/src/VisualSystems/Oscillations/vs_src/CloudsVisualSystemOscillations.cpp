//
//  CloudsVisualSystemOscillations.cpp
//

#include "CloudsVisualSystemOscillations.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events


#define NUMPOINTS 10000

void CloudsVisualSystemOscillations::selfSetupGui(){
    
    //Using James' values.
    float length = (gui->getGlobalCanvasWidth()-gui->getWidgetSpacing()*5)/3.;
    float dim = gui->getGlobalSliderHeight();
    
	curveControls = new ofxUISuperCanvas("Oscillations", gui);
	curveControls->copyCanvasStyle(gui);
	curveControls->copyCanvasProperties(gui);
	curveControls->setName("Custom");
	curveControls->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	curveControls->addSlider("Precision", 0.1, 100, &precision);
	curveControls->addSlider("CurveProgress", 0, 1, &curveProgress);
	curveControls->addSlider("Height", 0, 10000, &curveHeight);
	curveControls->addSlider("Width", 0, 10000, &curveWidth);
	curveControls->addSlider("Depth", 0, 10000, &curveDepth);
	curveControls->addSlider("Z Position", -1000, 1000, &curveZPos);
	curveControls->addSlider("Speed", 0, 0.5, &speed);

    
    curveControls->addSpacer("Color");
    curveControls->addMinimalSlider("Red", 0, 1, &curveColor.r, length, dim)->setShowValue(false);
    curveControls->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    curveControls->addMinimalSlider("Green", 0, 1, &curveColor.g, length, dim)->setShowValue(false);
    curveControls->addMinimalSlider("Blue", 0, 1, &curveColor.b, length, dim)->setShowValue(false);
    curveControls->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    curveControls->addMinimalSlider("Alpha", 0, 1, &curveColor.a);
    curveControls->addMinimalSlider("Line Width", 0.01, 20, &CurveLineWidth, length * 3./2., dim)->setShowValue(false);
    curveControls->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    curveControls->addToggle("Render Lines", &renderLines);
    
    curveControls->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    ofAddListener(curveControls->newGUIEvent, this, &CloudsVisualSystemOscillations::selfGuiEvent);
    
    gridControls = new ofxUISuperCanvas("Grid", gui);
    gridControls->copyCanvasStyle(gui);
	gridControls->copyCanvasProperties(gui);
	gridControls->setName("Grid");
	gridControls->setWidgetFontSize(OFX_UI_FONT_SMALL);

    gridControls->addToggle("Display Grid", &displayGrid);
    gridControls->addRangeSlider("Range", -5000, 5000, &(GridClipping.low), &(GridClipping.high));
    gridControls->addSlider("Spacing", 100, 2000, &GridPointSpacing);
    gridControls->addNumberDialer("Pattern", 0, 5, &GridPattern, 0);
    gridControls->addSlider("Line Width",0.01, 10, &GridLineWidth);
    
    gridControls->addSpacer("Color");
    gridControls->addMinimalSlider("Red", 0, 1, &gridColor.r, length, dim)->setShowValue(false);
    gridControls->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gridControls->addMinimalSlider("Green", 0, 1, &gridColor.g, length, dim)->setShowValue(false);
    gridControls->addMinimalSlider("Blue", 0, 1, &gridColor.b, length, dim)->setShowValue(false);
    gridControls->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gridControls->addMinimalSlider("Alpha", 0, 1, &gridColor.a);

    ofAddListener(gridControls->newGUIEvent, this, &CloudsVisualSystemOscillations::selfGuiEvent);
	
    colorControls = new ofxUISuperCanvas("Color", gui);
    colorControls->copyCanvasStyle(gui);
    colorControls->copyCanvasProperties(gui);
    colorControls->setName("Color");
    colorControls->setWidgetFontSize(OFX_UI_FONT_SMALL);
    colorControls->addSlider("Chromatic Abberation", 0.0, 2.0, &chromaAbbr);
    colorControls->addSlider("CRT Distortion", 0.0, 2.0, &lensDistortion);
    
    guis.push_back(curveControls);
    guis.push_back(gridControls);
    guis.push_back(colorControls);
	
    guimap[curveControls->getName()] = curveControls;
	guimap[gridControls->getName()] = gridControls;
    guimap[colorControls->getName()] = colorControls;
}

void CloudsVisualSystemOscillations::selfSetupCameraGui(){
	camGui->addSlider("near plane", 0.01, 10, &clipPlanes.min);
	camGui->addSlider("far plane" , 1000, 5000, &clipPlanes.max);
}

void CloudsVisualSystemOscillations::selfGuiEvent(ofxUIEventArgs &e){
    //Check if the grid was updated
    if (e.widget->getName() == "Grid"){
        BuildGrid();
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOscillations::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOscillations::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOscillations::selfSetupRenderGui(){

}

void CloudsVisualSystemOscillations::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOscillations::selfSetup(){
    
    ofEnableAlphaBlending();
    
    ofFloatColor zero = ofFloatColor(0,0,0);
    for (int i = 0; i < NUMPOINTS ; i++){
        //the Zpos serves as an index
        mesh.addVertex(ofPoint(0,0,i));
    }
    
    //TODO: Find way to update on every resize
    offsetX = offsetY = 0;
    BuildGrid();
    loadShader();
}

void CloudsVisualSystemOscillations::loadShader(){
    
    oscillator.load(getVisualSystemDataPath() + "shaders/oscillationsShader");
    crtShader.load(getVisualSystemDataPath() + "shaders/chromaticAbberation");

}

void CloudsVisualSystemOscillations::selfSetDefaults(){
    curveWidth = 400;
    curveHeight = 400;
    curveDepth = 7000;
    speed = 0.015;
    renderLines = true;
    bgColor = curveColor.getInverted();
    CurveLineWidth = 0.1;
    displayGrid = false;
    curveProgress = NUMPOINTS;
    
    clipPlanes.min = 0.01;
    clipPlanes.max = 5000;
}


// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOscillations::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOscillations::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOscillations::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOscillations::selfUpdate(){
    
    width = ofGetWidth();
    height = ofGetHeight();
    offsetX += speed;
    
    if (renderLines){
        mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
    } else {
        mesh.setMode(OF_PRIMITIVE_POINTS);
    }
    
    //FIXME: This shouldn't happen unprovoked. It needs to be a callback to the UI.
    BuildGrid();
	
	getCameraRef().setNearClip(clipPlanes.min);
	getCameraRef().setFarClip(clipPlanes.max);
}
// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOscillations::selfDraw(){
    
	ofPushStyle();
    glPushAttrib(GL_LINE_SMOOTH | GL_DEPTH_TEST | GL_POINT_SIZE);
    glEnable( GL_LINE_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glDisable(GL_DEPTH_TEST);
//    ofEnableBlendMode(OF_BLENDMODE_ADD);
    if (displayGrid) {
        glLineStipple((int)GridPattern, 0x8888);
        ofSetLineWidth(GridLineWidth);
        
        glEnable(GL_LINE_STIPPLE);
        grid.setMode(OF_PRIMITIVE_LINES);
        grid.drawWireframe();
        glDisable(GL_LINE_STIPPLE);
    }
    
	
    
    //TODO: Customize
    glPointSize(2.0);

    oscillator.begin();
    
    oscillator.setUniform1f("numPoints", (float) NUMPOINTS);
    oscillator.setUniform4fv("targetColor", curveColor.v); //FIXME: needs to be on color and off color
    oscillator.setUniform1f("curveProgress",curveProgress);
    oscillator.setUniform1f("selfPrecision", precision);
    oscillator.setUniform1f("offsetX", offsetX);
    oscillator.setUniform1f("offsetY", offsetY);
    oscillator.setUniform1f("curveWidth", curveWidth);
    oscillator.setUniform1f("curveHeight", curveHeight);
    oscillator.setUniform1f("curveZPos", curveZPos);
    oscillator.setUniform1f("curveDepth", curveDepth);
    oscillator.setUniform2f("resolution",width,height);
	
    ofSetLineWidth(CurveLineWidth);
	mesh.draw();
    oscillator.end();
    glPopAttrib();
	ofPopStyle();
}

// draw any debug stuff here
void CloudsVisualSystemOscillations::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOscillations::selfDrawBackground(){
    
    
	//turn the background refresh off
	//bClearBackground = false;
	
}


//Overriding the superclass
void CloudsVisualSystemOscillations::selfPostDraw(){

    crtShader.begin();
    crtShader.setUniformTexture("screen", getSharedRenderTarget(), 1 );
    crtShader.setUniform2f("resolution",
						   float(getSharedRenderTarget().getWidth()),
						   float(getSharedRenderTarget().getHeight()));
    crtShader.setUniform1f("abberationAmount", chromaAbbr);
    crtShader.setUniform1f("distortionAmount", lensDistortion);
    
    getSharedRenderTarget().draw(0,CloudsVisualSystem::getSharedRenderTarget().getHeight(),
													 CloudsVisualSystem::getSharedRenderTarget().getWidth(),
													 -CloudsVisualSystem::getSharedRenderTarget().getHeight());
	CloudsVisualSystem::selfPostDraw();
	
    crtShader.end();

}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOscillations::selfEnd(){
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOscillations::selfExit(){
	//FIXME: This isn't a proper destructor.
    mesh.clear();
    grid.clear();
    
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOscillations::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOscillations::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOscillations::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOscillations::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOscillations::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOscillations::selfMouseReleased(ofMouseEventArgs& data){
	
}



/* Custom Functions */

void CloudsVisualSystemOscillations::BuildGrid(){
    grid.clear();
    int spacing = (int) floor(GridPointSpacing);
    for (float x = GridClipping.low; x < GridClipping.high ; x += spacing){
        for (float y = GridClipping.low; y < GridClipping.high ; y +=spacing){
            float   _x1 =x, _x2 = x,
                    _y1 = y, _y2 = y,
                    _z1 = GridClipping.low, _z2 = GridClipping.high;
            
            for (int i = 0 ; i < 3 ; i++){
                grid.addVertex(ofPoint(_x1,_y1,_z1));
                grid.addVertex(ofPoint(_x2,_y2,_z2));
                grid.addColor(gridColor); grid.addColor(gridColor);
                //Group theory in yo face: (12)(23) ~ (123)
                swap(_y1, _z1); swap(_x1, _y1);
                swap(_y2, _z2); swap(_x2, _y2);
            }
        }
    }
}
