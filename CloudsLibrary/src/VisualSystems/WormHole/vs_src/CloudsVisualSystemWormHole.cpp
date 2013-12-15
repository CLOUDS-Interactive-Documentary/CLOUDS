//
//  CloudsVisualSystemWormHole.cpp
//

#include "CloudsVisualSystemWormHole.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemWormHole::selfSetupGui(){
	
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
	
	meshGui = new ofxUISuperCanvas("Mesh", gui);
	meshGui->copyCanvasStyle(gui);
	meshGui->copyCanvasProperties(gui);
	meshGui->setName("Mesh");
	meshGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	meshGui->addRadio("meshes", meshNames );
	
	ofAddListener(meshGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(meshGui);
	guimap[meshGui->getName()] = meshGui;
	
	
	cameraGui = new ofxUISuperCanvas("CameraPath", gui);
	cameraGui->copyCanvasStyle(gui);
	cameraGui->copyCanvasProperties(gui);
	cameraGui->setName("CameraPath");
	cameraGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	cameraGui->addRadio("paths", cameraPathNames );
	
	ofAddListener(cameraGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(cameraGui);
	guimap[cameraGui->getName()] = cameraGui;
	
	
	shaderGui = new ofxUISuperCanvas("SHADERS", gui);
	shaderGui->copyCanvasStyle(gui);
	shaderGui->copyCanvasProperties(gui);
	shaderGui->setName("Shaders");
	shaderGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	vector<string> shaderNames;
	for (map<string, ofShader*>::iterator it=shaderMap.begin(); it!=shaderMap.end(); it++)
	{
		shaderNames.push_back(it->first);
	}
	shaderGui->addRadio("shaders", shaderNames);
	
	shaderGui->addImageSampler("c1", &colorSampleImage, 100, 100);
	
	ofAddListener(shaderGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(shaderGui);
	guimap[shaderGui->getName()] = shaderGui;
	
}

void CloudsVisualSystemWormHole::selfGuiEvent(ofxUIEventArgs &e){
	string name = e.getName();
	
	if(name == "c1")
	{
		//...
	}
	
	else if(e.getKind() == OFX_UI_WIDGET_TOGGLE && e.getToggle() )
	{
		string parentName = e.getToggle()->getParent()->getName();
		if( parentName == "cameraPaths" )
		{
			
		}
		
		else if(parentName == "shaders")
		{
			
		}
		
		else if(parentName == "meshes")
		{
			for(int i=0; i<meshNames.size(); i++)
			{
				if(name == meshNames[i])
				{
					loadMesh( meshNames[i] );
				}
			}
		}
	}
}

void CloudsVisualSystemWormHole::loadMesh(string name)
{
	if(name != currentMeshName)
	{
		currentMeshName = name;
		
		mesh.clear();
		
		ofxObjLoader::load( modelPath + name, mesh );
		
		cout << "loading " << modelPath + name << endl;
	}
	
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemWormHole::selfSetupSystemGui(){
	
}

void CloudsVisualSystemWormHole::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemWormHole::selfSetupRenderGui(){

}

void CloudsVisualSystemWormHole::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemWormHole::selfSetup(){
	
	//meshes
	modelPath = getVisualSystemDataPath() + "models/";
	string cameraPathPath = getVisualSystemDataPath() + "cameraPaths/";
	
	cout << modelPath << endl;
	cout << cameraPathPath << endl;
	

	ofDirectory dir;
	dir.allowExt("obj");
	dir.listDir( modelPath );
	for(int i = 0; i < dir.numFiles(); i++){
		meshNames.push_back( dir.getName(i) );
		//cout << "OBJ FILE NAME: " << dir.getName( i ) << endl;
	}
	
	ofDirectory camdir;
	camdir.allowExt("xml");
	camdir.listDir( cameraPathPath );
	for(int i = 0; i < camdir.numFiles(); i++){
		cameraPathNames.push_back( camdir.getName(i) );
		//cout << "CAMERA PATH FILE NAME: " << camdir.getName( i ) << endl;
	}
	
	
	loadMesh( "example.obj" );

	
	//gui
	string cloudsDataPath = "../../../CloudsData/";
	colorSampleImage.loadImage( cloudsDataPath + "colors/defaultColorPalette.png" );
	
}

void CloudsVisualSystemWormHole::selfBegin(){
	
}


//normal update call
void CloudsVisualSystemWormHole::selfUpdate(){

}

void CloudsVisualSystemWormHole::selfDraw()
{
	glPointSize(10);
	ofSetColor(255, 0, 0);
	
	ofPushMatrix();
	ofMultMatrix( meshNode.getGlobalTransformMatrix() );

//	mesh.draw();
	mesh.drawVertices();
	
	ofPopMatrix();
}

void CloudsVisualSystemWormHole::selfPresetLoaded(string presetPath){
	
}
void CloudsVisualSystemWormHole::selfSceneTransformation(){
	
}

// draw any debug stuff here
void CloudsVisualSystemWormHole::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemWormHole::selfDrawBackground(){

	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemWormHole::selfEnd()
{

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemWormHole::selfExit(){
	mesh.clear();
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemWormHole::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemWormHole::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemWormHole::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemWormHole::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemWormHole::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemWormHole::selfMouseReleased(ofMouseEventArgs& data){
	
}