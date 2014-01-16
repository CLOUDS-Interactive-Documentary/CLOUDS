    //
//  CloudsVisualSystemCubeCraft.cpp
//

#include "CloudsVisualSystemCubeCraft.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemCubeCraft::selfSetupGui()
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
	
	customGui->addIntSlider("dimX", 1, 250, &dimX);
	customGui->addIntSlider("dimY", 1, 40, &dimY);
	customGui->addIntSlider("dimZ", 1, 250, &dimZ);
	
	customGui->addSpacer();
	
	customGui->addSlider("noiseScale", .0005, .2, &noiseScale);
	customGui->addSlider("noiseThreshold", -1, 1, &noiseThreshold);
	
	customGui->addSpacer();
	
	customGui->addSlider("speed", -10, 10, &speed);
	customGui->addSlider("noiseDirectionX", -1, 1, &noiseDirection.x);
	customGui->addSlider("noiseDirectionY", -1, 1, &noiseDirection.y);
	customGui->addSlider("noiseDirectionZ", -1, 1, &noiseDirection.z);
	
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemCubeCraft::selfGuiEvent);
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
	
	meshRenderGui->addLabel("EdgeColor");
	meshRenderGui->addImageSampler("edgeColor", &colorMap, 100, 100);
	
	meshRenderGui->addLabel("FillColor");
	meshRenderGui->addImageSampler("fillColor", &colorMap, 100, 100);
	
	meshRenderGui->addLabel("SpecColor");
	meshRenderGui->addImageSampler("specColor", &colorMap, 100, 100);
	
	ofAddListener(meshRenderGui->newGUIEvent, this, &CloudsVisualSystemCubeCraft::selfGuiEvent);
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
	
	
	fogGui->addSpacer();
	
	fogGui->addLabel("FOG_Color");
//	fogGui->addMinimalSlider("fogHue", 0, 255, &fogHue)->setIncrement(1);
//	fogGui->addMinimalSlider("fogSaturation", 0, 255, &fogSaturation)->setIncrement(1);
//	fogGui->addMinimalSlider("fogBrightness", 0, 255, &fogBrightness)->setIncrement(1);
	fogGui->addIntSlider("fogHue", 0, 255, &fogHue);
	fogGui->addIntSlider("fogSaturation", 0, 255, &fogSaturation);
	fogGui->addIntSlider("fogBrightness", 0, 255, &fogBrightness);
	
	fogGui->addSpacer();
	fogGui->addSlider("fogDist", 10, 200, &fogDist);
	fogGui->addSlider("fogExpo", .6, 3., &fogExpo);
	
   //fogGui->addImageSampler("fogColor", &colorMap, 100, 100);
	
	ofAddListener(fogGui->newGUIEvent, this, &CloudsVisualSystemCubeCraft::selfGuiEvent);
	guis.push_back(fogGui);
	guimap[fogGui->getName()] = fogGui;
	
	
	
	//mineCraftGui gui
	mineCraftGui = new ofxUISuperCanvas("MINECRAFT", gui);
	mineCraftGui->copyCanvasStyle(gui);
	mineCraftGui->copyCanvasProperties(gui);
	mineCraftGui->setName("MINECRAFT");
	mineCraftGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	mineCraftGui->addIntSlider("mcDimX", 10, 200, &mineCraftDimX);
	mineCraftGui->addIntSlider("mcDimY", 10, 200, &mineCraftDimY);
	
	mineCraftGui->addSpacer();
	
	mineCraftGui->addSlider("groundDrama", 0, 1, &groundDrama );
	mineCraftGui->addSlider("cloudThreshold", 0, 1, &cloudThreshold );
	mineCraftGui->addSlider("cloudHeight", 0, 10, &cloudHeight );
	mineCraftGui->addSlider("cloudThickness", 0, 50, &cloudThickness );
	mineCraftGui->addSlider("cloudSpeed", -.1, .1, &cloudSpeed )->setIncrement(.001);
	mineCraftGui->addSlider("cloudShadow", 0., 1, &cloudShadow );
	
	//colors
	mineCraftGui->addSpacer();
	
	mineCraftGui->addLabel("GroundHSB");
	mineCraftGui->addMinimalSlider("groundHue", 0, 255, &groundHue );
	mineCraftGui->addMinimalSlider("groundSaturation", 0, 255, &groundSaturation );
	mineCraftGui->addMinimalSlider("groundBrightness", 0, 255, &groundBrightness );
	mineCraftGui->addLabel("UndergroundHSB");
	mineCraftGui->addMinimalSlider("undergroundHue", 0, 255, &undergroundHue );
	mineCraftGui->addMinimalSlider("undergroundSaturation", 0, 255, &undergroundSaturation );
	mineCraftGui->addMinimalSlider("undergroundBrightness", 0, 255, &undergroundBrightness );
	mineCraftGui->addLabel("CloudShadowHSB");
	mineCraftGui->addMinimalSlider("cloudShadowHue", 0, 255, &cloudShadowHue );
	mineCraftGui->addMinimalSlider("cloudShadowSaturation", 0, 255, &cloudShadowSaturation );
	mineCraftGui->addMinimalSlider("cloudShadowBrightness", 0, 255, &cloudShadowBrightness );

	
	ofAddListener(mineCraftGui->newGUIEvent, this, &CloudsVisualSystemCubeCraft::selfGuiEvent);
	guis.push_back(mineCraftGui);
	guimap[mineCraftGui->getName()] = mineCraftGui;
	
}

