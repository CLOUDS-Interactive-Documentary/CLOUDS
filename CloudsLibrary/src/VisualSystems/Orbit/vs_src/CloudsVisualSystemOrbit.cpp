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
    drawNormals = false;
    drawInflections = false;
    drawAcc = false;
    drawLine = false;
    drawMesh = true;
    //multiplePaths = false;
    additiveBlending = false;
    motion = WAVY;
    lastPathTime = 0;
    fogStart = 200.f;
    fogEnd = 500.f;
    speed = 1.f;
    
    itgPath::maxLineLength = 10;
    itgPath::maxMeshLength = 10;
    itgPath::lineWidth = 1.f;
    
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    post.init(getCanvasWidth(), getCanvasHeight(), true);
    post.createPass<FxaaPass>();
    post.createPass<BloomPass>();
    
    path.init(meshRadius, getVisualSystemDataPath());
    
    for (unsigned i = 0; i < post.size(); ++i)
    {
        post[i]->setEnabled(false);
    }
    
    shader.load(getVisualSystemDataPath() + "shaders/orbit");
}

//normal update call
void CloudsVisualSystemOrbit::selfUpdate()
{
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    if (post.getWidth() != getCanvasWidth() || post.getHeight() != getCanvasHeight()) post.init(getCanvasWidth(), getCanvasHeight(), true);

    switch (motion)
    {
        case WAVY:
        {
            float t = speed * ofGetElapsedTimef();
            ofVec3f v(100.f * sin(t + ofNoise(t)),
                      60.f * ofSignedNoise(20.f + t),
                      60.f * ofSignedNoise(t) + 100.f * cos(t)
                      );
            path.addVertex(v);
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
                ofVec3f prev = path.back().getPos();
                ofVec3f v = prev + h * ofVec3f(
                                            a * (prev.y - prev.x),
                                            (prev.x * (b - prev.z) - prev.y),
                                            (prev.x * prev.y - c * prev.z));
                path.addVertex(v);
            }
            break;
        }
            
        default:
            break;
    }
    
    if (lockCam)
    {
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
    
    ofSetColor(255);
    shader.begin();
    shader.setUniform1f("fogStart", fogStart);
    shader.setUniform1f("fogEnd", fogEnd);
    shader.setUniform1f("litAmount", litAmount);
    //if (drawNormals) path.drawNormals(10);
    if (drawInflections) path.drawInflections();
    if (drawAcc) path.drawAcc();
    shader.end();
    
    if (drawMesh) path.drawMesh(fogStart, fogEnd, litAmount);
    if (drawLine) path.drawLine(fogStart, fogEnd);
    
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
    
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    if (post.getNumProcessedPasses()) post.getProcessedTextureReference().draw(0, getCanvasHeight(),
                                             getCanvasWidth(),
                                             -getCanvasHeight());
    else CloudsVisualSystem::getSharedRenderTarget().draw(0, getCanvasHeight(),
                                                          getCanvasWidth(),
                                                          -getCanvasHeight());
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
    rdrGui->addIntSlider("maxLineLength", 10, 5000, &itgPath::maxLineLength);
    rdrGui->addSlider("lineFadeLength", 0, 1, &itgPath::lineFadeLength);
    rdrGui->addSlider("lineWidth", .1f, 5.f, &itgPath::lineWidth);
    rdrGui->addRangeSlider("hueRange", 0.f, 1.f, &itgPath::hueMin, &itgPath::hueMax);
    
    rdrGui->addLabel("Mesh");
    rdrGui->addToggle("drawMesh", &drawMesh);
    rdrGui->addIntSlider("maxMeshLength", 10, 5000, &itgPath::maxMeshLength);
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
    rdrGui->addSlider("speed", 0.f, 100.f, &speed);
    
}

void CloudsVisualSystemOrbit::guiRenderEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "meshRadius")
    {
        path.setMeshRadius(meshRadius);
    }
    else if (e.widget->getName() == "lorenz" && ((ofxUIToggle*)e.widget)->getValue())
    {
        path.clear();
        path.addVertex(ofVec3f(0.1, 0, 0));
        motion = LORENZ;
    }
    else if (e.widget->getName() == "wavy" && ((ofxUIToggle*)e.widget)->getValue())
    {
        lastPathTime = ofGetElapsedTimeMillis();
        path.clear();
        motion = WAVY;
    }
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOrbit::selfSetupGui(){

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