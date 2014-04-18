//
//  CloudsVisualSystemRulez.cpp
//

#include "CloudsVisualSystemRulez.h"
#include "CloudsRGBDVideoPlayer.h"
#include "ofxObjLoader.h"

CloudsVisualSystemRulez::CloudsVisualSystemRulez() :
    structure(NULL), maxBranches(4)
{
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemRulez::selfSetup()
{
    ofDirectory dir;
    dir.listDir(getVisualSystemDataPath() + "rules");
    dir.sort();
    for (unsigned i = 0; i < dir.size(); ++i)
    {
        structures[dir.getName(i)].load(dir.getPath(i));
    }
    generate();
    if (!structures.empty()) structure = &structures.begin()->second;
    
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
	post.init(getCanvasWidth(), getCanvasHeight(), true);
    post.createPass<EdgePass>();
    post.createPass<FxaaPass>();
    post.createPass<BloomPass>();
    
    shader.load(getVisualSystemDataPath() + "shaders/rulez");
    
    /*
	videoLoaded = false;
	
	if(ofFile::doesFileExist(getVisualSystemDataPath() + "TestVideo/Jer_TestVideo.mov")){
		getRGBDVideoPlayer().setup(getVisualSystemDataPath() + "TestVideo/Jer_TestVideo.mov",
								   getVisualSystemDataPath() + "TestVideo/Jer_TestVideo.xml" );
		
		getRGBDVideoPlayer().swapAndPlay();
		
		for(int i = 0; i < 640; i += 2){
			for(int j = 0; j < 480; j+=2){
				simplePointcloud.addVertex(ofVec3f(i,j,0));
			}
		}
		
		pointcloudShader.load(getVisualSystemDataPath() + "shaders/rgbdcombined");
		videoLoaded = true;
	}
	*/
	
    //	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}
void CloudsVisualSystemRulez::selfSetDefaults(){
    primaryCursorMode = CURSOR_MODE_INACTIVE;
    secondaryCursorMode = CURSOR_MODE_INACTIVE;
}
void CloudsVisualSystemRulez::generate()
{
    for (auto it = structures.begin(); it != structures.end(); ++it)
    {
        it->second.clear();
        //it->second.setMaxBranches(maxBranches);
        it->second.start();
        while (it->second.step()) ;
    }
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemRulez::selfBegin(){
}

//normal update call
void CloudsVisualSystemRulez::selfUpdate()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
    //rules.step();
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    if (post.getWidth() != getCanvasWidth() || post.getHeight() != getCanvasHeight()) post.init(getCanvasWidth(), getCanvasHeight(), true);
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemRulez::selfDraw()
{
    if (structure)
    {
        shader.begin();
    #warning take light pos from clouds
        shader.setUniform3f("lightPos", 1000, 1000, 1000);
        //MA: replaced ofGetMouseX() with GetCloudsInputX()
        //MA: changed ofGetWidth() to getCanvasWidth()
        shader.setUniform1f("growth", structure->getMaxDepth() * (getCanvasWidth() - GetCloudsInputX()) / (float)getCanvasWidth());
        shader.setUniform1f("maxDepth", structure->getMaxDepth());
        structure->draw();
        shader.end();
    }
    /*
	if(videoLoaded){
		ofPushMatrix();
		setupRGBDTransforms();
		pointcloudShader.begin();
		getRGBDVideoPlayer().setupProjectionUniforms(pointcloudShader);
		simplePointcloud.drawVertices();
		pointcloudShader.end();
		ofPopMatrix();
	}*/
}

void CloudsVisualSystemRulez::selfPostDraw()
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    post.process(CloudsVisualSystem::getSharedRenderTarget(), false);
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    if (post.getNumProcessedPasses()) post.getProcessedTextureReference().draw(0, getCanvasHeight(), getCanvasWidth(), -getCanvasHeight());
    else CloudsVisualSystem::getSharedRenderTarget().draw(0, getCanvasHeight(), getCanvasWidth(), -getCanvasHeight());
    glPopAttrib();
}

//use render gui for display settings, like changing colors
void CloudsVisualSystemRulez::selfSetupRenderGui()
{
    rdrGui->addToggle("regenerate", false);
    rdrGui->addToggle("save mesh", false);
    rdrGui->addLabel("Rule sets");
    vector<string> names;
    for (auto it = structures.begin(); it != structures.end(); ++it)
    {
        names.push_back(it->first);
    }
    radio = rdrGui->addRadio("rules", names);
    rdrGui->addLabel("Post");
    for (unsigned i = 0; i < post.size(); ++i)
    {
        rdrGui->addToggle(post[i]->getName(), &post[i]->getEnabledRef());
    }
}

void CloudsVisualSystemRulez::saveMesh()
{
    ofFileDialogResult result = ofSystemSaveDialog("mesh", "Export mesh to ply file");
    if (structure)
    {
        ostringstream oss;
        oss << result.getPath();
        if (result.getPath().find(".ply") == string::npos) oss << ".ply";
        structure->getMeshRef().save(oss.str());
    }
    //ofxObjLoader saver;
    //saver.save(result.getPath(), structure->getMeshRef());
}

void CloudsVisualSystemRulez::guiRenderEvent(ofxUIEventArgs &e)
{
    if (e.widget->getParent() && e.widget->getParent()->getName() == "rules")
    {
        ofxUIRadio* radio = static_cast<ofxUIRadio*>(e.widget->getParent());
        for (unsigned i = 0; i < radio->getToggles().size(); ++i)
        {
            if (radio->getToggles()[i] && radio->getToggles()[i]->getValue())
            {
                structure = &structures[radio->getToggles()[i]->getName()];
                break;
            }
        }
    }
    else if (e.widget->getName() == "regenerate")
    {
        ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
        if (toggle->getValue())
        {
            generate();
            toggle->setValue(false);
        }
    }
    else if (e.widget->getName() == "save mesh")
    {
        ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
        if (toggle->getValue())
        {
            saveMesh();
            toggle->setValue(false);
        }
    }
}

void CloudsVisualSystemRulez::selfGuiEvent(ofxUIEventArgs &e)
{
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemRulez::selfPresetLoaded(string presetPath)
{
}


//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemRulez::selfSetupSystemGui(){
	
}

void CloudsVisualSystemRulez::guiSystemEvent(ofxUIEventArgs &e){
	
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemRulez::selfSetupGui() {
    
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemRulez::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
    
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemRulez::selfSceneTransformation(){
	
}

// draw any debug stuff here
void CloudsVisualSystemRulez::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemRulez::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemRulez::selfEnd(){
	
	simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemRulez::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemRulez::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemRulez::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemRulez::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemRulez::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemRulez::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemRulez::selfMouseReleased(ofMouseEventArgs& data){
	
}