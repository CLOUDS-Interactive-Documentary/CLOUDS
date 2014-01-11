//
//  CloudsVisualSystemOpenP5Sixteen.cpp
//

#include "CloudsVisualSystemOpenP5Sixteen.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Sixteen::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("SIXTEEN", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Sixteen");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5Sixteen::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Sixteen::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5Sixteen::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5Sixteen::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5Sixteen::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5Sixteen::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Sixteen::selfSetup()
{
//    width = ofGetWidth();
//    height = ofGetHeight();
    width = height = 255;
    pixels = new unsigned char[width * height * 4];
    tex.allocate(width, height, GL_RGBA);
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5Sixteen::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5Sixteen::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5Sixteen::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Sixteen::selfUpdate()
{
    t++;
    for (int i = 0; i < width * height; i++) {
        pixels[i * 4 + 0] = 0;    // R
        pixels[i * 4 + 1] = 0;    // G
        pixels[i * 4 + 2] = 0;    // B
        pixels[i * 4 + 3] = 255;  // A
    }
    
    int x, y, c;
    for (int idx = 0; idx < width * height; idx++) {
        x = idx % width + noise(idx);
        y = idx / width + noise(idx);
        
        c = 50 + (toHex(getColor(x, y)) >> 8 & width);
        setColor(x, y, ofColor(c, c, c * 2));
    }
    
//    cout << ofGetFrameRate() << endl;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Sixteen::selfDraw()
{

}

// draw any debug stuff here
void CloudsVisualSystemOpenP5Sixteen::selfDrawDebug(){

}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Sixteen::selfDrawBackground()
{
    tex.loadData(pixels, width, height, GL_RGBA);
    
	ofRectangle textureRect(0,0,width,height);
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
	ofRectangle screenRect(0,0, getCanvasWidth(), getCanvasHeight());
	textureRect.alignTo(screenRect, OF_ALIGN_HORZ_CENTER, OF_ALIGN_VERT_CENTER);
	
	tex.draw(textureRect);
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5Sixteen::selfEnd(){

	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Sixteen::selfExit()
{

}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5Sixteen::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5Sixteen::selfKeyReleased(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Sixteen::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemOpenP5Sixteen::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Sixteen::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Sixteen::selfMouseReleased(ofMouseEventArgs& data)
{

}

//--------------------------------------------------------------
int CloudsVisualSystemOpenP5Sixteen::noise(int i)
{
    return (int)(ofNoise(i % width / 99.0f, i / width / 99.0f, t / 99.0f + m++ % 2) * width - width / 2);
}

//--------------------------------------------------------------
int CloudsVisualSystemOpenP5Sixteen::toHex(ofColor col)
{
    return ((0xff & (unsigned char)col.a) << 24) |
           ((0xff & (unsigned char)col.r) << 16) |
           ((0xff & (unsigned char)col.g) <<  8) |
           ((0xff & (unsigned char)col.b));
}

//--------------------------------------------------------------
ofColor CloudsVisualSystemOpenP5Sixteen::getColor(int x, int y)
{
    if ((x < 0) || (y < 0) || (x >= width) || (y >= height)) return 0;
    
    int idx = y * width + x;
    return ofColor(pixels[idx * 4 + 0],   // R
                   pixels[idx * 4 + 1],   // G
                   pixels[idx * 4 + 2],   // B
                   pixels[idx * 4 + 3]);  // A
}

//--------------------------------------------------------------
int CloudsVisualSystemOpenP5Sixteen::setColor(int x, int y, ofColor col)
{
    if ((x < 0) || (y < 0) || (x >= width) || (y >= height)) return;
    
    int idx = y * width + x;
    pixels[idx * 4 + 0] = col.r;
    pixels[idx * 4 + 1] = col.g;
    pixels[idx * 4 + 2] = col.b;
    pixels[idx * 4 + 3] = col.a;
}
