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
    customGui->addToggle("Hashtags", &HASHTAGS);
    customGui->addToggle("Regenerate", &shouldRegenerate);
    
	customGui->addToggle("filled", &filled);
	customGui->addIntSlider("num circles", 20, 300, &numCircles);
	customGui->addSlider("hero percent", 0, 1.0, &heroPercent);
    customGui->addIntSlider("initial Number of Circles", 0, 200, &initialNumberofCircles);
	customGui->addSlider("small size 1",5, 100, &small1);
    customGui->addSlider("small size 2",5, 100, &small2);
	customGui->addSlider("large size 1",5, 100, &large1);
    customGui->addSlider("large size 2",5, 100, &large2);
	customGui->addSlider("color H Primary", 0., 1.0, &primaryColor.r);
    customGui->addSlider("color H Secondary", 0., 1.0, &secondaryColor.r);
	customGui->addSlider("color S Primary", 0., 1.0, &primaryColor.g);
    customGui->addSlider("color S Secondary", 0., 1.0, &secondaryColor.g);
	customGui->addSlider("color V Primary", 0., 1.0, &primaryColor.b);
    customGui->addSlider("color V Secondary", 0., 1.0, &secondaryColor.b );
	customGui->addSlider("alpha", 0, 1.0, &alph);

//	customGui->addSlider("secondary color H", 0., 1.0, &secondaryColor.r);
//	customGui->addSlider("secondary color S", 0., 1.0, &secondaryColor.g);
//	customGui->addSlider("secondary color V", 0., 1.0, &secondaryColor.b);
	
   // customGui->addSlider("Deviation", 0., 1.0, &deviation);

    
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
        BLANKS = false;
        HASHTAGS = false;

    }
    
    else if (e.widget->getName() == "BlankCircles" && ((ofxUIToggle *)e.widget)->getValue()) {
        BLANKS = true;
        HASHTAGS = false;
        NASDAQ = false;
    }
    
    else if (e.widget->getName() == "Hashtags" && ((ofxUIToggle *)e.widget)->getValue()) {
        HASHTAGS = true;
        NASDAQ = false;
        BLANKS = false;
    }

    else if (e.widget->getName() == "Regenerate" && ((ofxUIToggle *)e.widget)->getValue()) {
        regenerate();
    }
    
//    if(e.widget->getName() == "color H" || e.widget->getName() == "color S" || e.widget->getName() == "color V")
//	{
//		ofFloatColor col;
//		col.setHsb(primaryColor.r, primaryColor.g, primaryColor.b);
//		customGui->getWidget("color H")->setColorFill(col);
//		customGui->getWidget("color S")->setColorFill(col);
//		customGui->getWidget("color V")->setColorFill(col);
//	}
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
    
    //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
    pack = CirclePacker(1.0f*getCanvasWidth(),1.0f*getCanvasHeight(), 12);
    
    if (NASDAQ){
        initializeNasdaq();
    }
    else if (BLANKS){
        
        initializeBlanks();
    }
    
    else if (HASHTAGS){
        
        initializeHashtags();
    }
    
	pack.pack();
    
    
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemCirclePacking::selfSetup(){
	
   // Circle::Font.loadFont(getVisualSystemDataPath() + "Verdana14.ttf", 14, true, true, true);
    Circle::Font.loadFont(getVisualSystemDataPath() + "Verdana14.ttf", 14);
    
    //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
	pack = CirclePacker(1.0f*getCanvasWidth(),1.0f*getCanvasHeight(), 15);
    
    if (NASDAQ){
        initializeNasdaq();
    }
    else if (BLANKS){
    
        initializeBlanks();
    }
    
    else if (HASHTAGS){
        
        initializeHashtags();
    }
    
    alph = 1.f;
    
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

//	for (int i = 0; i<pack.circles.size(); i++){
//		cout << "circle radius : " << pack.circles[i].r << endl;
//	}
    
	ofPushStyle();
    ofEnableAlphaBlending();
    if(!filled){
        ofNoFill();
    }
    pack.draw(NASDAQ, BLANKS, HASHTAGS);
    ofDisableAlphaBlending();
	ofPopStyle();
	
	//turn the background refresh off
	//bClearBackground = false;
	
}

void CloudsVisualSystemCirclePacking::initializeBlanks(){
    
    for(int i = 0; i < initialNumberofCircles; i++){
		if(ofRandomuf() > .9){
            
            //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
			pack.circles.push_back( Circle(ofRandom(getCanvasWidth()),
										   ofRandom(getCanvasHeight()),
										   ofMap(powf(ofRandomuf(), 3.), 0, 1.0,
												 large1, large2), " ", primaryColor, secondaryColor, alph ));
		}
		else{
            //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
			pack.circles.push_back( Circle(ofRandom(getCanvasWidth()),
										   ofRandom(getCanvasHeight()),
										   ofMap(powf(ofRandomuf(), 3.), 0.,1.0,
                                                 small1, small2), " ", primaryColor, secondaryColor, alph  ));
        }
    }
}