void CloudsVisualSystemCubeCraft::selfGuiEvent(ofxUIEventArgs &e)
{
	string name = e.getName();
	
	if(name == "edgeColor")
	{
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		edgeColor =  sampler->getColor();
	}
	else if(name == "fillColor")
	{
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		fillColor = sampler->getColor();
	}
	else if(name == "specColor")
	{
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		fillColor2 = sampler->getColor();
	}
	
	else if(name == "fogSaturation" || name == "fogHue" || name == "fogBrightness" )
	{
		fogColor.setHsb(fogHue, fogSaturation, fogBrightness);
		
		fogGui->getWidget("FOG_Color")->setColorFill( fogColor);
		fogGui->getWidget("fogHue")->setColorFill( fogColor);
		fogGui->getWidget("fogSaturation")->setColorFill(fogColor);
		fogGui->getWidget("fogBrightness")->setColorFill(fogColor);
		
//		fogGui->setColorBack( fogColor );
		
		bgHue = fogHue;
		bgSat = fogSaturation;
		bgBri = fogBrightness;
	
	}
	
	else if(name == "groundHue" || name == "groundSaturation" || name == "groundBrightness")
	{
		groundColor.setHue(groundHue);
		groundColor.setSaturation(groundSaturation);
		groundColor.setBrightness(groundBrightness);
		
		mineCraftGui->getWidget("GroundHSB")->setColorFill(groundColor);
		mineCraftGui->getWidget("groundHue")->setColorFill(groundColor);
		mineCraftGui->getWidget("groundSaturation")->setColorFill(groundColor);
		mineCraftGui->getWidget("groundBrightness")->setColorFill(groundColor);
	}
	
	else if(name == "cloudShadowHue" || name == "cloudShadowSaturation" || name == "cloudShadowBrightness")
	{
		cloudShadowColor.setHue(cloudShadowHue);
		cloudShadowColor.setSaturation(cloudShadowSaturation);
		cloudShadowColor.setBrightness(cloudShadowBrightness);
		
		mineCraftGui->getWidget("CloudShadowHSB")->setColorFill(cloudShadowColor);
		mineCraftGui->getWidget("cloudShadowHue")->setColorFill(cloudShadowColor);
		mineCraftGui->getWidget("cloudShadowSaturation")->setColorFill(cloudShadowColor);
		mineCraftGui->getWidget("cloudShadowBrightness")->setColorFill(cloudShadowColor);
	}
	
	
	else if(name == "undergroundHue" || name == "undergroundSaturation" || name == "undergroundBrightness")
	{
		undergroundColor.setHue(undergroundHue);
		undergroundColor.setSaturation(undergroundSaturation);
		undergroundColor.setBrightness(undergroundBrightness);
		
		mineCraftGui->getWidget("UndergroundHSB")->setColorFill(undergroundColor);
		mineCraftGui->getWidget("undergroundHue")->setColorFill(undergroundColor);
		mineCraftGui->getWidget("undergroundSaturation")->setColorFill(undergroundColor);
		mineCraftGui->getWidget("undergroundBrightness")->setColorFill(undergroundColor);
	}
	
	if(name == "dimX" || name == "dimY" || name == "dimZ" )
	{
		if(bDrawCubeCraft)
		{
			dimY = 1;
		}
		resizeVoxelGrid();
	}
	
	if(name == "DrawCubeCraft" && e.getToggle()->getValue())
	{
		bDrawVoxels = false;
		//resizeVoxelGrid();
	}
	if( name == "DrawVoxels" && e.getToggle()->getValue())
	{
		bDrawCubeCraft = false;
		//resizeVoxelGrid();
	}
}

