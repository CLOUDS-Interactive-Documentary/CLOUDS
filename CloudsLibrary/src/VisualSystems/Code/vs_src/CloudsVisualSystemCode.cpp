//
//  CloudsVisualSystemCode.cpp
//

#include "CloudsVisualSystemCode.h"
#include "CloudsGlobal.h"
#include "ofxMtlBoxFitting.h"

bool rectsort(ofRectangle a, ofRectangle b){
	return a.x < b.x;
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemCode::selfSetupGui(){

	typeGui = new ofxUISuperCanvas("TYPE", gui);
	typeGui->copyCanvasStyle(gui);
	typeGui->copyCanvasProperties(gui);
	typeGui->setName("Type");
	typeGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	typeGui->addRangeSlider("SPEED RANGE", .01, 1, &speedRange.min, &speedRange.max);
	typeGui->addIntSlider("FONT SIZE", 4, 12, &fontSize);
	typeGui->addSlider("TRACKING", 0., 3.0, &typeTracking);
	
	ofAddListener(typeGui->newGUIEvent, this, &CloudsVisualSystemCode::selfGuiEvent);
	guis.push_back(typeGui);
	guimap[typeGui->getName()] = typeGui;

	
	boxGui = new ofxUISuperCanvas("BOXES", gui);
	boxGui->copyCanvasStyle(gui);
	boxGui->copyCanvasProperties(gui);
	
	boxGui->addSlider("WIDTH", 40, 300, &maxwidth);
	boxGui->addSlider("HEIGHT", 40, 500, &maxheight);
	
	//any width/height smaller thin this will become a widget
	boxGui->addIntSlider("min subdivision size", 4, 200, &minsize);
	boxGui->addSlider("min code window size", 40, 500, &minTextboxSize);
	boxGui->addIntSlider("box gen seed", 1, 300, &boxSeed);
	boxGui->addSlider("outline alpha", 0, 1.0, &outlineAlpha);
	
	
	ofAddListener(boxGui->newGUIEvent, this, &CloudsVisualSystemCode::selfGuiEvent);
	guis.push_back(boxGui);
	guimap[boxGui->getName()] = boxGui;
	
	
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
	rectTests.clear();
	
	ofxMtlBoxFitting boxFitting;
	boxFitting.setup(getCanvasWidth(),getCanvasHeight(),
					 maxwidth, maxheight, minsize);

	boxFitting.generate(boxSeed);
	
	for(int i = 0; i < boxFitting.boxes.size(); i++) {
		ofRectangle r = ofRectangle(boxFitting.boxes[i].x,
									boxFitting.boxes[i].y,
									boxFitting.boxes[i].w,
									boxFitting.boxes[i].h);
		r.x += 1;
		r.y += 1;
		r.width  -= 2;
		r.height -= 2;
		
		rectTests.push_back(r);
	}
	
	sort(rectTests.begin(),rectTests.end(), rectsort);
	
	for(int i = 0; i < panels.size(); i++){
		delete panels[i];
	}
	panels.clear();
	
	for(int i = 0; i < rectTests.size(); i++){
		Panel* p;
		if(MIN(rectTests[i].height, rectTests[i].width) < minTextboxSize){
			p = new PanelGraph();
		}
		else{
			PanelCode* code = new PanelCode();
			code->sharedFont = &sharedFont;
			p = code;
		}
		p->setup( getVisualSystemDataPath() + "code_test.txt" );

		p->outlineAlpha = &outlineAlpha;
		p->scanSpeed = powf(ofRandom(speedRange.min,speedRange.max),2);
		p->drawRect = rectTests[i];
		panels.push_back(p);
	}
	
	generatedPanelWidth = getCanvasWidth();
	generatedPanelHeight = getCanvasHeight();
	regeneratePanels = false;
	
	cout << "generating panels" << endl;
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
	for(int i = 0; i < panels.size(); i++){
		panels[i]->startAnimation();
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
		currentFontSize = fontSize;
	}
	
	sharedFont.setTracking(typeTracking);
	
	if(regeneratePanels ||
	   getCanvasWidth() != generatedPanelWidth ||
	   getCanvasHeight() != generatedPanelHeight )
	{
		generatePanels();
		for(int i = 0; i < panels.size(); i++){
			panels[i]->startAnimation();
		}
	}
	
	updateColors();
	
	for(int i = 0; i < panels.size(); i++){
		panels[i]->tint = tintColor;
		panels[i]->matchColorTypes = matchColorTypes;
		panels[i]->update();
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

	for(int i = 0; i < panels.size(); i++){
		panels[i]->draw( getCanvasHeight() );
	}
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemCode::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemCode::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemCode::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		generatePanels();
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