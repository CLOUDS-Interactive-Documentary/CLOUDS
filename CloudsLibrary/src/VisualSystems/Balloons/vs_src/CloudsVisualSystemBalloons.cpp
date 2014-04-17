//
//  CloudsVisualSystemBalloons.cpp
//

#include "CloudsVisualSystemBalloons.h"
#include "CloudsRGBDVideoPlayer.h"

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

void CloudsVisualSystemBalloons::selfSetupSystemGui(){
	
}

void CloudsVisualSystemBalloons::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemBalloons::selfSetupRenderGui(){

}

void CloudsVisualSystemBalloons::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemBalloons::selfSetDefaults(){

}

void CloudsVisualSystemBalloons::selfSetup()
{
	//make our ballons
	dim = 300;
	
	//make data
	dimX = 64;
	dimY = 64;
	
	vector<ofVec3f>pos(dimY*dimX);
	vector<ofVec3f>vel(dimY*dimX);
	vector<ofVec3f>col(pos.size());
	
	for (int i=0; i<dimY; i++)
	{
		for(int j=0; j<dimX; j++)
		{
			pos[i*dimX + j].set(ofRandom(-dim, dim) * .5, ofRandom(-dim*3, -dim), ofRandom(-dim, dim) * .5);
		}
	}
	
	for(int i=0; i<col.size(); i++)
	{
		col[i].set(ofRandom(0,1),ofRandom(0,1),ofRandom(0,1));
		vel[i].set(0,ofRandom(0, 2),0);
	}
	
	//store data
	//posiiton
	posFbo0.allocate(dimX, dimY, GL_RGB32F);
	posFbo0.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    posFbo0.getTextureReference().loadData( &pos[0][0], dimX, dimY, GL_RGB);
	
	posFbo1.allocate(dimX, dimY, GL_RGB32F);
	posFbo1.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    posFbo1.getTextureReference().loadData( &pos[0][0], dimX, dimY, GL_RGB);
	
	//velocity
	velFbo0.allocate(dimX, dimY, GL_RGB32F);
	velFbo0.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    velFbo0.getTextureReference().loadData( &vel[0][0], dimX, dimY, GL_RGB);
	
	velFbo1.allocate(dimX, dimY, GL_RGB32F);
	velFbo1.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    velFbo1.getTextureReference().loadData( &vel[0][0], dimX, dimY, GL_RGB);
	
	//rotations
	quatFbo.allocate(dimX, dimY, GL_RGBA32F);
	quatFbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	
	p0 = &posFbo0;
	p1 = &posFbo1;
	
	v0 = &velFbo0;
	v1 = &velFbo1;
	
	//color
	colFbo.allocate(dimX, dimY, GL_RGB);
	colFbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    colFbo.getTextureReference().loadData( &col[0][0], dimX, dimY, GL_RGB);

	//load balloon mesh
	ofMesh temp;
//	ofxObjLoader::load( getVisualSystemDataPath() + "models/balloon_low.obj", temp);
//	ofxObjLoader::load( getVisualSystemDataPath() + "models/balloon_mid.obj", temp);
	ofxObjLoader::load( getVisualSystemDataPath() + "models/balloon.obj", temp);
	
	vector<ofVec3f>& v = temp.getVertices();
	vector<ofVec3f>& n = temp.getNormals();
	
	total = v.size();

	vbo.setVertexData(&v[0], v.size(), GL_STATIC_DRAW);
	vbo.setNormalData(&n[0], n.size(), GL_STATIC_DRAW);
	
	shader.load(getVisualSystemDataPath() + "shaders/normalShader");
	posShader.load(getVisualSystemDataPath() + "shaders/posShader");
	velShader.load(getVisualSystemDataPath() + "shaders/velShader");
	quatShader.load(getVisualSystemDataPath() + "shaders/quatShader");
}

void CloudsVisualSystemBalloons::selfPresetLoaded(string presetPath){
	
}

void CloudsVisualSystemBalloons::selfBegin()
{
	
}

void CloudsVisualSystemBalloons::selfSceneTransformation(){
	
}

