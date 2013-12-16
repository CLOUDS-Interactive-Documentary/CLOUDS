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
	
	meshRenderGui->addLabel("edge color");
	meshRenderGui->addImageSampler("edgeColor", &colorMap, 100, 100);
	
	meshRenderGui->addLabel("fill color");
	meshRenderGui->addImageSampler("fillColor", &colorMap, 100, 100);
	
	meshRenderGui->addLabel("spec color");
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
	fogGui->addImageSampler("fogColor", &colorMap, 100, 100);
	
	ofAddListener(fogGui->newGUIEvent, this, &CubeCraft::selfGuiEvent);
	guis.push_back(fogGui);
	guimap[fogGui->getName()] = fogGui;
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
	}
	
	if(name == "dimX" || name == "dimY" || name == "dimZ" )
	{
		resizeVoxelGrid();
	}
}

//Use system gui for global or logical settings, for exmpl
void CubeCraft::selfSetupSystemGui(){
	
}

void CubeCraft::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CubeCraft::selfSetup()
{
	//defaults
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
	
	dimX = 30;
	dimY = 10;
	dimZ = 30;
	
	noiseSpeed = 1.;
	noiseDirection.set( 0,1,0);
	
	edgeSmoothing = .01;
	specExpo = 4;
	specScale = 1.2;
	
	//gui

	colorMap.loadImage( GetCloudsDataPath() + "colors/defaultColorPalette.png");
	
	cout << "Number of boxes == " << (dimX * dimY * dimZ) << endl;
	
	
	ofxObjLoader::load( getVisualSystemDataPath() + "models/box.obj", cubeMesh );
	
	cubeShader.load( getVisualSystemDataPath() + "shaders/voxelShader");
	
	//setup the voxels
	resizeVoxelGrid();
}

void CubeCraft::selfBegin()
{
	
}


void CubeCraft::selfUpdate()
{
	float currentTime = ofGetElapsedTimef();
//	ofSetWindowTitle( ofToString( ofGetFrameRate() ) );
	noiseTime += (currentTime - lastTime) * noiseSpeed;
	
	lastTime = currentTime;
}


void CubeCraft::selfDraw()
{
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glEnable( GL_DEPTH_TEST );
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	float scale = 30.;
	ofPushMatrix();
	ofScale(scale,scale,scale);
	
	cubeShader.begin();
	cubeShader.setUniform1i("scaleCube", bScaleCubes);
	cubeShader.setUniform1f("cubeScale", cubeScale);
	cubeShader.setUniform1f("drawCenters", bFillCubes);
	cubeShader.setUniform1f("noiseCutoff", noiseThreshold);
	cubeShader.setUniform1f("cameraCutoffDistance", 3);
	cubeShader.setUniform1f("edgeThreshold", 1. - edgeWidth);
	
	
	cubeShader.setUniform1f("specExpo", specExpo);
	cubeShader.setUniform1f("specScale", specScale);
	
	cubeShader.setUniform4f("fogColor", fogColor.r, fogColor.g, fogColor.g, fogColor.a );
	cubeShader.setUniform1f("fogDist", fogDist );
	cubeShader.setUniform1f("fogExpo", fogExpo );
	
	cubeShader.setUniform1f("useFog", bUseFog);
	
	cubeShader.setUniform1f("edgeSmoothing", edgeSmoothing );
	cubeShader.setUniform4f("edgeColor", edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a );
	cubeShader.setUniform4f("fillColor", fillColor.r, fillColor.g, fillColor.b, fillColor.a );
	cubeShader.setUniform4f("specularColor", fillColor2.r, fillColor2.g, fillColor2.b, fillColor2.a );
	
	cubeShader.setUniform1f("dimX", dimX );
	cubeShader.setUniform1f("dimY", dimY );
	cubeShader.setUniform1f("dimZ", dimZ );
	
	cubeShader.setUniform1f("noiseScale", noiseScale);
	cubeShader.setUniform3f("noiseOffset", noiseDirection.x * noiseTime, noiseDirection.y * noiseTime, noiseDirection.z * noiseTime);
//	cubeShader.setUniform3f("noiseOffset", 0, ofGetElapsedTimef() * .1, 0);
	
	ofVec3f cp = getCameraRef().getPosition() / scale;
	cubeShader.setUniform3f("cameraPos", cp.x, cp.y, cp.z );
	
	cubeVbo.draw(GL_TRIANGLES, 0, cubeIndexCount );
	
	cubeShader.end();
	
	ofPopMatrix();
	
	glDisable(GL_CULL_FACE);
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
	
	cubeVbo.setVertexData( &vertices[0], vertices.size(), GL_STATIC_DRAW);
	cubeVbo.setNormalData( &normals[0], normals.size(), GL_STATIC_DRAW);
	cubeVbo.setColorData( &positions[0], positions.size(), GL_STATIC_DRAW);
	
	cubeIndexCount = vertices.size();
	
	vertices.clear();
	normals.clear();
	positions.clear();

	glPopAttrib();
}


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
	cubeVbo.clear();
}

void CubeCraft::selfKeyPressed(ofKeyEventArgs & args)
{
	if(args.key == 'l' || args.key == 'L' )
	{
		cubeShader.unload();
		cubeShader.load( getVisualSystemDataPath() + "shaders/voxelShader");
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