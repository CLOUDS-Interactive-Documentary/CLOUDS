//
//  CloudsVisualSystemFlying.cpp
//

#include "CloudsVisualSystemFlying.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

const float CloudsVisualSystemFlying::CAM_X_ROT = 20;
const string CloudsVisualSystemFlying::RULES_FILES[] = { "rules/tree_flying.xml", "rules/flower.xml" };


CloudsVisualSystemFlying::CloudsVisualSystemFlying() :
    numPlantMeshes(20), floorW(2000), floorD(2000), floorHalfW(.5f * floorW), floorHalfD(.5f * floorD),
    noiseAmp(20.f), noiseFreq(5.f), xResolution(100), zResolution(100), xStep(floorW / (float)xResolution), zStep(floorD / (float)zResolution),
    cameraControl(true), fogStart(200.f), fogEnd(500.f), growDist(300.f), drawPlantPosns(false), numNearbyPlants(200)
{
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemFlying::selfSetup()
{
    post.init(ofGetWidth(), ofGetHeight(), true);
    //post.createPass<EdgePass>();
    post.createPass<FxaaPass>();
    post.createPass<BloomPass>();
    
    // plants
    plantsShader.load(getVisualSystemDataPath() + "shaders/plants");
    
    // floor
    floorShader.load(getVisualSystemDataPath() + "shaders/floor");
    ofDisableArbTex();
    floorTex.setCompression(OF_COMPRESS_ARB);
    floorTex.loadImage(getVisualSystemDataPath() + "images/wf.png");
    floorTex.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    floorTex.unbind();
    ofEnableArbTex();
    
    // meshes
    generate();
}

void CloudsVisualSystemFlying::generate()
{
    plantMeshes.clear();
    plants.clear();
    floor.clear();
    
    // plants
    for (unsigned i = 0; i < numPlantMeshes; ++i)
    {
        plantMeshes.push_back(ofxRules());
        plantMeshes.back().load(getVisualSystemDataPath() + RULES_FILES[rand() % NUM_RULES_FILES]);
        plantMeshes.back().start();
        while (plantMeshes.back().step()) ;
    }
    
    // floor - don't share vertices so can have
    // face info them
    floor.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for (unsigned z = 0; z < zResolution; ++z)
    {
        for (unsigned x = 0; x < xResolution; ++x)
        {
            float u0 = (float)x / (float)xResolution;
            float u1 = (float)(x + 1) / (float)xResolution;
            float v0 = (float)z / (float)zResolution;
            float v1 = (float)(z + 1) / (float)zResolution;
            
            ofVec2f averagePos;
            floor.addVertex(ofVec3f(-floorHalfW + u0 * floorW, 0.f, -floorD + v0 * floorD));
            floor.addTexCoord(ofVec2f(u0, v0));
            averagePos += ofVec2f(floor.getVertices().back().x, floor.getVertices().back().z);
            
            floor.addVertex(ofVec3f(-floorHalfW + u1 * floorW, 0.f, -floorD + v1 * floorD));
            floor.addTexCoord(ofVec2f(u1, v1));
            averagePos += ofVec2f(floor.getVertices().back().x, floor.getVertices().back().z);
            
            floor.addVertex(ofVec3f(-floorHalfW + u1 * floorW, 0.f, -floorD + v0 * floorD));
            floor.addTexCoord(ofVec2f(u1, v0));
            averagePos += ofVec2f(floor.getVertices().back().x, floor.getVertices().back().z);
            
            // stick the average posns coords into the normals
            // would be nice if OF supported attributes or multitex coords
            averagePos *= 1.f / 3.f;
            for (unsigned i = 0; i < 3; ++i)
            {
                floor.addNormal(ofVec3f(averagePos));
            }
            
            averagePos.set(0.f);
            floor.addVertex(ofVec3f(-floorHalfW + u0 * floorW, 0.f, -floorD + v0 * floorD));
            floor.addTexCoord(ofVec2f(u0, v0));
            averagePos += ofVec2f(floor.getVertices().back().x, floor.getVertices().back().z);
            
            floor.addVertex(ofVec3f(-floorHalfW + u0 * floorW, 0.f, -floorD + v1 * floorD));
            floor.addTexCoord(ofVec2f(u0, v1));
            averagePos += ofVec2f(floor.getVertices().back().x, floor.getVertices().back().z);
            
            floor.addVertex(ofVec3f(-floorHalfW + u1 * floorW, 0.f, -floorD + v1 * floorD));
            floor.addTexCoord(ofVec2f(u1, v1));
            averagePos += ofVec2f(floor.getVertices().back().x, floor.getVertices().back().z);
            
            averagePos *= 1.f / 3.f;
            for (unsigned i = 0; i < 3; ++i)
            {
                floor.addNormal(ofVec3f(averagePos));
            }
        }
    }
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemFlying::selfBegin()
{
    getCameraRef().setPosition(0, 200, floorHalfD);
}

//normal update call
void CloudsVisualSystemFlying::selfUpdate()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
    if (cameraControl)
    {
        getCameraRef().move(0, 0, -ofMap(ofGetMouseY(), 0, ofGetHeight(), 600.f, -600.f) * ofGetLastFrameTime());
        getCameraRef().setOrientation(ofVec3f(-CAM_X_ROT, ofMap(ofGetMouseX(), 0.f, ofGetWidth(), 20, -20), 0.f));
    }
    float distToFloor = getCameraRef().getPosition().y / cos(DEG_TO_RAD * (90 + getCameraRef().getRoll()));
    floorLookAt = getCameraRef().getPosition() + getCameraRef().getLookAtDir().normalized() * distToFloor;
    
    const float maxPlantDist = growDist * 5.f;
    const float maxPlantDistSq = maxPlantDist * maxPlantDist;
    
    for (auto it = plants.begin(); it != plants.end(); ++it)
    {
        if ((it->pos - floorLookAt).lengthSquared() > maxPlantDistSq) it = plants.erase(it);
    }
    
    while (plants.size() < numNearbyPlants)
    {
        ofVec3f pos(0.f, 0.f, ofRandom(growDist, maxPlantDist));
        pos.rotate(ofRandom(0, 360), ofVec3f(0, 1, 0));
        Plant plant;
        plant.pos = pos + ofVec3f(floorLookAt.x, 0, floorLookAt.z);
        plant.meshIdx = rand() % plantMeshes.size();
        plants.push_back(plant);
    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemFlying::selfDraw()
{
    glPushAttrib(GL_ENABLE_BIT);
    // icosphere seems to be wound backwards :(
    // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    // DEBUG
    if (drawPlantPosns)
    {
        ofPushStyle();
        ofSetColor(255, 0, 0);
        for (auto it = plants.begin(); it != plants.end(); ++it)
        {
            ofRect(it->pos, 5, 5);
        }
        ofPopStyle();
    }
    
    ofMatrix4x4 modelview;
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview.getPtr());
    
    // eye space light pos
    ofVec3f lEye = ofVec3f(1000, 1000, 1000);// * modelview;
    
    float growDistSq = growDist * growDist;
    
    // PLANTS
    plantsShader.begin();
    for (auto it = plants.begin(); it != plants.end(); ++it)
    {
        float growth = ofMap((it->pos - floorLookAt).lengthSquared(), 0.f, growDistSq, 1.4f * plantMeshes[it->meshIdx].getNumSteps(), 0.f, true);
        if (growth > 0.f)
        {
            plantsShader.setUniform1f("growth", growth);
            plantsShader.setUniform1f("maxDepth", plantMeshes[it->meshIdx].getMaxDepth());
            plantsShader.setUniform3fv("lEye", lEye.getPtr());
            plantsShader.setUniform1f("noiseFreq", noiseFreq);
            plantsShader.setUniform1f("noiseAmp", noiseAmp);
            plantsShader.setUniform1f("fogStart", fogStart);
            plantsShader.setUniform1f("fogEnd", fogEnd);
            plantsShader.setUniform3fv("base", it->pos.getPtr());
            ofPushMatrix();
            ofTranslate(it->pos);
            plantMeshes[it->meshIdx].draw();
            ofPopMatrix();
        }
    }
    plantsShader.end();
    
    // FLOOR
    hsv0.set(48.f / 360.f, 1.f, 1.f);
    hsv1.set(349.f / 360.f, 0.598f, 0.984f);
    floorShader.begin();
    floorShader.setUniform3fv("hsv0", hsv0.getPtr());
    floorShader.setUniform3fv("hsv1", hsv1.getPtr());
    floorShader.setUniform3fv("lEye", lEye.getPtr());
    floorShader.setUniform1f("noiseFreq", noiseFreq);
    floorShader.setUniform1f("noiseAmp", noiseAmp);
    floorShader.setUniform1f("fogStart", fogStart);
    floorShader.setUniform1f("fogEnd", fogEnd);
    floorShader.setUniform3f("offset", cam.getPosition().x - fmod(cam.getPosition().x, xStep), 0.f, cam.getPosition().z - fmod(cam.getPosition().z, zStep));
    floorShader.setUniformTexture("tex", floorTex, 0);
    floorShader.setUniform2f("resolution", xResolution, zResolution);
    floorShader.setUniform3fv("floorLookAt", floorLookAt.getPtr());
    floorShader.setUniform1f("growDist", growDist);
    floor.draw();
    floorShader.end();
    
    glPopAttrib();
}

void CloudsVisualSystemFlying::selfPostDraw()
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    post.process(CloudsVisualSystem::getSharedRenderTarget(), false);
    if (post.getNumProcessedPasses()) post.getProcessedTextureReference().draw(0, ofGetHeight(), ofGetWidth(), -ofGetHeight());
    else CloudsVisualSystem::getSharedRenderTarget().draw(0, ofGetHeight(), ofGetWidth(), -ofGetHeight());
    glPopAttrib();
}

//use render gui for display settings, like changing colors
void CloudsVisualSystemFlying::selfSetupRenderGui()
{
    rdrGui->addToggle("regenerate", false);
    rdrGui->addSlider("growDist", 100.f, 1000.f, &growDist);
    rdrGui->addSlider("numNearbyPlants", 20, 500, &numNearbyPlants);
    rdrGui->addSlider("fogStart", 100.f, 4000.f, &fogStart);
    rdrGui->addSlider("fogEnd", 100.f, 4000.f, &fogEnd);
    rdrGui->addToggle("cameraControl", &cameraControl);
    rdrGui->addToggle("drawPlantPosns", &drawPlantPosns);
    rdrGui->addLabel("Floor");
    rdrGui->addSlider("noiseFreq", 0.001, 0.01, &noiseFreq);
    rdrGui->addSlider("noiseAmp", 0.f, 150.f, &noiseAmp);
    rdrGui->addLabel("Post");
    for (unsigned i = 0; i < post.size(); ++i)
    {
        rdrGui->addToggle(post[i]->getName(), &post[i]->getEnabledRef());
    }
}

void CloudsVisualSystemFlying::guiRenderEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "regenerate")
    {
        ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
        if (toggle->getValue())
        {
            generate();
            toggle->setValue(false);
        }
    }
}

void CloudsVisualSystemFlying::selfGuiEvent(ofxUIEventArgs &e)
{
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemFlying::selfSetupSystemGui(){
	
}

void CloudsVisualSystemFlying::guiSystemEvent(ofxUIEventArgs &e){
	
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemFlying::selfSetupGui() {
    
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemFlying::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
    
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemFlying::selfPresetLoaded(string presetPath){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemFlying::selfSceneTransformation(){
	
}

// draw any debug stuff here
void CloudsVisualSystemFlying::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemFlying::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemFlying::selfEnd(){
	
	simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemFlying::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemFlying::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemFlying::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemFlying::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemFlying::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemFlying::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemFlying::selfMouseReleased(ofMouseEventArgs& data){
	
}