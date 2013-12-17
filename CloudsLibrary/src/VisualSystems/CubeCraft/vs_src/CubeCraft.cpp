    //
//  CubeCraft.cpp
//

#include "CubeCraft.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CubeCraft::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("Cubes", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Cubes");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addToggle("DrawCubeCraft", &bDrawCubeCraft );
	customGui->addToggle("DrawVoxels", &bDrawVoxels );
	
	customGui->setColorBack(ofFloatColor(0,0,0,.85));
	customGui->setColorFill(ofFloatColor(1,1,1,.85));
	customGui->setColorFillHighlight(ofFloatColor(1,1,1,1));
	
	customGui->addSpacer();
	
	customGui->addIntSlider("dimX", 1, 70, &dimX);
	customGui->addIntSlider("dimY", 1, 70, &dimY);
	customGui->addIntSlider("dimZ", 1, 70, &dimZ);
	
	customGui->addSpacer();
	
	customGui->addSlider("noiseScale", .0005, .2, &noiseScale);
	customGui->addSlider("noiseThreshold", -1, 1, &noiseThreshold);
	
	customGui->addSpacer();
	
	customGui->addSlider("noiseSpeed", -10, 10, &noiseSpeed);
	customGui->addSlider("noiseDirectionX", -1, 1, &noiseDirection.x);
	customGui->addSlider("noiseDirectionY", -1, 1, &noiseDirection.y);
	customGui->addSlider("noiseDirectionZ", -1, 1, &noiseDirection.z);
	
	
	ofAddListener(customGui->newGUIEvent, this, &CubeCraft::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
	
	//mesh render gui
	meshRenderGui = new ofxUISuperCanvas("MESH_RENDER", gui);
	meshRenderGui->copyCanvasStyle(gui);
	meshRenderGui->copyCanvasProperties(gui);
	meshRenderGui->setName("MESH_RENDER");
	meshRenderGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	meshRenderGui->setColorBack(ofFloatColor(0,0,0,.85));
	meshRenderGui->setColorFill(ofFloatColor(1,1,1,.85));
	meshRenderGui->setColorFillHighlight(ofFloatColor(1,1,1,1));
	
	meshRenderGui->addSlider("edgeWidth", 0, 1, &edgeWidth);
	meshRenderGui->addToggle("FillCubes", &bFillCubes);
	meshRenderGui->addToggle("ScaleCubes", &bScaleCubes);
	meshRenderGui->addSlider("cubeScale", 0, 10, &cubeScale);
	meshRenderGui->addSpacer();
	
	meshRenderGui->addSlider("specExpo", .6, 32, &specExpo);
	meshRenderGui->addSlider("specScale", 0, 5, &specScale);
	
	meshRenderGui->addSpacer();
	
//	meshRenderGui->addLabel("edgeColor");
	meshRenderGui->addImageSampler("edgeColor", &colorMap, 100, 100);
	
//	meshRenderGui->addLabel("FillColor");
	meshRenderGui->addImageSampler("fillColor", &colorMap, 100, 100);
	
//	meshRenderGui->addLabel("specColor");
	meshRenderGui->addImageSampler("specColor", &colorMap, 100, 100);
	
	ofAddListener(meshRenderGui->newGUIEvent, this, &CubeCraft::selfGuiEvent);
	guis.push_back(meshRenderGui);
	guimap[meshRenderGui->getName()] = meshRenderGui;
	
	//fog gui
	fogGui = new ofxUISuperCanvas("FOG", gui);
	fogGui->copyCanvasStyle(gui);
	fogGui->copyCanvasProperties(gui);
	fogGui->setName("FOG");
	fogGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	fogGui->setColorBack(ofFloatColor(0,0,0,.85));
	fogGui->setColorFill(ofFloatColor(1,1,1,.85));
	fogGui->setColorFillHighlight(ofFloatColor(1,1,1,1));
	
	fogGui->addToggle("bUseFog", &bUseFog);
	fogGui->addSlider("fogDist", 10, 200, &fogDist);//->setColorFill(ofFloatColor(1,1,1,1));
	fogGui->addSlider("fogExpo", .6, 3., &fogExpo);//->setColorFill(ofFloatColor(1,1,1,1));
												   //	fogGui->addImageSampler("fogColor", &colorMap, 100, 100);
	fogGui->addSlider("fogHue", 0, 255, &fogHue);
	fogGui->addSlider("fogSaturation", 0, 255, &fogSaturation);
	fogGui->addSlider("fogBrightness", 0, 255, &fogBrightness);
	
	ofAddListener(fogGui->newGUIEvent, this, &CubeCraft::selfGuiEvent);
	guis.push_back(fogGui);
	guimap[fogGui->getName()] = fogGui;
	
	
	
	//mineCraftGui gui
	mineCraftGui = new ofxUISuperCanvas("MINECRAFT", gui);
	mineCraftGui->copyCanvasStyle(gui);
	mineCraftGui->copyCanvasProperties(gui);
	mineCraftGui->setName("MINECRAFT");
	mineCraftGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	mineCraftGui->addSlider("groundDrama", 0, 1, &groundDrama );
	mineCraftGui->addSlider("cloudThreshold", 0, 1, &cloudThreshold );
	mineCraftGui->addSlider("cloudHeight", 0, 10, &cloudHeight );

	ofAddListener(mineCraftGui->newGUIEvent, this, &CubeCraft::selfGuiEvent);
	guis.push_back(mineCraftGui);
	guimap[mineCraftGui->getName()] = mineCraftGui;
	
}

