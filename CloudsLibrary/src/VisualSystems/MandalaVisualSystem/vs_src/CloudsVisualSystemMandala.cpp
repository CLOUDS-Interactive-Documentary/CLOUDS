//
//  CloudsVisualSystemMandala.cpp
//

#include "CloudsVisualSystemMandala.h"
#include "CloudsRGBDVideoPlayer.h"
#include <stdlib.h>     /* atoi */

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemMandala::selfSetupGui()
{
	vector<string> blendTypes;
	blendTypes.push_back("OF_BLENDMODE_ADD");
	blendTypes.push_back("OF_BLENDMODE_ALPHA");
	blendTypes.push_back("OF_BLENDMODE_DISABLED");
	blendTypes.push_back("OF_BLENDMODE_MULTIPLY");
	blendTypes.push_back("OF_BLENDMODE_SCREEN");
	blendTypes.push_back("OF_BLENDMODE_SUBTRACT");
	
	
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->addFPS();
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemMandala::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
	
	//shapes gui
	shapesGui = new ofxUISuperCanvas("SHAPES", gui);
	shapesGui->copyCanvasStyle(gui);
	shapesGui->copyCanvasProperties(gui);
	shapesGui->setName("SHAPES");
	shapesGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	shapesGui->setColorFill( ofColor( 30,30,35,250) );
	
	vector<string> shapeNames;
	shapeNames.push_back("square"), shapeNames.push_back("triangle"), shapeNames.push_back("circle");
	shapesGui->addLabel("shape types");
	shapesGui->addSpacer();
	shapesGui->addRadio("shapeTypes", shapeNames );
	shapesGui->addSpacer();
	shapesGui->addToggle("randomColors", false );
	shapesGui->addSlider("shapesScale", .1, 10, &shapesScale);
	shapesGui->addSlider("numW", 1, 20, &numW)->setIncrement(1);
	shapesGui->addSlider("numH", 1, 10, &numH)->setIncrement(1);
	
	shapesGui->addSpacer();
	shapesGui->addSlider("cutoff", 0., 1., &cutoff);
	shapesGui->addSlider("alphaSmoothScale", .1, 5., &alphaSmoothScale);
	shapesGui->addSlider("alphaSmoothExpo", 1., 10., &alphaSmoothExpo);
	
	shapesGui->addSpacer();
	shapesGui->addSlider("shapeRotation", 0, 30, &noduleRotation);
	
	shapesGui->addSpacer();
	shapesGui->addSlider("blinkyness",0, 10, &blinkyness);
	shapesGui->addSlider("blinkynessTimeScale", .01, .2, &blinkynessTimeScale);
	shapesGui->addSlider("blinkynessScale", .001, .1, &blinkynessScale);
	
	ofAddListener(shapesGui->newGUIEvent, this, &CloudsVisualSystemMandala::selfGuiEvent);
	guis.push_back(shapesGui);
	guimap[customGui->getName()] = shapesGui;
	
	
	//surface gui
	surfaceGui = new ofxUISuperCanvas("SURFACE", gui);
	surfaceGui->copyCanvasStyle(gui);
	surfaceGui->copyCanvasProperties(gui);
	surfaceGui->setName("SURFACE");
	surfaceGui->addFPS();
	surfaceGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	surfaceGui->setColorFill( ofColor(30,30,40, 255) );
	
	surfaceGui->addSpacer();
	surfaceGui->addToggle("smoothSurface", &bSmoothSurface );
	
	surfaceGui->addSpacer();
	surfaceGui->addSlider( "noiseOffset", 0, 1000, &noiseOffset);
	surfaceGui->addSlider( "noiseScale", .001, .2, &noiseScale);
	surfaceGui->addSlider( "noiseTimeScale", -100, 100, &noiseTimeScale)->setIncrement(.1);
	
	surfaceGui->addSpacer();
	vector<string> textureNames;
	textureNames.push_back("shapes fbo");
	textureNames.push_back("space balls");
	surfaceGui->addRadio("textures", textureNames);
	
	ofAddListener(surfaceGui->newGUIEvent, this, &CloudsVisualSystemMandala::selfGuiEvent);
	guis.push_back(surfaceGui);
	guimap[customGui->getName()] = surfaceGui;
	
}

