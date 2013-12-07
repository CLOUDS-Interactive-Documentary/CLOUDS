//
//  CloudsVisualSystemMetaballs.cpp
//

#include "CloudsVisualSystemMetaballs.h"
#include "CloudsRGBDVideoPlayer.h"
#include <stdlib.h>     /* atoi */


//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemMetaballs::selfSetupGui(){
	
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("scl1", 0, 1, &scl1 );
	customGui->addSlider("scl2", 0, 1, &scl2 );
	customGui->addToggle("Wireframe", &wireframe);
	customGui->addToggle("Smoothing", &smoothing);
	customGui->addToggle("depth test", &depthTest);
	customGui->addSlider("threshold", 0, 1, &threshold );
	customGui->addSlider("speed", -20, 20, &speed );
	
	customGui->addImageSampler("c1", &colorMap, (float)colorMap.getWidth()/2, (float)colorMap.getHeight()/2 );
	customGui->addSlider("alpha1", 0, 1, &alpha1 );
	customGui->addImageSampler("c2", &colorMap, (float)colorMap.getWidth()/2, (float)colorMap.getHeight()/2 );
	customGui->addSlider("alpha2", 0, 1, &alpha2 );
	customGui->addSlider("mixScale", 0, 2, &mixScale );
	
	customGui->addSlider("depthAlphaScl", .1, 2, &depthAlphaScl );
	customGui->addSlider("depthAlphaExpo", .6, 10, &depthAlphaExpo );
	
	vector<string> modes;
	modes.push_back("alpha");
	modes.push_back("add");
	modes.push_back("sub");
	modes.push_back("mult");
	modes.push_back("screen");
	modes.push_back("disable");
	customGui->addRadio("blending", modes); 
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemMetaballs::selfGuiEvent);
	
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
	
	
	
	meshGui = new ofxUISuperCanvas("Mesh", gui);
	meshGui->copyCanvasStyle(gui);
	meshGui->copyCanvasProperties(gui);
	meshGui->setName("Mesh");
	meshGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	
	vector<string> isoSurfaceType;
	isoSurfaceType.push_back("noise");
	isoSurfaceType.push_back("metaballs");
	meshGui->addRadio("isoSurfaceType", isoSurfaceType);
	
	meshGui->addSlider("mc.scale.x", 10, 1000, &mc.scale.x );
	meshGui->addSlider("mc.scale.y", 10, 1000, &mc.scale.y );
	meshGui->addSlider("mc.scale.z", 10, 1000, &mc.scale.z );
	meshGui->addSlider("tileTranslateScale", .25, 1, &tileTranslateScale );
	meshGui->addSlider("cloudHeight", -200, 200, &cloudHeight );
	
	meshGui->addToggle("tiling", &tiling);
	
	
	ofAddListener(meshGui->newGUIEvent, this, &CloudsVisualSystemMetaballs::selfGuiEvent);
	
	guis.push_back(meshGui);
	guimap[meshGui->getName()] = meshGui;
}

