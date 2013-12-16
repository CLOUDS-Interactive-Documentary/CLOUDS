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
	
	customGui = new ofxUISuperCanvas("FireworBehavior", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("FireworBehavior");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("particle speed", .01, 3, &speed);
	
	customGui->addSlider("particleSpread", 0, 60., &particleSpread);
	
	customGui->addSlider("explosionFrequency", .1, 1, &explosionFrequencey);
	customGui->addIntSlider("emissonRate", 1, 5, &emissonRate);
	customGui->addSlider("spawnDistance", 10, 400, &spawnDistance);
	
	customGui->addSlider("maxFireworkVelocity", 1, 300, &maxFWVel );
	
	customGui->addSlider("gravity", -10, 10, &(gravity.y) );
	customGui->addSpacer();
	customGui->addToggle("burst", &bBurst);
	customGui->addToggle("octahedron", &bOctahedron);
	customGui->addToggle("tetrahedron", &bTetrahedron);
	customGui->addToggle("dodecagedron", &bDodecagedron);
	customGui->addSpacer();
	
	
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemFireworks::selfGuiEvent);
	
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
	
	camGui = new ofxUISuperCanvas("cameraMotion", gui);
	camGui->copyCanvasStyle(gui);
	camGui->copyCanvasProperties(gui);
	camGui->setName("cameraMotion");
	camGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	camGui->addToggle("bAnimateCamera", &bAnimateCamera);
	
	camGui->addSlider("camSpeed", -.1, 5, &camSpeed );
	camGui->addSlider("cameraMotionScale", 0, 1, &cameraMotionScl );
	
	
	ofAddListener(camGui->newGUIEvent, this, &CloudsVisualSystemFireworks::selfGuiEvent);
	guis.push_back(camGui);
	guimap[camGui->getName()] = camGui;
	
	
	
	fireworkGui = new ofxUISuperCanvas("FireworkRender", gui);
	fireworkGui->copyCanvasStyle(gui);
	fireworkGui->copyCanvasProperties(gui);
	fireworkGui->setName("FireworkRender");
	fireworkGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	
	fireworkGui->addSlider("particleSize", 1, 20, &particleSize);
	
	fireworkGui->addSpacer();
	fireworkGui->addToggle("squares", &bUseSquare);
	fireworkGui->addToggle("circles", &bUseCircle);
	fireworkGui->addToggle("triangles", &bUseTriangle);
	fireworkGui->addToggle("points", &bUseDot);
	fireworkGui->addSpacer();
	
	fireworkGui->addLabel("color_birth");
	fireworkGui->addImageSampler("birth_color_map", &colorSampleImage, (float)colorSampleImage.getWidth()/2, (float)colorSampleImage.getHeight()/2 );
	fireworkGui->addSlider("birth_saturation", 0, 1, &startColorSaturation );
	
	fireworkGui->addLabel("color_death");
	fireworkGui->addImageSampler("death_color_map", &colorSampleImage, (float)colorSampleImage.getWidth()/2, (float)colorSampleImage.getHeight()/2 );
	fireworkGui->addSlider("death_saturation", 0, 1, &endColorSaturation );
	
	ofAddListener(fireworkGui->newGUIEvent, this, &CloudsVisualSystemFireworks::selfGuiEvent);
	guis.push_back(fireworkGui);
	guimap[fireworkGui->getName()] = fireworkGui;
	
	fireworkFogGui = new ofxUISuperCanvas("FireworkFog", gui);
	fireworkFogGui->copyCanvasStyle(gui);
	fireworkFogGui->copyCanvasProperties(gui);
	fireworkFogGui->setName("FireworkFog");
	fireworkFogGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	fireworkFogGui->addSpacer();
	fireworkFogGui->addLabel("FogColor");
	fireworkFogGui->addImageSampler("fogColor", &colorSampleImage, 100, 100);
	fireworkFogGui->addSlider("fogSaturation", 0, 1.1, &fogSaturation);
	fireworkFogGui->addSpacer();
	fireworkFogGui->addSlider("FogDistance", 100, 3000, &fogDistance);
	fireworkFogGui->addSlider("fogAttenuation", 0, 2, &fogAttenuation);
	
	ofAddListener(fireworkFogGui->newGUIEvent, this, &CloudsVisualSystemFireworks::selfGuiEvent);
	guis.push_back(fireworkFogGui);
	guimap[fireworkFogGui->getName()] = fireworkFogGui;
	
}

