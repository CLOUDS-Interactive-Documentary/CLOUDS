//
//  CloudsVisualSystemFlying.cpp
//

#include "CloudsVisualSystemFlying.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsInput.h"

//const string CloudsVisualSystemFlying::RULES_FILES[] = { "flower1.xml", "tree1.xml", "tree2.xml", "tree3.xml" };
//const string CloudsVisualSystemFlying::RULES_FILES[] = { "rules/tree_flying2.xml" };
const float CloudsVisualSystemFlying::CAM_DAMPING = .08f;

using namespace Tonic;

CloudsVisualSystemFlying::CloudsVisualSystemFlying() :
    numPlantMeshes(100), floorW(2000), floorD(2000), floorHalfW(.5f * floorW), floorHalfD(.5f * floorD),
    noiseAmp(20.f), noiseFreq(5.f), xResolution(100), zResolution(100), xStep(floorW / (float)xResolution), zStep(floorD / (float)zResolution),
    cameraControl(true), fogStart(200.f), fogEnd(500.f), growDist(300.f), drawPlantPosns(false), numNearbyPlants(200),
    camSpeed(-500.f), currentLookAngle(-20.f, 0.f), camAvoidDist(500.f), regenerate(false), hsv0(48.f / 360.f, 1.f, 1.f), hsv1(349.f / 360.f, 0.598f, 0.984f)
{
    ofDirectory dir;
    dir.listDir(getVisualSystemDataPath() + "rules");
    for (unsigned i = 0; i < dir.size(); ++i)
    {
        rulesFileNames.push_back(dir.getName(i));
        rules.push_back(ofxRules());
        rules.back().load(dir.getPath(i));
        rulesWeights.push_back(1.f);
    }

	soundFiles.push_back("SriLankaForest.aif");
	soundFiles.push_back("FOREST.aif");
	soundFiles.push_back("organ_slower.aif");

	for(int i = 0; i < soundFiles.size(); i++){ 
		playSample[i]	= 0;
		sampleVolume[i] = 0;
	}

}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemFlying::selfSetup()
{
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    post.init(getCanvasWidth(), getCanvasHeight(), true);
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
    
    // sound
    synth.setOutputGen(buildSynth());
}

void CloudsVisualSystemFlying::generate()
{
    plantMeshes.clear();
    plants.clear();
    floor.clear();
    
    float totalWeight = 0.f;
    for (unsigned i = 0; i < rulesWeights.size(); ++i)
    {
        totalWeight += rulesWeights[i];
    }
    
    vector<float> normalisedWeights;
    for (int i = 0; i < rulesWeights.size(); ++i)
    {
        if (i == 0) normalisedWeights.push_back(rulesWeights[i] / totalWeight);
        else normalisedWeights.push_back(normalisedWeights[i - 1] + rulesWeights[i] / totalWeight);
    }
    
    // plants
    for (unsigned i = 0; i < numPlantMeshes; ++i)
    {
        vector<float>::const_iterator it = lower_bound(normalisedWeights.begin(), normalisedWeights.end(), ofRandomuf());
        unsigned idx = it - normalisedWeights.begin();
        plantMeshes.push_back(rules[idx]);
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
    
    ofAddListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemFlying::audioRequested);

    for (int i=0; i<3; i++)
    {
        if (playSample[i]) {
            soundTriggers[i].trigger();
        }
    }
}

