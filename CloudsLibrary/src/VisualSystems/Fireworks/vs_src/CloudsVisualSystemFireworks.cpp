    //
//  CloudsVisualSystemFireworks.cpp
//


//TODO: fix camera:
// all swelf dra in self draw
// selfpost
//

#include "CloudsVisualSystemFireworks.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemFireworks::selfSetupGui(){
	
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("minLifeSpan", .1, 10, &minLifeSpan);
	customGui->addSlider("maxLifeSpan", 1, 10, &maxLifeSpan);
	
	customGui->addSlider("minExplosionTime", .1, 2, &minExplosionTime);
	customGui->addSlider("maxExplosionTime", .5, 5, &maxExplosionTime);
	
	customGui->addSlider("minParticleVelocity", 0, 100, &minVel);
	customGui->addSlider("maxParticleVelocity", 0, 200, &maxVel );
	customGui->addSlider("maxFireworkVelocity", 1, 300, &maxFWVel );
	
	customGui->addSlider("particle gravity", -100, 100, &(particleGravity.y) );
	
	customGui->addSlider("firework gravity", -1, 1, &(fireworkGravity.y) );
	
	customGui->addSlider("camSpeed", -.1, 5, &camSpeed );
		
	customGui->addToggle("Custom Toggle", &customToggle);
	
	customGui->addImageSampler("birth color map", &colorSampleImage, (float)colorSampleImage.getWidth()/2, (float)colorSampleImage.getHeight()/2 );
	
	customGui->addImageSampler("death color map", &colorSampleImage, (float)colorSampleImage.getWidth()/2, (float)colorSampleImage.getHeight()/2 );
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemFireworks::selfGuiEvent);
	
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemFireworks::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
	
	else if( e.widget->getName() == "birth color map"){
		
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		ofFloatColor col =  sampler->getColor();
		startColor.set( col.r, col.g, col.b, 1. );
		
		cout << startColor << endl;
	}
	else if( e.widget->getName() == "death color map"){
		
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		ofFloatColor col =  sampler->getColor();
		endColor.set( col.r, col.g, col.b, 1. );
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemFireworks::selfSetupSystemGui(){
	
}

void CloudsVisualSystemFireworks::guiSystemEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemFireworks::selfSetup()
{
	
	
	//setupParticles
	FIREWORKS_NUM_PARTICLES = 200000;
	
	positions = new ofVec3f[ FIREWORKS_NUM_PARTICLES ];
	velocities = new ofVec3f[ FIREWORKS_NUM_PARTICLES ];
	lifeData = new ofFloatColor[ FIREWORKS_NUM_PARTICLES ];
	indices = new ofIndexType[ FIREWORKS_NUM_PARTICLES ];
	
	minVel = 0; maxVel = 200;
	
	float lifespan;
	float t = ofGetElapsedTimef();
	
	vbo.setVertexData( &positions[0], FIREWORKS_NUM_PARTICLES, GL_DYNAMIC_DRAW );
	vbo.setNormalData( &velocities[0], FIREWORKS_NUM_PARTICLES, GL_DYNAMIC_DRAW );
	vbo.setIndexData( &indices[0], FIREWORKS_NUM_PARTICLES, GL_DYNAMIC_DRAW );
	
	
	vbo.setColorData( &lifeData[0], FIREWORKS_NUM_PARTICLES, GL_DYNAMIC_DRAW );
	
	colorSampleImage.loadImage( getVisualSystemDataPath() + "GUI/defaultColorPalette.png" );
	
	loadFileToGeometry( getVisualSystemDataPath() +  "animationTargets/dodecahedron.txt", dodecagedronPoints );
	loadFileToGeometry( getVisualSystemDataPath() +  "animationTargets/octahedron.txt", octahedronPoints );
	loadFileToGeometry( getVisualSystemDataPath() +  "animationTargets/tetrahedron.txt", tetrahedronPoints );
	loadFileToGeometry( getVisualSystemDataPath() + "animationTargets/icosahedron.txt", icosahedronPoints );
	
	//particle behavior
	fireworkGravity.set(0, -6 / 120., 0 );
	particleGravity.set( 0, 40, 0);
	minVel = 4;
	maxVel = 60;
	maxFWVel = 2.4;
	
	//shader
	shader.load(getVisualSystemDataPath() + "shaders/base.vert", getVisualSystemDataPath() + "shaders/base.frag");
	
	shader.begin();
	shader.setUniform3f( "gravity", particleGravity.x, particleGravity.y, particleGravity.z );
	shader.end();
	startColor.set( .9, .95, 1.95, 1 );
	endColor.set( .6, 1.3, .2, 1 );
	
	spriteImage.loadImage(getVisualSystemDataPath() + "images/sphereNormal.png");
	
	minLifeSpan = 2;
	maxLifeSpan = 4;
	
	//camera
	camSpeed = 2;
	
	//particle rendering
	bUpdateVbo = true;
	indexCount = 0;
	nextIndex = 0;
	numSprites = 0;
	
	nextFireworkExplosionTime = ofGetElapsedTimef() + 1;
	
	ofDisableArbTex();
	
	
	//	sprites["star"].loadImage(  getVisualSystemDataPath() + "images/star.png" );
	triangleImage.loadImage(  getVisualSystemDataPath() + "images/triangle-sprite.png" );
	squareImage.loadImage(  getVisualSystemDataPath() + "images/square-sprite.png" );
	circleImage.loadImage(  getVisualSystemDataPath() + "images/circle-sprite.png" );
	
	ofEnableArbTex();
	
	camera.setPosition(0, 0, 0);
	camTarget.set( 0,0,300);
	
	glowFbo0.allocate( ofGetWidth(), ofGetHeight(), GL_RGB );
	glowFbo1.allocate( glowFbo0.getWidth()/2, glowFbo0.getHeight()/2, GL_RGB );;
	glowFbo2.allocate( glowFbo1.getWidth()/2, glowFbo1.getHeight()/2, GL_RGB );;
	glowFbo3.allocate( glowFbo2.getWidth()/2, glowFbo2.getHeight()/2, GL_RGB );;
	glowFbo4.allocate( glowFbo3.getWidth()/2, glowFbo3.getHeight()/2, GL_RGB );;
	glowFbo5.allocate( glowFbo4.getWidth()/2, glowFbo4.getHeight()/2, GL_RGB );;
	glowFbo6.allocate( glowFbo5.getWidth()/2, glowFbo5.getHeight()/2, GL_RGB );;
	
	glowShader.load(getVisualSystemDataPath() + "shaders/post");
}

void CloudsVisualSystemFireworks::loadFileToGeometry( string loc, vector<ofVec3f>& points )
{
	ofBuffer buffer = ofBufferFromFile( loc );
    points.clear();
    if(buffer.size()) {
		
		while (buffer.isLastLine() == false) {
			string line = buffer.getNextLine();
			
			vector<string> vals = ofSplitString( line, ",");
			
			points.push_back( ofVec3f( ofToFloat(vals[0]),ofToFloat(vals[1]),ofToFloat(vals[2]) ) );
		}
		
    }
	
	buffer.clear();
}


void CloudsVisualSystemFireworks::selfBegin()
{	
	//particle behavior
//	fireworkGravity.set(0, -6 / 120., 0 );
//	particleGravity.set( 0, 40, 0);
//	minVel = 4;
//	maxVel = 60;
//	maxFWVel = 2.4;
	
	//shader
	shader.begin();
	shader.setUniform3f( "gravity", particleGravity.x, particleGravity.y, particleGravity.z );
	shader.end();
	
//	startColor.set( .9, .95, 1.95, 1 );
//	endColor.set( .6, 1.3, .2, 1 );
//	
//	minLifeSpan = .1;
//	maxLifeSpan = 1;
//	
//	//camera
//	camSpeed = 1;
	
	//particle rendering
	bUpdateVbo = true;
	indexCount = 0;
	nextIndex = 0;
	numSprites = 0;
	
	nextFireworkExplosionTime = ofGetElapsedTimef() + 1;
	
	ofEnableArbTex();
	
	camera.setPosition(0, 0, 0);
	camTarget.set( 0,0,300);
}


void CloudsVisualSystemFireworks::selfUpdate()
{
	float t = ofGetElapsedTimef();
	for (int i=emitters.size()-1; i>=0; i--) {
		emitters[i].update( t );
		
		if( emitters[i].bStarted )
		{
			
			emitters[i].pos += fireworkGravity * emitters[i].span * emitters[i].age * 10;

			ofVec3f p0 = emitters[i].pos;
			ofVec3f nScl = p0 * .01;
			float nx = ofSignedNoise(nScl.x + emitters[i].age, nScl.y, nScl.z);
			float ny = ofSignedNoise(nScl.x, nScl.y + emitters[i].age, nScl.z);
			float nz = ofSignedNoise(nScl.x, nScl.y, nScl.z + emitters[i].age);
			p0 += ofVec3f( nx, ny, nz ) * 10. * emitters[i].age;

			trailPoint( p0, emitters[i].vel, 4 );
		}
		if(emitters[i].bEnded)
		{
			emitters.erase( emitters.begin() + i );
		}
	}
	
//	for (int i=rockets.size()-1; i>=0; i--)
//	{
//		rockets[i].update( t );
//
//		if(rockets[i].bEnded)
//		{
//			rockets.erase( rockets.begin() + i );
//		}
//	}
	
	
	//camera
	ofVec3f eul = camera.getOrientationEuler();
	float xDamp = ofMap( abs(eul.x), 70, 90, 1, 0, true );
	
	float noiseTimeScl = .1;
	float noiseOffsetScl = 800;
	float mouseScl = .25;
	float panScl = -5;
	
	float noiseValX = ofSignedNoise( ofGetElapsedTimef() * noiseTimeScl + 1. ) * noiseOffsetScl;
	float noiseValY = ofSignedNoise( ofGetElapsedTimef() * noiseTimeScl ) * noiseOffsetScl;
	
	float pan = ofMap(ofGetMouseX() + noiseValX, 0, ofGetWidth(), mouseScl, -mouseScl);
	float tilt = ofMap(ofGetMouseY() + noiseValY, 0, ofGetHeight(), -mouseScl, mouseScl) * xDamp;
	if(abs(eul.x) < 90) camera.tilt( tilt );
	camera.pan( pan );
	
	float roll = abs(pan) * pan * panScl;
	camera.roll( roll );
	
	camera.move(0, abs(roll), 0);
	
	ofVec3f vel = camera.getLookAtDir();
	camera.move( vel * camSpeed );
	
	float targetDistance = 300;
	camTarget = vel * targetDistance + camera.getPosition();
	
	
	//particles
	bool updateIndices = false;
	
	indexCount = 0;
	for(int i=0; i<FIREWORKS_NUM_PARTICLES; i++){
		//if the age + lifespan is less then the current time we want to draw
		if(lifeData[i].r + lifeData[i].g > t){
			indices[indexCount] = i;
			indexCount++;
			updateIndices = true;
		}
	}
	
	if(updateIndices){
		vbo.updateIndexData( indices, indexCount );
	}
	
	
	//fireworks...
	for (int i = spawnTime.size()-1; i>=0; i--) {
		//if it's alive apply gravity
		if(spawnTime[i] + 2 > t ){
			spawnPos[i] += spawnVel[i];
			spawnVel[i] += fireworkGravity;
			spawnVel[i] *= .99;
			
			trailPoint( spawnPos[i], spawnVel[i].normalized() );
		}
		else
		{
			spawnPos.erase( spawnPos.begin()+i );
			spawnVel.erase( spawnVel.begin()+i );
			spawnTime.erase( spawnTime.begin()+i );
		}
	}
	
	if(	bUpdateVbo )
	{
		updateVbo();
	}
	
	if( nextFireworkExplosionTime < t )
	{
		explodeFireWorkAtRandom();
	}
	
}


void CloudsVisualSystemFireworks::selfDraw()
{
	//nothing to see here
}


void CloudsVisualSystemFireworks::updateVbo()
{
	int total = FIREWORKS_NUM_PARTICLES;
	vbo.updateVertexData( &positions[0], total );
	vbo.updateNormalData( &velocities[0], total );
	vbo.updateColorData( &lifeData[0], total );
	
	vbo.updateIndexData( &indices[0], numSprites );
	
	bUpdateVbo = false;
}

void CloudsVisualSystemFireworks::trailPoint( ofVec3f point, ofVec3f vel, int count )
{
	for(int i=0; i<count; i++){
		emitFromPoint( point, vel, ofRandom(minLifeSpan, maxLifeSpan), ofGetElapsedTimef() );
	}
}

void CloudsVisualSystemFireworks::emitFromPoint( ofVec3f point, ofVec3f dir, float lifespan, float t )
{
	
	int i = nextIndex;
	nextIndex ++;
	if( nextIndex > FIREWORKS_NUM_PARTICLES ) nextIndex = 0;
	
	//TODO: pass in quat rather then dir
	ofQuaternion rotQuat;
	rotQuat.makeRotate( ofVec3f(0,1,0), dir);
	
	positions[i] = point;
	velocities[i] = ofVec3f(ofRandom( minVel, maxVel ),ofRandom( minVel, maxVel ),ofRandom( minVel, maxVel )) * rotQuat;
	lifeData[i].set( t, lifespan, 0, 0 );
	
	bUpdateVbo = true;
	
	numSprites = min( numSprites+1, FIREWORKS_NUM_PARTICLES );
}

void CloudsVisualSystemFireworks::explodeFireWork( ofVec3f origin, ofVec3f vel )
{
	fireWorkExplosionTime = ofGetElapsedTimef();
	
	
	int numEmittersPerExplosion = 50;
	float t = ofGetElapsedTimef();
	for(int i=0; i<numEmittersPerExplosion; i++){
		FireworkEmitter e;
		e.setup( t, ofRandom(1.5, 3.), origin, origin + ofVec3f( ofRandom(-maxFWVel,maxFWVel), ofRandom(-maxFWVel,maxFWVel), ofRandom(-maxFWVel,maxFWVel) ) );
		emitters.push_back( e );
//		emitterCount++;
//		emitters[emitterCount].setup( t, ofRandom(1.5, 3.), origin, origin + ofVec3f( ofRandom(-maxFWVel,maxFWVel), ofRandom(-maxFWVel,maxFWVel), ofRandom(-maxFWVel,maxFWVel) ) );

	}
}


//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemFireworks::selfSceneTransformation(){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemFireworks::selfSetupRenderGui(){
	
}

