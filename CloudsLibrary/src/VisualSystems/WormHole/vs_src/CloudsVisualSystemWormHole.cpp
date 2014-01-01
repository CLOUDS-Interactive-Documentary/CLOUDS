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
	
//	meshGui->addToggle("FacetMesh", &bFacetMesh);
//	meshGui->addToggle("SmoothMesh", &bSmoothMesh);
	
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
	shaderGui->addSlider("facingRatioExpo", .6, 64, &facingRatioExpo);
	shaderGui->addSlider("shininess", .6, 64, &shininess);
	
	shaderGui->addSpacer();
	shaderGui->addLabel("color1");
	shaderGui->addIntSlider("c1Hue", 0, 255, &c1Hue);
	shaderGui->addIntSlider("c1Saturation", 0, 255, &c1Sat);
	shaderGui->addIntSlider("c1Brightness", 0, 255, &c1Bri);
	
	shaderGui->addLabel("color2");
	shaderGui->addIntSlider("c2Hue", 0, 255, &c2Hue);
	shaderGui->addIntSlider("c2Saturation", 0, 255, &c2Sat);
	shaderGui->addIntSlider("c2Brightness", 0, 255, &c2Bri);
	
	ofAddListener(shaderGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(shaderGui);
	guimap[shaderGui->getName()] = shaderGui;
	
	
	fogGui = new ofxUISuperCanvas("FOG", gui);
	fogGui->copyCanvasStyle(gui);
	fogGui->copyCanvasProperties(gui);
	fogGui->setName("Fog");
	fogGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	//fogHue, fogSaturation, fogBrightness;
	fogGui->addSlider("fogDistance", 1, 1000, &fogDist)->setIncrement(1);
	fogGui->addSlider("fogExpo", .6, 10, &fogExpo);
	
	fogGui->addSpacer();
	fogGui->addIntSlider("fogHue", 0, 255, &fogHue);
	fogGui->addIntSlider("fogSaturation", 0, 255, &fogSaturation);
	fogGui->addIntSlider("fogBrightness", 0, 255, &fogBrightness);
	
	ofAddListener(fogGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(fogGui);
	guimap[fogGui->getName()] = fogGui;

		
	wormholeLightGui = new ofxUISuperCanvas("WH_light", gui);
	wormholeLightGui->copyCanvasStyle(gui);
	wormholeLightGui->copyCanvasProperties(gui);
	wormholeLightGui->setName("w_h_light");
	wormholeLightGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	//fogHue, fogSaturation, fogBrightness;
	wormholeLightGui->addSlider("pathOffset", 0, .5, &lightPathOffset)->setIncrement(.001);
	wormholeLightGui->addSpacer();
	wormholeLightGui->addSlider("constantAttenuation", 0, 2, &lightConstantAttenuation)->setIncrement(.01);
	wormholeLightGui->addSlider("linearAttenuation", 0, 1, &lightLinearAttenuation)->setIncrement(.01);
	wormholeLightGui->addSlider("quadraticAttenuation", 0, 1, &lightQuadraticAttenuation)->setIncrement(.01);
	wormholeLightGui->addIntSlider("lightHue", 0, 255, &lightHue);
	wormholeLightGui->addIntSlider("lightSaturation", 0, 255, &lightSaturation);
	wormholeLightGui->addIntSlider("lightBrightness", 0, 255, &lightBrightness);
	
	
	ofAddListener(wormholeLightGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(wormholeLightGui);
	guimap[wormholeLightGui->getName()] = wormholeLightGui;
	
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
				}
			}
		}
	}
	
	if(name == "c1")
	{	
		ofFloatColor tempColor = ((ofxUIImageSampler *) e.widget)->getColor();
		c1.r = tempColor.r;
		c1.g = tempColor.g;
		c1.b = tempColor.b;
	}
	
	else if( name == "c1Hue"|| name == "c1Saturation"|| name == "c1Brightness")
	{
		c1.setHue(c1Hue);
		c1.setSaturation(c1Sat);
		c1.setBrightness(c1Bri);
		
		shaderGui->getWidget("c1Hue")->setColorFill(c1);
		shaderGui->getWidget("c1Saturation")->setColorFill(c1);
		shaderGui->getWidget("c1Brightness")->setColorFill(c1);
	}
	
	else if( name == "c2Hue"|| name == "c2Saturation"|| name == "c2Brightness")
	{
		c2.setHue(c2Hue);
		c2.setSaturation(c2Sat);
		c2.setBrightness(c2Bri);
		
		shaderGui->getWidget("c2Hue")->setColorFill(c2);
		shaderGui->getWidget("c2Saturation")->setColorFill(c2);
		shaderGui->getWidget("c2Brightness")->setColorFill(c2);
	}
	
	else if( name == "fogHue"|| name == "fogSaturation"|| name == "fogBrightness")
	{
		fogColor.setHue(fogHue);
		fogColor.setSaturation(fogSaturation);
		fogColor.setBrightness(fogBrightness);
		
		fogGui->getWidget("fogHue")->setColorFill(fogColor);
		fogGui->getWidget("fogSaturation")->setColorFill(fogColor);
		fogGui->getWidget("fogBrightness")->setColorFill(fogColor);
		
		bgHue = fogHue;
		bgSat = fogSaturation;
		bgBri = fogBrightness;
		
		//??? would it be better to set the fog to the background color
	}
	else if( name == "lightHue"|| name == "lightSaturation"|| name == "lightBrightness")
	{
		lightColor.setHue(lightHue);
		lightColor.setSaturation(lightSaturation);
		lightColor.setBrightness(lightBrightness);
		
		wormholeLightGui->getWidget("lightHue")->setColorFill(lightColor);
		wormholeLightGui->getWidget("lightSaturation")->setColorFill(lightColor);
		wormholeLightGui->getWidget("lightBrightness")->setColorFill(lightColor);
	}
}