void CloudsVisualSystemCubeCraft::updateAllColors()
{
	fogColor.setHue(fogHue);
	fogColor.setSaturation(fogSaturation);
	fogColor.setBrightness(fogBrightness);
	
	undergroundColor.setHue(undergroundHue);
	undergroundColor.setSaturation(undergroundSaturation);
	undergroundColor.setBrightness(undergroundBrightness);
	
	cloudShadowColor.setHue(cloudShadowHue);
	cloudShadowColor.setSaturation(cloudShadowSaturation);
	cloudShadowColor.setBrightness(cloudShadowBrightness);
	
	groundColor.setHue(groundHue);
	groundColor.setSaturation(groundSaturation);
	groundColor.setBrightness(groundBrightness);
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemCubeCraft::selfSetupSystemGui(){
	
}

void CloudsVisualSystemCubeCraft::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemCubeCraft::selfSetDefaults()
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
	
	speed = 1.;
	noiseDirection.set( 0,1,0);
	
	edgeSmoothing = .01;
	specExpo = 4;
	specScale = 1.2;
	
	
	//cube craft
	updateAllColors();
}

void CloudsVisualSystemCubeCraft::selfSetup()
{
	
	//gui

	colorMap.loadImage( GetCloudsDataPath() + "colors/defaultColorPalette.png");
	
	//cout << "Number of boxes == " << (dimX * dimY * dimZ) << endl;
	ofxObjLoader::load( getVisualSystemDataPath() + "models/box.obj", cubeMesh );
	
	loadShaders();
	
	updateAllColors();
}

void CloudsVisualSystemCubeCraft::loadShaders()
{
	unloadShaders();
	
	voxelShader.load( getVisualSystemDataPath() + "shaders/voxelShader");
	cubeCraftShader.load( getVisualSystemDataPath() + "shaders/cubeCraftShader" );
	mineCraftGroundShader.load( getVisualSystemDataPath() + "shaders/mineCraftGround");
	mineCraftCloudsShader.load( getVisualSystemDataPath() + "shaders/mineCraftClouds");
}

void CloudsVisualSystemCubeCraft::unloadShaders()
{
	voxelShader.unload();
	cubeCraftShader.unload();
	mineCraftGroundShader.unload();
	mineCraftCloudsShader.unload();
}

void CloudsVisualSystemCubeCraft::selfBegin()
{
	
}


