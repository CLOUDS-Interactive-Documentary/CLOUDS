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
	
	customGui->addSlider("noiseThreshold", -1, 1, &noiseThreshold);
	customGui->addSlider("noiseScale", .0005, .2, &noiseScale);
	customGui->addSlider("edgeWidth", 0, 1, &edgeWidth);
	customGui->addToggle("FillCubes", &bFillCubes);
	customGui->addToggle("ScaleCubes", &bScaleCubes);
	
	
	ofAddListener(customGui->newGUIEvent, this, &CubeCraft::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CubeCraft::selfGuiEvent(ofxUIEventArgs &e)
{
	
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
	
	dimX = 50;
	dimY = 20;
	dimZ = 50;
	halfDimX = dimX * .5;
	halfDimY = dimY * .5;
	halfDimZ = dimZ * .5;
	
	cout << "Number of boxes == " << (dimX * dimY * dimZ) << endl;
	
	
	ofxObjLoader::load( getVisualSystemDataPath() + "models/box.obj", cubeMesh );
	
	cubeShader.load( getVisualSystemDataPath() + "shaders/normalShader");
	
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
}

void CubeCraft::selfBegin()
{
	
}


void CubeCraft::selfUpdate()
{
//	ofSetWindowTitle( ofToString( ofGetFrameRate() ) );
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
	cubeShader.setUniform1f("drawCenters", bFillCubes);
	cubeShader.setUniform1f("noiseCutoff", noiseThreshold);
	cubeShader.setUniform1f("cameraCutoffDistance", 3);
	cubeShader.setUniform1f("edgeThreshold", 1. - edgeWidth);
	
	cubeShader.setUniform1f("dimX", dimX );
	cubeShader.setUniform1f("dimY", dimY );
	cubeShader.setUniform1f("dimZ", dimZ );
	
	cubeShader.setUniform1f("noiseScale", noiseScale);
	cubeShader.setUniform3f("noiseOffset", 0, ofGetElapsedTimef() * .1, 0);
	
	ofVec3f cp = getCameraRef().getPosition() / scale;
	cubeShader.setUniform3f("cameraPos", cp.x, cp.y, cp.z );
	
	cubeVbo.draw(GL_TRIANGLES, 0, cubeIndexCount );
	
	cubeShader.end();
	
	ofPopMatrix();
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

void CubeCraft::selfKeyPressed(ofKeyEventArgs & args){

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