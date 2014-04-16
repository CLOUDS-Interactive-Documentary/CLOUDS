//
//  CloudsVisualSystemBalloons.cpp
//

#include "CloudsVisualSystemBalloons.h"
#include "CloudsRGBDVideoPlayer.h"

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemBalloons::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemBalloons::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemBalloons::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemBalloons::selfSetupSystemGui(){
	
}

void CloudsVisualSystemBalloons::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemBalloons::selfSetupRenderGui(){

}

void CloudsVisualSystemBalloons::guiRenderEvent(ofxUIEventArgs &e){
	
}

//This is called whenever a new preset is loaded, before selfSetup()
//use it to ensure all your simple variables are initialized to an
//acceptable default state
void CloudsVisualSystemBalloons::selfSetDefaults(){

}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemBalloons::selfSetup()
{
	//load balloon mesh
	ofMesh temp;
	ofxObjLoader::load( getVisualSystemDataPath() + "models/balloon.obj", temp);
	
	vector<ofVec3f>& v = temp.getVertices();
	vector<ofVec3f>& n = temp.getNormals();
//	vector<ofIndexType>& i = temp.getIndices();
	
	total = v.size();

	vbo.setVertexData(&v[0], v.size(), GL_STATIC_DRAW);
	vbo.setNormalData(&n[0], n.size(), GL_STATIC_DRAW);
//	vbo.setIndexData(&i[0], i.size(), GL_STATIC_DRAW);
	
	//make our ballons
	dim = 500;
	balloons.resize(5000);
	for(int i=0; i<balloons.size(); i++)
	{
		balloons[i].pos.set(ofRandom(-dim,dim),ofRandom(-dim,dim),ofRandom(-dim,dim));
		
		balloons[i].color.set(ofRandom(0,255),ofRandom(0,255),ofRandom(0,255));
		
		balloons[i].vel.set(0,ofRandom(0, 1),0);
	}
	
	shader.load(getVisualSystemDataPath() + "shaders/normalShader");
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemBalloons::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemBalloons::selfBegin()
{
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemBalloons::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemBalloons::selfUpdate()
{
	float nScl = .01;
	float nWeight = .005;
	ofQuaternion q;
	for (int i=0; i<balloons.size(); i++)
	{
		balloons[i].vel.x *= .99;
		balloons[i].vel.z *= .99;
		
		balloons[i].vel.x += ofSignedNoise(balloons[i].pos.y * nScl, balloons[i].pos.z * nScl) * nWeight;
		balloons[i].vel.z += ofSignedNoise(balloons[i].pos.y * nScl, balloons[i].pos.x * nScl) * nWeight;
		
		balloons[i].pos += balloons[i].vel;
		
		for(int j=0; j<3; j++)
		{
			if(balloons[i].pos[j] < -dim)
			{
				balloons[i].pos[j] += dim * 2;
			}
			else if(balloons[i].pos[j] > dim)
			{
				balloons[i].pos[j] -= dim * 2;
			}
		}
		
		balloons[i].transform.setTranslation(balloons[i].pos);
		q.makeRotate(ofVec3f(0,1,0), balloons[i].vel);
		balloons[i].transform.setRotate(q);
	}

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemBalloons::selfDraw()
{
	ofPushStyle();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	shader.begin();
	shader.setUniform1f("dim", dim * 2);
	
	//bind the balloon once and draw it each point
	vbo.bind();
	for(int i=0; i<balloons.size(); i++)
	{
		ofSetColor(balloons[i].color);
		
		ofPushMatrix();
		ofMultMatrix(balloons[i].transform);
		ofScale(15,15,15);

		glDrawArrays(GL_TRIANGLES, 0, total);
		
		ofPopMatrix();
	}
	vbo.unbind();
	
	shader.end();
	
	glPopAttrib();
	ofPopStyle();
}

// draw any debug stuff here
void CloudsVisualSystemBalloons::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemBalloons::selfDrawBackground(){

	//we are using this to draw what keywords are missing content
	if(mainKeyword != ""){
		string keystodraw = "PICKED RANDOM PRESET\n";
		keystodraw += mainKeyword + "\n" + ofJoinString(keywords, ",");
		ofPushMatrix();
		ofScale(5,5);
		ofDrawBitmapString(keystodraw, 20,20);
		ofPopMatrix();
	}
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemBalloons::selfEnd(){	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemBalloons::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemBalloons::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemBalloons::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemBalloons::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemBalloons::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemBalloons::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemBalloons::selfMouseReleased(ofMouseEventArgs& data){
	
}