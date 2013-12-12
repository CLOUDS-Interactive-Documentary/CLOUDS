//
//  CloudsVisualSystemOrbit.cpp
//

#include "CloudsVisualSystemOrbit.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOrbit::selfSetup()
{
    ofAddListener(ofEvents().windowResized, this, &CloudsVisualSystemOrbit::onWindowResized);
    
    drawNormals = false;
    drawInflections = false;
    drawAcc = false;
    drawLine = false;
    drawMesh = true;
    multiplePaths = false;
    additiveBlending = false;
    motion = WAVY;
    lastPathTime = 0;
    fogStart = 200.f;
    fogEnd = 500.f;
    speed = 1.f;
    
    /*
    drawEllipses = false;
    ellipseW = 80;
    ellipseH = 30;
    ellipseAlpha = 20;
    */
    
    Path::maxLineLength = 10;
    Path::maxMeshLength = 10;
    Path::lineWidth = 1.f;
    
    post.init(ofGetWidth(), ofGetHeight(), true);
    post.createPass<FxaaPass>();
    post.createPass<BloomPass>();
    
    for (unsigned i = 0; i < post.size(); ++i)
    {
        post[i]->setEnabled(false);
    }
    
    shader.load(getVisualSystemDataPath() + "shaders/orbit");
}

//normal update call
void CloudsVisualSystemOrbit::selfUpdate()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
    if (multiplePaths && (paths.empty() || ofGetElapsedTimeMillis() - lastPathTime > PATH_INTERVAL))
    {
        paths.push_back(itg::Path(meshRadius));
        lastPathTime = ofGetElapsedTimeMillis();
    }
    switch (motion)
    {
        case WAVY:
        {
            float t = speed * ofGetElapsedTimef();
            ofVec3f v(100.f * sin(t + ofNoise(t)),
                      60.f * ofSignedNoise(20.f + t),
                      60.f * ofSignedNoise(t) + 100.f * cos(t)
                      );
            paths.back().addVertex(v);
            break;
        }
            
        case LORENZ:
        {
            float h = 0.01;
            float a = 10.0;
            float b = 28.0;
            float c = 8.0 / 3.0;
            
            for (unsigned i = 0; i < speed; ++i)
            {
                ofVec3f prev = paths.back().back().getPos();
                ofVec3f v = prev + h * ofVec3f(
                                            a * (prev.y - prev.x),
                                            (prev.x * (b - prev.z) - prev.y),
                                            (prev.x * prev.y - c * prev.z));
                paths.back().addVertex(v);
            }
            break;
        }
            
        default:
            break;
    }
    
    if (lockCam && paths.size())
    {
        itg::Path& path = paths.back();
        ofxPtf& ptf = path.getPtfRef();
        const int cameraPositionLag = 8;
        lockedCam.setNearClip(0.1f);
        if (ptf.getFrames().size() > cameraPositionLag)
        {
            lockedCam.setTransformMatrix(ptf.getFrames()[ptf.getFrames().size() - cameraPositionLag]);
            lockedCam.lookAt(path.back().getPos());
        }
        else lockedCam.setTransformMatrix(cam.getGlobalTransformMatrix());
    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOrbit::selfDraw()
{
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    //ofPushStyle();
    if (additiveBlending) ofEnableBlendMode(OF_BLENDMODE_ADD);
    if (lockCam) lockedCam.begin();
    shader.begin();
    shader.setUniform1f("fogStart", fogStart);
    shader.setUniform1f("fogEnd", fogEnd);
    shader.setUniform3f("fogColour", 0, 0, 0);
    shader.setUniform1f("litAmount", litAmount);
    shader.setUniform3f("lEye", 1000, 1000, 1000);
    
    for (unsigned i = 0; i < paths.size(); ++i)
    {
        ofSetColor(255);
        if (drawMesh) paths[i].drawMesh();
        if (drawLine) paths[i].drawLine();
        if (drawNormals) paths[i].drawNormals(10);
        if (drawInflections) paths[i].drawInflections();
        if (drawAcc) paths[i].drawAcc();
    }
    
    shader.end();
    if (additiveBlending)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ofDisableBlendMode();
    }
    if (lockCam) lockedCam.end();
    //ofPopStyle();
    
    glPopAttrib();
}

