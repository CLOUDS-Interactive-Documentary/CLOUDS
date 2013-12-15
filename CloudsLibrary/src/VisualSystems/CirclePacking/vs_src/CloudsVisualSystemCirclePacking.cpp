//
//  CloudsVisualSystemCirclePacking.cpp
//



#include "CloudsVisualSystemCirclePacking.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//float CloudsVisualSystemCirclePacking::NCircles = 50;

//These methods let us add custom GUI parameters and respond to their events

void CloudsVisualSystemCirclePacking::selfSetupGui(){

	
    customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

    customGui->addToggle("BlankCircles", &BLANKS);
    customGui->addToggle("Nasdaq", &NASDAQ);
    customGui->addToggle("Regenerate", &shouldRegenerate);
    
	customGui->addToggle("filled", &filled);
	customGui->addIntSlider("num circles", 20, 300, &numCircles);
	customGui->addSlider("hero percent", 0, 1.0, &heroPercent);
	customGui->addRangeSlider("small size",5, 100, &smallSizeRange.min, &smallSizeRange.max);
	customGui->addRangeSlider("large size",5, 100, &largeSizeRange.min, &largeSizeRange.max);
	
	customGui->addSlider("primary color H", 0., 1.0, &primaryColor.r);
	customGui->addSlider("primary color S", 0., 1.0, &primaryColor.g);
	customGui->addSlider("primary color V", 0., 1.0, &primaryColor.b);

	customGui->addSlider("secondary color H", 0., 1.0, &secondaryColor.r);
	customGui->addSlider("secondary color S", 0., 1.0, &secondaryColor.g);
	customGui->addSlider("secondary color V", 0., 1.0, &secondaryColor.b);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemCirclePacking::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
	
}

void CloudsVisualSystemCirclePacking::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
    
    else if (e.widget->getName() == "Nasdaq" && ((ofxUIToggle *)e.widget)->getValue()) {
        NASDAQ = true;
    }
    
    else if (e.widget->getName() == "BlankCircles" && ((ofxUIToggle *)e.widget)->getValue()) {
        BLANKS = true;
    }

    else if (e.widget->getName() == "Regenerate" && ((ofxUIToggle *)e.widget)->getValue()) {
        regenerate();
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemCirclePacking::selfSetupSystemGui(){
	
}

void CloudsVisualSystemCirclePacking::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemCirclePacking::selfSetupRenderGui(){

}

void CloudsVisualSystemCirclePacking::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemCirclePacking::regenerate(){
 
    pack.circles.clear();
    
    pack = CirclePacker(1.0f*ofGetWidth(),1.0f*ofGetHeight());
    
    if (NASDAQ){
        initializeNasdaq();
    }
    else if (BLANKS){
        
        initializeBlanks();
    }
    
	pack.pack();
    
    
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemCirclePacking::selfSetup(){
	
    Circle::Font.loadFont(getVisualSystemDataPath() + "Verdana14.ttf", 14, true, true, true);
    

    
	pack = CirclePacker(1.0f*ofGetWidth(),1.0f*ofGetHeight());
    
    if (NASDAQ){
        initializeNasdaq();
    }
    else if (BLANKS){
    
        initializeBlanks();
    }

	pack.pack();
		
}   

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemCirclePacking::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemCirclePacking::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemCirclePacking::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemCirclePacking::selfUpdate(){
    
   
   pack.update();

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemCirclePacking::selfDraw(){
	
   

	
}

// draw any debug stuff here
void CloudsVisualSystemCirclePacking::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemCirclePacking::selfDrawBackground(){

	for (int i = 0; i<pack.circles.size(); i++){
		cout << "circle radius : " << pack.circles[i].r << endl;
	}
    
	ofPushStyle();
	ofNoFill();
    pack.draw(NASDAQ, BLANKS);
	ofPopStyle();
	
	//turn the background refresh off
	//bClearBackground = false;
	
}

void CloudsVisualSystemCirclePacking::initializeBlanks(){
    
    for(int i = 0; i < 200; i++){
		if(ofRandomuf() > .9){
			pack.circles.push_back( Circle(ofRandom(ofGetWidth()),
										   ofRandom(ofGetHeight()),
										   ofMap(powf(ofRandomuf(), 3.), 0.,1.0,
												 30, 60.), "blank") );
		}
		else{
			pack.circles.push_back( Circle(ofRandom(ofGetWidth()),
										   ofRandom(ofGetHeight()),
										   ofMap(powf(ofRandomuf(), 3.), 0.,1.0,
                                                 5, 10), "blank") );
        }
    }
}

void CloudsVisualSystemCirclePacking::initializeNasdaq(){
    
    
    string companies[50] = { "SQR", "ZNGA","ALBB","BIDU", "SMCI", "SSYS", "NCR", "DELL", "FTNT","RAX","VMW", "EQUIX", "XXIA", "CTSH", "OPEN", "SFLY", "SSTK","EPAM", "IPGP", "DDD","YHOO", "TWTR","MSFT","FB","IBM","GOOG","HPQ","AMZN","EBAY","AAPL","CSCO","SNE","NOK","INTC","NVDA","AMD","ADBE","EA","FSQR", "LNKD", "YELP", "TMBL","SNAP", "BNCH", "LPMT","LLBT","LYFT", "GIT", "AOL", "OVR" };
    float marketCap[50] = { 4.1, 4.11, 149, 59.9, .700, 5.85, 5.37, 24.38, 2.89, 4.88, 36.9, 8.4, .942, 28.3, 1.78, 1.82, 2.62, 1.51, 3.75, 8.32, 40.3, 137, 306, 130, 187.6, 354.4, 51.345, 175.9, 66.736, 498.8, 108.2, 18.169, 27.991, 120.7, 8.539, 2.670, 30.438, 6.869, 1, 25.7, 4.23, 2, 3, 4, 5, 5, 4, 6, 5, 3 };
	
    int size = sizeof(marketCap)/sizeof(marketCap[0]);
    
    for(int i = 0; i < size; i++){
        pack.circles.push_back( Circle(ofRandom(ofGetWidth()),ofRandom(ofGetHeight()), (marketCap[i]/2), companies[i]));
        
        }
    }

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemCirclePacking::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemCirclePacking::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemCirclePacking::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemCirclePacking::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemCirclePacking::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCirclePacking::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCirclePacking::selfMousePressed(ofMouseEventArgs& data){
    
    pack.circles.push_back( Circle(data.x,
                                   data.y,
                                   ofMap(powf(ofRandomuf(), 5.), 0. ,1.0,
                                         10, 80.), "blank") );

	
}

void CloudsVisualSystemCirclePacking::selfMouseReleased(ofMouseEventArgs& data){
	
}