void CloudsVisualSystemFireworks::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemFireworks::selfPresetLoaded(string presetPath){
	
}

void CloudsVisualSystemFireworks::explodeFireWorkAtPoint( ofVec3f point, float t )
{
	nextFireworkExplosionTime = t + ofRandom( minExplosionTime, maxExplosionTime );
	ofVec3f offset = point;
	explodeFireWork( camTarget + offset );
}

void CloudsVisualSystemFireworks::explodeFireWorkAtRandom()
{
	float t = ofGetElapsedTimef();
	nextFireworkExplosionTime = t + ofRandom( minExplosionTime, maxExplosionTime );
	
	
	ofVec3f offset( ofRandom(-1, 1), ofRandom(-.75,.75), ofRandom(-1.5, .5));
	offset.normalize();
	offset *= 200;
	
	ofVec3f rocketStart = offset + ofVec3f( ofRandom(-400, 400), 2000, 0);
	
	offset = offset * camera.getOrientationQuat();
	rocketStart = rocketStart;
	
	fireWorkExplosionTime = ofGetElapsedTimef();
	

	
	int randFWType = ofRandom(0,6);
	cout << "randFWType: "<< randFWType << endl;
	switch (randFWType) {
		case 0:
			explodeGeometry( dodecagedronPoints, camTarget + offset, camTarget + rocketStart );
			break;
			
		case 1:
			explodeFireWork( camTarget + offset );
			//explodeGeometry( tetrahedronPoints, camTarget + offset, camTarget + rocketStart );
			
			break;
		case 2:
			explodeGeometry( octahedronPoints, camTarget + offset, camTarget + rocketStart );
			break;
			
		case 3:
			explodeGeometry( tetrahedronPoints, camTarget + offset, camTarget + rocketStart );
			break;
			
		case 4:
			explodeGeometry( dodecagedronPoints, camTarget + offset, camTarget + rocketStart );
			break;
			
		default:
			explodeGeometry( dodecagedronPoints, camTarget + offset, camTarget + rocketStart );
			break;
	}
}

