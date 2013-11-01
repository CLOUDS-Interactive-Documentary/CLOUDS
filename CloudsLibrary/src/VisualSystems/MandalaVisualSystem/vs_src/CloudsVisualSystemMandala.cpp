//
//  CloudsVisualSystemMandala.cpp
//

#include "CloudsVisualSystemMandala.h"
#include "CloudsRGBDVideoPlayer.h"
#include <stdlib.h>     /* atoi */

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
	shapeNames.push_back("square");
	shapeNames.push_back("triangle");
	shapeNames.push_back("circle");
	
	shapesGui->addLabel("shape types");
	shapesGui->addSpacer();
	shapesGui->addRadio("shapeTypes", shapeNames );
	shapesGui->addSpacer();
	shapesGui->addToggle("randomColors", &bUseRandomColors );
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
	surfaceGui->addToggle("bDrawSurface", &bDrawSurface );
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
	
	surfaceGui->addSpacer();
	surfaceGui->addLabel("animation profile");
	
	profileTypeNames.push_back("half circle");
	profileTypeNames.push_back("wave");
	profileTypeNames.push_back("half circle & wave");
	surfaceGui->addRadio("profileTypes", profileTypeNames );
	
	
	
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
		setupNodules( *currentShape, numW, numH );
	}
	
	else if(kind == OFX_UI_WIDGET_TOGGLE && e.getToggle()->getValue() )
	{
//		cout << e.getToggle()->getParent()->getName() << endl;
		
		if(name == "square")	setupNodules( square, numW, numH );
		else if(name == "triangle")	setupNodules( triangle, numW, numH );
		else if(name == "circle")	setupNodules( circle, numW, numH );
		
		else if(name == "shapes fbo")	currentTexture = &animatedMap.getTextureReference();
		else if(name == "space balls")	currentTexture = &debugImage.getTextureReference();
		
		else if(e.getToggle()->getParent()->getName() == "profileTypes")
		{
			for (int i=0; i<profileTypeNames.size(); i++) {
				if(profileTypeNames[i] == name)	profileType = name;
			}
		}
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

void CloudsVisualSystemMandala::selfSetupSystemGui(){
	
}

void CloudsVisualSystemMandala::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemMandala::selfSetupRenderGui(){

}

void CloudsVisualSystemMandala::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemMandala::selfSetup()
{
	//set defaults
	controlSplineOffset = .25;
	controlSplinesLineWidth = 5;
	noduleLineWidth = 4;
	bDrawFill = true;
	bDrawEdges = false;
	bDrawPoints = false;
	bDrawSurface = false;
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
	bUseRandomColors = false;
	
	profileType = "half circle";
	
	//load some images for gui and debug
	colorMap.loadImage( getVisualSystemDataPath() + "GUI/defaultColorPalette.png" );
	debugImage.loadImage( getVisualSystemDataPath() + "images/spaceBalls.png" );
	
	loadShaders();
	
	//load some simple geometry for our surface animations
	ofxObjLoader::load( getVisualSystemDataPath() + "models/square.obj", square, false );
	ofxObjLoader::load( getVisualSystemDataPath() + "models/circle.obj", circle, false );
	ofxObjLoader::load( getVisualSystemDataPath() + "models/triangle.obj", triangle, false );
	ofxObjLoader::load( getVisualSystemDataPath() + "models/box.obj", box, false );
	ofxObjLoader::load( getVisualSystemDataPath() + "models/dodecahedron.obj", dodecahedron, false );
	
	meshes.push_back( &square );
	meshes.push_back( &circle );
	meshes.push_back( &triangle );
	meshes.push_back( &box );
	meshes.push_back( &dodecahedron );
	
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
	
	animatedMap.allocate( 2048, 1024);
//	int fboWidth = ofGetWidth() * 2;
//	animatedMap.allocate(fboWidth, fboWidth / 3);
		
	currentTexture = &animatedMap.getTextureReference();
	
	//shapes content
	setupNodules( triangle, numW, numH );
	
	
	posTicker.begin(tickDebug, ofVec3f(-1,-1,-1), ofVec3f(1,1,1), 3);
	
//	surfaceBoxUV.set(0., .5);
//	uvTicker.begin( surfaceBoxUV, ofVec2f(.025,0.5), .25 );
//	uvTicker.setContinue( true );
	
	step = .1;
	float stepJ = .05;
	for (int i=0; i<10; i++)
	{
		for (int j=0; j<20; j++)
		{
			MandalaSurfaceShape* shape = addSurfaceShape( &surface, &box );
			shape->setScale( 30, 60, 60 );
			
			MandalaTicker<ofVec2f> uv;
			uv.setDelay( step );
//			uv.begin( shape->uv, ofVec2f( step* i, stepJ*j), ofVec2f( step*(i+1), stepJ*j ), 1 );
			uv.begin( shape->uv, ofVec2f( stepJ*j, step* i), ofVec2f( stepJ*j, step*(i+1)), 1 );
			uv.setContinue( true );
			addTicker( uv );
			
			MandalaTicker<float> offset;
			uv.setDelay( step*i);
			offset.begin(shape->offset, 0, 100, .5, step * i + stepJ*j);
			offset.setReverse( true );
			addTicker( offset );
			
		}
	}
}

MandalaSurfaceShape* CloudsVisualSystemMandala::addSurfaceShape( ofxSimpleSurface* s, ofVboMesh* m )
{
	MandalaSurfaceShape* shape = new MandalaSurfaceShape(s, m);
	surfaceShapes.push_back( shape );
	return shape;
}

void CloudsVisualSystemMandala::setupNodules( ofVboMesh& m, int numW, int numH )
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
			if(bUseRandomColors)	nodules[index].offsetColor.set(ofRandom(2.), ofRandom(2.), ofRandom(2.) );
			else	nodules[index].offsetColor.set(1,1,1);
		}
	}
}