void CloudsVisualSystemMetaballs::selfGuiEvent(ofxUIEventArgs &e){
	string name = e.widget->getName();
	
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
	
	else if(e.widget->getName() == "Smoothing"){
		
		ofxUIToggle* smooth = (ofxUIToggle *) e.widget;
		mc.setSmoothing( smooth->getValue() );
	}
	
	else if(name == "add"){
		ofxUIToggle *t = (ofxUIToggle*) e.widget;
		if(t->getValue()){
			blendMode = OF_BLENDMODE_ADD;
		}
	}
	
	else if(name == "sub"){
		ofxUIToggle *t = (ofxUIToggle*) e.widget;
		if(t->getValue()){
			blendMode = OF_BLENDMODE_SUBTRACT;
		}
	}
	
	
	
	else if(name == "mult"){
		ofxUIToggle *t = (ofxUIToggle*) e.widget;
		if(t->getValue()){
			blendMode = OF_BLENDMODE_MULTIPLY;
		}
	}
	
	else if(name == "screen"){
		ofxUIToggle *t = (ofxUIToggle*) e.widget;
		if(t->getValue()){
			blendMode = OF_BLENDMODE_SCREEN;
		}
	}
	
	else if(name == "disable"){
		ofxUIToggle *t = (ofxUIToggle*) e.widget;
		if(t->getValue()){
			blendMode = OF_BLENDMODE_DISABLED;
		}
	}
	
	else if(name == "alpha"){
		ofxUIToggle *t = (ofxUIToggle*) e.widget;
		if(t->getValue()){
			blendMode = OF_BLENDMODE_ALPHA;
		}
	}
	
	else if( e.widget->getName() == "c1"){
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		ofFloatColor col = sampler->getColor();
		c1.set(col.r, col.g, col.b, 1.);
	}
	else if( e.widget->getName() == "c2"){
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		
		ofFloatColor col = sampler->getColor();
		c2.set(col.r, col.g, col.b, 1.);
	}
	
	else if(name == "noise"){
		ofxUIToggle *t = (ofxUIToggle*) e.widget;;
		if(t->getValue()){
			mcType = 0;
		}
	}
	
	else if(name == "metaballs"){
				ofxUIToggle *t = (ofxUIToggle*) e.widget;
		if(t->getValue()){
			mcType = 1;
		}
	}
	
	
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemMetaballs::selfSetupSystemGui(){
	
}

void CloudsVisualSystemMetaballs::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemMetaballs::selfSetupRenderGui(){

}

void CloudsVisualSystemMetaballs::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemMetaballs::selfSetup(){
	
	colorMap.loadImage( getVisualSystemDataPath() + "GUI/defaultColorPalette.png" );
	
	
	//MC
	mc.setup();
	shader.load( getVisualSystemDataPath() + "shaders/facingRatio" );
	depthAlphaScl = 1.25;
	
	drawGrid = true;
	mc.setResolution(32,16,32);
	mc.scale.set( 300, 100, 300 );
	maxVerts = 200000;
	
	scl1 = .15;
	scl2 = .1;
	
	mcType = 1;
	tileTranslateScale = .5;
	mc.setSmoothing( smoothing );
	
	depthTest = true;
	tiling = true;
	noiseValsCached = false;
	
	mcRadiusScale = 1.5;
	
	
	//used to speed up noise rendering
	cacheNoiseVals();
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemMetaballs::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemMetaballs::selfBegin(){
	
	balls.resize( 10 );
	ballRadius.resize(balls.size());
	ballVelocity.resize(balls.size());
	
	for(int i=0; i<balls.size(); i++){
		balls[i].set( ofRandom(-100, 100), ofRandom(-100, 100), ofRandom(-100, 100));
		ballRadius[i] = ofRandom(10, 30);
	}
	
//	updateMeshFauxBalls();
//	
//	updateMesh();
}

void CloudsVisualSystemMetaballs::cacheNoiseVals(){
	
	//create our noise samples
	float scl1 = 10;//.1;
	float scl2 = 5;//.15;
	float scl3 = 2;//.2;
	
	noiseVals.resize( mc.resX );
	float iStep = 1. / float(mc.resX);
	float jStep = 1. / float(mc.resY);
	float kStep = 1. / float(mc.resZ);
	for (int i=0; i<noiseVals.size(); i++) {
		noiseVals[i].resize(mc.resY);
		for (int j=0; j<noiseVals[i].size(); j++) {
			noiseVals[i][j].resize( mc.resZ );
			for (int k=0; k<noiseVals[i][j].size(); k++) {
				noiseVals[i][j][k].set( ofNoise(scl1*i*iStep,scl1*j*jStep,scl1*k*kStep),
									   ofNoise(scl2*i*iStep,scl2*j*jStep,scl2*k*kStep),
									   ofNoise(scl3*i*iStep,scl3*j*jStep,scl3*k*kStep));
			}
		}
	}
	
	noiseValsCached = true;
}

void CloudsVisualSystemMetaballs::addBallToMC(ofVec3f pos, float rad){
	//center of the metaballs == half it's scale
	ofVec3f hlfScl = mc.scale/2;
	ofVec3f p0 = hlfScl + pos;
	
	float scaledRad = rad * mcRadiusScale;
	float radSqr = scaledRad;// * scaledRad;
	
	int minX = ofClamp( ofMap( p0.x - rad, 0, mc.scale.x, 0, mc.resX), 0, mc.resX);
	int maxX = ofClamp( ofMap( p0.x + rad, 0, mc.scale.x, 0, mc.resX), 0, mc.resX);
	
	int minY = ofClamp( ofMap( p0.y - rad, 0, mc.scale.y, 0, mc.resY), 0, mc.resY);
	int maxY = ofClamp( ofMap( p0.y + rad, 0, mc.scale.y, 0, mc.resY), 0, mc.resY);
	
	int minZ = ofClamp( ofMap( p0.z - rad, 0, mc.scale.z, 0, mc.resZ), 0, mc.resZ);
	int maxZ = ofClamp( ofMap( p0.z + rad, 0, mc.scale.z, 0, mc.resZ), 0, mc.resZ);
	
	ofVec3f step( 1./float(mc.resX), 1./float(mc.resY), 1./float(mc.resZ) );
	ofVec3f indexPos, wp;
	
	for (int x=minX; x<=maxX; x++) {
		indexPos.x = x;
		for(int y=minY; y<=maxY; y++){
			indexPos.y = y;
			for(int z=minZ; z<=maxZ; z++){
				indexPos.z = z;
				wp = (step * indexPos) * mc.scale;
				float dist = wp.distance( p0 );
				if( dist < radSqr ){
					mc.setIsoValue( x, y, z, 1. - dist / radSqr + mc.getIsoValue(x,y,z) ) ;
				}
			}
		}
	}
}

void CloudsVisualSystemMetaballs::updateMeshNoise(){
	
	if(!noiseValsCached)	cacheNoiseVals();
	
	float t=ofGetElapsedTimef() * speed;
	float val;
	ofVec3f center( mc.resX, mc.resY, mc.resZ );
	center /= 2;
	
	float lengthSq = center.lengthSquared();
	ofVec3f gridPos;
	float distSq;
	float mixval, lerped, verticalFade;
	int i0, i1;
	
	float mixI, mixJ, mixK;
	
	mixval = t - floor(t);
	if(mixval < 0)	mixval += 1.;
	for(int i=0; i<mc.resX; i++){
		
		i0 = int(floor(i+t)) % mc.resX ;
		if(i0<0)	i0+=mc.resX;
		
		i1 = int(ceil(i+t)) % mc.resX ;
		if(i1<0)	i1+=mc.resX;
		
		for(int j=0; j<mc.resY; j++){
			
			verticalFade = (1. - abs((float)j / (float) mc.resY - .5) * 2.);
			
			for(int k=0; k<mc.resZ; k++){
				gridPos.set(i,j,k);
				distSq = gridPos.distanceSquared(center)/lengthSq;
				
				val = verticalFade;
				val *= noiseVals[i1][j][k].x * mixval + noiseVals[i0][j][k].x * (1. - mixval);
				val *= noiseVals[i][j][k].y;
				
				mc.setIsoValue( i, j, k, val * (1. - distSq) );
			}
		}
	}
}

void CloudsVisualSystemMetaballs::updateMeshFauxBalls(){
	//clear the isoVals
	mc.wipeIsoValues();
	
	//update the balls positions
	float attenuation = .999;
	float attraction = -.1;
	float force, distance, diff;
	ofVec3f delta;
	for (int i=0; i<balls.size(); i++) {
		//add the global force and attenuation
		ballVelocity[i] *= attenuation;
		ballVelocity[i] += balls[i].normalized() * attraction;
		
		balls[i] += ballVelocity[i];
		
		//collide with the other balls
		for (int j=0; j<balls.size(); j++) {
			if(i != j){
				delta = balls[i] - balls[j];
				distance = delta.length();
				float minDist = ballRadius[i] + ballRadius[j];
				if( minDist > distance ){
					
					float overlap = minDist - distance;
					
					delta.normalize();
					delta *= overlap * .5;
					
					balls[i] += delta;
					balls[j] -= delta;
					
					ballVelocity[i] += delta;
					ballVelocity[j] -= delta;
					
				}
			}
		}
	}
	
	//add them to the marching cubes
	for(int i=0; i<balls.size(); i++){		
		//add it to the marching cubes
		addBallToMC(balls[i], ballRadius[i]);
	}
	
}

void CloudsVisualSystemMetaballs::updateMesh()
{
	switch (mcType) {
		case 0:
			updateMeshNoise();
			break;
			
		case 1:
			updateMeshFauxBalls();
			break;
			
		default:
			updateMeshNoise();
			break;
	}
	
	//update the mesh
	mc.update(threshold);
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemMetaballs::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemMetaballs::selfUpdate(){
	
	float t = ofGetElapsedTimef() * 5.;
	
	float camSpeed = 1;
	ofVec3f camVec = getCameraRef().getPosition() -	((CloudsRGBDCamera*)&getCameraRef())->targetPosition;
	camVec.normalize();
	camVec *= camSpeed * t;
	
	
	updateMesh();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemMetaballs::selfDraw()
{
	
	if(!depthTest)	glDisable( GL_DEPTH_TEST );
	else	glEnable( GL_DEPTH_TEST );
	
	ofEnableBlendMode( blendMode );
	
	//draw front faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glPointSize( 4 );
	
	//draw the mesh
	shader.begin();
	shader.setUniform3f("c1", c1.r, c1.g, c1.b);
	shader.setUniform3f("c2", c2.r, c2.g, c2.b);
	shader.setUniform1f("alpha1", alpha1 );
	shader.setUniform1f("alpha2", alpha2 );
	shader.setUniform1f("mixScale", mixScale );
	shader.setUniform1f("depthAlphaScl", depthAlphaScl );
	shader.setUniform1f("depthAlphaExpo", depthAlphaExpo );
	
	//draw some repeating cloud tiles with a scale factor so they overlap
	ofVec3f tileTranslate = mc.scale * tileTranslateScale;
	ofPushMatrix();
	
	ofTranslate(0,cloudHeight,0);
	
	if(tiling){
		for(int x=2; x>=-2; x--){
			for (int z=0; z<=3; z++) {
				
				if(!(x == 0 && z == 0)){
					
					ofPushMatrix();
					ofTranslate(tileTranslate.x * x, 0, tileTranslate.z * z);
					
					wireframe?	mc.drawWireframe() : mc.draw();
					
					ofPopMatrix();
				}
			}
		}
	}
	
	//draw the backside of the front clouds
	glCullFace(GL_FRONT);
	wireframe?	mc.drawWireframe() : mc.draw();
	
	//now draw the front
	glCullFace(GL_BACK);
	wireframe?	mc.drawWireframe() : mc.draw();
	
	shader.end();
	
//	for (int i=0; i<balls.size(); i++) {
//		ofSphere(balls[i].x, balls[i].y, balls[i].z, ballRadius[i] );
//	}
	
	ofPopMatrix();
	
	
	glDisable(GL_CULL_FACE);
	glDisable( GL_DEPTH_TEST );
}

// draw any debug stuff here
void CloudsVisualSystemMetaballs::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemMetaballs::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemMetaballs::selfEnd(){
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemMetaballs::selfExit(){
	mc.clear();
	
	//delete some other mc data
	mc.gridPointComputed.clear();
	mc.vertices.clear();
	mc.normals.clear();
	mc.boundryBox.clear();
	mc.vbo.clear();
	
	for (int i=0; i<noiseVals.size(); i++) {
		for (int j=0; j<noiseVals[i].size(); j++) {
			noiseVals[i][j].clear();
		}
		noiseVals[i].clear();
	}
	noiseVals.clear();
	
	for (int i=0; i<ballVals.size(); i++) {
		for (int j=0; j<ballVals[i].size(); j++) {
			ballVals[i][j].clear();
		}
		ballVals[i].clear();
	}
	ballVals.clear();
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemMetaballs::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		shader.load( getVisualSystemDataPath() + "shaders/facingRatio" );
	}
}
void CloudsVisualSystemMetaballs::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemMetaballs::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMetaballs::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMetaballs::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMetaballs::selfMouseReleased(ofMouseEventArgs& data){
	
}