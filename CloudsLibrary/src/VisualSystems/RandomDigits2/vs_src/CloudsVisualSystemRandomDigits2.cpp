//
//  CloudsVisualSystemRandomDigits.cpp
//

#include "CloudsVisualSystemRandomDigits2.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemRandomDigits2::selfSetupGui(){
    
    
    ofGetRectMode() == OF_RECTMODE_CENTER;
    
    customGui = new ofxUISuperCanvas("RANDOM DIGITS 2", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
    customGui->setName("RANDOM DIGITS 2");
    customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addIntSlider("X Count", 1, 200, &xCount);
    customGui->addIntSlider("Y Count", 1, 200, &yCount);
    customGui->addIntSlider("Z Count", 1, 200, &zCount);
    customGui->addIntSlider("X Spacing", 1, 200, &xSpacing);
    customGui->addIntSlider("Y Spacing", 1, 200, &ySpacing);
    customGui->addIntSlider("Z Spacing", 1, 1000, &zSpacing);
    
    customGui->addSlider("X Noise", 0, 200, &xNoise);
    customGui->addSlider("Y Noise", 0, 200, &yNoise);
    customGui->addSlider("Z Noise", 0, 500, &zNoise);
    customGui->addButton("Update Number Placing", true);
    customGui->addSlider("Dark Color", 0, 2, &dark);
    customGui->addSlider("Light Color", 0, 2, &light);
    customGui->addSlider("FOG DENSITY", 0.0f, 0.1f, &fogDensity);
    
     ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemRandomDigits2::selfGuiEvent);
     guis.push_back(customGui);
     guimap[customGui->getName()] = customGui;
 
}