void CloudsVisualSystemMandala::selfPresetLoaded(string presetPath){
	
}

void CloudsVisualSystemMandala::selfBegin()
{
}

void CloudsVisualSystemMandala::selfSceneTransformation(){
	
}

void CloudsVisualSystemMandala::updateTickers( float t )
{
	//float
	for (int i=tickersf.size()-1; i>=0; i--)
	{
		tickersf[i].update(t);
		if(tickersf[i].ended())
		{
			//romve the dead ones
			tickersf.erase( tickersf.begin() + i );
		}
		else if(tickersf[i].endTrigger)
		{
			//something on loop/continue start...
		}
	}
	//2f
	for (int i=tickers2f.size()-1; i>=0; i--)
	{
		tickers2f[i].update(t);
		if(tickers2f[i].ended())
		{
			//romve the dead ones
			tickers2f.erase( tickers2f.begin() + i );
		}
		else if(tickers2f[i].endTrigger)
		{
			//something on loop/continue start...
		}
	}
	
	//3f
	for (int i=tickers3f.size()-1; i>=0; i--)
	{
		tickers3f[i].update(t);
		if(tickers3f[i].ended())
		{
			//romve the dead ones
			tickers3f.erase( tickers3f.begin() + i );
		}
		else if(tickers3f[i].endTrigger)
		{
			//something on loop/continue start...
		}
	}
	
	//4f
	for (int i=tickers4f.size()-1; i>=0; i--)
	{
		tickers4f[i].update(t);
		if(tickers4f[i].ended())
		{
			//romve the dead ones
			tickers4f.erase( tickers4f.begin() + i );
		}
		else if(tickers4f[i].endTrigger)
		{
			//something on loop/continue start...
		}
	}
}


void CloudsVisualSystemMandala::selfUpdate()
{
	float t = ofGetElapsedTimef();

	updateTickers( t );
	
	//move the control vertices around
	
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
	ofVec3f offsetV(0,0,controlSplineOffset);
	for (int i=0; i<numSpans; i++)
	{
		sweeper.rotate(-iStep, ofVec3f(0,1,0));
		
		for (int j=0; j<profileVertices.size(); j++)
		{
			if(profileType == "half circle")	cv[i][j] = (profileVertices[j] + offsetV) * sweeper.getGlobalTransformMatrix();
			else if(profileType == "wave")	cv[i][j] = (waveCV[j] + offsetV) * sweeper.getGlobalTransformMatrix();
			else{
				curveMix = ofMap( curveMixStep*j, .25, .35, 0, 1, true);
				curveMix *= curveMix;
				cv[i][j] = ((curveMix * waveCV[j] + (1. - curveMix) + profileVertices[j]) + offsetV) * sweeper.getGlobalTransformMatrix();
			}
			
			if(noiseOffset>.1)
			{
				nx = (cv[i][j].x + noiseTime) * noiseScale;
				//ny = cv[i][j].y * noiseScale;
				nz = cv[i][j].z * noiseScale;
				
				cv[i][j].y += ofSignedNoise(nx,nz) * noiseOffset;
			}
		}
	}
	
	//update the surface geometry
	if(bDrawSurface)	surface.update();
	
	//TODO: delete this:
	ofSetWindowTitle( ofToString(ofGetFrameRate()) );
	
}

