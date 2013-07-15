#include "CloudsVisualSystemLia.h"


//--------------------------------------------------------------
string CloudsVisualSystemLia::getSystemName(){
	return "Lia";
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfSetup(){
    
    
    bClearBackground = false; //turn off background refresh
    
    // ---------------------------------------------- circle res
    ofSetCircleResolution(200);
    
    // ---------------------------------------------- set up color palettes
	paletteImage.loadImage(getVisualSystemDataPath() +  "_images/theimage.jpg");
	globals.theColors = new ColorPalette();
	globals.theColors->setup(paletteImage);
    
    
    float currentScreenSize = ofGetWidth()+ofGetHeight();
    num = int(  ofMap( currentScreenSize ,  (1024+768), (1920+1080), 9,  11)  );
     // ofLogNotice()<<"num: " << num;
   
       
    // ---------------------------------------------- setup modules AFTER the GUI values are set !
	for (int i = 0; i < num; i++){
		modules.push_back(Module());
        modules[i].setup(i, num, &modules);
	}
    
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfSetupGuis(){
	
}


void CloudsVisualSystemLia::selfPresetLoaded(string presetPath){
    
}


//--------------------------------------------------------------
void CloudsVisualSystemLia::selfSetupSystemGui(){
	
	// sysGui->addSlider("num circles", 0, 1000, &numCircles);
	// sysGui->addSlider("max radius", 0, 1000, &maxRadius);
    
    
    // ---------------------------------------------- max arc rad of modules 
    maxRadius = 0.5; // 200
    sysGui->addSlider("maxArcRadius", 0, 1, &moduleArcRad);
    
    // ---------------------------------------------- mover speed
    moverSpeed = 0.5; // pSpeed: 35
    sysGui->addSlider("moverSpeed", 0, 1, &moverSpeed);
    
    // ---------------------------------------------- size of tiny crosses
    moduleSizeOfTinyCross = 0.5; // crossSize: 1
    sysGui->addSlider("crossSize", 0, 1, &moduleSizeOfTinyCross);
    
    // ----------------------------------------------  arcAlpha
    moduleArcAlpha = 0.23;  // arcAlpha: 80
    sysGui->addSlider("arcAlpha", 0, 1, &moduleArcAlpha);
    
    // ----------------------------------------------  lightsStrokeWidth
    moduleLightsStrokeWidth = 1; // LightsStrokeWidth: 20
    sysGui->addSlider("lightsStrokeWidth", 0, 1, &moduleLightsStrokeWidth);
    
    // ----------------------------------------------  lightsStrokeAlpha
    moduleLightsStrokeAlpha = 0.3;  // lightsStrokeAlpha: 3
    sysGui->addSlider("lightsStrokeAlpha", 0, 1, &moduleLightsStrokeAlpha);
    
    // ----------------------------------------------  arcLenBase
    moduleArcLenBase = 0.66; // arcLenBase: ~120
    sysGui->addSlider("arcLenBase", 0, 1, &moduleArcLenBase); 
    
    // ----------------------------------------------  arcLenRandom
    moduleArcLenRandom = 0.47; // AarcLenRandom: ~50
    sysGui->addSlider("arcLenRandom", 0, 1, &moduleArcLenRandom); 
    
    // ---------------------------------------------- arcRotationRandom
    moduleArcRotationRandom = 0.;
    sysGui->addSlider("arcRotationRandom", 0, 1, &moduleArcRotationRandom); 
    
    // ---------------------------------------------- fakeMouseX
    float defaultFakeMouseX = 0;
    for (int i = 0; i < num; i++){
        modules[i].setPrevFakeMouseX(defaultFakeMouseX);
    }
    
    moduleFakeMouseX = defaultFakeMouseX;
    sysGui->addSlider("fakeMouseX", 0, 1, &moduleFakeMouseX); 
    
    // ---------------------------------------------- moduleKeepTheCircleSize
    moduleKeepTheCircleSize = 0.07; // keepTheCircleSize: 7
    sysGui->addSlider("moduleKeepTheCircleSize", 0, 1, &moduleKeepTheCircleSize); 
    
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfUpdate(){
    
    // ---------------------------------------------- update modules
    for (int i = 0; i < num; i++){
        modules[i].update();
    }
    
    for (int i = 0; i < num; i++){
        modules[i].setModuleMaxArcRad(moduleArcRad);
        modules[i].setSizeOfTinyCross(moduleSizeOfTinyCross);
        modules[i].mover.setSpeed(moverSpeed);
        modules[i].setLightsStrokeWidth(moduleLightsStrokeWidth);
        modules[i].setLightsStrokeAlpha(moduleLightsStrokeAlpha);
        modules[i].setArcLenBase(moduleArcLenBase);
        modules[i].setArcLenRandom(moduleArcLenRandom);
        modules[i].setArcRotationRandom(moduleArcRotationRandom);
        modules[i].setFakeMouseX(moduleFakeMouseX);
        modules[i].setKeepTheCircleSize(moduleKeepTheCircleSize);
        modules[i].setArcAlpha(moduleArcAlpha);
    }
    
    
    
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfDrawBackground(){
    
	//Use draw background for 2D drawing
	glDisable(GL_DEPTH_TEST); //  moved to setup
    ofEnableBlendMode(OF_BLENDMODE_SUBTRACT );
	
    // ---------------------------------------------- fade screen to black
    if( (ofGetFrameNum() % 7) == 0 ){
        ofSetColor(1,1,1,255);
        ofRect(0,0, ofGetWidth(), ofGetHeight());
    }
   	
    
	ofEnableAlphaBlending();
    ofEnableSmoothing();
    // ---------------------------------------------- draw modules
    for (int i = 0; i < num; i++){
        modules[i].draw();
    }    
    
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfDrawDebug(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfSceneTransformation(){
    
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfDraw(){
    //use this for 3D drawings
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfExit(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfBegin(){
    
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfEnd(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfKeyPressed(ofKeyEventArgs & args){
    
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfKeyReleased(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfMouseDragged(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfMousePressed(ofMouseEventArgs& data){
    
    // ---------------------------------------------- setMousePos: changes the color inside the module class 
    for (int i = 0; i < num; i++){
        modules[i].setHadInteraction(true);
        modules[i].setMousePosAndResetMoverPosAndColor(data.x);
    }
	
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfMouseReleased(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfSetupGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfGuiEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::guiSystemEvent(ofxUIEventArgs &e){
    
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::selfSetupRenderGui(){
}

//--------------------------------------------------------------
void CloudsVisualSystemLia::guiRenderEvent(ofxUIEventArgs &e){
	
}