void CloudsVisualSystemRandomDigits2::selfGuiEvent(ofxUIEventArgs &e)
{
    if(e.widget->getName() == "Update Number Placing") {
        initRandomNumbers();
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemRandomDigits2::selfSetupSystemGui(){
	
}

void CloudsVisualSystemRandomDigits2::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemRandomDigits2::selfSetupRenderGui(){
    
}

void CloudsVisualSystemRandomDigits2::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemRandomDigits2::selfSetup()
{
    // load texture
    numbersImg.loadImage(getVisualSystemDataPath() + "/numbers.png");
    
    // init defaults
    xCount = 30;
    xSpacing = 50;
    yCount = 30;
    ySpacing = 50;
    zCount = 100;
    zSpacing = 100;
    xNoise = 0;
    yNoise = 0;
    zNoise = 0;
    dark = 0.6;
    light = 2;
    fogDensity = 0.025f;
}


// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemRandomDigits2::selfPresetLoaded(string presetPath)
{
    initRandomNumbers();
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemRandomDigits2::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemRandomDigits2::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemRandomDigits2::selfUpdate()
{
    vector<ofVec2f> uvs;
    vector<ofFloatColor> colors;

    ofFloatColor darkColor = ofFloatColor(dark, dark, dark);
    ofFloatColor lightColor = ofFloatColor(light, light, light);
    
    for (int i=0; i<numbers.size(); i++)
    {
        if (ofRandom(400) < 1) {
            numbers[i].triggerCount();
        }
        
        numbers[i].update();
        numbers[i].fillUvs(uvs);
        numbers[i].fillColors(colors, darkColor, lightColor);
    }
    
    vbo.setTexCoordData(&uvs[0], uvs.size(), GL_STATIC_DRAW);
    vbo.setColorData(&colors[0], colors.size(), GL_STATIC_DRAW);
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemRandomDigits2::selfDraw(){
    
    glPushAttrib(GL_FOG_BIT);
    
    glEnable(GL_FOG);
	glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, powf(fogDensity, 2));
    GLfloat fogColor[4] = { bgColor.r / 255.,bgColor.g / 255.,bgColor.b / 255., 1.0 };
    glFogfv(GL_FOG_COLOR, fogColor);
    glEnable(GL_DEPTH_TEST);
    ofEnableAlphaBlending();
    
    ofPushStyle();
//    cam.begin();
    
    numbersImg.bind();
    ofSetColor(255, 255, 255);
    glDisable(GL_DEPTH_TEST);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    vbo.drawElements(GL_TRIANGLES, numbers.size()*6);

    numbersImg.unbind();
    
//    cam.end();
    ofPopStyle();
    
}


// draw any debug stuff here
void CloudsVisualSystemRandomDigits2::selfDrawDebug(){
    
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemRandomDigits2::selfDrawBackground(){
    
	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemRandomDigits2::selfEnd(){
    
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemRandomDigits2::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemRandomDigits2::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemRandomDigits2::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemRandomDigits2::selfMouseDragged(ofMouseEventArgs& data){
    
    
	
}


void CloudsVisualSystemRandomDigits2::selfMouseMoved(ofMouseEventArgs& data){
    
}

void CloudsVisualSystemRandomDigits2::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemRandomDigits2::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemRandomDigits2::initRandomNumbers()
{
    // init the numbers
    cout<<"initRandomNumbers"<<endl;
    
    numbers.clear();
    
    int id=0;
    int zStart = -(zCount*zSpacing/2);
    int zEnd = zCount*zSpacing/2;
    int yStart = -(yCount*ySpacing/2);
    int yEnd = yCount*ySpacing/2;
    int xStart = -(xCount*xSpacing/2);
    int xEnd = xCount*xSpacing/2;
    for (int z=zStart; z<zEnd; z+=zSpacing)
    {
        for (int y=yStart; y<yEnd; y+=ySpacing)
        {
            for (int x=xStart; x<xEnd; x+=xSpacing)
            {
                int row = (int)ofRandom(102);
                numbers.push_back(RandomNumber(id++, x+ofRandom(xNoise), y+ofRandom(yNoise), z+ofRandom(zNoise), row, 0));
            }
        }
        
    }
    
    // create the geometry VBO
    vector<ofVec3f> verts;
    vector<ofIndexType> indices;
    for (int i=0; i<numbers.size(); i++)
    {
        numbers[i].fillVertices(verts);
        numbers[i].fillIndices(indices);
    }
    vbo.setVertexData(&verts[0], verts.size(), GL_STATIC_DRAW);
    vbo.setIndexData(&indices[0], indices.size(), GL_STATIC_DRAW);
}

ofFloatColor CloudsVisualSystemRandomDigits2::getRGBfromHSV(ofFloatColor& hsv)
{
    ofFloatColor col = ofFloatColor::fromHsb(hsv.r,hsv.g,hsv.b,hsv.a);
    if(labelColors.find(&hsv) != labelColors.end()){
        ofFloatColor colcpy = col;
        colcpy.a = .5 + colcpy.a*.5;
        labelColors[&hsv]->setColorFill(colcpy);
    }
    return col;
}

void CloudsVisualSystemRandomDigits2::addColorToGui(ofxUISuperCanvas* gui,string prefix,ofFloatColor& col, bool doAlpha, float min, float max)
{
    
   	float length = (gui->getGlobalCanvasWidth() - gui->getWidgetSpacing()*5)/3.;
    float dim    = gui->getGlobalSliderHeight();
    
    string shortprefix;
    vector<string> comps = ofSplitString(prefix, " ", true,true);
    for(int i = 0; i < comps.size(); i++) shortprefix += comps[i].at(0);
    
    ofxUILabel* label = gui->addLabel(prefix);
    labelColors[&col] = label;
    
    gui->addMinimalSlider(shortprefix + " HUE", min, max, &col.r, length, dim)->setShowValue(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addMinimalSlider(shortprefix + " SAT", min, max, &col.g, length, dim)->setShowValue(false);
    gui->addMinimalSlider(shortprefix + " BRI", min, max, &col.b, length, dim)->setShowValue(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    if(doAlpha){
        gui->addMinimalSlider(shortprefix + " ALPHA", min, max, &col.a);
    }
}


