//
//  CloudsVisualSystemGesturePaint.cpp
//

#include "CloudsVisualSystemGesturePaint.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemGesturePaint::selfSetupGui(){
    
	waterGui = new ofxUISuperCanvas("WATER", gui);
	waterGui->copyCanvasStyle(gui);
	waterGui->copyCanvasProperties(gui);
	waterGui->setName("Water");
	waterGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
	waterGui->addSlider("Blur Radius", 0, 5., &blurRadius);
	waterGui->addSlider("Dry Rate", 0, .5, &dryRate);
	waterGui->addSlider("Deposite Scale", 1., 256., &depositeScale);
	waterGui->addToggle("Show Debug Tex", &showWaterDebug);
    
	ofAddListener(waterGui->newGUIEvent, this, &CloudsVisualSystemGesturePaint::selfGuiEvent);
	guis.push_back(waterGui);
	guimap[waterGui->getName()] = waterGui;
    
	brushGui = new ofxUISuperCanvas("BRUSH", gui);
	brushGui->copyCanvasStyle(gui);
	brushGui->copyCanvasProperties(gui);
	brushGui->setName("Brush");
	brushGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	brushGui->addSlider("Brush Size", 1, 256., &brushSize);
	brushGui->addSlider("Palette Shift Speed", 0, 100., &paletteTraversalSpeed);
	brushGui->addSlider("Palette Expand", 0, 1.0, &paletteExpandPercent);
    
	//palettes
	ofDirectory palettes(getVisualSystemDataPath() + "images/palettes/");
	palettes.allowExt("jpg");
	palettes.listDir();
	vector<string> palettePaths;
	for(int i = 0; i < palettes.numFiles(); i++){
		palettePaths.push_back(palettes.getName(i));
	}
	
	if(palettes.size() > 0){
		brushGui->addRadio("Palette", palettePaths);
	}
	
	brushGui->addSpacer();
	brushGui->addToggle("Show Palette", &previewPalette);
	
	ofAddListener(brushGui->newGUIEvent, this, &CloudsVisualSystemGesturePaint::selfGuiEvent);
	guis.push_back(brushGui);
	guimap[brushGui->getName()] = brushGui;
}

void CloudsVisualSystemGesturePaint::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getParent()->getName() == "Palette" && ((ofxUIButton*)e.widget)->getValue()){
		cout << "selected palette "<< e.widget->getName() << endl;
		palette.loadImage(getVisualSystemDataPath() + "images/palettes/" + e.widget->getName() );
	}
}

void CloudsVisualSystemGesturePaint::selfSetupSystemGui(){
	
}

void CloudsVisualSystemGesturePaint::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemGesturePaint::selfSetupRenderGui(){
    
}

