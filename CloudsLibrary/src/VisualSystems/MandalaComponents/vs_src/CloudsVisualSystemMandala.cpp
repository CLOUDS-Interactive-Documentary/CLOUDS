//
//  CloudsVisualSystemMandala.cpp
//

#include "CloudsVisualSystemMandala.h"
#include "CloudsRGBDVideoPlayer.h"
#include <stdlib.h>     /* atoi */

int MandalaComponent::componentCount = 1;

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
	
	surfaceGui = new ofxUISuperCanvas("Surface", gui);
	surfaceGui->copyCanvasStyle(gui);
	surfaceGui->copyCanvasProperties(gui);
	surfaceGui->setName("Surface");
	surfaceGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	surfaceGui->addSpacer();
	surfaceGui->addToggle("draw surface", &bDrawSurface);
	surfaceGui->addToggle("smooth surface", &bSmoothSurface);
	surfaceGui->addSlider("noiseScale", .0001, .1, &noiseScale)->setIncrement(.0001);
	surfaceGui->addSlider("noiseOffset", 0, 100, &noiseOffset)->setIncrement(1);
	surfaceGui->addSlider("noiseTimeScale", .001, 2., &noiseTimeScale)->setIncrement(.001);
	
	ofAddListener(surfaceGui->newGUIEvent, this, &CloudsVisualSystemMandala::selfGuiEvent);
	guis.push_back(surfaceGui);
	guimap[surfaceGui->getName()] = surfaceGui;
	
	
	
	
	clockGui = new ofxUISuperCanvas("Clock", gui);
	clockGui->copyCanvasStyle(gui);
	clockGui->copyCanvasProperties(gui);
	clockGui->setName("Clock");
	clockGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	clockGui->addSpacer();
	
	clockGui->addToggle("draw clock", &bDrawClock);
	clockGui->addSlider("spread", .05, .75, &clockSpread)->setIncrement(.001);
	clockGui->addSlider("clockV", .1, .9, &clockV)->setIncrement(.001);
	clockGui->addSlider("numCogs", 3., 30., &clockNumCogs)->setIncrement(1);
	clockGui->addSlider("CogScale", 1., 200., &clockScale)->setIncrement(1);
	clockGui->addSlider("Octaves", 1., 20., &clockOctaves)->setIncrement(1);
	clockGui->addSlider("OctaveScale", 1, 5., &cloackOctaveScale)->setIncrement(.01);
	clockGui->addSlider("MaxSpeed", .001, .5, &clockMaxSpeed)->setIncrement(.001);
	
	ofAddListener(clockGui->newGUIEvent, this, &CloudsVisualSystemMandala::selfGuiEvent);
	guis.push_back(clockGui);
	guimap[clockGui->getName()] = clockGui;
	
	
	offsetMeshGui = new ofxUISuperCanvas("OffsetMesh", gui);
	offsetMeshGui->copyCanvasStyle(gui);
	offsetMeshGui->copyCanvasProperties(gui);
	offsetMeshGui->setName("OffsetMesh");
	offsetMeshGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	offsetMeshGui->addSpacer();
	
	offsetMeshGui->addToggle("draw offsetMesh", &bDrawOffsetMesh);
	
	modelNames.clear();;
	modelNames.push_back("diaSphere.txt");
	modelNames.push_back("hexSphere_mid.txt");
	modelNames.push_back("hexSphere.txt");
	modelNames.push_back("organicSphere.txt");
	
	offsetMeshGui->addDropDownList("models", modelNames);
	
	offsetMeshGui->addSpacer();
	offsetMeshGui->addImageSampler("c0", &colorMap, colorMap.getWidth()/2, colorMap.getHeight()/2);
	offsetMeshGui->addSlider("c0_alpha", 0, 1, &c0.a);
	offsetMeshGui->addImageSampler("c1", &colorMap, colorMap.getWidth()/2, colorMap.getHeight()/2);
	offsetMeshGui->addSlider("c1_alpha", 0, 1, &c1.a);
	offsetMeshGui->addSlider("colorMixScale", 0, 10, &colorMixScale);
	offsetMeshGui->addSpacer();
	offsetMeshGui->addSlider("faceScale", .0, 1., &faceScale)->setIncrement(.001);
	offsetMeshGui->addSlider("faceOffset", 0, 2, &faceOffset)->setIncrement(.001);
	offsetMeshGui->addSlider("noiseOctaveScale", .01, 10., &noiseOctaveScale)->setIncrement(.01);
	offsetMeshGui->addSlider("noiseExpo", .51, 10., &noiseExpo)->setIncrement(.01);
	offsetMeshGui->addSlider("noiseExpoScale", .8, 1.2, &noiseExpoScale)->setIncrement(.01);
	offsetMeshGui->addSlider("meshNoiseScale", .0001, .1, &meshNoiseScale)->setIncrement(.0001);
	
	
	ofAddListener(offsetMeshGui->newGUIEvent, this, &CloudsVisualSystemMandala::selfGuiEvent);
	guis.push_back(offsetMeshGui);
	guimap[offsetMeshGui->getName()] = offsetMeshGui;
	
}

