//
//  CloudsVisualSystemWormHole.cpp
//

#include "CloudsVisualSystemWormHole.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemWormHole::selfSetupGui(){
	
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
	
	//MESH
	meshGui = new ofxUISuperCanvas("Mesh", gui);
	meshGui->copyCanvasStyle(gui);
	meshGui->copyCanvasProperties(gui);
	meshGui->setName("Mesh");
	meshGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	meshGui->addToggle("FacetMesh", &bFacetMesh);
	meshGui->addToggle("SmoothMesh", &bSmoothMesh);
	
	meshGui->addSpacer();
	meshGui->addLabel("mesh files");
	meshGui->addSpacer();
	meshGui->addRadio("meshes", meshNames );
	
	meshGui->addSpacer();
	
	ofAddListener(meshGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(meshGui);
	guimap[meshGui->getName()] = meshGui;
	
	//CAMERA
	cameraGui = new ofxUISuperCanvas("CameraPath", gui);
	cameraGui->copyCanvasStyle(gui);
	cameraGui->copyCanvasProperties(gui);
	cameraGui->setName("CameraPath");
	cameraGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	cameraGui->addToggle("UseCameraPath", &bUseCameraPath);
	cameraGui->addSlider("pathPosition", 0, 1, &cameraPathPosition);
	
	cameraGui->addSpacer();
	cameraGui->addLabel("camera paths");
	cameraGui->addSpacer();
	
	cameraGui->addRadio("paths", cameraPathNames );
	
	ofAddListener(cameraGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(cameraGui);
	guimap[cameraGui->getName()] = cameraGui;
	
	//SHADERS
	shaderGui = new ofxUISuperCanvas("SHADERS", gui);
	shaderGui->copyCanvasStyle(gui);
	shaderGui->copyCanvasProperties(gui);
	shaderGui->setName("SHADERS");
	shaderGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	vector<string> blendNames;
	for (map<string, ofBlendMode>::iterator it=blendModes.begin(); it!=blendModes.end(); it++)
	{
		blendNames.push_back( it->first );
	}
	shaderGui->addSpacer();
	shaderGui->addToggle("depthTest", &bDepthTest);
	shaderGui->addSpacer();
	shaderGui->addLabel("ALPHA_BLENDING");
	shaderGui->addRadio("blendModes", blendNames);
	
	shaderGui->addSpacer();
	
	vector<string> shaderNames;
	for (map<string, ofShader*>::iterator it=shaderMap.begin(); it!=shaderMap.end(); it++){
		shaderNames.push_back(it->first);
	}
	
	shaderGui->addLabel("Shaders");
	shaderGui->addToggle("use shader", &bDoShader);
	shaderGui->addToggle("draw points", &bDrawPoints);
	
	shaderGui->addSpacer();
	shaderGui->addRadio("shaders", shaderNames);
	shaderGui->addSpacer();
	shaderGui->addSlider("shininess", .6, 64, &shininess);
	
	shaderGui->addSpacer();
	shaderGui->addLabel("color1");
	shaderGui->addImageSampler("c1", &colorSampleImage, 100, 100);
	shaderGui->addMinimalSlider("c1_alpha", 0, 1, &c1.a);
	shaderGui->addLabel("color2");
	shaderGui->addImageSampler("c2", &colorSampleImage, 100, 100);
	shaderGui->addMinimalSlider("c2_alpha", 0, 1, &c2.a);
	
	ofAddListener(shaderGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(shaderGui);
	guimap[shaderGui->getName()] = shaderGui;
	
	//displacement
	displacementGui = new ofxUISuperCanvas("Displacement", gui);
	displacementGui->copyCanvasStyle(gui);
	displacementGui->copyCanvasProperties(gui);
	displacementGui->setName("Displacement");
	displacementGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

	displacementGui->addSpacer();
	displacementGui->addLabel("Noise");
	displacementGui->addSpacer();
	displacementGui->addToggle("useNoiseDisplacement", &bUseNoiseDisplacement);
	displacementGui->addSpacer();
	displacementGui->addSlider("displacement", -10, 10, &noiseDisplacement);
	displacementGui->addSpacer();
	displacementGui->addSlider("noiseDirectionX", -1, 1, &noiseDir.x);
	displacementGui->addSlider("noiseDirectionY", -1, 1, &noiseDir.y);
	displacementGui->addSlider("noiseDirectionZ", -1, 1, &noiseDir.z);
	displacementGui->addSpacer();
	displacementGui->addSlider("noiseSpeed", -5, 5, &noiseSpeed);
	displacementGui->addSlider("noiseScale", 0, 100, &noiseScale);
	displacementGui->addSpacer();


	ofAddListener(displacementGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(displacementGui);
	guimap[displacementGui->getName()] = displacementGui;
}

void CloudsVisualSystemWormHole::selfGuiEvent(ofxUIEventArgs &e)
{
	string name = e.getName();
	
	if(e.getKind() == OFX_UI_WIDGET_TOGGLE && e.getToggle()->getValue() )
	{
		string parentName = e.getToggle()->getParent()->getName();
		
		if( parentName == "paths" )
		{
			for(int i=0; i<cameraPathNames.size(); i++)
			{
				if(name == cameraPathNames[i])
				{
					bUseCameraPath = true;
					pathCamera.loadPathFromFile(cameraPathPath + name);
				}
			}
		}
		
		else if(parentName == "shaders")
		{
			for (map<string, ofShader*>::iterator it=shaderMap.begin(); it!=shaderMap.end(); it++)
			{
				if(name == it->first)
				{
					currentShader = it->second;
					cout << "current shader == " << it->first << endl;
				}
			}
		}
		
		else if(parentName == "meshes")
		{
			for(int i=0; i<meshNames.size(); i++)
			{
				if(name == meshNames[i])
				{
					loadMesh( meshNames[i] );
					
					if(bFacetMesh && !bMeshHasBeenFaceted)
					{
						facetMesh(mesh, mesh);
					}
				}
			}
		}
		
		else if(parentName == "blendModes")
		{
			for (map<string, ofBlendMode>::iterator it=blendModes.begin(); it!=blendModes.end(); it++)
			{
				if(name == it->first)
				{
					currentBlendMode = it->second;
					
					cout << "WTF" << endl;
				}
			}
		}
		
		else if(name == "FacetMesh" )
		{
			if(!bMeshHasBeenFaceted)
			{
				facetMesh(mesh, mesh);
				bSmoothMesh = false;
			}
		}
		else if(name == "SmoothMesh" )
		{
			if(!bMeshHasBeenSmoothed)
			{
				smoothMesh(mesh, mesh);
				bFacetMesh = false;
			}
		}
	}
	
	if(name == "c1")
	{
		cout << "GUI event" << name << endl;
		
		ofFloatColor tempColor = ((ofxUIImageSampler *) e.widget)->getColor();
		c1.r = tempColor.r;
		c1.g = tempColor.g;
		c1.b = tempColor.b;
	}
	
	else if(name == "c2")
	{
		cout << "GUI event" << name << endl;
		ofFloatColor tempColor = ((ofxUIImageSampler *) e.widget)->getColor();
		c2.r = tempColor.r;
		c2.g = tempColor.g;
		c2.b = tempColor.b;
	}
}

void CloudsVisualSystemWormHole::loadMesh(string name)
{
	if(name != currentMeshName)
	{
		currentMeshName = name;
		
		mesh.clear();
		
		ofxObjLoader::load( modelPath + name, mesh, false );
		
		bMeshHasBeenFaceted = bMeshHasBeenSmoothed = false;
		
		if(bFacetMesh)	facetMesh(mesh, mesh);
		else if(bSmoothMesh)	smoothMesh(mesh, mesh);
	}	
}

void CloudsVisualSystemWormHole::loadShaders()
{
	loadShader("normalShader");
	loadShader("facingRatio");
	loadShader("XRayShader");
}

void CloudsVisualSystemWormHole::loadShader( string shaderName )
{
	if(shaderMap.find(shaderName) == shaderMap.end())
	{
		shaderMap[shaderName] = new ofShader();
	}
	else
	{
		shaderMap[shaderName]->unload();
	}
	
	shaderMap[shaderName]->load( getVisualSystemDataPath() + "shaders/" + shaderName );
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemWormHole::selfSetupSystemGui(){
	
}

void CloudsVisualSystemWormHole::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemWormHole::selfSetupRenderGui(){

}

void CloudsVisualSystemWormHole::guiRenderEvent(ofxUIEventArgs &e){
	
}


void CloudsVisualSystemWormHole::selfSetup()
{
	//defaults
	currentShader = NULL;
	bUseCameraPath = false;
	cameraPathPosition = 0;
	speed = .1;
	lastTime = ofGetElapsedTimef();
	bDoShader = true;
	bDrawPoints = false;
	
	shininess = 16;
	bDepthTest = true;
	
	bFacetMesh = true;
	bMeshHasBeenFaceted = false;
	
	currentBlendMode = OF_BLENDMODE_DISABLED;
	
	//meshes
	modelPath = getVisualSystemDataPath() + "models/";
	cameraPathPath = getVisualSystemDataPath() + "cameraPaths/";
	
	cout << modelPath << endl;
	cout << cameraPathPath << endl;
	

	ofDirectory dir;
	dir.allowExt("obj");
	dir.listDir( modelPath );
	for(int i = 0; i < dir.numFiles(); i++){
		meshNames.push_back( dir.getName(i) );
		//cout << "OBJ FILE NAME: " << dir.getName( i ) << endl;
	}
	
	//camera
	ofDirectory camdir;
	camdir.allowExt("xml");
	camdir.listDir( cameraPathPath );
	for(int i = 0; i < camdir.numFiles(); i++){
		cameraPathNames.push_back( camdir.getName(i) );
		//cout << "CAMERA PATH FILE NAME: " << camdir.getName( i ) << endl;
	}
	
	//shaders
	loadShaders();
	
	blendModes["OF_BLENDMODE_ADD"] = OF_BLENDMODE_ADD;
	blendModes["OF_BLENDMODE_ALPHA"] = OF_BLENDMODE_ALPHA;
	blendModes["OF_BLENDMODE_DISABLED"] = OF_BLENDMODE_DISABLED;
	blendModes["OF_BLENDMODE_MULTIPLY"] = OF_BLENDMODE_MULTIPLY;
	blendModes["OF_BLENDMODE_SCREEN"] = OF_BLENDMODE_SCREEN;
	blendModes["OF_BLENDMODE_SUBTRACT"] = OF_BLENDMODE_SUBTRACT;
	
	//gui
	colorSampleImage.loadImage( GetCloudsDataPath() + "colors/defaultColorPalette.png" );
	
}

void CloudsVisualSystemWormHole::selfBegin(){
	
}


//normal update call
void CloudsVisualSystemWormHole::selfUpdate()
{
	//lights
	lightPos = getCameraRef().getPosition();
	
	//camera
	if(bUseCameraPath)
	{
		pathCamera.update( cameraPathPosition );
		
		getCameraRef().setPosition(pathCamera.getPosition());
		getCameraRef().lookAt(pathCamera.getLookAtDir() + pathCamera.getPosition());
		
	}
	
	//action
	float t = ofGetElapsedTimef();
	float timeDelta = t - lastTime;
	sampleTime += timeDelta * speed;
	lastTime = t;
	
	noiseTime += timeDelta * noiseSpeed;
	
	getCameraRef().setNearClip(.01);
}

void CloudsVisualSystemWormHole::selfDraw()
{
	
	
	//alpha blending
	if(currentBlendMode == OF_BLENDMODE_DISABLED)
	{
		ofDisableAlphaBlending();
	}else{
		ofEnableBlendMode(currentBlendMode);
	}
	
	//depth testing
	bDepthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	
	//shader binding
	if (bDoShader && currentShader != NULL)
	{
		currentShader->begin();
		currentShader->setUniform1f("time", ofGetElapsedTimef());
		currentShader->setUniform1f("shininess", shininess );
		currentShader->setUniform4f("c1", c1.r, c1.g, c1.b, c1.a );
		currentShader->setUniform4f("c2", c2.r, c2.g, c2.b, c2.a );
		
		currentShader->setUniform1i("useNoiseDisplacement", bUseNoiseDisplacement );
		currentShader->setUniform3f("noiseOffset", noiseDir.x * noiseTime, noiseDir.y * noiseTime, noiseDir.z * noiseTime);
		currentShader->setUniform1f("noiseScale", noiseScale );
		currentShader->setUniform1f("noiseDisplacement", noiseDisplacement );
	}
	
	//draw mesh
	ofPushMatrix();
	ofMultMatrix( meshNode.getGlobalTransformMatrix() );

	if(!bDoShader){
		mat->begin();
	}
	
	if(bDrawPoints){
		mesh.drawVertices();
	}
	else{
		mesh.draw();
	}
	
	if(!bDoShader){
		mat->end();
	}
	
	ofPopMatrix();
	
	//unbind shade
	if (bDoShader && currentShader != NULL)	currentShader->end();
	
	//disable depth testing
	glDisable(GL_DEPTH_TEST);

	//disable alpha blending
	ofDisableAlphaBlending();
}

void CloudsVisualSystemWormHole::selfPresetLoaded(string presetPath){
	
}
void CloudsVisualSystemWormHole::selfSceneTransformation(){
	
}

// draw any debug stuff here
void CloudsVisualSystemWormHole::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemWormHole::selfDrawBackground(){

	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemWormHole::selfEnd()
{

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemWormHole::selfExit()
{
	//destry meshes
	mesh.clear();
	
	//unload the shaders
	for (map<string, ofShader*>::iterator it = shaderMap.begin(); it!= shaderMap.end(); it++)
	{
		//JG: fixed memory leak
		ofShader* shader = it->second;
		shader->unload();
		delete shader;
	}
	shaderMap.clear();
	
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemWormHole::selfKeyPressed(ofKeyEventArgs & args)
{
	if(args.key == 'l' || args.key == 'L')
	{
		loadShaders();
	}
}
void CloudsVisualSystemWormHole::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemWormHole::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemWormHole::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemWormHole::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemWormHole::selfMouseReleased(ofMouseEventArgs& data){
	
}


string CloudsVisualSystemWormHole::vec3ToString( ofVec3f v, int precision )
{
	return ofToString( v.x, precision) + "_" + ofToString( v.y, precision) + "_" + ofToString( v.z, precision);
}

ofVec3f CloudsVisualSystemWormHole::normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2)
{
	ofVec3f norm = (p2 - p1).cross( p0 - p1);
	return norm.normalized();
}

void CloudsVisualSystemWormHole::smoothMesh( ofMesh& facetedMesh, ofMesh& targetMesh, int precision)
{
	float startTime = ofGetElapsedTimeMillis();
	
	//get our vertex, uv and face info
	vector<ofVec3f>& v = facetedMesh.getVertices();
	vector<ofVec2f>& uv = facetedMesh.getTexCoords();
	vector<ofIndexType>& indices = facetedMesh.getIndices();
	bool hasTC = facetedMesh.getNumTexCoords();
	
	//use these to store our new mesh info
	map<string, unsigned int> mergeMap;
	vector<ofVec3f> smoothVertices;
	vector<ofVec3f> smoothNormals;
	vector<ofVec2f> smoothTexCoords;
	vector<ofIndexType> smoothIndices;
	
	//merge our vertices by pointing near by vertices to the same index
	for (int i=0; i<v.size(); i++)
	{
		mergeMap[ vec3ToString( v[i], precision ) ] = i;
	}
	
	//fill our smoothed vertex array with merged vertices & tex coords
	smoothVertices.resize( mergeMap.size() );
	if(hasTC)	smoothTexCoords.resize( mergeMap.size() );
	int smoothVertexCount = 0;
	for (map<string, unsigned int>::iterator it = mergeMap.begin(); it != mergeMap.end(); it++)
	{
		smoothVertices[smoothVertexCount] = v[it->second];
		if(hasTC)	smoothTexCoords[smoothVertexCount] = uv[it->second];
		it->second = smoothVertexCount;//store our new vertex index
		smoothVertexCount++;
	}
	
	
	//reconstruct our faces by reassigning their indices to the merged vertices
	smoothIndices.resize( indices.size() );
	for (int i=0; i<indices.size(); i++)
	{
		//use our old vertex poisition to retrieve our new index
		smoothIndices[i] = mergeMap[ vec3ToString( v[ indices[i] ], precision ) ];
	}
	
	//calculate our normals
	smoothNormals.resize( smoothVertices.size() );
	ofVec3f n;
	for (int i=0; i<smoothIndices.size(); i+=3)
	{
		n = normalFrom3Points( smoothVertices[smoothIndices[i]], smoothVertices[smoothIndices[i+1]], smoothVertices[smoothIndices[i+2]] );
		smoothNormals[smoothIndices[i]] += n;
		smoothNormals[smoothIndices[i+1]] += n;
		smoothNormals[smoothIndices[i+2]] += n;
	}
	
	for (int i=0; i<smoothNormals.size(); i++)
	{
		smoothNormals[i].normalize();
	}
	
	//setup our smoothed mesh. this should still work if our targetMesh is our facetedMesh
	targetMesh.clear();
	targetMesh.addVertices( smoothVertices );
	targetMesh.addNormals( smoothNormals );
	if(hasTC)	targetMesh.addTexCoords( smoothTexCoords );
	targetMesh.addIndices( smoothIndices );
	
	bMeshHasBeenSmoothed = true;
	bMeshHasBeenFaceted = false;
	
	cout << "smoothed mesh in "<< ofToString((ofGetElapsedTimeMillis() - startTime)) << " milli seconds" << endl;
}

void CloudsVisualSystemWormHole::facetMesh( ofMesh& smoothedMesh, ofMesh& targetMesh )
{
	float startTime = ofGetElapsedTimeMillis();
	//get our vertex, uv and face info
	vector<ofVec3f>& v = smoothedMesh.getVertices();
	vector<ofVec2f>& uv = smoothedMesh.getTexCoords();
	vector<ofIndexType>& indices = smoothedMesh.getIndices();
	bool hasTC = smoothedMesh.getNumTexCoords();
	
	//use these to store our new mesh info
	vector<ofVec3f> facetedVertices( indices.size() );
	vector<ofVec3f> facetedNormals( indices.size() );
	vector<ofVec2f> facetedTexCoords;
	if(hasTC){
		facetedTexCoords.resize( indices.size() );
	}
	vector<ofIndexType> facetedIndices( indices.size() );
	
	//store vertex and uv data
	for (int i=0; i < indices.size(); i++) {
		facetedIndices[i] = i;
		facetedVertices[i] = v[indices[i]];
		if(hasTC)	facetedTexCoords[i] = uv[indices[i]];
	}
	
	//calculate our face normals
	ofVec3f n;
	for (int i=0; i < facetedIndices.size(); i+=3) {
		n = normalFrom3Points( facetedVertices[i], facetedVertices[i+1], facetedVertices[i+2]);
		facetedNormals[i] = n;
		facetedNormals[i+1] = n;
		facetedNormals[i+2] = n;
	}
	
	//setup our faceted mesh. this should still work if our targetMesh is our smoothMesh
	targetMesh.clear();
	targetMesh.addVertices( facetedVertices );
	targetMesh.addNormals( facetedNormals );
	if(hasTC)	targetMesh.addTexCoords( facetedTexCoords );
	targetMesh.addIndices( facetedIndices );
	
	bMeshHasBeenFaceted = true;
	bMeshHasBeenSmoothed = false;
	cout << "smoothed mesh in "<< ofToString((ofGetElapsedTimeMillis() - startTime)) << " milli seconds" << endl;
}