void CloudsVisualSystemGesturePaint::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemGesturePaint::selfSetDefaults(){
	blurRadius = 1.;
	dryRate = .001;
	showWaterDebug = false;
	palette.clear();
	
	previewPalette = false;
	brushInterpolateStep = 4.;
	paletteTraversalSpeed = 1.0;
	
	currentDepositeScale = 0;
	depositeScale = 64.;
	
	brushSize = 128.;
	currentBrushSize = 0;
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemGesturePaint::selfSetup(){
	reloadShader();
}

void CloudsVisualSystemGesturePaint::reloadShader(){
	
	cout << "loading hblur" << endl;
	hblurShader.load(getVisualSystemDataPath() + "shaders/hblur.vert",
					 getVisualSystemDataPath() + "shaders/blur.frag");
	cout << "loading vblur" << endl;
	vblurShader.load(getVisualSystemDataPath() + "shaders/vblur.vert",
					 getVisualSystemDataPath() + "shaders/blur.frag");
    
	cout << "loading paper mix" << endl;
	paperMixShader.load(getVisualSystemDataPath() + "shaders/papermix");
	
	cout << "loading force brush shader" << endl;
	forceBrushShader.load(getVisualSystemDataPath() + "shaders/forcebrush");
    
	cout << "loading paint brush shader" << endl;
	paintBrushShader.load(getVisualSystemDataPath() + "shaders/paintbrush");
	
	brushImage.loadImage(getVisualSystemDataPath() + "images/brush.png");
	paperImage.loadImage(getVisualSystemDataPath() + "images/paper.jpg");
	noiseFlowTex.loadImage(getVisualSystemDataPath() + "images/noise.png");
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemGesturePaint::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemGesturePaint::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemGesturePaint::selfSceneTransformation(){
	
}

void CloudsVisualSystemGesturePaint::reallocateFramebuffers(){
	
	canvassrc.allocate(getSharedRenderTarget().getWidth(),
					   getSharedRenderTarget().getHeight(),
					   GL_RGBA);
	canvasdst.allocate(canvassrc.getWidth(),
					   canvassrc.getHeight(),
					   GL_RGBA);
	
	waterdst.allocate(getSharedRenderTarget().getWidth()*.25,
					  getSharedRenderTarget().getHeight()*.25,
					  GL_RGB32F);
	watersrc.allocate(getSharedRenderTarget().getWidth()*.25,
					  getSharedRenderTarget().getHeight()*.25,
					  GL_RGB32F);
	
	waterdst.begin();
	ofClear(0,0,0,0);
	waterdst.end();
	
	watersrc.begin();
	ofClear(0,0,0,0);
	watersrc.end();
	
	canvasdst.begin();
	ofClear(0,0,0,0);
	canvasdst.end();
	
	canvassrc.begin();
	ofClear(0,0,0,0);
	canvassrc.end();
	
	meshFromFbo(canvasMesh, canvassrc);
	meshFromFbo(waterMesh, watersrc);
	
	paperRect = ofRectangle(0,0,paperImage.getWidth(),paperImage.getHeight());
	ofRectangle screenRect(0,0,canvassrc.getWidth(),canvassrc.getHeight());
	paperRect.scaleTo(screenRect, OF_ASPECT_RATIO_KEEP_BY_EXPANDING);
}

void CloudsVisualSystemGesturePaint::meshFromFbo(ofMesh& m, ofFbo& f){
	
	m.clear();
	
	m.addVertex(ofVec3f(0,0,0));
	m.addVertex(ofVec3f(f.getWidth(),0,0));
	m.addVertex(ofVec3f(0,f.getHeight(),0));
	m.addVertex(ofVec3f(f.getWidth(),f.getHeight(),0));
	
	m.addTexCoord(ofVec2f(0,0));
	m.addTexCoord(ofVec2f(f.getWidth(),0));
	m.addTexCoord(ofVec2f(0,f.getHeight()));
	m.addTexCoord(ofVec2f(f.getWidth(),f.getHeight()));
	
	m.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
}

void CloudsVisualSystemGesturePaint::createWaterBrush(){
    
	forceBrushMesh.clear();
	forceBrushMesh.addVertex(ofVec3f(0,0,0));
	forceBrushMesh.addVertex(ofVec3f(depositeScale,0,0));
	forceBrushMesh.addVertex(ofVec3f(0,depositeScale,0));
	forceBrushMesh.addVertex(ofVec3f(depositeScale,depositeScale,0));
	
	forceBrushMesh.addTexCoord(ofVec2f(-1,-1));
	forceBrushMesh.addTexCoord(ofVec2f(1,-1));
	forceBrushMesh.addTexCoord(ofVec2f(-1,1));
	forceBrushMesh.addTexCoord(ofVec2f(1,1));
	
	forceBrushMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
	currentDepositeScale = depositeScale;
}

void CloudsVisualSystemGesturePaint::createPaintBrush(){
	
	paintBrushMesh.clear();
	
	paintBrushMesh.addVertex(ofVec3f(-brushSize/2,-brushSize/2,0));
	paintBrushMesh.addVertex(ofVec3f( brushSize/2,-brushSize/2,0));
	paintBrushMesh.addVertex(ofVec3f(-brushSize/2, brushSize/2,0));
	paintBrushMesh.addVertex(ofVec3f( brushSize/2, brushSize/2,0));
	
	paintBrushMesh.addTexCoord(ofVec2f(0,0));
	paintBrushMesh.addTexCoord(ofVec2f(1,0));
	paintBrushMesh.addTexCoord(ofVec2f(0,1));
	paintBrushMesh.addTexCoord(ofVec2f(1,1));
	
	paintBrushMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
	currentBrushSize = brushSize;
}

//normal update call
void CloudsVisualSystemGesturePaint::selfUpdate(){
	if(!canvassrc.isAllocated() ||
	   canvassrc.getWidth() != getSharedRenderTarget().getWidth() ||
	   canvassrc.getHeight() != getSharedRenderTarget().getHeight())
	{
		reallocateFramebuffers();
	}
	
	if(currentDepositeScale != depositeScale){
		createWaterBrush();
	}
	if(currentBrushSize != brushSize){
		createPaintBrush();
	}
	
	glDisable(GL_DEPTH_TEST);
	ofSetColor(255);
	
	waterdst.begin();
	ofDisableAlphaBlending();
	ofClear(0, 0, 0, 0);
	
	hblurShader.begin();
	hblurShader.setUniformTexture("s_texture", watersrc.getTextureReference(), 1);
	hblurShader.setUniform2f("dimensions", waterdst.getWidth(), waterdst.getHeight());
	hblurShader.setUniform1f("dryRate", dryRate);
	hblurShader.setUniform1f("blurRadius", blurRadius);
	
	waterMesh.draw();
	hblurShader.end();
	waterdst.end();
	
	swap(watersrc,waterdst);
	
	waterdst.begin();
	ofDisableAlphaBlending();
	ofClear(0, 0, 0, 0);
	
	vblurShader.begin();
	vblurShader.setUniformTexture("s_texture", watersrc.getTextureReference(), 1);
	vblurShader.setUniform2f("dimensions", waterdst.getWidth(), waterdst.getHeight());
	vblurShader.setUniform1f("dryRate", dryRate);
	vblurShader.setUniform1f("blurRadius", blurRadius);
	
	waterMesh.draw();
	vblurShader.end();
	
	ofEnableAlphaBlending();
	
	forceBrushShader.begin();
	ofVec2f centerTranslate = ofVec2f(depositeScale*.5,depositeScale*.5);
    map<int, vector<ofVec2f> >::iterator it;
    for (it = playerDepositPoints.begin(); it != playerDepositPoints.end(); it++) {
        for(int i = 0; i <it->second.size(); i++){
            ofPushMatrix();
            ofTranslate(it->second[i]*.25 - centerTranslate);
            forceBrushMesh.draw();
            ofPopMatrix();
        }
    }
	forceBrushShader.end();
	waterdst.end();
	
	swap(watersrc,waterdst);
    
	canvasdst.begin();
	ofDisableAlphaBlending();
	ofClear(0, 0, 0, 0);
	paperMixShader.begin();
	paperMixShader.setUniformTexture("source_texture",
									 canvassrc.getTextureReference(), 1);
	paperMixShader.setUniformTexture("water_texture",
									 watersrc.getTextureReference(),  2);
	paperMixShader.setUniformTexture("flow_texture",
									 noiseFlowTex.getTextureReference(),  3);
	paperMixShader.setUniform2f("dimensions",
								canvassrc.getWidth(), canvassrc.getHeight());
	float flowSwap = fmod(ofGetElapsedTimef(), 2.0f);
	if(flowSwap > 1.0) flowSwap = 2.0 - flowSwap;
	
	paperMixShader.setUniform1f("flowSwap", 1.0);//looks stupid
	paperMixShader.setUniform1f("flowWidth", noiseFlowTex.getWidth());
	
	canvasMesh.draw();
	paperMixShader.end();
	
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetRectMode(OF_RECTMODE_CENTER);
	
	paintBrushShader.begin();
	paintBrushShader.setUniform1i("useColorPalette", palette.isAllocated() ? 1 : 0);
	paintBrushShader.setUniformTexture("brush", brushImage, 1);
	paintBrushShader.setUniform2f("brushDimensions",
								  brushImage.getWidth(),
								  brushImage.getHeight());
	if(palette.isAllocated()){
		palettePosition.x = palette.getWidth()/2.;
		palettePosition.y = fmod(ofGetElapsedTimef()*paletteTraversalSpeed,palette.getHeight());
        
		paintBrushShader.setUniformTexture("palette", palette, 2);
		paintBrushShader.setUniform2f("palettePosition",palettePosition.x,palettePosition.y);
		paintBrushShader.setUniform2f("paletteDimensions",
									  palette.getWidth(),
									  palette.getHeight());
		paintBrushShader.setUniform1f("paletteExpandPercent",
									  paletteExpandPercent);
	}
	else{
		ofSetColor(ofColor::fromHsb(fmod(ofGetElapsedTimef()*20, 255.f), 255.0f, 255.0f));
	}
	
    for (it = playerDepositPoints.begin(); it != playerDepositPoints.end(); it++) {
        for(int i = 0; i <it->second.size(); i++){
            ofPushMatrix();
            ofTranslate(it->second[i]);
            paintBrushMesh.draw();
            ofPopMatrix();
        }
    }
    
	paintBrushShader.end();
	
    for (it = playerDepositPoints.begin(); it != playerDepositPoints.end(); it++) {
        it->second.clear();
    }

	ofPopStyle();
	
	canvasdst.end();
	
	swap(canvasdst,canvassrc);
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemGesturePaint::selfDraw(){
}

// draw any debug stuff here
void CloudsVisualSystemGesturePaint::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemGesturePaint::selfDrawBackground(){
    
    if(!watersrc.isAllocated()){
        ofLogError("CloudsVisualSystemGesturePaint::selfDrawBackground") << "texture not allocated";
        return;
    }
    
	glDisable(GL_DEPTH_TEST);
	ofEnableAlphaBlending();
	paperImage.draw(paperRect);
	canvassrc.getTextureReference().draw(0,0);
	if(previewPalette && palette.isAllocated()){
		palette.draw(0, 0);
		ofPushStyle();
		ofSetRectMode(OF_RECTMODE_CENTER);
		ofNoFill();
		ofColor paletteColor = palette.getColor(palettePosition.x,palettePosition.y);
		ofSetColor(paletteColor);
		ofRect(palettePosition.x,palettePosition.y,
			   palette.getWidth()*paletteExpandPercent,
			   palette.getHeight()*paletteExpandPercent);
		ofFill();
		ofRect(palettePosition.x,palettePosition.y,5,5);
		ofNoFill();
		ofSetColor(ofColor::black);
		ofRect(palettePosition.x,palettePosition.y,5,5);
        
		ofPopStyle();
	}
	if(showWaterDebug){
        //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
		watersrc.getTextureReference().draw(getCanvasWidth()-watersrc.getWidth(),0);
	}
    
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemGesturePaint::selfEnd(){
	
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemGesturePaint::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemGesturePaint::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		reloadShader();
	}
}

void CloudsVisualSystemGesturePaint::selfKeyReleased(ofKeyEventArgs & args){
	
}



ofVec2f ofHermiteInterpolate(ofVec2f y0, ofVec2f y1, ofVec2f y2, ofVec2f y3, float pct, float tension, float bias){
	ofVec2f m0,m1;
	float pct2,pct3;
	float a0,a1,a2,a3;
	pct2 = pct * pct;
	pct3 = pct2 * pct;
	m0  = (y1-y0)*(1+bias)*(1-tension)/2;
	m0 += (y2-y1)*(1-bias)*(1-tension)/2;
	m1  = (y2-y1)*(1+bias)*(1-tension)/2;
	m1 += (y3-y2)*(1-bias)*(1-tension)/2;
	a0 =  2*pct3 - 3*pct2 + 1;
	a1 =  pct3 - 2*pct2 + pct;
	a2 =  pct3 - pct2;
	a3 =  -2*pct3 + 3*pct2;
	return(a0*y1 + a1*m0+a2*m1+a3*y2);
}

void CloudsVisualSystemGesturePaint::selfInteractionMoved(CloudsInteractionEventArgs& args){

    playerHistoryMap[args.playerId].push_back(ofVec2f(args.position.x,args.position.y));
    
	vector<ofVec2f> splineHandles;
	//make a spline
	if(playerHistoryMap[args.playerId].size() == 1){
		return; //draw next time
	}
	if(playerHistoryMap[args.playerId].size() == 2){
		splineHandles.push_back(playerHistoryMap[args.playerId][0]);
		splineHandles.push_back(playerHistoryMap[args.playerId][0]);
		splineHandles.push_back(playerHistoryMap[args.playerId][1]);
		splineHandles.push_back(playerHistoryMap[args.playerId][1]);
	}
	else if(playerHistoryMap[args.playerId].size() == 3){
		splineHandles.push_back(playerHistoryMap[args.playerId][0]);
		splineHandles.push_back(playerHistoryMap[args.playerId][0]);
		splineHandles.push_back(playerHistoryMap[args.playerId][1]);
		splineHandles.push_back(playerHistoryMap[args.playerId][2]);
	}
	else{
		for(int i = playerHistoryMap[args.playerId].size()-4; i < playerHistoryMap[args.playerId].size(); i++){
			splineHandles.push_back(playerHistoryMap[args.playerId][i]);
		}
	}
	
	float stepsize = ofMap(brushSize, 2, 200, .005, .1, true);
	
	for(float a = 0; a < 1.; a+=stepsize){
		playerDepositPoints[args.playerId].push_back(ofHermiteInterpolate(splineHandles[0],
                                                                          splineHandles[1],
                                                                          splineHandles[2],
                                                                          splineHandles[3], a, 0, 0));
	}
    
	if(playerHistoryMap[args.playerId].size() > 4){
		playerHistoryMap[args.playerId].erase(playerHistoryMap[args.playerId].begin());
	}
}


void CloudsVisualSystemGesturePaint:: selfInteractionStarted(CloudsInteractionEventArgs& args){}
void CloudsVisualSystemGesturePaint::selfInteractionDragged(CloudsInteractionEventArgs& args){}
void CloudsVisualSystemGesturePaint:: selfInteractionEnded(CloudsInteractionEventArgs& args){}