void CloudsVisualSystemMandala::selfGuiEvent(ofxUIEventArgs &e)
{
	string name = e.getName();
	string parent = e.widget->getParent()->getName();
	
	if( name == "draw boxClock")
	{
		for(int i=0; i<boxClockComponents.size(); i++)
		{
			boxClockComponents[i]->bDraw = e.getToggle()->getValue();
		}
	}
	
	else if(name == "c0")
	{
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		ofFloatColor col =  sampler->getColor();
		c0.r = col.r;
		c0.g = col.g;
		c0.b = col.b;
	}
	else if(name == "c1")
	{
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		ofFloatColor col =  sampler->getColor();
		c1.r = col.r;
		c1.g = col.g;
		c1.b = col.b;
	}
	
	else if(name == "smooth surface")
	{
		sphereSurface.setFaceted( !e.getToggle()->getValue() );
	}
	
	else if( name == "numCogs" || name == "CogScale" || name == "Octaves" || name == "OctaveScale" || name == "MaxSpeed" || name == "clockV" || name == "spread" )
	{
		setClock( clockNumCogs, clockScale, clockOctaves, cloackOctaveScale, clockMaxSpeed );
	}
	
	else  if(parent == "models")
	{
		if(e.getToggle()->getValue())
		{
			for (int i=0; i<modelNames.size(); i++)
			{
				loadHexMesh( getVisualSystemDataPath() + "models/" + name );
			}
		}
	}
}

void CloudsVisualSystemMandala::selfSetupSystemGui()
{
	
}

void CloudsVisualSystemMandala::guiSystemEvent(ofxUIEventArgs &e)
{
	
}

void CloudsVisualSystemMandala::selfSetupRenderGui()
{

}

void CloudsVisualSystemMandala::guiRenderEvent(ofxUIEventArgs &e)
{
	
}

void CloudsVisualSystemMandala::selfSetup()
{
	//set defaults
	//TODO: add these to surfaceGui for re-construction 
	numU = 13, numV = 12;
	radius = 100;
	
	bDrawSurface = bDrawClock = bSmoothSurface = false;
	
	
	noiseTimeScale = .2;
	noiseScale = .01;
	noiseOffset = 50;
	
	clockSpread = .2;
	clockV = .5;
	clockNumCogs = 16;
	clockScale = 60;
	clockOctaves = 10;
	cloackOctaveScale = 2.;
	clockMaxSpeed = .05;
	
	bDrawOffsetMesh = false;
	faceScale = 1.;
	faceOffset = .5;
	noiseOctaveScale = 4.;
	noiseExpo = 2.;
	noiseExpoScale = 1.4;
	
	c0.set(.2,.8,1.3, .5);
	c1.set(.7,.15,.4, 0.);
	colorMixScale = 6;
	
	//load some images for gui and debug
	colorMap.loadImage( getVisualSystemDataPath() + "GUI/defaultColorPalette.png" );
	
	//load shaders
	loadShaders();
	
	//setup our surfaces
	
	//sphereSurface
	setupSphereSurface(radius, numU, numV);
	
	//load our meshes
	ofxObjLoader::load(getVisualSystemDataPath() + "models/box.obj", cubeMesh );
	
	//setup our animation
	setClock();
	
//	string loc = getVisualSystemDataPath() + "models/organicSphere.txt";
//	string loc = getVisualSystemDataPath() + "models/hexSphere.txt";
	string loc = getVisualSystemDataPath() + "models/hexSphere_mid.txt";
//	string loc = getVisualSystemDataPath() + "models/diaSphere.txt";
	
	loadHexMesh( loc );
}