void CubeCraft::selfGuiEvent(ofxUIEventArgs &e)
{
	string name = e.getName();
	
	if(name == "edgeColor")
	{
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		ofFloatColor col =  sampler->getColor();
		edgeColor.r = col.r;
		edgeColor.g = col.g;
		edgeColor.b = col.b;
	}
	else if(name == "fillColor")
	{
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		ofFloatColor col =  sampler->getColor();
		fillColor.r = col.r;
		fillColor.g = col.g;
		fillColor.b = col.b;
	}
	else if(name == "specColor")
	{
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		ofFloatColor col =  sampler->getColor();
		fillColor2.r = col.r;
		fillColor2.g = col.g;
		fillColor2.b = col.b;
	}
	
	else if(name == "fogColor")
	{
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		ofFloatColor col =  sampler->getColor();
		fogColor.r = col.r;
		fogColor.g = col.g;
		fogColor.b = col.b;
		
		fogColor.setSaturation(fogSaturation);
	}
	else if(name == "fogSaturation" || name == "fogHue" || name == "fogBrightness" )
	{
		fogColor = ofColor::fromHsb(MIN(fogHue,254.), fogSaturation, bgBri, 255);
	}
	
	if(name == "dimX" || name == "dimY" || name == "dimZ" )
	{
		resizeVoxelGrid();
	}
	
	if(name == "DrawCubeCraft" && e.getToggle()->getValue())
	{
		bDrawVoxels = false;
	}
	if( name == "DrawVoxels" && e.getToggle()->getValue())
	{
		bDrawCubeCraft = false;
	}
}

//Use system gui for global or logical settings, for exmpl
void CubeCraft::selfSetupSystemGui(){
	
}

void CubeCraft::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CubeCraft::selfSetDefaults()
{
	//defaults
	bDrawVoxels = bDrawCubeCraft = false;
	
	edgeWidth = .1;
	noiseScale = .075;
	bFillCubes = true;
	bScaleCubes = false;
	cubeScale = 1.;
	
	edgeColor.set( 1, 0.1, 1, 1 );
	fillColor.set( 1, 1, .1, 1. );
	fillColor2.set( .1, 1, 1, 1. );
	
	fogColor.set(0,0,0,1);
	fogDist = 70;
	fogExpo = 1.;
	bUseFog = true;
	
	dimX = 2;
	dimY = 2;
	dimZ = 2;
	
	noiseSpeed = 1.;
	noiseDirection.set( 0,1,0);
	
	edgeSmoothing = .01;
	specExpo = 4;
	specScale = 1.2;
	
	
	//cube craft
}

void CubeCraft::selfSetup()
{
	
	//gui

	colorMap.loadImage( GetCloudsDataPath() + "colors/defaultColorPalette.png");
	
	cout << "Number of boxes == " << (dimX * dimY * dimZ) << endl;
	
	
	ofxObjLoader::load( getVisualSystemDataPath() + "models/box.obj", cubeMesh );
	
	cubeIndexCount = cubeMesh.getIndices().size();
	cubeVbo.setVertexData( &cubeMesh.getVertices()[0], cubeMesh.getVertices().size(), GL_STATIC_DRAW );
	cubeVbo.setNormalData( &cubeMesh.getNormals()[0], cubeMesh.getNormals().size(), GL_STATIC_DRAW );
	cubeVbo.setIndexData( &cubeMesh.getIndices()[0], cubeMesh.getIndices().size(), GL_STATIC_DRAW );
	
	loadShaders();
	
}

void CubeCraft::loadShaders()
{
	voxelShader.unload();
	voxelShader.load( getVisualSystemDataPath() + "shaders/voxelShader");
	
	cubeCraftShader.unload();
	cubeCraftShader.load( getVisualSystemDataPath() + "shaders/cubeCraftShader" );
}

void CubeCraft::selfBegin()
{
	
}