void CloudsVisualSystemFireworks::explodeGeometry( vector<ofVec3f>& vertices, ofVec3f offset, ofVec3f rocketStart )
{
	ofVec3f origin = offset;
	float rad = 10 + ofRandom(-5, 5);
	
	ofQuaternion q;
	q.makeRotate(33, ofRandom(-10,10), ofRandom(-10,10), ofRandom(-10,10));
	ofVec3f p, p1, rPos;
	
	float t = ofGetElapsedTimef();
	float rSpan = 0;//.5;
	for(int i=0; i<vertices.size(); i+=2){
		
		p = (vertices[i] * rad ) * q;
		p1 = (vertices[i+1] * rad ) * q;
		
//		rPos = rocketStart + ofVec3f(ofRandom(-500,500), ofRandom(-500,500), ofRandom(-500, 500));
//		rPos = (rPos * camera.getOrientationQuat()) + origin;
		
//		FireworkRocket rocket;
		FireworkEmitter e0, e1;
		
		e0.setup( t+rSpan, 2, p + origin, p1 + origin  );
		
		e1.setup( t+rSpan, 2, p1 + origin , p + origin );
		
//		rocket.setup( t, rSpan, origin, p+ origin, NULL );
		
		emitters.push_back( e0 );
		emitters.push_back( e1 );
		
//		rockets.push_back( rocket );
	}
}