void CloudsVisualSystemCubeCraft::selfUpdate()
{
	float currentTime = ofGetElapsedTimef();
//	ofSetWindowTitle( ofToString( ofGetFrameRate() ) );
	float tDelta = currentTime - lastTime;
	noiseTime += tDelta * speed;
	
	lastTime = currentTime;
	
	cameraOffset += getCameraRef().getLookAtDir() * tDelta * speed;
	
	ofSetWindowTitle( ofToString( ofGetFrameRate() ) );
}


void CloudsVisualSystemCubeCraft::selfDraw()
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

void CloudsVisualSystemCubeCraft::drawVoxelGrid()
{
	ofPushStyle();
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
	
	fc = fogColor;
	voxelShader.setUniform4f("fogColor", fc.r, fc.g, fc.g, fc.a );
	voxelShader.setUniform1f("fogDist", fogDist );
	voxelShader.setUniform1f("fogExpo", fogExpo );
	
	voxelShader.setUniform1f("useFog", bUseFog);
	
	voxelShader.setUniform1f("edgeSmoothing", edgeSmoothing );
	
	fc = edgeColor;
	voxelShader.setUniform4f("edgeColor", fc.r, fc.g, fc.b, fc.a );
	fc = fillColor;
	voxelShader.setUniform4f("fillColor", fc.r, fc.g, fc.b, fc.a );
	fc = fillColor2;
	voxelShader.setUniform4f("specularColor", fc.r, fc.g, fc.b, fc.a );
	
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
	
	glPopAttrib();
	ofPopStyle();
}

void CloudsVisualSystemCubeCraft::drawCubeCraft()
{
	ofPushStyle();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glEnable( GL_DEPTH_TEST );
	ofDisableAlphaBlending();
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	float scale = 30.;
	ofPushMatrix();
	ofScale(scale,scale,scale);
	
	mineCraftGroundShader.begin();

	mineCraftGroundShader.begin();
	mineCraftGroundShader.setUniform1f("cameraCutoffDistance", 3);
	
	mineCraftGroundShader.setUniform1f("specExpo", specExpo);
	mineCraftGroundShader.setUniform1f("specScale", specScale);

	fc = fogColor;
	mineCraftGroundShader.setUniform4f("fogColor", fc.r, fc.g, fc.b, fc.a );
	mineCraftGroundShader.setUniform1f("fogDist", fogDist );
	mineCraftGroundShader.setUniform1f("fogExpo", fogExpo );

	mineCraftGroundShader.setUniform1f("useFog", bUseFog);

	fc = fillColor2;
	mineCraftGroundShader.setUniform4f("specularColor", fc.r, fc.g, fc.b, fc.a );

	fc = groundColor;
	mineCraftGroundShader.setUniform4f("groundColor", fc.r, fc.g, fc.b, fc.a );

	fc = undergroundColor;
	mineCraftGroundShader.setUniform4f("undergroundColor", fc.r, fc.g, fc.b, fc.a );

	fc = cloudShadowColor;
	mineCraftGroundShader.setUniform4f("cloudShadowColor", fc.r, fc.g, fc.b, fc.a );

	ofVec3f cp = getCameraRef().getPosition() / scale;
	mineCraftGroundShader.setUniform3f("cameraPos", cp.x, cp.y, cp.z );
	
	mineCraftGroundShader.setUniform1f("cloudThreshold", cloudThreshold);
	mineCraftGroundShader.setUniform1f("cloudShadow", 1. - cloudShadow);
	mineCraftGroundShader.setUniform1f("groundDrama", groundDrama);

	ofVec3f cloudVel = noiseDirection * noiseTime * cloudSpeed;
	mineCraftGroundShader.setUniform3f("noiseOffset", cloudVel.x, cloudVel.y, cloudVel.z);
	mineCraftGroundShader.setUniform3f("cameraOffset", -cameraOffset.x, 0., -cameraOffset.z);
	
	mineCraftGroundShader.setUniform3f("cubeScale", 1,1,1 );

	voxelVbo.draw(GL_TRIANGLES, 0, voxelIndexCount );

	mineCraftGroundShader.end();
	
	
	//CLOUDS
	mineCraftCloudsShader.begin();
	mineCraftCloudsShader.setUniform1f("cameraCutoffDistance", 3);
	
	mineCraftCloudsShader.setUniform1f("specExpo", specExpo);
	mineCraftCloudsShader.setUniform1f("specScale", specScale);
	
	fc = fogColor;
	mineCraftCloudsShader.setUniform4f("fogColor", fc.r, fc.g, fc.b, fc.a );
	mineCraftCloudsShader.setUniform1f("fogDist", fogDist );
	mineCraftCloudsShader.setUniform1f("fogExpo", fogExpo );
	
	mineCraftCloudsShader.setUniform1f("useFog", bUseFog);
	
	fc = fillColor2;
	mineCraftCloudsShader.setUniform4f("specularColor", fc.r, fc.g, fc.b, fc.a );
	
	fc = groundColor;
	mineCraftCloudsShader.setUniform4f("groundColor", fc.r, fc.g, fc.b, fc.a );
	
	fc = undergroundColor;
	mineCraftCloudsShader.setUniform4f("undergroundColor", fc.r, fc.g, fc.b, fc.a );
	
	
	mineCraftCloudsShader.setUniform1f("cloudHeight", cloudHeight);
	mineCraftCloudsShader.setUniform1f("cloudThickness", cloudThickness);

	mineCraftCloudsShader.setUniform3f("cameraPos", cp.x, cp.y, cp.z );
	
	mineCraftCloudsShader.setUniform1f("cloudThreshold", cloudThreshold);
	mineCraftCloudsShader.setUniform1f("cloudShadow", 1. - cloudShadow);
	mineCraftCloudsShader.setUniform1f("groundDrama", groundDrama);
	
	mineCraftCloudsShader.setUniform3f("noiseOffset", cloudVel.x, cloudVel.y, cloudVel.z);
	mineCraftCloudsShader.setUniform3f("cameraOffset", -cameraOffset.x, 0., -cameraOffset.z);
	
	voxelVbo.draw(GL_TRIANGLES, 0, voxelIndexCount );
	
	mineCraftCloudsShader.end();
	
	ofPopMatrix();
	
	glPopAttrib();
	ofPopStyle();
}

