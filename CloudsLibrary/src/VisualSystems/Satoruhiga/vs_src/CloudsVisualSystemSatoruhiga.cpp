//
//  CloudsVisualSystemSatoruhiga.cpp
//

#include "CloudsVisualSystemSatoruhiga.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif



struct Grid2D
{
    float rows;
    float cols;
    float width;
    float height;
    float scale;
    
    void operator()(Replecator *repl)
    {
        int X = rows;
        int Y = cols;
        
        repl->children.resize(X * Y);
        
        float s = scale;
        width *= s;
        height *= s;
        
        float inv_w = width / (X - 1);
        float inv_h = height / (Y - 1);
        
        int idx = 0;
        for (int y = 0; y < Y; y++)
        {
            for (int x = 0; x < X; x++)
            {
                repl->children[idx].m.glScale(s, s, s);
                repl->children[idx].m.glTranslate(inv_w * x, inv_h * y, 0);
                
                idx++;
            }
        }
    }
};

struct Drawer
{
    float width;
    float height;
    float rot_x;
    float rot_y;
    
    void operator()(Replecator *repl)
    {
		ofPushStyle();
		
        if (repl->parent == NULL) return;
        
        if (repl->idx % 2 == 0) ofNoFill();
        else ofFill();
        
        float t = ofGetElapsedTimef();
        
        float hue = fmodf(ofMap(repl->idx, 0, 24, 0, 255) + t, 255);
        ofColor c = ofColor::fromHsb(hue, 255, 255, 255);
        ofSetColor(c);
        
        ofTranslate(0, 0, hue);
        ofRotateY(hue * t * rot_x);
        ofRotateX(hue * t * rot_y);
        ofRect(0, 0, width, height);
		
		ofPopStyle();
    }
};

static Drawer drawer = {100, 100, 1, 0.5};
static Grid2D grid2d = {10, 10, 2000, 2000, 0.5};

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemSatoruhiga::selfSetupGui(){

	replecatorGui = new ofxUISuperCanvas("REPLECATOR", gui);
    replecatorGui->copyCanvasStyle(gui);
    replecatorGui->copyCanvasProperties(gui);
    replecatorGui->setName("Replecator");
    replecatorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    replecatorGui->addButton("REGENERATE", &regenerate);
    
    replecatorGui->addSlider("ROWS", 2, 20, &grid2d.rows);
    replecatorGui->addSlider("COLS", 2, 20, &grid2d.cols);
    replecatorGui->addSlider("WIDTH", 1, 2000, &grid2d.width);
    replecatorGui->addSlider("HEIGHT", 1, 2000, &grid2d.height);
    replecatorGui->addSlider("ITERATION_SCALE", 0.1, 1, &grid2d.scale);
    
    guis.push_back(replecatorGui);
    guimap[replecatorGui->getName()] = replecatorGui;
    
    //
    
    drawerGui = new ofxUISuperCanvas("DRAWER", gui);
    drawerGui->copyCanvasStyle(gui);
    drawerGui->copyCanvasProperties(gui);
    drawerGui->setName("Drawer");
    drawerGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    drawerGui->addSlider("RECT_W", 1, 1000, &drawer.width);
    drawerGui->addSlider("RECT_H", 1, 1000, &drawer.height);
    
    drawerGui->addSlider("ROT_X", -1, 1, &drawer.rot_x);
    drawerGui->addSlider("ROT_Y", -1, 1, &drawer.rot_y);
    
    guis.push_back(drawerGui);
    guimap[drawerGui->getName()] = drawerGui;
}

void CloudsVisualSystemSatoruhiga::selfGuiEvent(ofxUIEventArgs &e){
	
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemSatoruhiga::selfSetupSystemGui(){
	
}

void CloudsVisualSystemSatoruhiga::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemSatoruhiga::selfSetupRenderGui(){

}

void CloudsVisualSystemSatoruhiga::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemSatoruhiga::selfSetup(){
	
		
	
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemSatoruhiga::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemSatoruhiga::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemSatoruhiga::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemSatoruhiga::selfUpdate(){
    if (regenerate)
    {
        repl.setup(grid2d, 2);
    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemSatoruhiga::selfDraw(){
	mat->begin();
	repl.draw(drawer);
	mat->end();
}

// draw any debug stuff here
void CloudsVisualSystemSatoruhiga::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemSatoruhiga::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemSatoruhiga::selfEnd(){
	
	//simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemSatoruhiga::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemSatoruhiga::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemSatoruhiga::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemSatoruhiga::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSatoruhiga::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSatoruhiga::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSatoruhiga::selfMouseReleased(ofMouseEventArgs& data){
	
}