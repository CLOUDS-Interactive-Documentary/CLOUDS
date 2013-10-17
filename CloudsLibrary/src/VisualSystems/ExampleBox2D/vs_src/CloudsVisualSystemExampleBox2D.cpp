//
//  CloudsVisualSystemEmpty.cpp
//

#include "CloudsVisualSystemExampleBox2D.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events

void CloudsVisualSystemExampleBox2D::selfSetupGui(){
    customGui = new ofxUISuperCanvas("EXAMPLE BOX 2D", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	customGui->setName("EXAMPLE BOX 2D");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addToggle("GAVITY CONTROL MODE", &bGravityMod);
    
    customGui->addSlider("TRIGGER FORCE", 0, 30, &triggerForce);
    customGui->addSlider("MAX OBJECTS", 10, 1000, &maxObjects);
    
    customGui->addSlider("CIRCLE MIN SIZE", 5, 50, &circleSizeMin);
    customGui->addSlider("CIRCLE MAX SIZE", 5, 50, &circleSizeMax);
    customGui->addSlider("RECT MIN WIDTH", 5, 50, &rectSizeMin.x);
    customGui->addSlider("RECT MAX WIDTH", 5, 50, &rectSizeMax.x);
    customGui->addSlider("RECT MIN HEIGHT", 5, 50, &rectSizeMin.y);
    customGui->addSlider("RECT MAX HEIGHT", 5, 50, &rectSizeMax.y);
    
    customGui->addToggle("FILL", &bFill);
    customGui->addToggle("STATIC PLATFORMS", &bStaticPlatforms);
    customGui->addToggle("RANDOM PLATFORMS", &bRandomPlatforms);
    
    float length = (customGui->getGlobalCanvasWidth()-customGui->getWidgetSpacing()*5)/3.;
    float dim = customGui->getGlobalSliderHeight();
    
    /* Circle color */
    customGui->addLabel("CIRCLE COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("CR", 0.0, 1.0, &circleColor.r, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("CG", 0.0, 1.0, &circleColor.g, length, dim)->setShowValue(true);
    customGui->addMinimalSlider("CB", 0.0, 1.0, &circleColor.b, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    /* Rectangle color */
    customGui->addLabel("RECT COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("RR", 0.0, 1.0, &rectColor.r, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("RG", 0.0, 1.0, &rectColor.g, length, dim)->setShowValue(true);
    customGui->addMinimalSlider("RB", 0.0, 1.0, &rectColor.b, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    
    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemExampleBox2D::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemExampleBox2D::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "GAVITY CONTROL MODE") {
        selfBegin();
    }
    
    if (e.widget->getName() == "STATIC PLATFORMS") {
        if (bStaticPlatforms) {
            addStaticPlatforms();
        }
        else {
            for (int i=0; i<platforms.size(); i++) {
                platforms[i].destroy();
            }
            platforms.clear();
        }
    }
    
    if (e.widget->getName() == "RANDOM PLATFORMS") {
        if (bRandomPlatforms) {
            randomPlatformCounter=150;
        }
        else {
            removeRandomPlatform();
        }
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemExampleBox2D::selfSetupSystemGui(){
	
}

void CloudsVisualSystemExampleBox2D::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemExampleBox2D::selfSetupRenderGui(){
    
}

void CloudsVisualSystemExampleBox2D::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemExampleBox2D::selfSetup(){
    
    box2d.init();
    box2d.setGravity(0, 7);
    box2d.createBounds();
    box2d.setFPS(60);
    
    // preset init
    bGravityMod = false;
    gravityForce = ofVec2f(0, 5);
    minObjectCount = 50;
    maxObjects = 200;
    triggerForce = 15;
    
    bFill = false;
    bStaticPlatforms = false;
    bRandomPlatforms = false;
    
    randomPlatformCounter = 60;
    
    circleSizeMin = 4;
    circleSizeMax = 20;
    rectSizeMin.x = 5;
    rectSizeMax.x = 30;
    rectSizeMin.y = 5;
    rectSizeMax.y = 30;
    
    circleColor = ofFloatColor(0.16, 0.35, 0.31);
    rectColor = ofFloatColor(0.53, 0.68, 0.47);
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemExampleBox2D::selfPresetLoaded(string presetPath){
    
    cout<<"selfPresetLoaded\n";
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemExampleBox2D::selfBegin()
{
    cout<<"selfBegin()\n";
    if (bGravityMod) {
        // add objects if needed
        int objNum = circles.size() + rects.size();
        
        while (objNum < minObjectCount) {
            addRandomCircle();
            addRandomRect();
            objNum += 2;
        }
    }
    else {
        box2d.setGravity(0, 10);
    }
    
    if (bStaticPlatforms) {
        addStaticPlatforms();
    }
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemExampleBox2D::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemExampleBox2D::selfUpdate(){
    
    // remove excessive objects
    handleObjectLimit();

    box2d.setFPS(ofGetFrameRate());
    box2d.update();
    
    // handle random platform
    if (bRandomPlatforms) {
        randomPlatformCounter--;
        if (randomPlatformCounter==0) {
            addRandomPlatform();
        }
        if (randomPlatformCounter<-400) {
            removeRandomPlatform();
            randomPlatformCounter = 150;
        }
    }
    
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemExampleBox2D::selfDraw(){
    
}

// draw any debug stuff here
void CloudsVisualSystemExampleBox2D::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemExampleBox2D::selfDrawBackground()
{
    if (bFill) {
        ofFill();
    } else {
        ofNoFill();
        ofSetLineWidth(2);
    }
    
    // draw circles
    for (int i=0; i<circles.size(); i++)
    {
        ofSetColor(circleColor);
        ofPushMatrix();
        ofTranslate(circles[i].getPosition());
        ofRotate(circles[i].getRotation());
        ofEllipse(0, 0, circles[i].getRadius()*2, circles[i].getRadius()*2);
        ofSetColor(rectColor);
        ofLine(0, 0, circles[i].getRadius(), 0);
        ofPopMatrix();
    }
    
    // draw rectangles
    ofSetColor(rectColor);
    for (int i=0; i<rects.size(); i++)
    {
        ofPushMatrix();
        ofTranslate(rects[i].getPosition());
        ofRotate(rects[i].getRotation());
        ofRect(-rects[i].getWidth(), -rects[i].getHeight(),
               rects[i].getWidth()*2, rects[i].getHeight()*2);
        ofPopMatrix();
    }
    
    // draw static platforms
    ofNoFill();
    ofSetLineWidth(1);
    ofSetColor(200);
    for (int i=0; i<platforms.size(); i++)
    {
        platforms[i].draw();
    }
    
    // draw random platforms
    if (bRandomPlatforms)
    {
        float alpha;
        if (randomPlatformCounter<0) {
            if (randomPlatformCounter < -370) {
                alpha = ofMap(randomPlatformCounter, -370, -400, 255, 0);
            }
            else {
                alpha = ofMap(randomPlatformCounter, 0, -30, 0, 255);
            }
            ofSetColor(255, 255, 255, alpha);
            for (int i=0; i<randomPlatforms.size(); i++)
            {
                randomPlatforms[i].draw();
            }
        }
    }
    
    box2d.drawGround();
    
    if (bGravityMod) {
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        float angle = (atan2(gravityLine.y, gravityLine.x) * 180 / PI);
        float len = gravityLine.length();
        ofRotate(angle);
        ofSetColor(255, 255, 255);
        ofSetLineWidth(1);
        ofLine(0, 0, len, 0);
        ofLine(len, 0, len-5, -5);
        ofLine(len, 0, len-5, 5);
        ofPopMatrix();
    }
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemExampleBox2D::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemExampleBox2D::selfExit(){
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemExampleBox2D::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemExampleBox2D::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemExampleBox2D::selfMouseDragged(ofMouseEventArgs& data){
    prevMouse.x = data.x;
    prevMouse.y = data.y;
}

void CloudsVisualSystemExampleBox2D::selfMouseMoved(ofMouseEventArgs& data){
    
    ofVec2f curMouse(data.x, data.y);
    
    if (!bGravityMod) {
        ofVec2f speed = curMouse - prevMouse;
        
        if (speed.length() > triggerForce) {
            if (speed.x > 0) {
                // create circles
                float sd = (circleSizeMax - circleSizeMin) / 2;
                float mean = (circleSizeMin + circleSizeMax) / 2;
                float r = getGaussian()*sd + mean;
                ofxBox2dCircle circle;
                circle.setPhysics(3, 0.53, 0.1);
                circle.setup(box2d.getWorld(), data.x, data.y, r);
                circle.setVelocity(speed/2);
                circles.push_back(circle);
            }
            else if (speed.x < 0) {
                // create rects
                float wsd = (rectSizeMax.x - rectSizeMin.x)/2;
                float wmean = (rectSizeMax.x + rectSizeMin.x)/2;
                float w = getGaussian()*wsd + wmean;
                float hsd = (rectSizeMax.y - rectSizeMin.y)/2;
                float hmean = (rectSizeMax.y + rectSizeMin.y)/2;
                float h = getGaussian()*hsd + hmean;
                ofxBox2dRect rect;
                rect.setPhysics(3, 0.53, 0.1);
                rect.setup(box2d.getWorld(), data.x, data.y, w, h);
                rect.setVelocity(speed.x/2, speed.y/2);
                rects.push_back(rect);
            }
        }
        
        prevMouse = curMouse;
    }
    else {
        // gravity control mode
        gravityLine = ofVec2f(curMouse.x - ofGetWidth()/2,
                              curMouse.y - ofGetHeight()/2);
        ofVec2f gravityForce = gravityLine / 10;
        box2d.setGravity(gravityForce.x, gravityForce.y);
    }
}

void CloudsVisualSystemExampleBox2D::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleBox2D::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleBox2D::addRandomCircle()
{
    float r = ofRandom(circleSizeMin, circleSizeMax);
    ofxBox2dCircle circle;
    circle.setPhysics(3, 0.53, 0.1);
    circle.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()/2, r);
    circles.push_back(circle);
}

void CloudsVisualSystemExampleBox2D::addRandomRect()
{
    float w = ofRandom(rectSizeMin.x, rectSizeMax.x);
    float h = ofRandom(rectSizeMin.y, rectSizeMax.y);
    ofxBox2dRect rect;
    rect.setPhysics(3, 0.53, 0.1);
    rect.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()/2, w, h);
    rects.push_back(rect);
}

void CloudsVisualSystemExampleBox2D::addStaticPlatforms()
{
    if (platforms.size() > 0) {
        return;
    }
    
    ofxBox2dPolygon polyLine;
    polyLine.addVertex(ofGetWidth()*0.2, ofGetHeight()*0.8);
    polyLine.addVertex(ofGetWidth()*0.3, ofGetHeight()*0.9);
    polyLine.addVertex(ofGetWidth()*0.5, ofGetHeight()*0.86);
    polyLine.setPhysics(0.0, 0.5, 0.5);
    polyLine.create(box2d.getWorld());
    platforms.push_back(polyLine);
    
    polyLine = ofxBox2dPolygon();
    polyLine.addVertex(ofGetWidth()*0.8, ofGetHeight()*0.5);
    polyLine.addVertex(ofGetWidth()*0.7, ofGetHeight()*0.6);
    polyLine.addVertex(ofGetWidth()*0.5, ofGetHeight()*0.7);
    polyLine.setPhysics(0.0, 0.5, 0.5);
    polyLine.create(box2d.getWorld());
    platforms.push_back(polyLine);
}

void CloudsVisualSystemExampleBox2D::addRandomPlatform()
{
    ofxBox2dPolygon polygon;
    ofVec2f vec(ofRandom(100, 200), ofRandom(-100, 100));
    polygon.addVertex(vec);
    polygon.addVertex(vec*2);
    polygon.addVertex(vec*3);
    polygon.setPhysics(0.0, 0.5, 0.5);
    polygon.create(box2d.getWorld());
    polygon.setPosition(ofRandom(-100, ofGetWidth()*0.7),
                        ofRandom(ofGetHeight()/2, ofGetHeight()*0.8));
    randomPlatforms.push_back(polygon);
}

void CloudsVisualSystemExampleBox2D::removeRandomPlatform()
{
    if (randomPlatforms.size() <= 0) {
        return;
    }
    
    randomPlatforms[0].destroy();
    randomPlatforms.clear();
}

void CloudsVisualSystemExampleBox2D::handleObjectLimit()
{
    int delCircleNum = circles.size() - (int)maxObjects/2;
    for (int i=0; i<delCircleNum; i++)
    {
        ofxBox2dCircle c = circles.front();
        c.destroy();
        circles.erase(circles.begin());
    }
    
    int delRectNum = rects.size() - (int)maxObjects/2;
    for (int i=0; i<delRectNum; i++)
    {
        ofxBox2dRect r = rects.front();
        r.destroy();
        rects.erase(rects.begin());
    }
}

float CloudsVisualSystemExampleBox2D::getGaussian() {
    
    float x1 = ofRandomuf();
    
    float x2 = ofRandomuf();
    
    return sqrt (-2.0 * log(x1)) * cos(2.0 * PI * x2);
}