void CloudsVisualSystemMandala::selfDraw()
{
	float t = ofGetElapsedTimef();
	
	//draw shapes to fbo
	if(bDrawSurface)	drawShapesToFbo(t);
	
	
	if(bDrawSurface)
	{
		//draw the 3D scene
		glEnable( GL_DEPTH_TEST );
		ofSetColor(255,255,255);
		
		ofEnableBlendMode(surfaceBlendMode);
		
		drawSurface( surface );
	}
	
	ofVec3f sPos, sNorm;
	surface.getMeshPositionAndNormal( sPos, sNorm, surfaceBoxUV.x, surfaceBoxUV.y);
	
	surfaceBox.resetTransform();
	surfaceBox.setPosition( sPos );
	surfaceBox.setScale(1,1,2);
	ofQuaternion q;
	q.makeRotate( surfaceBox.getZAxis(), sNorm );
	surfaceBox.setOrientation( q );
	

	glEnable(GL_DEPTH_TEST);
	ofSetColor(255);
//	surfaceBox.transformGL();
//	ofBox(0,0,0, 30);
//	surfaceBox.restoreTransformGL();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	facingRatio.begin();
	for (int i=0; i<surfaceShapes.size(); i++)
	{
		surfaceShapes[i]->draw();
	}
	facingRatio.end();
	
	glDisable(GL_CULL_FACE);
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
	float t = ofGetElapsedTimef();
	
	
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
		surfaceShader.begin();
		surfaceShader.setUniformTexture("map", *currentTexture, 0);
		surfaceShader.setUniform2f("mapDim", currentTexture->getWidth(), currentTexture->getHeight() );
		surfaceShader.setUniform1f("polarAlphaExpo", polarAlphaExpo);
		surfaceShader.setUniform1f("polarAlphaExpoScale", polarAlphaExpoScale);
		
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
//		glEnable( GL_DEPTH_TEST );
		surface.draw();
		
		glDisable(GL_CULL_FACE);
		
		
		
		surfaceShader.end();
		
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

void CloudsVisualSystemMandala::selfDrawDebug(){
	
}

void CloudsVisualSystemMandala::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}

void CloudsVisualSystemMandala::selfEnd(){
}

void CloudsVisualSystemMandala::selfExit()
{
	for(int i=0;i<meshes.size();i++)
	{
		meshes[i]->clear();
	}
	
	for (int i=0; i<surfaceShapes.size(); i++)
	{
		delete surfaceShapes[i];
	}
	surfaceShapes.clear();
}

void CloudsVisualSystemMandala::selfKeyPressed(ofKeyEventArgs & args)
{
	if(args.key == 'L' || args.key == 'l'){
//		shader.load( getVisualSystemDataPath() + "shaders/surfaceShader" );
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
	surfaceShader.load( getVisualSystemDataPath() + "shaders/surfaceShader");
	facingRatio.load( getVisualSystemDataPath() + "shaders/facingRatio");
	noduleShader.load( getVisualSystemDataPath() + "shaders/noduleShader" );
}


void CloudsVisualSystemMandala::addTicker( MandalaTicker<float> ticker ){
	tickersf.push_back( ticker );
}
void CloudsVisualSystemMandala::addTicker( MandalaTicker<ofVec2f> ticker ){
	tickers2f.push_back( ticker );
}
void CloudsVisualSystemMandala::addTicker( MandalaTicker<ofVec3f> ticker ){
	tickers3f.push_back( ticker );
}
void CloudsVisualSystemMandala::addTicker( MandalaTicker<ofVec4f> ticker ){
	tickers4f.push_back( ticker );
}