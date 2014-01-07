//
//  CloudsVisualSystemCode.cpp
//

#include "CloudsVisualSystemCode.h"
#include "CloudsGlobal.h"
#include "ofxMtlBoxFitting.h"

bool rectareasort(ofRectangle a, ofRectangle b){
	return a.getArea() > b.getArea();
}

bool rectsort(ofRectangle a, ofRectangle b){
	return a.x < b.x;
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemCode::selfSetupGui(){

	
	////////////PANELS
	panelsGui = new ofxUISuperCanvas("PANELS", gui);
	panelsGui->copyCanvasStyle(gui);
	panelsGui->copyCanvasProperties(gui);
	panelsGui->setName("Panels");
	panelsGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	panelsGui->addToggle("Enable CODE", &bEnableCode);
	panelsGui->addToggle("Enable CONSOLE", &bEnableConsole);
	panelsGui->addToggle("Enable TENPRINT", &bEnableTenPrint);
	
	ofAddListener(panelsGui->newGUIEvent, this, &CloudsVisualSystemCode::selfGuiEvent);
	guis.push_back(panelsGui);
	guimap[panelsGui->getName()] = panelsGui;
	////////////PANELS
	
	////////////TYPE
	typeGui = new ofxUISuperCanvas("TYPE", gui);
	typeGui->copyCanvasStyle(gui);
	typeGui->copyCanvasProperties(gui);
	typeGui->setName("Type");
	typeGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

	typeGui->addIntSlider("FONT SIZE", 4, 12, &fontSize);
	typeGui->addSlider("TRACKING", 0., 3.0, &typeTracking);
	typeGui->addSlider("LEFT MARGIN", 0, 20, &typeMarginLeft);
	
	ofAddListener(typeGui->newGUIEvent, this, &CloudsVisualSystemCode::selfGuiEvent);
	guis.push_back(typeGui);
	guimap[typeGui->getName()] = typeGui;
	//////////END TYPE
	
	//////////BOXES
	boxGui = new ofxUISuperCanvas("BOXES", gui);
	boxGui->copyCanvasStyle(gui);
	boxGui->copyCanvasProperties(gui);
	boxGui->setName("Boxes");
	boxGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

	boxGui->addSlider("WIDTH", 40, 300, &maxwidth);
	boxGui->addSlider("HEIGHT", 40, 500, &maxheight);
	//any width/height smaller thin this will become a widget
	boxGui->addIntSlider("min subdivision size", 4, 200, &minsize);
	boxGui->addSlider("min code window size", 40, 500, &minTextboxSize);
	boxGui->addIntSlider("box gen seed", 1, 300, &boxSeed);
	boxGui->addIntSlider("box gen seed 2", 1, 300, &boxSeed2);
	boxGui->addSlider("outline alpha", 0, 1.0, &outlineAlpha);
	
	ofAddListener(boxGui->newGUIEvent, this, &CloudsVisualSystemCode::selfGuiEvent);
	guis.push_back(boxGui);
	guimap[boxGui->getName()] = boxGui;
	//////////BOXES
	   
	//////////ANIMATION
	animationGUi = new ofxUISuperCanvas("ANIMATION", gui);
	animationGUi->copyCanvasStyle(gui);
	animationGUi->copyCanvasProperties(gui);
	animationGUi->setName("Animation");
	animationGUi->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	animationGUi->addSlider("START TIME RANGE", 0, 4, &startTimeDelay);
	animationGUi->addRangeSlider("EXPAND SPEED RANGE", 0, 2, &expandSpeed.min, &expandSpeed.max);
	animationGUi->addRangeSlider("SPEED RANGE", .01, 1, &speedRange.min, &speedRange.max);
	animationGUi->addSlider("10PRINT SPEED", 0, 50, &tenPrintSpeedBoost);
	animationGUi->addToggle("CASCADE", &bCascadeMode);
	
	ofAddListener(animationGUi->newGUIEvent, this, &CloudsVisualSystemCode::selfGuiEvent);
	guis.push_back(animationGUi);
	guimap[animationGUi->getName()] = animationGUi;
	//////////ANIMATION
	
	//////////COLOR
	colorGui = new ofxUISuperCanvas("COLOR", gui);
	colorGui->copyCanvasStyle(gui);
	colorGui->copyCanvasProperties(gui);
	colorGui->setName("Color");
	colorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	baseColorLabel = colorGui->addLabel("TEXT");
	colorGui->addMinimalSlider("TEXT COLOR H", 0, .999, &tintColorHSV.r);
	colorGui->addMinimalSlider("TEXT COLOR S", 0, .999, &tintColorHSV.g);
	colorGui->addMinimalSlider("TEXT COLOR B", 0, .999, &tintColorHSV.b);

	string colorNames[MATCH_TYPES];
	colorNames[0] = "comments";
	colorNames[1] = "strings";
	colorNames[2] = "base types";
	colorNames[3] = "preprocessors";
	colorNames[4] = "keywords";

	for(int i = 0; i < MATCH_TYPES; i++){
		ofxUILabel* lb = colorGui->addLabel("color " + ofToString(i+1) + " " +colorNames[i] );
		colorGui->addMinimalSlider("COLOR " + ofToString(i+1) + " H", 0, .999, &matchColorTypesHSV[i].r);
		colorGui->addMinimalSlider("COLOR " + ofToString(i+1) + " S", 0, .999, &matchColorTypesHSV[i].g);
		colorGui->addMinimalSlider("COLOR " + ofToString(i+1) + " B", 0, .999, &matchColorTypesHSV[i].b);
		textColorLabels.push_back(lb);
	}
	
	ofAddListener(colorGui->newGUIEvent, this, &CloudsVisualSystemCode::selfGuiEvent);
	guis.push_back(colorGui);
	guimap[colorGui->getName()] = colorGui;
	//////////COLOR
}

void CloudsVisualSystemCode::selfGuiEvent(ofxUIEventArgs &e){
	//change the gui color on slider change
	
	if(e.widget->getName().find("COLOR") != string::npos) {
		updateColors();
		for(int i = 0; i < textColorLabels.size(); i++){
			textColorLabels[i]->setColorFill( matchColorTypes[i] );
		}
		baseColorLabel->setColorFill(tintColor);
	}
}

void CloudsVisualSystemCode::generatePanels(){

	populatePanels(panelsFront, boxSeed);
	populatePanels(panelsBack, boxSeed2);
	
	generatedPanelWidth = getCanvasWidth();
	generatedPanelHeight = getCanvasHeight();
	regeneratePanels = false;
	
	cout << "generating panels" << endl;
}


//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemCode::populatePanels(vector<Panel*>& panels, int seed){
	
	for(int i = 0; i < panels.size(); i++){
		delete panels[i];
	}
	panels.clear();
	
	ofxMtlBoxFitting boxFitting;
	boxFitting.setup(getCanvasWidth(),getCanvasHeight(),
					 maxwidth, maxheight, minsize);
	
	boxFitting.generate(seed);
	
	vector<ofRectangle> rectTests;
	ofRectangle allRects;
	for(int i = 0; i < boxFitting.boxes.size(); i++) {
		ofRectangle r = ofRectangle(boxFitting.boxes[i].x,
									boxFitting.boxes[i].y,
									boxFitting.boxes[i].w,
									boxFitting.boxes[i].h);
		r.x += 1;
		r.y += 1;
		r.width  -= 2;
		r.height -= 2;
		
		allRects.growToInclude(r);
		
		rectTests.push_back(r);
	}
	
	ofVec2f topOffset = ofVec2f((getCanvasWidth() - allRects.width) / 2.0,
								(getCanvasHeight() - allRects.height) / 2.0);
	for(int i = 0; i < rectTests.size(); i++){
		rectTests[i].translate(topOffset);
	}
	sort(rectTests.begin(), rectTests.end(), rectsort);
	
	vector<ofRectangle> rectCopies = rectTests;
	sort(rectCopies.begin(), rectCopies.end(), rectareasort);
	
	bool canHasCode = bEnableCode;
	for(int i = 0; i < rectTests.size(); i++){
		Panel* p;
		if(canHasCode && rectTests[i].getArea() >= rectCopies[0].getArea()){
			p = new PanelCode();
			canHasCode = false;
		}
		else{
			if(bEnableConsole && bEnableTenPrint){
				if(ofRandomuf() > .5){
					p = new PanelConsole();
				}
				else {
					p = new PanelTenPrint();
					p->speedBoost = &tenPrintSpeedBoost;					
				}
			}
			else if(bEnableConsole){
				p = new PanelConsole();
			}
			else{
				p = new PanelTenPrint();
				p->speedBoost = &tenPrintSpeedBoost;
			}
		}
		
		p->bgColor = &bgColor;
		p->dataPath = getVisualSystemDataPath();
		p->sharedFont = &sharedFont;
		p->sharedLayout = &sharedLayout;
		
		p->startTimeDelay = &startTimeDelay;
		p->expandSpeed = &expandSpeed;

		p->outlineAlpha = &outlineAlpha;
		p->marginLeft = &typeMarginLeft;
		
		p->scanSpeed = powf(ofRandom(speedRange.min,speedRange.max),2);
		p->drawRect = rectTests[i];
		p->setup();
		
		panels.push_back(p);
	}
	
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemCode::selfSetupSystemGui(){
	
}

void CloudsVisualSystemCode::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemCode::selfSetupRenderGui(){

}

void CloudsVisualSystemCode::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemCode::selfSetDefaults(){
	
	regeneratePanels = false;
	fontSize = 5;
	currentFontSize = -1;
	generatedPanelWidth = -1;
	generatedPanelHeight = -1;
	
	bEnableCode = true;
	bEnableConsole = true;
	bEnableTenPrint = true;


	boxSeed  =   0;
	boxSeed2 = 100;
	
	typeMarginLeft = 5;
	
	tenPrintSpeedBoost = 20;
	
	startTimeDelay  =  .3;
	expandSpeed.min =  .5;
	expandSpeed.max = 1.0;
	speedRange.min  =  .4;
	speedRange.max  =  .9;
	
	bCascadeMode = false;
	
}
// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemCode::selfSetup(){
	matchColorTypes.resize(MATCH_TYPES);
	regeneratePanels = true;
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemCode::selfPresetLoaded(string presetPath){
	regeneratePanels = true;
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemCode::selfBegin(){
	for(int i = 0; i < panelsFront.size(); i++){
		panelsFront[i]->startAnimation();
	}
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemCode::selfSceneTransformation(){
	
}

void CloudsVisualSystemCode::updateColors(){
	
	for(int i = 0; i < MATCH_TYPES; i++){
		matchColorTypes[i] = ofFloatColor::fromHsb(matchColorTypesHSV[i].r,
												   matchColorTypesHSV[i].g,
												   matchColorTypesHSV[i].b);
	}
	
	tintColor = ofFloatColor::fromHsb(tintColorHSV.r,
									  tintColorHSV.g,
									  tintColorHSV.b);
}

//normal update call
void CloudsVisualSystemCode::selfUpdate(){
	
	if(currentFontSize != fontSize){
		sharedFont.loadFont(GetCloudsDataPath() + "font/Consolas.ttf", fontSize);
		sharedLayout.loadFont(GetCloudsDataPath() + "font/Consolas.ttf", fontSize);
		currentFontSize = fontSize;
	}
	
	sharedFont.setTracking(typeTracking);
	
	if(regeneratePanels ||
	   getCanvasWidth() != generatedPanelWidth ||
	   getCanvasHeight() != generatedPanelHeight )
	{
		generatePanels();
		for(int i = 0; i < panelsFront.size(); i++){
			panelsFront[i]->startAnimation();
		}
	}
	
	updateColors();
	for(int i = 0; i < panelsBack.size(); i++){
		panelsBack[i]->tint = tintColor;
		panelsBack[i]->matchColorTypes = matchColorTypes;
		panelsBack[i]->update();
	}
	
	bool allFinished = true;
	for(int i = 0; i < panelsFront.size(); i++){
		panelsFront[i]->tint = tintColor;
		panelsFront[i]->matchColorTypes = matchColorTypes;
		panelsFront[i]->update();
		allFinished &= panelsFront[i]->isDone();
	}
	
	if(bCascadeMode && allFinished){
		for(int i = 0; i < panelsBack.size(); i++){
			panelsBack[i]->resetAnimation();
		}
		swap(panelsBack, panelsFront);
		for(int i = 0; i < panelsFront.size(); i++){
			panelsFront[i]->startAnimation();
		}
	}
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemCode::selfDraw(){
	//TODO: draw 3d
}

// draw any debug stuff here
void CloudsVisualSystemCode::selfDrawDebug(){
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemCode::selfDrawBackground(){

	if(bCascadeMode){
		for(int i = 0; i < panelsBack.size(); i++){
			panelsBack[i]->draw( getCanvasHeight() );
		}
	}
	
	for(int i = 0; i < panelsFront.size(); i++){
		panelsFront[i]->draw( getCanvasHeight() );
	}
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemCode::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delete anything you made in setup
void CloudsVisualSystemCode::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemCode::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		regeneratePanels = true;
	}
}
void CloudsVisualSystemCode::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemCode::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCode::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCode::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCode::selfMouseReleased(ofMouseEventArgs& data){
	
}