void CloudsVisualSystemMandala::loadHexMesh( string loc )
{
	hexMesh.clear();
	
	vector<ofVec3f> vertices;
	vector<ofVec3f> meshvertices;
	vector<ofVec3f> meshnormals;
	
	ofBuffer buffer = ofBufferFromFile( loc );
    if(buffer.size()) {
		
		int i0, i1;
		while (buffer.isLastLine() == false) {
			string line = buffer.getNextLine();
			
			vector<string> vals = ofSplitString( line, " ");
			if(vals[0] == "v")
			{
				vertices.push_back(ofVec3f( ofToFloat(vals[1]), ofToFloat(vals[2]), ofToFloat(vals[3]) ) );
			}
			
			else if( vals[0] == "fv")
			{
				//TODO: we can get ride of the face normals. we're just using the centers
				vector<string> fc = ofSplitString( buffer.getNextLine(), " ");
				vector<string> fn = ofSplitString( buffer.getNextLine(), " ");
				
				ofVec3f faceCenter(ofToFloat(fc[1]),ofToFloat(fc[2]),ofToFloat(fc[3]));
				//ofVec3f faceNormal(ofToFloat(fn[1]),ofToFloat(fn[2]),ofToFloat(fn[3]));
				
				for (int i=1; i<vals.size()-1; i++)
				{
					i0 = ofToInt(vals[i]);
					i1 = ofToInt(vals[i+1]);
					
					meshvertices.push_back( faceCenter );
					meshvertices.push_back( vertices[i0] );
					meshvertices.push_back( vertices[i1] );
					
					meshnormals.push_back( faceCenter );
					meshnormals.push_back( faceCenter );
					meshnormals.push_back( faceCenter );
				}
				
				meshvertices.push_back( faceCenter );
				meshvertices.push_back( vertices[i1] );
				meshvertices.push_back( vertices[ofToInt(vals[1])] );
				
				meshnormals.push_back( faceCenter );
				meshnormals.push_back( faceCenter );
				meshnormals.push_back( faceCenter );
			}
		}
    }
	
	buffer.clear();
	
	hexMesh.setVertexData( &meshvertices[0], meshvertices.size(), GL_STATIC_DRAW );
	hexMesh.setNormalData( &meshnormals[0], meshnormals.size(), GL_STATIC_DRAW );
	hexMeshVertexCount = meshvertices.size();
	
	vertices.clear();
	meshvertices.clear();
	meshnormals.clear();
}

void CloudsVisualSystemMandala::setClock( int numCogs, float scale, int octaves, float octaveScale, float startSpeed )
{
	//remove the old parts if there are any
	clearClock();
	
	//build our new clock
	float step = 1. / numCogs;
	
	for (int i=0; i<octaves; i++)
	{
		float offset = i * scale;// + scale * .5;
		float oSpeed = startSpeed * octaveScale * i;
		for (int j=0; j<numCogs; j++)
		{
			SPMesh m;
			m.sp = addSurfacePoint( sphereSurface, ofVec2f(step * i + step*.5, clockV - clockSpread*.5 ) );
			m.m = &cubeMesh;
			m.setScale( scale );
			m.offset = ofVec3f(offset,offset,offset);
			
			//animate it
			MandalaTicker<float> ticker;
			ticker.begin( m.sp->uv.x, step * j, step * (j+1), oSpeed);
			ticker.setContinue();
			ticker.setEaseType( MandalaTicker<float>::OFX_TWEEN_QUINT );
			m.tickers.push_back( ticker );
			
			MandalaTicker<float> ticker0;
			ticker0.setDelay(j);
			ticker0.begin( m.sp->uv.y, m.sp->uv.y, m.sp->uv.y + clockSpread, oSpeed );
			ticker0.setReverse();
			ticker0.setEaseType( MandalaTicker<float>::OFX_TWEEN_QUINT );
			m.tickers.push_back( ticker0 );
			
			//lets hold on to this mesh
			surfaceMeshes.push_back( m );
		}
	}
	
	
	//animation of the noise deformation
	noiseTimeTicker.begin( surfaceNoiseTime, surfaceNoiseTime, surfaceNoiseTime+noiseTimeScale, 1, 0);
	noiseTimeTicker.setContinue();
	noiseTimeTicker.setEaseType( MandalaTicker<float>::OFX_TWEEN_QUINT );
}

