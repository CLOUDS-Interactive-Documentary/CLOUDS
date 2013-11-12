//
//  CloudsVisualSystemLaplacianTunnel.cpp
//

#include "CloudsVisualSystemLaplacianTunnel.h"
#include "CloudsRGBDVideoPlayer.h"

int loadMesh(ofVbo &vbo, string path) {
    char* buffer;
    long size;
	
	//cout << "path is " << path << endl;
	
    ifstream data(ofToDataPath(path,true).c_str(), ios::in | ios::binary);
    data.seekg(0, ios::end);
    size = data.tellg();
    data.seekg(0, ios::beg);
    buffer = new char[size];
	
    data.read(buffer, size);
    data.close();
	
    unsigned int *ints = (unsigned int *) buffer;
    int numPts = ints[0];
    int numTriangles = ints[1];
    float *pts = (float *) (ints+2);
    unsigned int * indices = ints + 2 + numPts*6;
    //not sure what is enable or disable by default
    vbo.enableIndices();
    vbo.enableNormals();
    vbo.disableColors();
    vbo.disableTexCoords();
    vbo.setVertexData(pts,3,numPts,GL_STATIC_DRAW,sizeof(float)*3);
    vbo.setNormalData(pts+numPts*3,numPts,GL_STATIC_DRAW,sizeof(float)*3);
    vbo.setIndexData(indices,numTriangles*3,GL_STATIC_DRAW);
	
	//cout << "File " << path << " has " << numTriangles << " triangles " << endl;
	return numTriangles*3;
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemLaplacianTunnel::selfSetupGui(){

    customGui = new ofxUISuperCanvas("Settings", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	
	customGui->addSlider("num replications", 1, 100, &numReplications);
	customGui->addSlider("replication offset", 0, 500, &replicationOffset);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemLaplacianTunnel::selfGuiEvent);
	
    guis.push_back(customGui);
    guimap[customGui->getName()] = customGui;

}

void CloudsVisualSystemLaplacianTunnel::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemLaplacianTunnel::selfSetupSystemGui(){
	
}

void CloudsVisualSystemLaplacianTunnel::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemLaplacianTunnel::selfSetupRenderGui(){

}

void CloudsVisualSystemLaplacianTunnel::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemLaplacianTunnel::selfSetup(){
	frameCount = 0;
	fps = 15;
	
	ofDirectory objs(getVisualSystemDataPath() + "Meshes/");
	objs.allowExt("vbo");

	objs.listDir();
	objs.sort();
	
	clear();
	
	int numFiles = objs.numFiles();
	vbos.resize( numFiles );
	indexCount.resize( numFiles );
	for(int i = 0; i < numFiles; i++){
		vbos[i] = new ofVbo();
		indexCount[i] = loadMesh(*vbos[i], objs.getPath( i ) );
	}
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemLaplacianTunnel::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemLaplacianTunnel::selfBegin(){
	
	cout << "*** BEGIN!" << endl;
	

	startTime = ofGetElapsedTimef();
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemLaplacianTunnel::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemLaplacianTunnel::selfUpdate(){

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemLaplacianTunnel::selfDraw(){
	if(vbos.size() > 0){
		
		int vboIndex = int( (ofGetElapsedTimef() - startTime) * fps)  % vbos.size() ;
		
		ofPushMatrix();
		for(int i = 0; i < numReplications; i++){
			
			ofTranslate(0,replicationOffset,0);
			
			vbos[vboIndex]->drawElements(GL_TRIANGLES, indexCount[vboIndex]);
		}
		ofPopMatrix();
	}
	
}

// draw any debug stuff here
void CloudsVisualSystemLaplacianTunnel::selfDrawDebug(){
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemLaplacianTunnel::selfDrawBackground(){
	//turn the background refresh off
	//bClearBackground = false;
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemLaplacianTunnel::selfEnd(){
		
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemLaplacianTunnel::selfExit(){
	clear();	
}

void CloudsVisualSystemLaplacianTunnel::clear(){
	if(vbos.size() > 0){
		for(int i = 0; i < vbos.size(); i++){
			vbos[i]->clear();
			delete vbos[i];
		}
		vbos.clear();
		indexCount.clear();
	}
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemLaplacianTunnel::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemLaplacianTunnel::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemLaplacianTunnel::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemLaplacianTunnel::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemLaplacianTunnel::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemLaplacianTunnel::selfMouseReleased(ofMouseEventArgs& data){
	
}