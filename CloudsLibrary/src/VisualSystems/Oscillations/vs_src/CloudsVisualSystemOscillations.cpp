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

	customGui = new ofxUISuperCanvas("Oscillations", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Precision", 1, 100, &precision);
	customGui->addSlider("CurveProgress", 0, 1, &curveProgress);
	customGui->addSlider("Height", 0, 10000, &curveHeight);
	customGui->addSlider("Width", 0, 10000, &curveWidth);
	customGui->addSlider("Depth", 0, 10000, &curveDepth);
	customGui->addSlider("Z Position", -1000, 1000, &curveZPos);
	customGui->addSlider("Speed", 0, 1, &speed);
    customGui->addSlider("Line Width", 0, 20, &lineWidth);
    
    customGui->addToggle("Black on white", &invertColorScheme);
    
//  customGui = new ofxUISuperCanvas("Shader", gui);
//	customGui->copyCanvasStyle(gui);
//	customGui->copyCanvasProperties(gui);
//	customGui->setName("Lens Effects");
//	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
//    
//    customGui->addSlider("Chromatic Abberation", 0, 1, &chromaAbbr);


    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOscillations::selfGuiEvent);
    
    gridControls = new ofxUISuperCanvas("Grid", gui);
    gridControls->copyCanvasStyle(gui);
	gridControls->copyCanvasProperties(gui);
	gridControls->setName("Grid");
	gridControls->setWidgetFontSize(OFX_UI_FONT_SMALL);

    gridControls->addToggle("Display Grid", &displayGrid);
    gridControls->addRangeSlider("Range", -5000, 5000, &(GridClipping.low), &(GridClipping.high));
    gridControls->addSlider("Spacing", 100, 2000, &GridPointSpacing);
    gridControls->addNumberDialer("Pattern", 0, 5, &GridPattern, 0);
    gridControls->addSlider("Line Width",0,10, &GridLineWidth);
    gridControls->addSlider("Alpha", 0, 1, &GridPointAlpha);

    ofAddListener(gridControls->newGUIEvent, this, &CloudsVisualSystemOscillations::selfGuiEvent);
	
    
    guis.push_back(customGui);
    guis.push_back(gridControls);
    guimap[customGui->getName()] = customGui;
	guimap[gridControls->getName()] = gridControls;
}

void CloudsVisualSystemOscillations::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}

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
        mesh.addColor(zero);
//        mesh.addVertex(ofPoint(i*1.0/NUMPOINTS,0,0));
                mesh.addVertex(ofPoint(0,0,i));
    }
    
    mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
    //TODO: Find way to update on every resize
    
    offsetX = offsetY = 0;
    BuildGrid();

    oscillator.load(getVisualSystemDataPath() +"shaders/oscillationsShader");
    crtShader.load(getVisualSystemDataPath() +"shaders/chromaticAbberation");

	
}

// selfPresetLoaded is called whenever a new preset is triggered
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
    
    ofFloatColor zero = invertColorScheme? ofFloatColor(1,1,1,0) : ofFloatColor(0,0,0,0);
    ofFloatColor c =    invertColorScheme? ofFloatColor(0,0,0,0.1) : ofFloatColor(1,1,1,0.2);
    
    
    oscillator.begin();
    oscillator.setUniform1f("numPoints", (float) NUMPOINTS);
    oscillator.setUniform4fv("targetColor", c.v); //FIXME: needs to be on color and off color
    oscillator.setUniform1f("curveProgress",curveProgress);
    oscillator.setUniform1f("precision", precision);
    oscillator.setUniform1f("offsetX", offsetX);
    oscillator.setUniform1f("offsetY", offsetY);
    oscillator.setUniform1f("curveWidth", curveWidth);
    oscillator.setUniform1f("curveHeight", curveHeight);
    oscillator.setUniform1f("curveZPos", curveZPos);
    oscillator.setUniform1f("curveDepth", curveDepth);
    oscillator.setUniform2f("resolution",width,height);
	
    oscillator.end();
    
    
    
//    float px, py, pz;
//    for (int i = 0; i < NUMPOINTS;  i++){
//        mesh.setColor(i, (i/(NUMPOINTS*1.0))>curveProgress ? zero : c);
//        px = cos((0.7+ 0.09 * 0.3 / width) * i * precision + offsetX);
//        py = sin(i * precision + offsetY);
//        pz = abs((-NUMPOINTS/2.0)+i)/(NUMPOINTS/2.0);
//        mesh.setVertex(i, ofPoint( -curveWidth/2.0 + px * curveWidth, -curveHeight/2.0 + py * curveHeight, -curveDepth/2.0 + curveZPos + pz * curveDepth));
//    }
    
    
    
    //FIXME: This shouldn't happen unprovoked. It needs to be a callback to the UI.
    BuildGrid();
}
// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOscillations::selfDraw(){
    
	ofPushStyle();
	
	//TODO: fix this without setting background
	
	//ofBackground(invertColorScheme? ofColor(180,180,180) : ofColor(0,0,0));
    
//    ofEnableBlendMode(OF_BLENDMODE_ADD);    
    if (displayGrid) {
        glLineStipple((int)GridPattern, 0x8888);
        ofSetLineWidth(GridLineWidth);
        glEnable(GL_LINE_STIPPLE);
        grid.setMode(OF_PRIMITIVE_LINES);
        
        
        grid.drawWireframe();
//        grid.drawVertices();
//        grid.draw();

        glDisable(GL_LINE_STIPPLE);
    }
    
	glDisable(GL_DEPTH_TEST);

    ofSetLineWidth(lineWidth);
    oscillator.begin();
//	mesh.drawWireframe();
	mesh.draw();
    oscillator.end();
	
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
    //DO NOT CHANGE THIS LINE< REZA MADE THIS
//    crtShader.setUniformTexture("screen", fbo.getTextureReference(), fbo.getTextureReference().getTextureData().textureID);
    
    
    crtShader.begin();
//    crtShader.setUniform1i("screen", GL_TEXTURE0);
    crtShader.setUniformTexture("screen", getSharedRenderTarget(), 0 );
    crtShader.setUniform2f("resolution",
						   float(getSharedRenderTarget().getWidth()),
						   float(getSharedRenderTarget().getHeight()));
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
    ofFloatColor c = (!invertColorScheme)? ofFloatColor(1,1,1,GridPointAlpha) : ofFloatColor(0,0,0,GridPointAlpha);
    for (float x = GridClipping.low; x < GridClipping.high ; x += spacing){
        for (float y = GridClipping.low; y < GridClipping.high ; y +=spacing){
            float   _x1 =x, _x2 = x,
                    _y1 = y, _y2 = y,
                    _z1 = GridClipping.low, _z2 = GridClipping.high;
            
            for (int i = 0 ; i < 3 ; i++){
                grid.addVertex(ofPoint(_x1,_y1,_z1));
                grid.addVertex(ofPoint(_x2,_y2,_z2));
                grid.addColor(c); grid.addColor(c);
                //Group theory in your face: (12)(23) ~ (123)
                swap(_y1, _z1); swap(_x1, _y1);
                swap(_y2, _z2); swap(_x2, _y2);
            }
        }
    }
}