void CloudsVisualSystemOrbit::selfPostDraw()
{
    post.process(CloudsVisualSystem::getSharedRenderTarget(), false);
    
    if (post.getNumProcessedPasses()) post.getProcessedTextureReference().draw(0, ofGetHeight(),
                                             ofGetWidth(),
                                             -ofGetHeight());
    else CloudsVisualSystem::getSharedRenderTarget().draw(0, ofGetHeight(),
                                                          ofGetWidth(),
                                                          -ofGetHeight());
}

//use render gui for display settings, like changing colors
void CloudsVisualSystemOrbit::selfSetupRenderGui()
{
    rdrGui->addLabel("Motion");
    vector<string> options;
    options.push_back("lorenz");
    options.push_back("wavy");
    rdrGui->addRadio("motion", options);
    
    rdrGui->addLabel("Post");
    for (unsigned i = 0; i < post.size(); ++i)
    {
        rdrGui->addToggle(post[i]->getName(), &post[i]->getEnabledRef());
    }
    
    rdrGui->addLabel("Line");
    rdrGui->addToggle("drawLine", &drawLine);
    rdrGui->addSlider("maxLineLength", 10, 5000, &Path::maxLineLength);
    rdrGui->addSlider("lineFadeLength", 0, 1, &Path::lineFadeLength);
    rdrGui->addSlider("lineWidth", .1f, 5.f, &Path::lineWidth);
    
    rdrGui->addLabel("Mesh");
    rdrGui->addToggle("drawMesh", &drawMesh);
    rdrGui->addSlider("maxMeshLength", 10, 5000, &Path::maxMeshLength);
    rdrGui->addSlider("meshRadius", 0.1f, 4.f, &meshRadius);
    
    //rdrGui->addLabel("Ellipses");
    //rdrGui->addToggle("drawEllipses", &drawEllipses);
    
    //rdrGui->addToggle("multiplePaths", &multiplePaths);
    rdrGui->addLabel("Misc");
    rdrGui->addToggle("additiveBlending", &additiveBlending);
    rdrGui->addToggle("drawNormals", &drawNormals);
    rdrGui->addToggle("drawInflections", &drawInflections);
    rdrGui->addToggle("drawAcc", &drawAcc);
    rdrGui->addToggle("lockCam", &lockCam);
    rdrGui->addSlider("lockedCamLag", 0, 50, &lockedCamLag);
    rdrGui->addSlider("fogStart", 0, 2000, &fogStart);
    rdrGui->addSlider("fogEnd", 0, 2000, &fogEnd);
    rdrGui->addSlider("litAmount", 0, 1, &litAmount);
    rdrGui->addSlider("speed", 1, 100, &speed);
    
}

void CloudsVisualSystemOrbit::guiRenderEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "meshRadius")
    {
        if (!paths.empty()) paths.back().setMeshRadius(meshRadius);
    }
    else if (e.widget->getName() == "lorenz" && ((ofxUIToggle*)e.widget)->getValue())
    {
        multiplePaths = false;
        paths.clear();
        paths.push_back(itg::Path(meshRadius));
        paths.back().addVertex(ofVec3f(0.1, 0, 0));
        motion = LORENZ;
    }
    else if (e.widget->getName() == "wavy" && ((ofxUIToggle*)e.widget)->getValue())
    {
        lastPathTime = ofGetElapsedTimeMillis();
        paths.clear();
        paths.push_back(itg::Path(meshRadius));
        motion = WAVY;
    }
}

void CloudsVisualSystemOrbit::onWindowResized(ofResizeEventArgs& args)
{
    post.init(args.width, args.height, true);
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOrbit::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOrbit::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemOrbit::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOrbit::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOrbit::guiSystemEvent(ofxUIEventArgs &e){
	
}





// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOrbit::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOrbit::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOrbit::selfSceneTransformation(){
	
}



// draw any debug stuff here
void CloudsVisualSystemOrbit::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOrbit::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOrbit::selfEnd(){
	
	simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOrbit::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOrbit::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOrbit::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOrbit::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOrbit::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOrbit::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOrbit::selfMouseReleased(ofMouseEventArgs& data){
	
}