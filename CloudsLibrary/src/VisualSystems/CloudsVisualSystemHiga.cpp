
#include "CloudsVisualSystemHiga.h"

CloudsVisualSystemHiga::CloudsVisualSystemHiga(){

}

string CloudsVisualSystemHiga::getSystemName(){
	return "HigaSystem";
}


void CloudsVisualSystemHiga::selfSetup(){

	//HIGA:
	//Normal set up stuff can go here
	
	for(int i = 0; i < 30; i++){
		ofNode n;
		n.setPosition( ofRandom(-300,300),ofRandom(-300,300),ofRandom(-300,300) );

		testNodeExamples.push_back(n);
	}


}

void CloudsVisualSystemHiga::selfSetupGuis(){
	
    customGui = new ofxUISuperCanvas("EXAMPLE CUSTOM", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
    customGui->setName("ExampleCustom");
    customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	//EXAMPLES
//	customGui->addButton("REGENERATE", &shouldRegenerateMesh);
//	
	customGui->addSlider("EXAMPLE", 10, 1000, &exampleSlider);
//	customGui->addSlider("CLOTH HEIGHT", 10, 1000, &hieght);
//	
//	customGui->addSlider("GRID SIZE", 10, 100, &gridSize);
//	
//	customGui->addSlider("SPRING STRENGTH", .001, 1.0, &springStrength);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemHiga::selfGuiEvent);
	
    guis.push_back(customGui);
    guimap[customGui->getName()] = customGui;
}


void CloudsVisualSystemHiga::selfUpdate(){

}

void CloudsVisualSystemHiga::selfDrawBackground(){

}

void CloudsVisualSystemHiga::selfDrawDebug(){

}

void CloudsVisualSystemHiga::selfSceneTransformation(){

}

void CloudsVisualSystemHiga::selfDraw(){
	for(int i = 0; i < testNodeExamples.size(); i++) testNodeExamples[i].draw();
}

void CloudsVisualSystemHiga::selfExit(){

}

void CloudsVisualSystemHiga::selfBegin(){

}

void CloudsVisualSystemHiga::selfEnd(){

}


void CloudsVisualSystemHiga::selfKeyPressed(ofKeyEventArgs & args){

}

void CloudsVisualSystemHiga::selfKeyReleased(ofKeyEventArgs & args){

}


void CloudsVisualSystemHiga::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemHiga::selfMouseMoved(ofMouseEventArgs& data){

}

void CloudsVisualSystemHiga::selfMousePressed(ofMouseEventArgs& data){

}

void CloudsVisualSystemHiga::selfMouseReleased(ofMouseEventArgs& data){

}


void CloudsVisualSystemHiga::selfSetupGui(){

}

void CloudsVisualSystemHiga::selfGuiEvent(ofxUIEventArgs &e){

}


void CloudsVisualSystemHiga::selfSetupSystemGui(){

}

void CloudsVisualSystemHiga::guiSystemEvent(ofxUIEventArgs &e){

}


void CloudsVisualSystemHiga::selfSetupRenderGui(){

}

void CloudsVisualSystemHiga::guiRenderEvent(ofxUIEventArgs &e){
	
}