void CloudsVisualSystemCirclePacking::initializeNasdaq(){
    
    
    string companies[50] = { "SQR", "ZNGA","ALBB","BIDU", "SMCI", "SSYS", "NCR", "DELL", "FTNT","RAX","VMW", "EQUIX", "XXIA", "CTSH", "OPEN", "SFLY", "SSTK","EPAM", "IPGP", "DDD","YHOO", "TWTR","MSFT","FB","IBM","GOOG","HPQ","AMZN","EBAY","AAPL","CSCO","SNE","NOK","INTC","NVDA","AMD","ADBE","EA","FSQR", "LNKD", "YELP", "TMBL","SNAP", "BNCH", "LPMT","LLBT","LYFT", "GIT", "AOL", "OVR" };
    float marketCap[50] = { 4.1, 4.11, 149, 59.9, .700, 5.85, 5.37, 24.38, 2.89, 4.88, 36.9, 8.4, .942, 28.3, 1.78, 1.82, 2.62, 1.51, 3.75, 8.32, 40.3, 137, 306, 130, 187.6, 354.4, 51.345, 175.9, 66.736, 498.8, 108.2, 18.169, 27.991, 120.7, 8.539, 2.670, 30.438, 6.869, 1, 25.7, 4.23, 2, 3, 4, 5, 5, 4, 6, 5, 3 };
	
    int size = sizeof(marketCap)/sizeof(marketCap[0]);
    
    for(int i = 0; i < size; i++){
        //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
        pack.circles.push_back( Circle(ofRandom(getCanvasWidth()),ofRandom(getCanvasHeight()), (marketCap[i]/2), companies[i], primaryColor, secondaryColor, alph  ));
        
        }
    }

void CloudsVisualSystemCirclePacking::initializeHashtags(){
    
    
    string hashtags[10] = { "IMHO", "Jacked-in", "Meatspace", "cyberbully", "datasexual", "Grrrl", "Hater", "Troll", "illiterati",
    "Infomania"};
	
    int size = sizeof(hashtags)/sizeof(hashtags[0]);
    
    for(int i = 0; i < 10; i++){
        //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
        pack.circles.push_back( Circle(ofRandom(getCanvasWidth()),ofRandom(getCanvasHeight()), ofRandom(40,100), hashtags[i], primaryColor, secondaryColor, alph ));
        cout << "word " << i << hashtags[i] << endl;
        
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

void CloudsVisualSystemCirclePacking::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCirclePacking::selfInteractionDragged(CloudsInteractionEventArgs& args){
    packThemCircles(ofVec2f(args.position.x, args.position.y));

}

void CloudsVisualSystemCirclePacking::selfInteractionMoved(CloudsInteractionEventArgs& args){
    packThemCircles(ofVec2f(args.position.x, args.position.y));
}

void CloudsVisualSystemCirclePacking::packThemCircles(ofVec2f data){
    
    if (BLANKS){
        if(ofRandomuf() > .9){
            
            //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
            pack.circles.push_back( Circle(data.x,
                                           data.y,
                                           ofMap(powf(ofRandomuf(), 3.), 0, 1.0,
                                                 large1, large2), " ", primaryColor, secondaryColor, alph ));
        }
        else{
            //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
            pack.circles.push_back( Circle(data.x,
                                           data.y,
                                           ofMap(powf(ofRandomuf(), 3.), 0.,1.0,
                                                 small1, small2), " ", primaryColor, secondaryColor, alph  ));
        }
    }
}

void CloudsVisualSystemCirclePacking::selfMousePressed(ofMouseEventArgs& data){
    
    string hashtags[105] = { "IMHO", "jacked-in", "meatspace", "cyberbully", "datasexual", "Grrrl", "Hater", "Troll",
        "infomania", "muggle", "Noob", "Bit Bucket", "OBVS", "yolo", "leetspeak", "OMFG", "Twerk", "Tweeps", "Troll",
        "screenager", "Sexting", "totes", "unfriend", "Cloud Computing", "upcycle", "whatevs", "woot", "lurker", "AFIK", "WYSIWYG", "NSFW", "PONE", "Frag", "Leak", "Meme", "<3", "hashtag", "emojinal", "Zerg Rush", "Trollface", "Doge", "ermahgerd", "Y U NO Guy", "Okay Guy", "F*ck Yea", "DERP","philosoraptor", "Do You Even Lift?","pedobear", "Nyan Cat", "Lulz", "Xzibit Yo", "Grumpy Cat", "awkward penguin", "i know that feel bro", "facepalm", "deal with it", "Fap", "impossibru", "U MAD?", "oh god why", "i see what you did there", "argument invalid", "creeper", "Business Cat", "Swag", "divide by zero", "O RLY?", "technohonks", "googlization", "unlike", "unhate", "filter bubble", "fuckparade", "figwit", "4chan", "all your base", "anonymous", "poke", "ping", "n00b", "flashmob", "hampster dance", "in real life", "flame-war", "WDUWTA", "thx", "ttyl", "rotflol", "p2p", "GAGF", "griefer", "googlish", "l337", "bloggage", "less than three", "warez", "d00d", "intertubes", "btw", "nyfb", "otaku", "netizen", "larping", "hacking into mainframe"} ;
	
    int size = sizeof(hashtags)/sizeof(hashtags[0]);
    
    if (HASHTAGS){
        pack.circles.push_back( Circle(data.x,
                                       data.y,
                                       ofMap(powf(ofRandomuf(), 5.), 0. ,1.0,
                                             70, 200), hashtags[int(ofRandom(size))],primaryColor, secondaryColor, alph));
    }

}