void CloudsVisualSystemWormHole::loadMesh(string name)
{
	if(name != currentMeshName)
	{
		float startTime = ofGetElapsedTimeMillis();
		currentMeshName = name;
		
		mesh.clear();
		
		ofxObjLoader::load( modelPath + name, mesh, false );
		
		cout << name + " loaded in " << ofGetElapsedTimeMillis() - startTime << " milliseconds" << endl;
	}
}

void CloudsVisualSystemWormHole::loadShaders()
{
	loadShader("normalShader");
	loadShader("facingRatio");
	loadShader("XRayShader");
	loadShader("WormholeShader");
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

void CloudsVisualSystemWormHole::selfSetupCameraGui(){
    
    camGui->addSlider("near clip plane", .001, .05, &nearClipPlane); 
	
}

void CloudsVisualSystemWormHole::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemWormHole::selfSetupRenderGui(){

}

void CloudsVisualSystemWormHole::guiRenderEvent(ofxUIEventArgs &e){
	
}


void CloudsVisualSystemWormHole::selfSetDefaults()
{
	//defaults
	currentShader = shaderMap.find("WormholeShader") != shaderMap.end() ? shaderMap["WormholeShader"] : NULL;
	bUseCameraPath = false;
	cameraPathPosition = 0;
	speed = .1;
	lastTime = ofGetElapsedTimef();
	bDoShader = true;
	bDrawPoints = false;
	
	shininess = 16;
	bDepthTest = true;
	
	currentBlendMode = OF_BLENDMODE_DISABLED;
	
	fogColor.set(0,0,0, 255);
	fogDist = 70;
	
	lightPathOffset = .05;
	
	lightConstantAttenuation = .7;
	lightLinearAttenuation = .01;
	lightQuadraticAttenuation = .01;
}

void CloudsVisualSystemWormHole::selfSetup()
{
	currentShader = NULL;
	
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
	//light(s)	
    
    getCameraRef().setNearClip(nearClipPlane);
	
	//camera
	if(bUseCameraPath)
	{
		pathCamera.update( cameraPathPosition );
		
		getCameraRef().setPosition(pathCamera.getPosition());
		getCameraRef().lookAt(pathCamera.getLookAtDir() + pathCamera.getPosition());
		
		//light on path
		lightPos = pathCamera.getPositionSpline().getPoint( ofClamp(pathCamera.u + lightPathOffset, 0, 1) );
		
	}
	else
	{
		lightPos = getCameraRef().getPosition() + getCameraRef().getLookAtDir() * 10;
	}
	
	//transform light to modelview space
//	lightPos = lightPos * getCameraRef().getModelViewMatrix();
	
	//action
	float t = ofGetElapsedTimef();
	float timeDelta = t - lastTime;
	sampleTime += timeDelta * speed;
	lastTime = t;
	
	noiseTime += timeDelta * noiseSpeed;
    
}

void CloudsVisualSystemWormHole::selfDraw()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
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

		ofFloatColor lc = lightColor;
		currentShader->setUniform4f("lightColor", lc.r, lc.g, lc.b, lc.a);
		currentShader->setUniform3f("lightPosition", lightPos.x, lightPos.y, lightPos.z );
		currentShader->setUniform1f("lightConstantAttenuation", lightConstantAttenuation);
		currentShader->setUniform1f("lightLinearAttenuation", lightLinearAttenuation * lightLinearAttenuation);
		currentShader->setUniform1f("lightQuadraticAttenuation", lightQuadraticAttenuation * lightQuadraticAttenuation);

		ofFloatColor fc = fogColor;
		currentShader->setUniform4f("fogColor", fc.r, fc.g, fc.b, fc.a);
		currentShader->setUniform1f("fogDistance", fogDist );
		currentShader->setUniform1f("fogExpo", fogExpo );
		
		currentShader->setUniform1f("facingRatioExpo", facingRatioExpo );
		currentShader->setUniform1f("shininess", shininess );
		ofFloatColor c1f = c1;
		ofFloatColor c2f = c2;
		currentShader->setUniform4f("c1", c1f.r, c1f.g, c1f.b, c1f.a );
		currentShader->setUniform4f("c2", c2f.r, c2f.g, c2f.b, c2f.a );

//		currentShader->setUniform1i("useNoiseDisplacement", bUseNoiseDisplacement );
//		currentShader->setUniform3f("noiseOffset", noiseDir.x * noiseTime, noiseDir.y * noiseTime, noiseDir.z * noiseTime);
//		currentShader->setUniform1f("noiseScale", noiseScale );
//		currentShader->setUniform1f("noiseDisplacement", noiseDisplacement );
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
	
	//unbind shade
	if (bDoShader && currentShader != NULL)	currentShader->end();
	
	ofPopMatrix();
	
	
	//disable depth testing
	glDisable(GL_DEPTH_TEST);
	
	
	glDisable(GL_CULL_FACE);

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
	
	cout << "faceted mesh in "<< ofToString((ofGetElapsedTimeMillis() - startTime)) << " milli seconds" << endl;
}