// draw any debug stuff here
void CloudsVisualSystemFireworks::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemFireworks::selfDrawBackground()
{
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
	//    mat->begin();
	
	ofPushStyle();
	
	ofEnableAlphaBlending();
	ofBlendMode( OF_BLENDMODE_ADD );
	ofEnablePointSprites();
	
	glowFbo0.begin();
	ofClear(0,0,0,255);
	

	
//	glDisable( GL_DEPTH_TEST );
//	ofSetColor(225, 235, 255, 30 );
//	for (int i=0; i<rockets.size(); i++) {
//		rockets[i].draw();
//	}
	
	shader.begin();
	shader.setUniform1f( "time", ofGetElapsedTimef() );
	shader.setUniform1f( "nearClip", camera.getNearClip() );
	shader.setUniform1f( "farClip", camera.getFarClip() );
	shader.setUniform3f("cameraPosition", camPos.x, camPos.y, camPos.z );
	shader.setUniform4f("startColor", startColor.x, startColor.y, startColor.z, startColor.w );
	shader.setUniform4f("endColor", endColor.x, endColor.y, endColor.z, endColor.w );
	
	shader.setUniform3f( "gravity", particleGravity.x, particleGravity.y, particleGravity.z );
	
	shader.setUniformTexture("triangleMap", triangleImage.getTextureReference(), 2 );
	shader.setUniformTexture("squareMap", squareImage.getTextureReference(), 1 );
	shader.setUniformTexture("circleMap", circleImage.getTextureReference(), 0 );
	
	vbo.drawElements( GL_POINTS, numSprites );
	
	shader.end();
	
//	ofSetColor(255,0,0);
//	for (int i=0; i<emitterCount; i++) {
//		ofPushMatrix();
//		ofTranslate( emitters[i].pos );
//		
//		ofRect(-5, -5, 10, 10);
//		
//		ofPopMatrix();
//	}
	
	
	glowFbo0.end();
	
	ofPopStyle();
	
	ofSetColor(255);
	int alpha = 255;
	
	glowFbo1.begin();
	ofClear(0,0,0,alpha);
	glowFbo0.draw(0, 0, glowFbo1.getWidth(), glowFbo1.getHeight() );
	glowFbo1.end();
	
	glowFbo2.begin();
	ofClear(0,0,0, alpha);
	glowFbo1.draw(0, 0, glowFbo2.getWidth(), glowFbo2.getHeight() );
	glowFbo2.end();
	
	glowFbo3.begin();
	ofClear(0,0,0, alpha);
	glowFbo2.draw(0, 0, glowFbo3.getWidth(), glowFbo3.getHeight() );
	glowFbo3.end();
	
	glowFbo4.begin();
	ofClear(0,0,0, alpha);
	glowFbo3.draw(0, 0, glowFbo4.getWidth(), glowFbo4.getHeight() );
	glowFbo4.end();
	
	glowFbo5.begin();
	ofClear(0,0,0, alpha);
	glowFbo4.draw(0, 0, glowFbo5.getWidth(), glowFbo5.getHeight() );
	glowFbo5.end();
	
	glowFbo6.begin();
	ofClear(0,0,0, alpha);
	glowFbo5.draw(0, 0, glowFbo6.getWidth(), glowFbo6.getHeight() );
	glowFbo6.end();
	
	glowShader.begin();
	glowShader.setUniformTexture( "fbo", glowFbo0.getTextureReference(), 0);
	
	glowShader.setUniformTexture( "mm1", glowFbo1.getTextureReference(), 1);
	glowShader.setUniformTexture( "mm2", glowFbo2.getTextureReference(), 2);
	glowShader.setUniformTexture( "mm3", glowFbo3.getTextureReference(), 4);
	glowShader.setUniformTexture( "mm4", glowFbo4.getTextureReference(), 5);
	glowShader.setUniformTexture( "mm5", glowFbo5.getTextureReference(), 6);
	glowShader.setUniformTexture( "mm6", glowFbo6.getTextureReference(), 7);
	glowFbo0.draw(0, 0, ofGetWidth(), ofGetHeight() );
	
	glowShader.end();
	
	ofDisablePointSprites();
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemFireworks::selfEnd()
{
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemFireworks::selfExit()
{
	
	dodecagedronPoints.clear();
	octahedronPoints.clear();
	tetrahedronPoints.clear();
	icosahedronPoints.clear();
	
	colorSampleImage.clear();
	triangleImage.clear();
	squareImage.clear();
	circleImage.clear();
	
	//???: whats the right way to de-allocate these?
	glowFbo0.allocate( 0, 0 );
	glowFbo1.allocate( 0, 0 );
	glowFbo2.allocate( 0, 0 );
	glowFbo3.allocate( 0, 0 );
	glowFbo4.allocate( 0, 0 );
	glowFbo5.allocate( 0, 0 );
	glowFbo6.allocate( 0, 0 );
	
	emitters.clear();
	rockets.clear();
	
	vbo.clear();
	
    delete [] positions;
    delete [] velocities;
    delete [] baseVelocities;
    delete [] lifeData;
    delete [] indices;
	
	//???: shuld I be deleting this here? its added to the guis vector
	//	delete customGui;
	
//	delete startColorSampler;
//	delete endColorSampler;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemFireworks::selfKeyPressed(ofKeyEventArgs & args){
	if (args.key == 'l') {
		shader.load(getVisualSystemDataPath() + "shaders/base.vert", getVisualSystemDataPath() + "shaders/base.frag");
		
		glowShader.load(getVisualSystemDataPath() + "shaders/post");
	}
}
void CloudsVisualSystemFireworks::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemFireworks::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemFireworks::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemFireworks::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemFireworks::selfMouseReleased(ofMouseEventArgs& data){
	
}