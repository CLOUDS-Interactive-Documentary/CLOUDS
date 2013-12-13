//
//  CloudsVisualSystemLaplacianTunnel.cpp
//

#include "CloudsVisualSystemLaplacianTunnel.h"
#include "CloudsRGBDVideoPlayer.h"

bool meshsort(NamedVbo a, NamedVbo b){
	return ofToInt( ofSplitString(a.name,"Tunnel")[1] ) < ofToInt( ofSplitString(b.name,"Tunnel")[1] );
}

int CloudsVisualSystemLaplacianTunnel::loadMesh(ofVbo &vbo, string path) {
    char* buffer;
    long size;
	
	//cout << "path is " << path << endl;
	
    ifstream data( ofToDataPath(path, true).c_str(), ios::in | ios::binary);
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
	
	for(int i = 0; i < numPts; i++){
		ofVec3f p(pts[i*3+0],pts[i*3+1],pts[i*3+2]);
		center += p;
		min = ofVec3f(MIN(min.x,p.x),
					  MIN(min.y,p.y),
					  MIN(min.z,p.z));
		max = ofVec3f(MAX(max.x,p.x),
					  MAX(max.y,p.y),
					  MAX(max.z,p.z));
		
	}
	center /= numPts;
	
    unsigned int * indices = ints + 2 + numPts*6;
	
    //not sure what is enable or disable by default
    vbo.enableIndices();
    vbo.enableNormals();
    vbo.disableColors();
    vbo.disableTexCoords();
    vbo.setVertexData(pts,3,numPts, GL_STATIC_DRAW,sizeof(float)*3);
    vbo.setNormalData(pts+numPts*3,numPts, GL_STATIC_DRAW,sizeof(float)*3);
    vbo.setIndexData(indices,numTriangles*3, GL_STATIC_DRAW);
	
	//cout << "File " << path << " has " << numTriangles << " triangles " << endl;
	return numTriangles*3;
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemLaplacianTunnel::selfSetupGui(){

    customGui = new ofxUISuperCanvas("Settings", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	
	customGui->addIntSlider("num replications", 1, 5, &numReplications);
	customGui->addSlider("fog density", 0, .3, &fogDensity);
	customGui->addSlider("light distance", 20, 600, &lightDistance);
	customGui->addSlider("cam speed", 0, 10, &cameraSpeed);
	customGui->addSlider("corkscrew factor", 0, .2, &corkscrewFactor);
	customGui->addToggle("draw points", &bDrawPoints);
	customGui->addToggle("external debug cam", &bUseExternalCamera);
	customGui->addSlider("max look angle", 0, 90, &maxLookAngle);
	customGui->addToggle("palindrome", &bPalindrome);
	customGui->addSlider("growh fps", 0, 30, &growthFPS);
	
	
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
	bUseExternalCamera = false;
	bPalindrome = false;
	lastFrameTime = 0;
	growthFPS = 0;
	currentGrowthIndex = 0;
	
	ofDirectory objs(getVisualSystemDataPath(true) + "Meshes/");
	objs.allowExt("vbo");
	objs.listDir();
	
	clear();
	
	int numFiles = objs.numFiles();
	vbos.resize( numFiles );
	for(int i = 0; i < numFiles; i++){
		vbos[i].vbo = new ofVbo();
		vbos[i].name = objs.getName(i);
		vbos[i].indexCount = loadMesh(*vbos[i].vbo, objs.getPath( i ) );
	}
	
	sort(vbos.begin(), vbos.end(), meshsort);
	
	
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
	tunnelCam.setPosition(center + ofVec3f(0,(max.y - min.y)*.5,0));
	tunnelCam.lookAt(center, ofVec3f(1,0,0));
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemLaplacianTunnel::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemLaplacianTunnel::selfUpdate(){

	tunnelCam.setPosition( ofVec3f(tunnelCam.getPosition().x,
								   tunnelCam.getPosition().y + cameraSpeed,
								   tunnelCam.getPosition().z) );
	
	externalCam.setTarget(tunnelCam.getPosition());

	
	ofVec2f targetLookAngle;
	targetLookAngle.x = ofMap(GetCloudsInputX(), 0, ofGetWidth(), -maxLookAngle,maxLookAngle);
	targetLookAngle.y = ofMap(GetCloudsInputY(), 0, ofGetHeight(),-maxLookAngle,maxLookAngle);
	
	currentLookAngle.interpolate(targetLookAngle, .05);
	
	ofQuaternion base, rx,ry;
	base.makeRotate(90, 1, 0, 0); //straight up
	rx.makeRotate(currentLookAngle.x, 0, 0, -1);
	ry.makeRotate(currentLookAngle.y, -1, 0, 0);
	tunnelCam.setOrientation(base * rx * ry);
	
	headlight.setPointLight();
	headlight.setPosition(tunnelCam.getPosition() + ofVec3f(0,lightDistance,0));
	
	currentGrowthIndex += (ofGetElapsedTimef() - lastFrameTime) * growthFPS;
	lastFrameTime = ofGetElapsedTimef();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemLaplacianTunnel::selfDraw(){
	if(vbos.size() > 0){
		
		glPushAttrib(GL_FOG_BIT);
		
		glEnable(GL_FOG);
		glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
		glFogi(GL_FOG_MODE, GL_EXP);
		
		glFogf(GL_FOG_DENSITY, powf(fogDensity,2));
		
		ofFloatColor bgColor = ofFloatColor::fromHsb(bgHue, bgSat, bgBri);
		
		GLfloat fogColor[4] = {bgColor.r/255.,bgColor.g/255.,bgColor.b/255., 1.0 };
		glFogfv (GL_FOG_COLOR, fogColor);
		glEnable(GL_DEPTH_TEST);
		
		ofEnableAlphaBlending();

		headlight.enable();
		float spread = (max.y - min.y);
		float startY = min.y + tunnelCam.getPosition().y - fmod(tunnelCam.getPosition().y, spread);
		
		mat->begin();
		ofSetColor(255);
		
		for(int i = 0; i < numReplications; i++){
			ofPushMatrix();
			glPointSize(2);
			float translateAmount = (startY + i*spread);
			ofTranslate(0,translateAmount,0);
			ofTranslate(center);
			ofRotate(translateAmount*corkscrewFactor,0,1,0);
			ofTranslate(-center);
			float cameraoffset = tunnelCam.getPosition().y - translateAmount - spread;
			
			int index;
			if(bPalindrome){
				index = fmod(currentGrowthIndex, vbos.size()*2);
				//palindrome wrap
				if(index >= vbos.size()){
					index = vbos.size()*2 - index - 1;
				}
			}
			else{
				index = int( ofMap(cameraoffset, 0, -spread*numReplications, vbos.size()-1, 0.0, true) );
			}
			
			if(bDrawPoints){
				vbos[index].vbo->draw(GL_POINTS, 0, vbos[index].indexCount);
			}
			else{
				vbos[index].vbo->drawElements(GL_TRIANGLES, vbos[index].indexCount);
			}
			
			ofPopMatrix();
		}

		mat->end();
		headlight.disable();

		glPopAttrib();
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
	clear();
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemLaplacianTunnel::selfExit(){
	clear();	
}

void CloudsVisualSystemLaplacianTunnel::clear(){
	if(vbos.size() > 0){
		for(int i = 0; i < vbos.size(); i++){
			vbos[i].vbo->clear();
			delete vbos[i].vbo;
		}
		vbos.clear();
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