void CubeCraft::selfUpdate()
{
	float currentTime = ofGetElapsedTimef();
//	ofSetWindowTitle( ofToString( ofGetFrameRate() ) );
	float tDelta = currentTime - lastTime;
	noiseTime += tDelta * noiseSpeed;
	
	lastTime = currentTime;
	
	cameraOffset += getCameraRef().getLookAtDir() * tDelta * noiseSpeed;
	
	ofSetWindowTitle( ofToString( ofGetFrameRate() ) );
}


void CubeCraft::selfDraw()
{
	if(bDrawVoxels)
	{
		drawVoxelGrid();
	}
	
	else if(bDrawCubeCraft)
	{
		drawCubeCraft();
	}
}

void CubeCraft::drawVoxelGrid()
{
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glEnable( GL_DEPTH_TEST );
	
	ofDisableAlphaBlending();
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	float scale = 30.;
	ofPushMatrix();
	ofScale(scale,scale,scale);
	
	voxelShader.begin();
	voxelShader.setUniform1i("scaleCube", bScaleCubes);
	voxelShader.setUniform1f("cubeScale", cubeScale);
	voxelShader.setUniform1f("drawCenters", bFillCubes);
	voxelShader.setUniform1f("noiseCutoff", noiseThreshold);
	voxelShader.setUniform1f("cameraCutoffDistance", 3);
	voxelShader.setUniform1f("edgeThreshold", 1. - edgeWidth);
	
	voxelShader.setUniform1f("specExpo", specExpo);
	voxelShader.setUniform1f("specScale", specScale);
	
	voxelShader.setUniform4f("fogColor", fogColor.r, fogColor.g, fogColor.g, fogColor.a );
	voxelShader.setUniform1f("fogDist", fogDist );
	voxelShader.setUniform1f("fogExpo", fogExpo );
	
	voxelShader.setUniform1f("useFog", bUseFog);
	
	voxelShader.setUniform1f("edgeSmoothing", edgeSmoothing );
	voxelShader.setUniform4f("edgeColor", edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a );
	voxelShader.setUniform4f("fillColor", fillColor.r, fillColor.g, fillColor.b, fillColor.a );
	voxelShader.setUniform4f("specularColor", fillColor2.r, fillColor2.g, fillColor2.b, fillColor2.a );
	
	voxelShader.setUniform1f("dimX", dimX );
	voxelShader.setUniform1f("dimY", dimY );
	voxelShader.setUniform1f("dimZ", dimZ );
	
	voxelShader.setUniform1f("noiseScale", noiseScale);
	voxelShader.setUniform3f("noiseOffset", noiseDirection.x * noiseTime, noiseDirection.y * noiseTime, noiseDirection.z * noiseTime);
	
	ofVec3f cp = getCameraRef().getPosition() / scale;
	voxelShader.setUniform3f("cameraPos", cp.x, cp.y, cp.z );
	
	voxelVbo.draw(GL_TRIANGLES, 0, voxelIndexCount );
	
	voxelShader.end();
	
	ofPopMatrix();
	
	glDisable(GL_CULL_FACE);
	
	glPopAttrib();
}

void CubeCraft::drawCubeCraft()
{
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glEnable( GL_DEPTH_TEST );
	
	ofDisableAlphaBlending();
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	float scale = 30.;
	ofPushMatrix();
	ofScale(scale,scale,scale);
	
	cubeCraftShader.begin();
	cubeCraftShader.setUniform1f("cameraCutoffDistance", 3);
	
	cubeCraftShader.setUniform1f("specExpo", specExpo);
	cubeCraftShader.setUniform1f("specScale", specScale);
	
	cubeCraftShader.setUniform4f("fogColor", fogColor.r, fogColor.g, fogColor.b, fogColor.a );
	cubeCraftShader.setUniform1f("fogDist", fogDist );
	cubeCraftShader.setUniform1f("fogExpo", fogExpo );
	
	cubeCraftShader.setUniform1f("useFog", bUseFog);

	cubeCraftShader.setUniform4f("fillColor", fillColor.r, fillColor.g, fillColor.b, fillColor.a );
	cubeCraftShader.setUniform4f("specularColor", fillColor2.r, fillColor2.g, fillColor2.b, fillColor2.a );
	
	cubeCraftShader.setUniform1f("dimX", dimX );
	cubeCraftShader.setUniform1f("dimY", dimY );
	cubeCraftShader.setUniform1f("dimZ", dimZ );
	cubeCraftShader.setUniform3f("minBound", -.5 * dimX, -.5 * dimY, -.5 * dimZ);
	cubeCraftShader.setUniform3f("maxBound", .5 * dimX, .5 * dimY, .5 * dimZ);
	
	ofVec3f cp = getCameraRef().getPosition() / scale;
	cubeCraftShader.setUniform3f("cameraPos", cp.x, cp.y, cp.z );
	
	cubeCraftShader.setUniform1f("cloudThreshold", cloudThreshold);
	cubeCraftShader.setUniform1f("cloudHeight", cloudHeight);
	cubeCraftShader.setUniform1f("groundDrama", groundDrama);
	
	cubeCraftShader.setUniform3f("noiseOffset", -cameraOffset.x, 0., -cameraOffset.z);
	
	voxelVbo.draw(GL_TRIANGLES, 0, voxelIndexCount );
	
	cubeCraftShader.end();
	
	ofPopMatrix();
	
	glDisable(GL_CULL_FACE);
	
	glPopAttrib();
}

