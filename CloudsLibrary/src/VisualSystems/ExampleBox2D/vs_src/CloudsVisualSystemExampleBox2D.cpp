//
//  CloudsVisualSystemEmpty.cpp
//

#include "CloudsVisualSystemExampleBox2D.h"
#include "CloudsRGBDVideoPlayer.h"

static int id0 = 0;
static int id1 = 1;

//These methods let us add custom GUI parameters and respond to their events

void CloudsVisualSystemExampleBox2D::selfSetupGui(){
    customGui = new ofxUISuperCanvas("EXAMPLE BOX 2D", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	customGui->setName("EXAMPLE BOX 2D");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addToggle("GAVITY CONTROL MODE", &bGravityMod);
    
    customGui->addToggle("CIRCLES", &bCircles);
    customGui->addToggle("RECTANGLES", &bRects);
    
    customGui->addSlider("TRIGGER FORCE", 0, 30, &triggerForce);
    customGui->addSlider("MAX CIRCLES", 10, 1000, &maxCircles);
    customGui->addSlider("MAX RECTANGLES", 10, 1000, &maxRects);
    
    customGui->addSlider("CIRCLE MAX MEAN", 5, 50, &circleSizeMean);
    customGui->addSlider("CIRCLE SIZE DEVIATION", 5, 50, &circleSizeDev);
    customGui->addSlider("RECT MAX MEAN", 5, 50, &rectSizeMean);
    customGui->addSlider("RECT SIZE DEVIATION", 5, 50, &rectSizeDev);
    
    customGui->addToggle("FILL", &bFill);
    customGui->addToggle("STATIC PLATFORMS", &bStaticPlatforms);
    customGui->addToggle("RANDOM PLATFORMS", &bRandomPlatforms);
    
    float length = (customGui->getGlobalCanvasWidth()-customGui->getWidgetSpacing()*5)/3.;
    float dim = customGui->getGlobalSliderHeight();
    
    /* Circle color */
    customGui->addLabel("CIRCLE COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("H1", 0.0, 255, &circleHSB.r, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("S1", 0.0, 255, &circleHSB.g, length, dim)->setShowValue(true);
    customGui->addMinimalSlider("B1", 0.0, 255, &circleHSB.b, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    /* Circle Line color */
    customGui->addLabel("CIRCLE LINE COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("H2", 0.0, 255, &circleLineHSB.r, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("S2", 0.0, 255, &circleLineHSB.g, length, dim)->setShowValue(true);
    customGui->addMinimalSlider("B2", 0.0, 255, &circleLineHSB.b, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    /* Rectangle color */
    customGui->addLabel("RECT COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("H3", 0.0, 255, &rectHSB.r, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("S3", 0.0, 255, &rectHSB.g, length, dim)->setShowValue(true);
    customGui->addMinimalSlider("B3", 0.0, 255, &rectHSB.b, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    customGui->addSpacer("SOUNDS");
    
    customGui->addSlider("Volume", 0, 1, &masterVolume);

    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemExampleBox2D::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemExampleBox2D::selfSetDefaults(){
    primaryCursorMode = CURSOR_MODE_DRAW;
    secondaryCursorMode = CURSOR_MODE_DRAW;
}

void CloudsVisualSystemExampleBox2D::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "GAVITY CONTROL MODE") {
        selfBegin();
    }
    else if (e.widget->getName() == "STATIC PLATFORMS") {
        if (bStaticPlatforms) {
            addStaticPlatforms();
        }
        else {
            removeStaticPlatforms();
        }
    }
    else if (e.widget->getName() == "RANDOM PLATFORMS") {
        if (bRandomPlatforms) {
            randomPlatformCounter=150;
        }
        else {
            removeRandomPlatform();
        }
    }
    else if (e.widget->getName() == "CIRCLES") {
        if (!bCircles) {
            for (int i=0; i<circles.size(); i++)
            {
                circles[i].destroy();
            }
            circles.clear();
        }
    }
    else if (e.widget->getName() == "RECTANGLES") {
        if (!bRects) {
            for (int i=0; i<rects.size(); i++)
            {
                rects[i].destroy();
            }
            rects.clear();
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

void CloudsVisualSystemExampleBox2D::guiRenderEvent(ofxUIEventArgs &e)
{
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemExampleBox2D::selfSetup()
{
	bIs2D = true;
	
    box2d.init();
    box2d.setGravity(0, 7);
    box2d.createBounds();
    box2d.setFPS(60);
	ofAddListener(box2d.contactStartEvents,
                  this,
                  &CloudsVisualSystemExampleBox2D::contactStart);
    
    //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
    prevScreenSize = ofVec2f(getCanvasWidth(), getCanvasHeight());
    
    // preset init
    bGravityMod = false;
    gravityForce = ofVec2f(0, 5);
    minObjectCount = int(ofRandom(75,200));
    maxCircles = 200;
    maxRects = 150;
    triggerForce = 15;
    
    bCircles = true;
    bRects = true;
    bFill = false;
    bStaticPlatforms = false;
    bRandomPlatforms = false;
    
    randomPlatformCounter = 60;
    
    circleSizeDev = 10;
    circleSizeMean = 20;
    rectSizeDev = 10;
    rectSizeMean = 20;
    
    
    circleHSB = ofFloatColor(100, 100, 100);
    circleLineHSB = ofFloatColor(100, 100, 100);
    rectHSB = ofFloatColor(100, 100, 100);
    
    // setup sound synth
    masterVolume = 1;
    lastSampleTime = 0;
    soundsLoaded = true;
    for (int i=0; i<5; i++)
    {
        ostringstream fn;
        fn << GetCloudsDataPath() << "sound/triggers/drip" << i+1 << ".aif";
        soundsLoaded &= samplePlayer[i].loadSound(fn.str());
        samplePlayer[i].setMultiPlay(true);
        samplePlayer[i].setSpeed(0.5);
    }
    
    for (int i=0; i<5; i++)
    {
        ostringstream fn;
        fn << GetCloudsDataPath() << "sound/triggers/cardboard" << i+1 << ".aif";

        soundsLoaded &= samplePlayer[i+5].loadSound(fn.str());
        samplePlayer[i+5].setMultiPlay(true);

    }
    
    if(!soundsLoaded){
        ofLogError("CloudsVisualSystemExampleBox2D::selfSetup") << "Some clips failed to load. Disabling sound";
    }

}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemExampleBox2D::selfPresetLoaded(string presetPath)
{
    reinitBounds();
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemExampleBox2D::selfBegin()
{
    if (bGravityMod) {
        // add objects if needed
        createRandomObjects();
    }
    else {
        box2d.setGravity(0, 10);
    }
    
    if (bStaticPlatforms) {
        addStaticPlatforms();
    }
    else {
        removeStaticPlatforms();
    }
    
    if (!bRandomPlatforms) {
        removeRandomPlatform();
    }
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemExampleBox2D::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemExampleBox2D::selfUpdate(){
    
    // reinit world bound when screen size changes
    //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
    if (prevScreenSize.x != getCanvasWidth() ||
        prevScreenSize.y != getCanvasHeight()) {
        reinitBounds();
    
        prevScreenSize = ofVec2f(getCanvasWidth(), getCanvasHeight());
    }
    
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
	ofPushStyle();
	
    ofColor cColor = ofColor::fromHsb(circleHSB.r, circleHSB.g, circleHSB.b);
    ofColor clColor = ofColor::fromHsb(circleLineHSB.r, circleLineHSB.g, circleLineHSB.b);
    ofColor rColor = ofColor::fromHsb(rectHSB.r, rectHSB.g, rectHSB.b);
    
    if (bFill) {
        ofFill();
    } else {
        ofNoFill();
        ofSetLineWidth(2);
    }
    
    // draw circles
    for (int i=0; i<circles.size(); i++)
    {
        ofPushMatrix();
        ofTranslate(circles[i].getPosition());
        ofRotate(circles[i].getRotation());
        ofSetLineWidth(2);
        ofSetColor(cColor);
        ofEllipse(0, 0, circles[i].getRadius()*2, circles[i].getRadius()*2);
        ofSetColor(clColor);
        ofLine(0, 0, circles[i].getRadius(), 0);
        ofPopMatrix();
    }
    
    // draw rectangles
    ofSetColor(rColor);
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
        //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
        ofTranslate(getCanvasWidth()/2, getCanvasHeight()/2);
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
	
	ofPopStyle();
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemExampleBox2D::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemExampleBox2D::selfExit(){
	ofRemoveListener(box2d.contactStartEvents,
                  this,
                  &CloudsVisualSystemExampleBox2D::contactStart);

}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemExampleBox2D::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemExampleBox2D::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemExampleBox2D::selfMouseDragged(ofMouseEventArgs& data){
    ofVec2f curMouse(data.x, data.y);
    
    if (!bGravityMod) {
        ofVec2f speed = curMouse - prevMouse;
        
        if (speed.length() > triggerForce) {
            if (bCircles && (speed.x > 0 || !bRects)) {
                // create circles
                float r = getGaussian()*circleSizeDev + circleSizeMean;
                addCircle(curMouse, speed, r);
            }
            else if (bRects && (speed.x < 0 || !bCircles)) {
                // create rects
                float w = getGaussian()*rectSizeDev + rectSizeMean;
                float h = getGaussian()*rectSizeDev + rectSizeMean;
                addRect(curMouse, speed, ofVec2f(w, h));
            }
        }
        
        prevMouse = curMouse;
    }
    else {
        // gravity control mode
        //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
        gravityLine = ofVec2f(curMouse.x - getCanvasWidth()/2,
                              curMouse.y - getCanvasHeight()/2);
        ofVec2f gravityForce = gravityLine / 10;
        box2d.setGravity(gravityForce.x, gravityForce.y);
    }
}

void CloudsVisualSystemExampleBox2D::selfMouseMoved(ofMouseEventArgs& data){
    
    ofVec2f curMouse(data.x, data.y);
    
    if (!bGravityMod) {
        ofVec2f speed = curMouse - prevMouse;
        
        if (speed.length() > triggerForce) {
            if (bCircles && (speed.x > 0 || !bRects)) {
                // create circles
                float r = getGaussian()*circleSizeDev + circleSizeMean;
                addCircle(curMouse, speed, r);
            }
            else if (bRects && (speed.x < 0 || !bCircles)) {
                // create rects
                float w = getGaussian()*rectSizeDev + rectSizeMean;
                float h = getGaussian()*rectSizeDev + rectSizeMean;
                addRect(curMouse, speed, ofVec2f(w, h));
            }
        }
        
        prevMouse = curMouse;
    }
    else {
        // gravity control mode
        //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
        gravityLine = ofVec2f(curMouse.x - getCanvasWidth()/2,
                              curMouse.y - getCanvasHeight()/2);
        ofVec2f gravityForce = gravityLine / 10;
        box2d.setGravity(gravityForce.x, gravityForce.y);
    }
}

void CloudsVisualSystemExampleBox2D::selfMousePressed(ofMouseEventArgs& data){
}

void CloudsVisualSystemExampleBox2D::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleBox2D::createRandomObjects()
{
    if (!bCircles && !bRects) {
        // nothing to add
        return;
    }
    
    // add objects if needed
    int objNum = circles.size() + rects.size();

    while (objNum < minObjectCount) {
        if (bCircles) {
            addRandomCircle();
            objNum++;
        }
    
        if (bRects) {
            addRandomRect();
            objNum++;
        }
    }
}

void CloudsVisualSystemExampleBox2D::addRandomCircle()
{
    float r = getGaussian()/2*circleSizeDev + circleSizeMean;
    //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
    addCircle(ofVec2f(getCanvasWidth()/2, getCanvasHeight()/2), ofVec2f(0, 0), r);
}

void CloudsVisualSystemExampleBox2D::addRandomRect()
{
    float w = getGaussian()*rectSizeDev + rectSizeMean;
    float h = getGaussian()*rectSizeDev + rectSizeMean;
    //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
    addRect(ofVec2f(getCanvasWidth()/2, getCanvasHeight()/2), ofVec2f(0, 0), ofVec2f(w, h));
}

void CloudsVisualSystemExampleBox2D::addCircle(ofVec2f pos, ofVec2f vel, float rad)
{
    ofxBox2dCircle circle;
    circle.setPhysics(3, 0.53, 0.1);
    circle.setup(box2d.getWorld(), pos.x, pos.y, rad);
    circle.setVelocity(vel);
    circle.setRotation(ofRandom(180));
    circle.body->SetUserData(&id0);
//    circle.setData((void*)1234);
    circles.push_back(circle);
}


void CloudsVisualSystemExampleBox2D::addRect(ofVec2f pos, ofVec2f vel, ofVec2f size)
{
    ofxBox2dRect rect;
    rect.setPhysics(3, 0.53, 0.1);
    rect.setup(box2d.getWorld(), pos.x, pos.y, size.x, size.y);
    rect.setVelocity(vel);
    rect.setRotation(ofRandom(180));
    rect.body->SetUserData(&id1);
    rects.push_back(rect);
}


void CloudsVisualSystemExampleBox2D::addStaticPlatforms()
{
    if (platforms.size() > 0) {
        return;
    }
    
    ofxBox2dPolygon polyLine;
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    polyLine.addVertex(getCanvasWidth()*0.2, getCanvasHeight()*0.8);
    polyLine.addVertex(getCanvasWidth()*0.3, getCanvasHeight()*0.9);
    polyLine.addVertex(getCanvasWidth()*0.5, getCanvasHeight()*0.86);
    polyLine.setPhysics(0.0, 0.5, 0.5);
    polyLine.create(box2d.getWorld());
    platforms.push_back(polyLine);
    
    polyLine = ofxBox2dPolygon();
    polyLine.addVertex(getCanvasWidth()*0.8, getCanvasHeight()*0.5);
    polyLine.addVertex(getCanvasWidth()*0.7, getCanvasHeight()*0.6);
    polyLine.addVertex(getCanvasWidth()*0.5, getCanvasHeight()*0.7);
    polyLine.setPhysics(0.0, 0.5, 0.5);
    polyLine.create(box2d.getWorld());
    platforms.push_back(polyLine);
}

void CloudsVisualSystemExampleBox2D::removeStaticPlatforms()
{
    for (int i=0; i<platforms.size(); i++) {
        platforms[i].destroy();
    }
    platforms.clear();
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
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    polygon.setPosition(ofRandom(-100, getCanvasWidth()*0.7),
                        ofRandom(getCanvasHeight()/2, getCanvasHeight()*0.8));
    randomPlatforms.push_back(polygon);
}

void CloudsVisualSystemExampleBox2D::removeRandomPlatform()
{
    for (int i=0; i<randomPlatforms.size(); i++) {
        randomPlatforms[i].destroy();
    }
    
    randomPlatforms.clear();
}

void CloudsVisualSystemExampleBox2D::handleObjectLimit()
{
    int delCircleNum = circles.size() - (int)maxCircles;
    for (int i=0; i<delCircleNum; i++)
    {
        ofxBox2dCircle c = circles.front();
        c.destroy();
        circles.erase(circles.begin());
    }
    
    int delRectNum = rects.size() - (int)maxRects;
    for (int i=0; i<delRectNum; i++)
    {
        ofxBox2dRect r = rects.front();
        r.destroy();
        rects.erase(rects.begin());
    }
}

void CloudsVisualSystemExampleBox2D::reinitBounds()
{
    if (box2d.ground) {
        box2d.world->DestroyBody(box2d.ground);
        box2d.ground = NULL;
    }

    box2d.createBounds();
}


void CloudsVisualSystemExampleBox2D::contactStart(ofxBox2dContactArgs &e)
{
    
    if(!soundsLoaded){
        return;
    }
    
    float aVel = e.a->GetBody()->GetLinearVelocity().Length();
    float bVel = e.b->GetBody()->GetLinearVelocity().Length();
	if(e.a->GetBody()->GetUserData() == NULL){
		return;
	}

	int type = *(static_cast<int*>(e.a->GetBody()->GetUserData()));
    int sIndex = type;
    
    float maxVel = max(aVel, bVel);
    if (maxVel > 6)
    {
        int index = sIndex*5 + (int)ofRandom(5);
        float vol = maxVel / 200;
        vol = (vol<1) ? pow(vol, 2) : 1;

        if (ofRandom(maxRects) < (float)rects.size()*0.8) {
            return;
        }

        samplePlayer[index].setVolume(vol * masterVolume);
        samplePlayer[index].play();
    }
}

float CloudsVisualSystemExampleBox2D::getGaussian() {
    
    float x1 = ofRandomuf();
    
    float x2 = ofRandomuf();
    
    return (sqrt (-2.0 * log(x1)) * cos(2.0 * PI * x2)) / 2;
}