//normal update call
void CloudsVisualSystemFlying::selfUpdate()
{
    
    volumeControl.value(gain);
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    if (post.getWidth() != getCanvasWidth() || post.getHeight() != getCanvasHeight()) post.init(getCanvasWidth(), getCanvasHeight(), true);
 
    if (regenerate)
    {
        generate();
        regenerate = false;
    }
    
    if (!bUseOculusRift && cameraControl)
    {
        ofVec2f targetLookAngle;
        //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
        targetLookAngle.x = ofMap(GetCloudsInputY(), 0, getCanvasHeight(), -20.f, -30.f, true);
        targetLookAngle.y = ofMap(GetCloudsInputX(), 0, getCanvasWidth(), 20.f, -20.f, true);
        currentLookAngle.interpolate(targetLookAngle, .05);
        ofQuaternion rx, ry;
        rx.makeRotate(currentLookAngle.x, 1, 0, 0);
        ry.makeRotate(currentLookAngle.y, 0, 1, 0);
        getCameraRef().setOrientation(rx * ry);
        getCameraRef().move(0, 0, camSpeed * ofGetLastFrameTime());
        
        /*xRot += CAM_DAMPING * (ofMap(abs(GetCloudsInputY() - ofGetHeight() * .5f), 0, ofGetHeight() * 0.5, 30.f, 20.f) - xRot);
        yRot += CAM_DAMPING * (ofMap(GetCloudsInputX(), 0.f, ofGetWidth(), 20, -20) - yRot);
        zSpeed += CAM_DAMPING * (ofMap(GetCloudsInputY(), 0, ofGetHeight(), -600.f, 600.f) - zSpeed);
        getCameraRef().move(0, 0, zSpeed * ofGetLastFrameTime());
        getCameraRef().setOrientation(ofVec3f(-xRot, yRot, 0.f));*/
    }
    float distToFloor = getCameraRef().getPosition().y / cos(DEG_TO_RAD * (90 + getCameraRef().getRoll()));
    floorLookAt = getCameraRef().getPosition() + getCameraRef().getLookAtDir().normalized() * distToFloor;
    
    const float maxPlantDist = growDist * 5.f;
    const float maxPlantDistSq = maxPlantDist * maxPlantDist;
    
    for (auto& it = plants.begin(); it != plants.end();)
    {
        if ((it->pos - floorLookAt).lengthSquared() > maxPlantDistSq) {
			it = plants.erase(it);
		}
		else {
			++it;
		}
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
    
    //ofMatrix4x4 modelview;
    //glGetFloatv(GL_MODELVIEW_MATRIX, modelview.getPtr());
    
    // eye space light pos
    ofVec3f lEye = ofVec3f(1000, 1000, 1000);// * modelview;
    
    float growDistSq = growDist * growDist;
    
    // PLANTS
    plantsShader.begin();
    const float camAvoidDistSq = camAvoidDist * camAvoidDist;
    for (auto it = plants.begin(); it != plants.end(); ++it)
    {
        float growth = ofMap((it->pos - floorLookAt).lengthSquared(), 0.f, growDistSq, 1.4f * plantMeshes[it->meshIdx].getCurrentDepth(), 0.f, true);
        float distToCamSq = (it->pos - cam.getPosition()).lengthSquared();
        if (distToCamSq < camAvoidDistSq)
        {
            growth *= sqrt(distToCamSq) / camAvoidDist;
        }
        if (growth > 0.f)
        {
            plantsShader.setUniform1f("growth", growth);
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
    floorShader.begin();
    floorShader.setUniform3fv("hsv0", hsv0.getPtr());
    floorShader.setUniform3fv("hsv1", hsv1.getPtr());
    floorShader.setUniform3fv("lEye", lEye.getPtr());
    floorShader.setUniform1f("noiseFreq", noiseFreq);
    floorShader.setUniform1f("noiseAmp", noiseAmp);
    floorShader.setUniform1f("fogStart", fogStart);
    floorShader.setUniform1f("fogEnd", fogEnd);
    floorShader.setUniform3f("offset", cam.getPosition().x - fmod(cam.getPosition().x, xStep), 0.f, cam.getPosition().z - fmod(cam.getPosition().z, zStep));
    floorShader.setUniformTexture("tex", floorTex, 1);
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
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    if (post.getNumProcessedPasses()) post.getProcessedTextureReference().draw(0, getCanvasHeight(), getCanvasWidth(), -getCanvasHeight());
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    else CloudsVisualSystem::getSharedRenderTarget().draw(0, getCanvasHeight(), getCanvasWidth(), -getCanvasHeight());
    glPopAttrib();
}

//use render gui for display settings, like changing colors
void CloudsVisualSystemFlying::selfSetupRenderGui()
{
    rdrGui->addToggle("regenerate", &regenerate);
    rdrGui->addToggle("cameraControl", &cameraControl);
    rdrGui->addSlider("fogStart", 100.f, 4000.f, &fogStart);
    rdrGui->addSlider("fogEnd", 100.f, 4000.f, &fogEnd);
    rdrGui->addSlider("camSpeed", 0.f, -1000.f, &camSpeed);
    rdrGui->addLabel("Plants");
    rdrGui->addSlider("numNearbyPlants", 20, 500, &numNearbyPlants);
    for (unsigned i = 0; i < rulesFileNames.size(); ++i)
    {
        rdrGui->addSlider(rulesFileNames[i] + " weighting", 0.f, 1.f, &rulesWeights[i]);
    }
    rdrGui->addSlider("growDist", 100.f, 1000.f, &growDist);
    rdrGui->addSlider("camAvoidDist", 0.f, 1000.f, &camAvoidDist);
    rdrGui->addToggle("drawPlantPosns", &drawPlantPosns);
    rdrGui->addLabel("Floor");
    rdrGui->addRangeSlider("hueRange", 0.f, 1.f, &hsv0.x, &hsv1.x);
    rdrGui->addRangeSlider("satRange", 0.f, 1.f, &hsv0.y, &hsv1.y);
    rdrGui->addSlider("noiseFreq", 0.001, 0.01, &noiseFreq);
    rdrGui->addSlider("noiseAmp", 0.f, 150.f, &noiseAmp);
    rdrGui->addLabel("Post");
    for (unsigned i = 0; i < post.size(); ++i)
    {
        rdrGui->addToggle(post[i]->getName(), &post[i]->getEnabledRef());
    }
    
    rdrGui->addSpacer();
    rdrGui->addToggle(soundFiles[0], &playSample[0]);
    rdrGui->addToggle(soundFiles[1], &playSample[1]);
    rdrGui->addToggle(soundFiles[2], &playSample[2]);
    rdrGui->addSlider("Gain", 0, 1, &gain);
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemFlying::selfKeyPressed(ofKeyEventArgs & args)
{
    switch (args.key)
    {
        case 'R':
            regenerate = true;
            break;
        
        default:
            break;
    }
}

void CloudsVisualSystemFlying::guiRenderEvent(ofxUIEventArgs &e)
{
    for (int i=0; i<3; i++)
    {
        if (e.widget->getName() == soundFiles[i]) {
            ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
            playSample[i] = toggle->getValue();
            if (toggle->getValue() == true) {
                soundTriggers[i].trigger();
            }
        }
    }
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemFlying::selfPresetLoaded(string presetPath)
{
    generate();
}

void CloudsVisualSystemFlying::selfGuiEvent(ofxUIEventArgs &e){
    
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemFlying::selfSetupSystemGui(){
	
}

void CloudsVisualSystemFlying::guiSystemEvent(ofxUIEventArgs &e){
	
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemFlying::selfSetupGui(){
    
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
    volumeControl.value(0);
    ofRemoveListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemFlying::audioRequested);
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemFlying::selfExit(){
	
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


Generator CloudsVisualSystemFlying::buildSynth()
{
    string strDir = GetCloudsDataPath()+"sound/textures/";
    ofDirectory sdir(strDir);
    
    SampleTable samples[3];
    
    int nSounds = sizeof(soundFiles) / sizeof(string);
    for (int i=0; i<nSounds; i++)
    {
        string strAbsPath = sdir.getAbsolutePath() + "/" + soundFiles[i];
        samples[i] = loadAudioFile(strAbsPath);
    }
    
    Generator sampleGen1 = BufferPlayer().setBuffer(samples[0]).loop(1).trigger(soundTriggers[0]);
    Generator sampleGen2 = BufferPlayer().setBuffer(samples[1]).trigger(soundTriggers[1]).loop(1);
    Generator sampleGen3 = BufferPlayer().setBuffer(samples[2]).trigger(soundTriggers[2]).loop(1);
    
    return (sampleGen1 * 1.0f + sampleGen2 * 0.35f + sampleGen3 * 0.6f) * volumeControl;
}

void CloudsVisualSystemFlying::audioRequested(ofAudioEventArgs& args)
{
    synth.fillBufferOfFloats(args.buffer, args.bufferSize, args.nChannels);
}