void CloudsVisualSystemMandala::selfGuiEvent(ofxUIEventArgs &e)
{
	string name = e.getName();
	int kind = e.getKind();
	
	if(name == "smoothSurface")
	{
		surface.setFaceted( !bSmoothSurface );
	}
	else if(name == "randomColors")
	{
		cout << "e.getToggle()->getValue(); " << e.getToggle()->getValue() << endl;
		setupNodules( *currentShape, numW, numH, e.getToggle()->getValue() );
	}
	
	else if(kind == OFX_UI_WIDGET_TOGGLE && e.getToggle()->getValue() )
	{
		if(name == "square")	setupNodules( square, numW, numH );
		else if(name == "triangle")	setupNodules( triangle, numW, numH );
		else if(name == "circle")	setupNodules( circle, numW, numH );
		
		else if(name == "shapes fbo")	currentTexture = &animatedMap.getTextureReference();
		else if(name == "space balls")	currentTexture = &debugImage.getTextureReference();
	}
	
	else{
		if (name == "shapesScale")
		{
			for (int i=0; i<nodules.size(); i++)
			{
				nodules[i].setScale(noduleSize.y * shapesScale);
			}
		}
		else if(name == "numH" || name == "numW")
		{
			setupNodules(*currentShape, numW, numH);
		}
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemMandala::selfSetupSystemGui(){
	
}

void CloudsVisualSystemMandala::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemMandala::selfSetupRenderGui(){

}

void CloudsVisualSystemMandala::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemMandala::selfSetup()
{
	//set defaults
	controlSplinesLineWidth = 5;
	noduleLineWidth = 4;
	bDrawFill = true;
	bDrawEdges = false;
	bDrawPoints = false;
	bDrawSurfaceWirframe = false;
	bDrawSurfaceSplines = false;
	
	bMirrorSurface = true;
	
	polarAlphaExpo = 10;
	polarAlphaExpoScale = 2.;
	
	cutoff = .75;
	shapeWidth = .5;
	alphaSmoothScale = 1.;
	alphaSmoothExpo = 2.;
	
	noduleBlendType = OF_BLENDMODE_ALPHA;
	surfaceBlendMode = OF_BLENDMODE_ALPHA;
	bSmoothSurface = true;
	
	blendCurve0 = NULL;
	blendCurve1 = NULL;
	waveScale = 5.;
	
	noiseTimeScale = .1;
	noiseScale = .01;
	noiseOffset = 1;
	
	blinkyness = 0.;
	blinkynessTimeScale = .1;
	blinkynessScale = .01;
	
	noduleRotation = 2.;
	numW = 15;
	numH = 6;
	currentShape = &triangle;
	
	currentTexture = NULL;
	
	//load some images for gui and debug
	colorMap.loadImage( getVisualSystemDataPath() + "GUI/defaultColorPalette.png" );
	debugImage.loadImage( getVisualSystemDataPath() + "images/spaceBalls.png" );
	
	loadShaders();
	
	//load some simple geometry for our surface animations
	ofxObjLoader::load( getVisualSystemDataPath() + "models/square.obj", square );
	ofxObjLoader::load( getVisualSystemDataPath() + "models/circle.obj", circle );
	ofxObjLoader::load( getVisualSystemDataPath() + "models/triangle.obj", triangle );
	
	meshes.push_back( &square );
	meshes.push_back( &circle );
	meshes.push_back( &triangle );
	
	//create some control vertices
	profileVertices.resize(15);
	float step = PI / float(profileVertices.size()-1);
	for (int i=0; i<profileVertices.size(); i++)
	{
		profileVertices[i].x = sin(step * i * .999 +.0005);
		profileVertices[i].y = cos(step * i * .999 +.0005);
	}
	
	sweeper.setScale( 400 );
	numSpans = 20;
	
	//create some animated curves for blending into out profile
	vector<ofVec3f> waveCV(profileVertices.size());
	step = 1. / (waveCV.size() - 1);
	for (int i=0; i<waveCV.size(); i++)
	{
		waveCV[i].set( step * i, .125 * sin( 4. * TWO_PI * step * i), 0 );
	}
	
	waveSpline.addControlVertices( waveCV );
	
	
	
	//create our surface
	float iStep = 360. / float(numSpans-1);
	cv.resize( numSpans );
	sweeper.setOrientation(ofVec3f(0,0,0));
	for( int i=0; i<numSpans; i++)
	{
		cv[i].resize(profileVertices.size());
		
		for(int j=0; j<profileVertices.size(); j++)
		{
			//sweep our profile curve 
			cv[i][j] = profileVertices[j] * sweeper.getGlobalTransformMatrix();
		}
		
		sweeper.rotate(-iStep, ofVec3f(0,1,0));
	}
	
	//setup our surface
	surface.setControlVertices( cv );
	surface.setup(3,3);
	surface.setClosed( true, false );
	
	int fboWidth = ofGetWidth() * 2;
	animatedMap.allocate(fboWidth, fboWidth / 3);
		
	currentTexture = &animatedMap.getTextureReference();
	
	//shapes content
	setupNodules( triangle, numW, numH );
}

void CloudsVisualSystemMandala::setupNodules( ofVboMesh& m, int numW, int numH, bool useRandomColors )
{
	currentShape = &m;
	nodules.resize( numW * numH);
	noduleSize.set( float(animatedMap.getWidth()) / numW, float(animatedMap.getHeight()) / numH );
	for (int i=0; i<numW; i++)
	{
		for (int j=0; j<numH; j++)
		{
			int index = i*numH + j;
			nodules[index].setLoopBoundry(0,0,animatedMap.getWidth(), animatedMap.getHeight());
			nodules[index].setMesh( m );
			nodules[index].setPosition( i * noduleSize.x + noduleSize.x * .5 *(j%2), j * noduleSize.y+ noduleSize.x * .5 , 0);
			nodules[index].setScale( noduleSize.y * shapesScale );
			nodules[index].setOrientation(ofVec3f(0,0,0));
			if(useRandomColors == true)	nodules[index].offsetColor.set(ofRandom(2.), ofRandom(2.), ofRandom(2.) );
			else	nodules[index].offsetColor.set(1,1,1);
		}
	}
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemMandala::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemMandala::selfBegin()
{
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemMandala::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemMandala::selfUpdate()
{
	//move the control vertices around
	float t = ofGetElapsedTimef();
	float sweepScl = sin(t) * .25 + .75;
	float iStep = 360 / float(numSpans-1);
	
	sweeper.setOrientation(ofVec3f(0,0,0));
	
	vector<ofVec3f>& waveCV = waveSpline.getControlVertices();
	float step = 1. / (waveCV.size() - 1);
	for (int i=0; i<waveCV.size(); i++)
	{
		waveCV[i].set( step * i * waveScale, .125 * sin( 5. * TWO_PI * step * i - t ), 0 );
	}
	
	float curveMixStep = 1. / float(profileVertices.size()-1), curveMix;
	
	noiseTime += noiseTimeScale / max(1.f,ofGetFrameRate());
	float nx, ny, nz;
	for (int i=0; i<numSpans; i++)
	{
		sweeper.rotate(-iStep, ofVec3f(0,1,0));
		
		for (int j=0; j<profileVertices.size(); j++)
		{
			curveMix = ofMap( curveMixStep*j, .25, .35, 0, 1, true);
			curveMix *= curveMix;
			//cv[i][j] = profileVertices[j] * sweeper.getGlobalTransformMatrix();
//			cv[i][j] = waveCV[j] * sweeper.getGlobalTransformMatrix();
			cv[i][j] = (curveMix * waveCV[j] + (1. - curveMix) * profileVertices[j]) * sweeper.getGlobalTransformMatrix();
			
			if(noiseOffset>.1)
			{
				nx = (cv[i][j].x + noiseTime) * noiseScale;
//				ny = cv[i][j].y * noiseScale;
				nz = cv[i][j].z * noiseScale;
				
				cv[i][j].y += ofSignedNoise(nx,nz) * noiseOffset;
			}
		}
	}
	
	//update the surface geometry
	surface.update();
	
	//TODO: delete this:
	ofSetWindowTitle( ofToString(ofGetFrameRate()) );
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemMandala::selfDraw()
{
	float t = ofGetElapsedTimef();
	
	//draw shapes to fbo
	drawShapesToFbo(t);
	
	
	//draw the 3D scene
	glEnable( GL_DEPTH_TEST );
	ofSetColor(255,255,255);
	camera.begin();
	
	ofEnableBlendMode(surfaceBlendMode);
	
	drawSurface( surface );
	
	camera.end();
}

void CloudsVisualSystemMandala::drawShapesToFbo( float t)
{
	//draw animated texture
	animatedMap.begin();
	ofClear(0,0,0,0);
	
	ofSetColor( 255 );
	
	//	orthoCamera.begin( ofRectangle(-animatedMap.getWidth(), -animatedMap.getHeight(), animatedMap.getWidth(), animatedMap.getHeight() ) );
	
	ofEnableBlendMode( noduleBlendType );
	
	glDisable( GL_DEPTH_TEST );
	glLineWidth( noduleLineWidth );
	
	//	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	//	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
	
	noduleShader.begin();
	noduleShader.setUniform1f("cutoff", cutoff);
	noduleShader.setUniform1f("alphaSmoothScale", alphaSmoothScale);
	noduleShader.setUniform1f("alphaSmoothExpo", alphaSmoothExpo);
	
	float blinkTime = blinkynessTimeScale * t;
	for (int i=0; i<nodules.size(); i++)
	{
		ofVec3f p = nodules[i].getPosition() * blinkynessScale;
		if( !int( abs(ofNoise(p.x, p.y+blinkTime) * blinkyness)) )
		{
			nodules[i].rotate( noduleRotation/ofGetFrameRate(), ofVec3f(0,0,1));
			//			nodules[i].setOrientation(ofVec3f(0,0,45));
			nodules[i].draw(bDrawFill, bDrawEdges, bDrawPoints);
		}
	}
	
	noduleShader.end();
	
	//	orthoCamera.end();
	animatedMap.end();
}


void CloudsVisualSystemMandala::drawSurface(ofxSimpleSurface& surface )
{
	ofPushMatrix();
	//ofScale(1,.5, 1);
	
	if(bDrawSurfaceSplines)
	{
		//draw surface splines
		ofSetColor(255,255,255,255);
		glLineWidth( controlSplinesLineWidth );
		glNormal3f(0, 0, -1);
		surface.drawSplines();
	}
	
	//draw our surface mesh
	
	if(currentTexture != NULL)
	{
		ofSetColor( 255 );
		facingRatio.begin();
		facingRatio.setUniformTexture("map", *currentTexture, 0);
		facingRatio.setUniform2f("mapDim", currentTexture->getWidth(), currentTexture->getHeight() );
//		facingRatio.setUniformTexture("map", animatedMap, 0);
//		facingRatio.setUniform2f("mapDim", animatedMap.getWidth(), animatedMap.getHeight() );
		facingRatio.setUniform1f("polarAlphaExpo", polarAlphaExpo);
		facingRatio.setUniform1f("polarAlphaExpoScale", polarAlphaExpoScale);
		
		//	glDisable( GL_DEPTH_TEST );
		////	glEnable(GL_CULL_FACE);
		////	glCullFace( GL_FRONT);
		//	surface.draw();
		//
		//
		//	glEnable( GL_DEPTH_TEST );
		//	glCullFace( GL_FRONT );
		//	surface.draw();
		
		
		ofEnableAlphaBlending();
		glDisable( GL_DEPTH_TEST );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		
		surface.draw();
		
		glDisable(GL_CULL_FACE);
		
		
		facingRatio.end();
		
	}
	
	if(bDrawSurfaceWirframe)
	{
		glLineWidth( 1 );
		glDisable(GL_DEPTH_TEST);
		ofSetColor(255);
		surface.getMesh().drawWireframe();
	}
	
	ofPopMatrix();
	
	
	ofDisableAlphaBlending();
}

// draw any debug stuff here
void CloudsVisualSystemMandala::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemMandala::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemMandala::selfEnd(){
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemMandala::selfExit()
{
	for(int i=0;i<meshes.size();i++)
	{
		meshes[i]->clear();
	}
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemMandala::selfKeyPressed(ofKeyEventArgs & args)
{
	if(args.key == 'L' || args.key == 'l'){
//		shader.load( getVisualSystemDataPath() + "shaders/facingRatio" );
		loadShaders();
	}
	
	if(args.key == 'F')
	{
		surface.setFaceted( !surface.getFaceted() );
	}
}
void CloudsVisualSystemMandala::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemMandala::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMandala::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMandala::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMandala::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMandala::loadShaders()
{
	normalShader.load( getVisualSystemDataPath() + "shaders/normalShader");
	facingRatio.load( getVisualSystemDataPath() + "shaders/facingRatio");
	noduleShader.load( getVisualSystemDataPath() + "shaders/noduleShader" );
}