void CloudsVisualSystemFireworks::selfGuiEvent(ofxUIEventArgs &e){
	
	if( e.widget->getName() == "birth_color_map"){
		
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		startColor = sampler->getColor();
	}
	else if( e.widget->getName() == "death_color_map"){
		
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		endColor = sampler->getColor();
	}
	else if( e.widget->getName() == "fogColor")
	{
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		fogColor = sampler->getColor();
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
	//defaults
	bAnimateCamera = true;
	minLifeSpan = .5;
	maxLifeSpan = 1.5;
	
	minExplosionTime = .75;
	maxExplosionTime = 1.25;
	
	speed = .25;
	explosionFrequencey = .1;
	emissonRate = 2;
	particleSpread = 10;
	particleSize = 10;
	spawnDistance = 250;
	
	camSpeed = .7;
	cameraMotionScl = .25;
	
	gravity.set( 0, 0, 0);
	
	minVel = .75;
	maxVel = 1.25;
	
	maxFWVel = 200;
	
	fogDistance = 800;
	fogAttenuation = 1;
	fogColor.set(0,1,0,1);
	
	bUseCircle = bUseSquare = bUseTriangle = bUseDot = true;
	
	bBurst = bOctahedron = bTetrahedron = bDodecagedron = true;
	
	startColorSaturation = endColorSaturation = fogSaturation = 1;

	
	//setupParticles
	FIREWORKS_NUM_PARTICLES = 200000;
	
	positions = new ofVec3f[ FIREWORKS_NUM_PARTICLES ];
	velocities = new ofVec3f[ FIREWORKS_NUM_PARTICLES ];
	lifeData = new ofFloatColor[ FIREWORKS_NUM_PARTICLES ];
	indices = new ofIndexType[ FIREWORKS_NUM_PARTICLES ];
	
	float lifespan;
	float t = ofGetElapsedTimef();
	
	vbo.setVertexData( &positions[0], FIREWORKS_NUM_PARTICLES, GL_DYNAMIC_DRAW );
	vbo.setNormalData( &velocities[0], FIREWORKS_NUM_PARTICLES, GL_DYNAMIC_DRAW );
	vbo.setIndexData( &indices[0], FIREWORKS_NUM_PARTICLES, GL_DYNAMIC_DRAW );
	
	
	vbo.setColorData( &lifeData[0], FIREWORKS_NUM_PARTICLES, GL_DYNAMIC_DRAW );
	
	//TODO: mention to james that we might need a getCloudsData method
	string cloudsDataPath = "../../../CloudsData/";
	colorSampleImage.loadImage( cloudsDataPath + "colors/defaultColorPalette.png" );
	
	loadFileToGeometry( getVisualSystemDataPath() +  "animationTargets/dodecahedron.txt", dodecagedronPoints );
	loadFileToGeometry( getVisualSystemDataPath() +  "animationTargets/octahedron.txt", octahedronPoints );
	loadFileToGeometry( getVisualSystemDataPath() +  "animationTargets/tetrahedron.txt", tetrahedronPoints );
	loadFileToGeometry( getVisualSystemDataPath() + "animationTargets/icosahedron.txt", icosahedronPoints );
	
	//shader
	shader.load(getVisualSystemDataPath() + "shaders/base.vert", getVisualSystemDataPath() + "shaders/base.frag");
	
	shader.begin();
	shader.setUniform3f( "gravity", gravity.x, gravity.y, gravity.z );
	shader.end();
	startColor.set( .9, .95, 1.95, 1 );
	endColor.set( .6, 1.3, .2, 1 );
	
	spriteImage.loadImage(getVisualSystemDataPath() + "images/sphereNormal.png");
		
	//particle rendering
	bUpdateVbo = true;
	indexCount = 0;
	nextIndex = 0;
	numSprites = 0;
	
	nextFireworkExplosionTime = ofGetElapsedTimef() + 1;
	
	ofDisableArbTex();
	
	
	dotImage.loadImage(  getVisualSystemDataPath() + "images/sphereNormal.png" );
	triangleImage.loadImage(  getVisualSystemDataPath() + "images/triangle-sprite.png" );
	squareImage.loadImage(  getVisualSystemDataPath() + "images/square-sprite.png" );
	circleImage.loadImage(  getVisualSystemDataPath() + "images/circle-sprite.png" );
	
	ofEnableArbTex();
	
	getCameraRef().setPosition(0, 0, 0);
	camTarget.set( 0,0,spawnDistance);
	
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
	//shader
	shader.begin();
	shader.setUniform3f( "gravity", gravity.x, gravity.y, gravity.z );
	shader.end();
	
	//particle rendering
	bUpdateVbo = true;
	indexCount = 0;
	nextIndex = 0;
	numSprites = 0;
	
	nextFireworkExplosionTime = ofGetElapsedTimef() + 1;
	
	ofEnableArbTex();
	
	getCameraRef().setPosition(0, 0, 0);
	camTarget.set( 0,0,spawnDistance);
}


void CloudsVisualSystemFireworks::selfUpdate()
{
	float t = ofGetElapsedTimef();
	
	//camera
	if(bAnimateCamera)
	{
		ofVec3f eul = getCameraRef().getOrientationEuler();
		float xDamp = ofMap( abs(eul.x), 70, 90, 1, 0, true );
		
		float noiseTimeScl = .1;
		float noiseOffsetScl = 800;
		
		float panScl = -5;
		
		float noiseValX = ofSignedNoise( ofGetElapsedTimef() * noiseTimeScl + 1. ) * noiseOffsetScl;
		float noiseValY = ofSignedNoise( ofGetElapsedTimef() * noiseTimeScl ) * noiseOffsetScl;
		
		//pan and tilt with mouse
		float pan = ofMap(GetCloudsInputX() + noiseValX, 0, ofGetWidth(), cameraMotionScl, -cameraMotionScl);
		float tilt = ofMap(GetCloudsInputY() + noiseValY, 0, ofGetHeight(), cameraMotionScl, -cameraMotionScl) * xDamp;

		if(abs(eul.x) < 90) getCameraRef().tilt( tilt );
		getCameraRef().pan( pan );
		
		//roll it when we move left to right
		float roll = abs(pan) * pan * panScl;
		getCameraRef().roll( -roll );
		getCameraRef().move(0, abs(roll), 0);
		
		//move it
		ofVec3f vel = getCameraRef().getLookAtDir();
		getCameraRef().move( vel * camSpeed );
		
		//use this to determine where to explode fireworks
		float targetDistance = 300;
		camTarget = vel * targetDistance + getCameraRef().getPosition();
	}
	
	
	//emitters
	ofVec3f p0, nScl;
	float nx, ny, nz;
	for (int i=emitters.size()-1; i>=0; i--)
	{
		emitters[i].update( t );
		
		if( emitters[i].bStarted )
		{
			emitters[i].pos += gravity * emitters[i].span * emitters[i].age * 10;
			
			p0 = emitters[i].pos;
			nScl = p0 * .01;
			nx = ofSignedNoise(nScl.x + emitters[i].age, nScl.y, nScl.z);
			ny = ofSignedNoise(nScl.x, nScl.y + emitters[i].age, nScl.z);
			nz = ofSignedNoise(nScl.x, nScl.y, nScl.z + emitters[i].age);
			p0 += ofVec3f( nx, ny, nz ) * 10. * emitters[i].age;
			
			trailPoint( p0, emitters[i].vel, emissonRate, emitters[i].textureIndex );
		}
		
		if(emitters[i].bEnded)
		{
			emitters.erase( emitters.begin() + i );
		}
	}
	
	//particles
	bool updateIndices = false;
	
	indexCount = 0;
	for(int i=0; i<FIREWORKS_NUM_PARTICLES; i++){
		//if the age + lifespan is less then the current time we want to draw it. otherwise? it is dead to us.
		if(lifeData[i].r + lifeData[i].g / speed > t){
			indices[indexCount] = i;
			indexCount++;
			updateIndices = true;
		}
	}
	
	if(updateIndices){
		vbo.updateIndexData( indices, indexCount );
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
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
	//    mat->begin();
	
	ofPushStyle();
	
	ofEnableAlphaBlending();
	ofBlendMode( OF_BLENDMODE_ADD );
	ofEnablePointSprites();
	
	shader.begin();
	shader.setUniform1f( "time", ofGetElapsedTimef() );
	shader.setUniform1f( "nearClip", getCameraRef().getNearClip() );
	shader.setUniform1f( "farClip", getCameraRef().getFarClip() );
	shader.setUniform1f( "speed", speed);
	shader.setUniform1f( "particleSize", particleSize);
	
	shader.setUniform3f("cameraPosition", camPos.x, camPos.y, camPos.z );
	
	ofFloatColor c0 = startColor;
	c0.setSaturation( startColorSaturation );
	
	ofFloatColor c1 = endColor;
	c1.setSaturation( endColorSaturation );
	shader.setUniform4f("startColor", c0.r, c0.g, c0.b, c0.a );
	shader.setUniform4f("endColor", c1.r, c1.g, c1.b, c1.a );
	
	shader.setUniform3f( "gravity", gravity.x, gravity.y, gravity.z );
	
	shader.setUniformTexture("dotMap", dotImage.getTextureReference(), 4 );
	shader.setUniformTexture("triangleMap", triangleImage.getTextureReference(), 3 );
	shader.setUniformTexture("squareMap", squareImage.getTextureReference(), 2 );
	shader.setUniformTexture("circleMap", circleImage.getTextureReference(), 1 );
	
	shader.setUniform1f( "useCircleMap", bUseCircle );
	shader.setUniform1f( "useSquareMap", bUseSquare );
	shader.setUniform1f( "useTriangleMap", bUseTriangle );
	shader.setUniform1f( "useDotMap", bUseDot );
	
	ofVec3f camPos = getCameraRef().getPosition();
	shader.setUniform3f("camearPosition", camPos.x, camPos.y, camPos.z);
	shader.setUniform1f("fogDistance", fogDistance);
	shader.setUniform1f("fogAttenuation", fogAttenuation);
	ofFloatColor fc = fogColor;
	fc.setSaturation(fogSaturation);
	shader.setUniform4f("fogColor", fc.r, fc.g, fc.b, fc.a );
	
	vbo.drawElements( GL_POINTS, indexCount );
	
	shader.end();
	
	ofDisablePointSprites();
}


void CloudsVisualSystemFireworks::updateVbo()
{
	vbo.updateVertexData( &positions[0], FIREWORKS_NUM_PARTICLES );
	vbo.updateNormalData( &velocities[0], FIREWORKS_NUM_PARTICLES );
	vbo.updateColorData( &lifeData[0], FIREWORKS_NUM_PARTICLES );
	
	vbo.updateIndexData( &indices[0], FIREWORKS_NUM_PARTICLES );
	
	bUpdateVbo = false;
}

void CloudsVisualSystemFireworks::trailPoint( ofVec3f point, ofVec3f vel, int count, float texIndex )
{
	for(int i=0; i<count; i++){
		emitFromPoint( point, vel, ofRandom(minLifeSpan, maxLifeSpan), ofGetElapsedTimef(), texIndex );
	}
}

void CloudsVisualSystemFireworks::emitFromPoint( ofVec3f point, ofVec3f dir, float lifespan, float t, float texIndex )
{	
	ofQuaternion rotQuat;
	rotQuat.makeRotate( ofVec3f(0,1,0), dir);
	
	positions[nextIndex] = point;
	velocities[nextIndex] = (ofVec3f(ofRandom( minVel, maxVel ),ofRandom( minVel, maxVel ),ofRandom( minVel, maxVel )) * particleSpread) * rotQuat;
	lifeData[nextIndex].set( t, lifespan, texIndex, 0 );
	
	bUpdateVbo = true;
	
	nextIndex ++;
	if( nextIndex >= FIREWORKS_NUM_PARTICLES ) nextIndex = 0;
}

void CloudsVisualSystemFireworks::explodeFireWork( ofVec3f origin, ofVec3f vel )
{
	fireWorkExplosionTime = ofGetElapsedTimef();
	
	int numEmittersPerExplosion = 30;
	float t = ofGetElapsedTimef();
	for(int i=0; i<numEmittersPerExplosion; i++){
		FireworkEmitter e;
		e.setup(t,
				ofRandom(1.5, 3.),
				origin,
				origin + ofVec3f( ofRandom(-maxFWVel,maxFWVel), ofRandom(-maxFWVel,maxFWVel), ofRandom(-maxFWVel,maxFWVel) ),
				getRandomTextureIndex() );
		emitters.push_back( e );
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
	nextFireworkExplosionTime = t + ofRandom( minExplosionTime, maxExplosionTime ) * explosionFrequencey;
	ofVec3f offset = point;
	explodeFireWork( camTarget + offset );
}

void CloudsVisualSystemFireworks::explodeFireWorkAtRandom()
{
	float t = ofGetElapsedTimef();
	nextFireworkExplosionTime = t + ofRandom( minExplosionTime, maxExplosionTime ) * explosionFrequencey;
	
	ofVec3f offset( ofRandom(-1, 1), ofRandom(-.75,.75), ofRandom(-1.5, .5));
	offset.normalize();
	offset *= spawnDistance;
	
	ofVec3f rocketStart = offset + ofVec3f( ofRandom(-400, 400), 2000, 0);
	
	offset = offset * getCameraRef().getOrientationQuat();
	rocketStart = rocketStart;
	
	fireWorkExplosionTime = ofGetElapsedTimef();
	
	
	//get an active firework type and explode it
	int randFWType;
	vector<int> fwIndex;
	
	if(bDodecagedron) fwIndex.push_back( 0 );
	if(bBurst) fwIndex.push_back( 1 );
	if(bOctahedron) fwIndex.push_back( 2 );
	if(bTetrahedron) fwIndex.push_back( 3 );
	
	if(fwIndex.size() == 0)	randFWType = 4;
	else randFWType = fwIndex[ min( int(fwIndex.size())-1, (int) ofRandom(0, fwIndex.size() ) ) ];

	switch (randFWType) {
		case 0:
			explodeGeometry( dodecagedronPoints, camTarget + offset, camTarget + rocketStart );
			break;
			
		case 1:
			explodeFireWork( camTarget + offset );
			break;
			
		case 2:
			explodeGeometry( octahedronPoints, camTarget + offset, camTarget + rocketStart );
			break;
			
		case 3:
			explodeGeometry( tetrahedronPoints, camTarget + offset, camTarget + rocketStart );
			break;
			
		default:
			explodeFireWork( camTarget + offset );
			break;
	}
}

void CloudsVisualSystemFireworks::explodeGeometry( vector<ofVec3f>& vertices, ofVec3f offset, ofVec3f rocketStart )
{
	ofVec3f origin = offset;
	float rad = 10 + ofRandom(-3,3);
	
	ofQuaternion q;
	q.makeRotate(33, ofRandom(-10,10), ofRandom(-10,10), ofRandom(-10,10));
	ofVec3f p, p1, rPos;
	
	float t = ofGetElapsedTimef();
	for(int i=0; i<vertices.size(); i+=2){
		
		p = (vertices[i] * rad ) * q + origin;
		p1 = (vertices[i+1] * rad ) * q + origin;
		
		FireworkEmitter e0, e1;
		
		float texIndex = getRandomTextureIndex();
		
		e0.setup( t, ofRandom(1., 2.), p, p1, texIndex );
		e1.setup( t, ofRandom(1., 2.), p1, p, texIndex );
		
		emitters.push_back( e0 );
		emitters.push_back( e1 );
	}
}

// draw any debug stuff here
void CloudsVisualSystemFireworks::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemFireworks::selfDrawBackground()
{
//	glEnable(GL_DEPTH_TEST);
//    glEnable(GL_NORMALIZE);
//	//    mat->begin();
//	
//	ofPushStyle();
//	
//	ofEnableAlphaBlending();
//	ofBlendMode( OF_BLENDMODE_ADD );
//	ofEnablePointSprites();
//	
//	glowFbo0.begin();
//	ofClear(0,0,0,255);
//	
//	
//	shader.begin();
//	shader.setUniform1f( "time", ofGetElapsedTimef() );
//	shader.setUniform1f( "nearClip", camera.getNearClip() );
//	shader.setUniform1f( "farClip", camera.getFarClip() );
//	shader.setUniform3f("cameraPosition", camPos.x, camPos.y, camPos.z );
//	shader.setUniform4f("startColor", startColor.x, startColor.y, startColor.z, startColor.w );
//	shader.setUniform4f("endColor", endColor.x, endColor.y, endColor.z, endColor.w );
//	
//	shader.setUniform3f( "gravity", particleGravity.x, particleGravity.y, particleGravity.z );
//	
//	shader.setUniformTexture("triangleMap", triangleImage.getTextureReference(), 2 );
//	shader.setUniformTexture("squareMap", squareImage.getTextureReference(), 1 );
//	shader.setUniformTexture("circleMap", circleImage.getTextureReference(), 0 );
//	
//	vbo.drawElements( GL_POINTS, numSprites );
//	
//	shader.end();
//	
//	glowFbo0.end();
//	
//	ofPopStyle();
//	
//	ofSetColor(255);
//	int alpha = 255;
//	
//	glowFbo1.begin();
//	ofClear(0,0,0,alpha);
//	glowFbo0.draw(0, 0, glowFbo1.getWidth(), glowFbo1.getHeight() );
//	glowFbo1.end();
//	
//	glowFbo2.begin();
//	ofClear(0,0,0, alpha);
//	glowFbo1.draw(0, 0, glowFbo2.getWidth(), glowFbo2.getHeight() );
//	glowFbo2.end();
//	
//	glowFbo3.begin();
//	ofClear(0,0,0, alpha);
//	glowFbo2.draw(0, 0, glowFbo3.getWidth(), glowFbo3.getHeight() );
//	glowFbo3.end();
//	
//	glowFbo4.begin();
//	ofClear(0,0,0, alpha);
//	glowFbo3.draw(0, 0, glowFbo4.getWidth(), glowFbo4.getHeight() );
//	glowFbo4.end();
//	
//	glowFbo5.begin();
//	ofClear(0,0,0, alpha);
//	glowFbo4.draw(0, 0, glowFbo5.getWidth(), glowFbo5.getHeight() );
//	glowFbo5.end();
//	
//	glowFbo6.begin();
//	ofClear(0,0,0, alpha);
//	glowFbo5.draw(0, 0, glowFbo6.getWidth(), glowFbo6.getHeight() );
//	glowFbo6.end();
//	
//	glowShader.begin();
//	glowShader.setUniformTexture( "fbo", glowFbo0.getTextureReference(), 0);
//	
//	glowShader.setUniformTexture( "mm1", glowFbo1.getTextureReference(), 1);
//	glowShader.setUniformTexture( "mm2", glowFbo2.getTextureReference(), 2);
//	glowShader.setUniformTexture( "mm3", glowFbo3.getTextureReference(), 4);
//	glowShader.setUniformTexture( "mm4", glowFbo4.getTextureReference(), 5);
//	glowShader.setUniformTexture( "mm5", glowFbo5.getTextureReference(), 6);
//	glowShader.setUniformTexture( "mm6", glowFbo6.getTextureReference(), 7);
//	glowFbo0.draw(0, 0, ofGetWidth(), ofGetHeight() );
//	
//	glowShader.end();
//	
//	ofDisablePointSprites();
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
	dotImage.clear();
	
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
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemFireworks::selfKeyPressed(ofKeyEventArgs & args){
	if (args.key == 'l') {
		shader.unload();
		
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

float CloudsVisualSystemFireworks::getRandomTextureIndex()
{
	vector<float> mapIndex;
	if(bUseTriangle) mapIndex.push_back(0);
	if(bUseCircle) mapIndex.push_back(1);
	if(bUseSquare) mapIndex.push_back(2);
	if(bUseDot) mapIndex.push_back(3);
	
	if(mapIndex.size() == 0) return 3;
	
	return mapIndex[ min( int(mapIndex.size())-1, (int) ofRandom(0, mapIndex.size() ) ) ];
}