void CloudsVisualSystemCubeCraft::resizeVoxelGrid()
{
	halfDimX = dimX * .5;
	halfDimY = dimY * .5;
	halfDimZ = dimZ * .5;
	
	int numVertices = dimX*dimY*dimZ*cubeMesh.getVertices().size();
	int numIndices = dimX*dimY*dimZ*cubeMesh.getIndices().size();
	
	//cout << "numVertices: " << numVertices << endl;
	//cout << "numIndices: " << numIndices << endl;
	
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


//void CloudsVisualSystemCubeCraft::updateGroundMap()
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
void CloudsVisualSystemCubeCraft::selfSceneTransformation(){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemCubeCraft::selfSetupRenderGui(){
	
}

void CloudsVisualSystemCubeCraft::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemCubeCraft::selfPresetLoaded(string presetPath){

	//setup the voxels
	resizeVoxelGrid();
	
	updateAllColors();
}

void CloudsVisualSystemCubeCraft::selfDrawDebug(){
	
}

void CloudsVisualSystemCubeCraft::selfDrawBackground(){
}

void CloudsVisualSystemCubeCraft::selfEnd()
{
}

void CloudsVisualSystemCubeCraft::selfExit()
{
	cubeMesh.clear();
	
	voxelVbo.clear();
	
	unloadShaders();
}

void CloudsVisualSystemCubeCraft::selfKeyPressed(ofKeyEventArgs & args)
{
	if(args.key == 'l' || args.key == 'L' )
	{
		loadShaders();
	}

}
void CloudsVisualSystemCubeCraft::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemCubeCraft::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCubeCraft::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCubeCraft::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCubeCraft::selfMouseReleased(ofMouseEventArgs& data){
	
}