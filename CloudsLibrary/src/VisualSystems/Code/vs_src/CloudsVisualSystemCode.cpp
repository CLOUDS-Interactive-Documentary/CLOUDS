//
//  CloudsVisualSystemCode.cpp
//

#include "CloudsVisualSystemCode.h"
#include "CloudsRGBDVideoPlayer.h"
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
	typeGui->addRangeSlider("speed range", .01, 1, &speedRange.min, &speedRange.max);

	ofAddListener(typeGui->newGUIEvent, this, &CloudsVisualSystemCode::selfGuiEvent);
	guis.push_back(typeGui);
	guimap[typeGui->getName()] = typeGui;

	
	boxGui = new ofxUISuperCanvas("BOXES", gui);
	boxGui->copyCanvasStyle(gui);
	boxGui->copyCanvasProperties(gui);
	
	boxGui->addSlider("max width", 40, 300, &maxwidth);
	boxGui->addSlider("max height", 40, 500, &maxheight);
	
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
	
	colorGui->addLabel("tint");
	colorGui->addMinimalSlider("color ting h", 0, 1.0, &tintColorHSV.r);
	colorGui->addMinimalSlider("color ting s", 0, 1.0, &tintColorHSV.g);
	colorGui->addMinimalSlider("color ting v", 0, 1.0, &tintColorHSV.b);

	string colorNames[MATCH_TYPES];
	colorNames[0] = "comments";
	colorNames[1] = "strings";
	colorNames[2] = "base types";
	colorNames[3] = "preprocessors";
	colorNames[4] = "keywords";
	
	for(int i = 0; i < MATCH_TYPES; i++){
		colorGui->addLabel("color " + ofToString(i+1) + " " +colorNames[i] );
		colorGui->addMinimalSlider("color " + ofToString(i+1) + " h", 0, 1.0, &matchColorTypesHSV[i].r);
		colorGui->addMinimalSlider("color " + ofToString(i+1) + " s", 0, 1.0, &matchColorTypesHSV[i].g);
		colorGui->addMinimalSlider("color " + ofToString(i+1) + " v", 0, 1.0, &matchColorTypesHSV[i].b);
	}
	
	ofAddListener(colorGui->newGUIEvent, this, &CloudsVisualSystemCode::selfGuiEvent);
	guis.push_back(colorGui);
	guimap[colorGui->getName()] = colorGui;
}

void CloudsVisualSystemCode::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

void CloudsVisualSystemCode::generatePanels(){
	rectTests.clear();
	
	ofxMtlBoxFitting boxFitting;
	boxFitting.setup(getSharedRenderTarget().getWidth(),
					 getSharedRenderTarget().getHeight(), maxwidth, maxheight, minsize);

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
	
	panels.clear();
	
	for(int i = 0; i < rectTests.size(); i++){
		Panel* p;
		if(MIN(rectTests[i].height, rectTests[i].width) < minTextboxSize){
			p = new PanelGraph();
		}
		else{
			p = new PanelCode;
		}
		p->setup( getVisualSystemDataPath() + "code_test.txt" );
		
		p->outlineAlpha = &outlineAlpha;
		p->scanSpeed = powf(ofRandom(speedRange.min,speedRange.max),2);
		p->drawRect = rectTests[i];
		panels.push_back(p);
	
	
	}
	
	panelsGenerated = true;
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
	
}
// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemCode::selfSetup(){
//	testPanel.setup(getVisualSystemDataPath() + "code_test.txt");	
	matchColorTypes.resize(MATCH_TYPES);
//	matchColorTypesHSV.resize(MATCH_TYPES);
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemCode::selfPresetLoaded(string presetPath){
	generatePanels();
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemCode::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemCode::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemCode::selfUpdate(){
	for(int i = 0; i < MATCH_TYPES; i++){
		matchColorTypes[i] = ofFloatColor::fromHsb(matchColorTypesHSV[i].r,
												   matchColorTypesHSV[i].g,
												   matchColorTypesHSV[i].b);
	}
	
	tintColor = ofFloatColor::fromHsb(tintColorHSV.r,
									  tintColorHSV.g,
									  tintColorHSV.b);
	
	for(int i = 0; i < panels.size(); i++){
		panels[i]->tint = tintColor;
		panels[i]->matchColorTypes = matchColorTypes;
		panels[i]->update();
	}

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemCode::selfDraw(){
//	testPanel.draw();
}

// draw any debug stuff here
void CloudsVisualSystemCode::selfDrawDebug(){
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemCode::selfDrawBackground(){

//	ofPushStyle();
//	ofNoFill();
//	for(int i = 0; i < rectTests.size(); i++){
////		ofSetColor( ofColor::fromHsb(ofRandom(255), 255,255) );
//		ofRect(rectTests[i]);
//	}
//	ofPopStyle();

	for(int i = 0; i < panels.size(); i++){
		panels[i]->draw( getSharedRenderTarget().getHeight() );
	}
	
//	testPanel.draw();
	//turn the background refresh off
	//bClearBackground = false;
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