void CubeCraft::resizeVoxelGrid()
{
	halfDimX = dimX * .5;
	halfDimY = dimY * .5;
	halfDimZ = dimZ * .5;
	
	int numVertices = dimX*dimY*dimZ*cubeMesh.getVertices().size();
	int numIndices = dimX*dimY*dimZ*cubeMesh.getIndices().size();
	
	cout << "numVertices: " << numVertices << endl;
	cout << "numIndices: " << numIndices << endl;
	
	vector<ofVec3f> vertices( numVertices );
	vector<ofVec3f> normals( numVertices );
	vector<ofFloatColor> positions( numVertices );
	
	vector<ofVec3f> cubeVertices = cubeMesh.getVertices();
	vector<ofVec3f> cubeNormals = cubeMesh.getNormals();
	
	int index;
	int vCount = 0;
	int numCubeVertices = cubeVertices.size();
	
	for (int x=0; x<dimX; x++)
	{
		for (int y=0; y<dimY; y++)
		{
			for (int z=0; z<dimZ; z++)
			{
				int dim = x*dimY*dimZ*numCubeVertices + y*dimZ*numCubeVertices + z*numCubeVertices;
				for (int i=0; i<cubeVertices.size(); i++)
				{
					vertices[dim + i] = cubeVertices[i];
					positions[dim+i].set(x - halfDimX,y - halfDimY,z - halfDimZ, 0.);
					normals[dim + i] = cubeNormals[i];
				}
			}
		}
	}
	
	voxelVbo.setVertexData( &vertices[0], vertices.size(), GL_STATIC_DRAW);
	voxelVbo.setNormalData( &normals[0], normals.size(), GL_STATIC_DRAW);
	voxelVbo.setColorData( &positions[0], positions.size(), GL_STATIC_DRAW);
	
	voxelIndexCount = vertices.size();
	
	vertices.clear();
	normals.clear();
	positions.clear();
	
//	//resize and fill the ground map
//	updateGroundMap();
}


//void CubeCraft::updateGroundMap()
//{
//	//set groundMap
//	float noiseScale = .025;
//	float nVal;
//	
//	if(dimX != groundMap.width || dimZ != groundMap.height ){
//		groundMap.allocate(dimX, dimZ, OF_IMAGE_GRAYSCALE );
//		groundMap.resize( dimX, dimZ );
//	}
//	
//	for (int i=0; i<dimX; i++)
//	{
//		for (int j=0; j<dimZ; j++)
//		{
//			nVal = ofNoise( noiseScale * i, noiseScale * j);
//			nVal += ofNoise( 2. * noiseScale * i, 2. * noiseScale * j) * .5;
//			nVal += ofNoise( 4. * noiseScale * i, 4. * noiseScale * j) * .25;
//			groundMap.setColor(i, j, ofFloatColor( nVal ) );
//		}
//	}
//	groundMap.update();
//	groundMap.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
//}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CubeCraft::selfSceneTransformation(){
	
}
//use render gui for display settings, like changing colors
void CubeCraft::selfSetupRenderGui(){
	
}

void CubeCraft::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CubeCraft::selfPresetLoaded(string presetPath){

	//setup the voxels
	resizeVoxelGrid();
}

void CubeCraft::selfDrawDebug(){
	
}

void CubeCraft::selfDrawBackground(){
}

void CubeCraft::selfEnd()
{
}

void CubeCraft::selfExit()
{
	cubeMesh.clear();
	
	voxelVbo.clear();
	
	cubeVbo.clear();
}

void CubeCraft::selfKeyPressed(ofKeyEventArgs & args)
{
	if(args.key == 'l' || args.key == 'L' )
	{
		loadShaders();
	}

}
void CubeCraft::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CubeCraft::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CubeCraft::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CubeCraft::selfMousePressed(ofMouseEventArgs& data){
	
}

void CubeCraft::selfMouseReleased(ofMouseEventArgs& data){
	
}