void CloudsVisualSystemMandala::clearClock()
{
	for(int i=0; i<surfacePoints.size();i++)
	{
		delete surfacePoints[i];
	}
	surfacePoints.clear();
	
	surfaceMeshes.clear();
}

SurfacePoint* CloudsVisualSystemMandala::addSurfacePoint( ofxSimpleSurface& s, ofVec2f uv )
{
	SurfacePoint* sp = new SurfacePoint();
	sp->setSurface( &s );
	sp->uv = uv;
	
	surfacePoints.push_back( sp );
	
	return  sp;
}

void CloudsVisualSystemMandala::setupSphereSurface(float radius, int numU, int numV)
{
	float uStep = TWO_PI/(numU-1), vStep = PI/(numV-1), u, v;
	
	sphereBaseCV.resize(numU);
	for (int i=0; i<sphereBaseCV.size(); i++)
	{
		sphereBaseCV[i].resize(numV);
		u = uStep * float(i);
		
		for (int j=0; j<sphereBaseCV[i].size(); j++)
		{
			v = vStep * float(j) * .999 + .0005;
			
			sphereBaseCV[i][j].x = radius * cos( u ) * sin(v);
			sphereBaseCV[i][j].y = radius * cos( v);
			sphereBaseCV[i][j].z = radius * sin( u ) * sin( v );
		}
	}
	
	sphereSurface.addControlVertices( sphereBaseCV );
	sphereSurface.setup(3,3);
	sphereSurface.setClosed( true, false );
	sphereSurface.setFaceted(true);
}

void CloudsVisualSystemMandala::selfPresetLoaded(string presetPath)
{
	
}

void CloudsVisualSystemMandala::selfBegin()
{
}

void CloudsVisualSystemMandala::selfSceneTransformation()
{
	
}

void CloudsVisualSystemMandala::selfUpdate()
{
	lastTime = currentTime;
	currentTime = ofGetElapsedTimef();
	float delta = currentTime - lastTime;
	
	//animation tweens
	updateTickers(currentTime);
	
	//surface noise
	noiseTime += delta * noiseTimeScale;
	noiseTimeTicker.update( noiseTime );
	
	vector< vector<ofVec3f> >* cv = sphereSurface.getControlVertices();
	ofVec3f v;
	for (int i=0; i<cv->size(); i++)
	{
		for (int j=0; j<(*cv)[i].size(); j++)
		{
			v = sphereBaseCV[i][j] * noiseScale;
			(*cv)[i][j] = sphereBaseCV[i][j] + v.normalized() * ofSignedNoise( v.x, v.y + surfaceNoiseTime, v.z) * noiseOffset;
		}
	}
	
	//update mesh
	sphereSurface.update();

	//update clock
	if(bDrawClock)
	{
		for(int i=0; i<surfaceMeshes.size(); i++)
		{
			surfaceMeshes[i].update();
		}
	}
	
	ofSetWindowTitle( ofToString(ofGetFrameRate()) );
}

void CloudsVisualSystemMandala::selfDraw()
{
	float t = ofGetElapsedTimef();
	
	//surfaces
//	ofSetColor(0, 255, 255);
//	drawSurfaceControlVertices( sphereSurface );

	
	ofSetColor(255);
	normalShader.begin();
	
	if(bDrawSurface)	sphereSurface.draw();
	
	ofSetColor(255);
	
	if(bDrawClock)
	{
		for(int i=0; i<surfaceMeshes.size(); i++)
		{
			surfaceMeshes[i].draw();
		}
	}
	normalShader.end();

	
	//nested surfaces
	if(bDrawOffsetMesh)
	{
		drawOffsetMesh();
	}
	
}