void CloudsVisualSystemBalloons::selfUpdate()
{
	ofVec3f acc;
	ofVec3f p = getCameraPosition() * .01;
	float noiseScl = 1;
	float offset = .1;
	float attractionToCenter = 1;
	
	acc.x = ofSignedNoise(p.x+offset, p.y, p.z) - ofSignedNoise(p.x-offset, p.y, p.z);
	//acc.y = ofSignedNoise(p.x, p.y+offset, p.z) - ofSignedNoise(p.x, p.y-offset, p.z);
	acc.z = ofSignedNoise(p.x, p.y, p.z+offset) - ofSignedNoise(p.x, p.y, p.z-offset);
	
	acc *= noiseScl;
	
	
	//attract them to the center axis
	float camDistToOrigin = getCameraPosition().length();
	float minDist = dim * .3;
	if(camDistToOrigin > minDist)
	{
		acc -= getCameraPosition().normalized() * attractionToCenter * ofMap(camDistToOrigin, minDist, dim, 0, 1, true);
	}
	
	getCameraRef().move( acc );
	
	
	netHeight = ofGetElapsedTimef()*20. - dim;
	if (netHeight > 0)
	{
		netHeight = 100000;
	}
}

void CloudsVisualSystemBalloons::selfDraw()
{
	ofPushStyle();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	//update positions
	p0->begin();
    ofClear(0, 255);
	posShader.begin();
	posShader.setUniformTexture("posTexture", p1->getTextureReference(), 0);
	posShader.setUniformTexture("velTexture", v1->getTextureReference(), 1);
	posShader.setUniform1f("dimX", dimX);
	posShader.setUniform1f("dimY", dimY);
	posShader.setUniform1f("bound", dim);

	ofRect(-1,-1,2,2);
	
	posShader.end();
	
	p0->end();
	swap(p0, p1);
	
	//update velocities
	v0->begin();
    ofClear(0, 255);
	velShader.begin();
	velShader.setUniformTexture("posTexture", p1->getTextureReference(), 0);
	velShader.setUniformTexture("velTexture", v1->getTextureReference(), 1);
	velShader.setUniform1f("netHeight", netHeight );
	velShader.setUniform1f("dimX", dimX);
	velShader.setUniform1f("dimY", dimY);
	velShader.setUniform1f("bound", dim);
	
	ofVec3f camPos = getCameraRef().getPosition();
	velShader.setUniform3f("camPos", camPos.x, camPos.y, camPos.z);
	
	ofRect(-1,-1,2,2);
	
	velShader.end();
	v0->end();
	swap(v0, v1);
	
	//update the rotations
	quatFbo.begin();
    ofClear(0, 255);
	quatShader.begin();
	quatShader.setUniformTexture("velTexture", v1->getTextureReference(), 0);
	quatShader.setUniform1f("dimX", dimX);
	quatShader.setUniform1f("dimY", dimY);
	
	ofRect(-1,-1,2,2);
	
	quatShader.end();
	quatFbo.end();
	
	//draw the balloons
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	shader.begin();
	shader.setUniform1f("shininess", 128);
	
	shader.setUniform1f("dim", dim );
	shader.setUniform3f("camPos", camPos.x, camPos.y, camPos.z);
	shader.setUniform1f("facingRatio", .75);
	
	shader.setUniform1f("dimX", dimX);
	shader.setUniform1f("dimY", dimY);
	shader.setUniformTexture("posTexture", p0->getTextureReference(), 0);
	shader.setUniformTexture("velTexture", v0->getTextureReference(), 1);
	shader.setUniformTexture("colTexture", colFbo.getTextureReference(), 2);
	shader.setUniformTexture("quatTexture", quatFbo.getTextureReference(), 3);
	
	//vbo instancing
	vbo.bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, total, dimX*dimY);
	vbo.unbind();
	
	shader.end();
	
	glPopAttrib();
	ofPopStyle();
}

void CloudsVisualSystemBalloons::selfDrawDebug(){
	
}

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

void CloudsVisualSystemBalloons::selfEnd(){	
	
}

void CloudsVisualSystemBalloons::selfExit()
{
	posFbo0.getTextureReference().clear();
	posFbo1.getTextureReference().clear();
	velFbo0.getTextureReference().clear();
	velFbo1.getTextureReference().clear();
	colFbo.getTextureReference().clear();
	quatFbo.getTextureReference().clear();
}

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