void CloudsVisualSystemMandala::drawOffsetMesh()
{
	
	glDisable(GL_DEPTH_TEST);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	hexMeshNode.setScale(1);
	//	hexMeshNode.setOrientation( ofVec3f(surfaceNoiseTime*30.,0, 0) );
	noiseSurfaceShader.begin();
	
	noiseSurfaceShader.setUniform4f("c0", c0.r, c0.g, c0.b, c0.a);
	noiseSurfaceShader.setUniform4f("c1", c1.r, c1.g, c1.b, c1.a);
	noiseSurfaceShader.setUniform1f("colorMixScale", colorMixScale );
	
	noiseSurfaceShader.setUniform1f("time", surfaceNoiseTime );
	noiseSurfaceShader.setUniformMatrix4f("mMat", hexMeshNode.getGlobalTransformMatrix() );
	noiseSurfaceShader.setUniform1f("faceScale", faceScale );
	noiseSurfaceShader.setUniform1f("noiseOctaveScale", noiseOctaveScale );
	noiseSurfaceShader.setUniform1f("noiseExpo", noiseExpo );
	noiseSurfaceShader.setUniform1f("noiseExpoScale", noiseExpoScale );
	noiseSurfaceShader.setUniform1f("noiseScale", meshNoiseScale );

	
	ofPushMatrix();
	ofScale(10,10, 10);
	//	hexMeshNode.transformGL();
	
	//	hexMesh.draw(GL_TRIANGLES, 0, hexMeshVertexCount);
	
	//kind of like instancing here
	hexMesh.bind();
	float layerScale = .4;
	for (int i=0; i<20; i++)
	{
		noiseSurfaceShader.setUniform1f("faceOffset", faceOffset * i );
		glDrawArrays(GL_TRIANGLES, 0, hexMeshVertexCount);
	}
	hexMesh.unbind();
	
	//	hexMeshNode.restoreTransformGL();
	ofPopMatrix();
	
	noiseSurfaceShader.end();
	
	ofDisableAlphaBlending();
}

void CloudsVisualSystemMandala::drawSurfaceControlVertices(ofxSimpleSurface& s)
{
	vector< vector< ofVec3f > >* cv = s.getControlVertices();
	
	simpleShader.begin();
	for (int i=0; i<cv->size(); i++)
	{
		for (int j=0; j<(*cv)[i].size(); j++)
		{
//			ofSphere((*cv)[i][j], 3);
			
		}
	}
	simpleShader.end();
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
	hexMesh.clear();
	
	for(int i=0;i<meshes.size();i++)
	{
		meshes[i]->clear();
	}
	
	for(int i=0; i<surfacePoints.size();i++)
	{
		delete surfacePoints[i];
	}
	surfacePoints.clear();
}

void CloudsVisualSystemMandala::selfKeyPressed(ofKeyEventArgs & args)
{
	if(args.key == 'L' || args.key == 'l'){
//		shader.load( getVisualSystemDataPath() + "shaders/surfaceShader" );
		loadShaders();
	}
	
	if(args.key == 'F')
	{
//		surface.setFaceted( !surface.getFaceted() );
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
	normalShader.unload();
	surfaceShader.unload();
	facingRatio.unload();
	ribbonShader.unload();
	simpleShader.unload();
	noiseSurfaceShader.unload();
	
	noiseSurfaceShader.load( getVisualSystemDataPath() + "shaders/noiseSurface");
	normalShader.load( getVisualSystemDataPath() + "shaders/normalShader");
	surfaceShader.load( getVisualSystemDataPath() + "shaders/surfaceShader");
	facingRatio.load( getVisualSystemDataPath() + "shaders/facingRatio");
	ribbonShader.load( getVisualSystemDataPath() + "shaders/ribbonShader" );
	simpleShader.load( getVisualSystemDataPath() + "shaders/simple" );
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