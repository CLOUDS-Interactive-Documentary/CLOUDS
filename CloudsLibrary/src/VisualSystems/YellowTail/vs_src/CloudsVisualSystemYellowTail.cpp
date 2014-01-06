
#include "CloudsVisualSystemYellowTail.h"


CloudsVisualSystemYellowTail::CloudsVisualSystemYellowTail(){

}

string CloudsVisualSystemYellowTail::getSystemName(){
	return "YellowTail";
}

void CloudsVisualSystemYellowTail::selfSetup(){
	
    // It'll be 16 & 11 on iPad, 8 & 6 on iPhone
	float deviceScale = ofClamp(ofGetHeight() / 1024,1, 1024);
	softMaxGestures = MIN(ceil(16.0 * deviceScale), MAX_GESTURES);
	softMaxTouches  = MIN(ceil(11.0 * deviceScale), MAX_TOUCHES);
	
//	ofBackground(0,0,0);
	bUpdatingGeometry = true;
	bIs2D = true;
	
	minimumTravelForNewPoint = 3;
	mNewGestureStyle = GestureStyleTraveling;
    neverTouchedBefore = true;
	clearGestures();
    	
}

void CloudsVisualSystemYellowTail::selfSetupGuis(){
	
    customGui = new ofxUISuperCanvas("CUSTOM", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
    customGui->setName("CustomSettings");
    customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
//	customGui->addSlider("SOME PARAM", 0, 100, &someParam);
//	customGui->addButton("SOME BUTTON", false);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemYellowTail::selfGuiEvent);
	
    guis.push_back(customGui);
    guimap[customGui->getName()] = customGui;
	
}

void CloudsVisualSystemYellowTail::selfPresetLoaded(string presetPath){
	
}

void CloudsVisualSystemYellowTail::selfUpdate(){
	if (bUpdatingGeometry){
		updateGeometry();
    }
	
    if (gestures.size() >= softMaxGestures && gestures.size() > 0) {
        Gesture *toFade = gestures.front();
        gestures.pop_front();
        fadingGestures.push_back(toFade);
    }
}

void CloudsVisualSystemYellowTail::selfDrawBackground(){
	
	ofPushStyle();
	ofEnableAlphaBlending();
	
	glEnable(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	ofSetLineWidth(1.0f);
    for (vector<Gesture *>::iterator gesture = fadingGestures.begin(); gesture != fadingGestures.end(); gesture++) {
        (*gesture)->fade();
        (*gesture)->render();
    }
    
    for (deque<Gesture *>::iterator gesture = gestures.begin(); gesture != gestures.end(); ++gesture) {
        (*gesture)->render();
	}
    
    for (set<Gesture *>::iterator gesture = potentialGestures.begin(); gesture != potentialGestures.end(); ++gesture) {
        (*gesture)->render();
	}
    
    for (vector<Gesture *>::iterator gesture = fadingGestures.begin(); gesture != fadingGestures.end();) {
        if (!(*gesture)->isVisible()) {
            delete *gesture;
            gesture = fadingGestures.erase(gesture);
        } else {
            ++gesture;
        }
    }
	
	ofPopStyle();
    
//    ((CloudsInputKinectOSC *)GetCloudsInput().get())->debug(0, 0, ofGetWidth(), ofGetHeight());
}

void CloudsVisualSystemYellowTail::selfDrawDebug(){
	
}

void CloudsVisualSystemYellowTail::selfSceneTransformation(){
	
}

void CloudsVisualSystemYellowTail::selfDraw(){

}

void CloudsVisualSystemYellowTail::selfExit(){
	
}

void CloudsVisualSystemYellowTail::selfBegin(){

    Gesture *introGesture = new Gesture(ofGetWidth(), ofGetHeight());
    introGesture->fromXMLFile( getVisualSystemDataPath() + "strokes/y_stroke.xml" );
    gestures.push_back(introGesture);
}

void CloudsVisualSystemYellowTail::selfEnd(){
	//CLEAR ALL MEMORY
	clearGestures();
}

void CloudsVisualSystemYellowTail::selfKeyPressed(ofKeyEventArgs & args){
	switch (args.key){
		case 'g':
			gestures.clear();
			break;
		case 'c':
			bUpdatingGeometry = !bUpdatingGeometry;
			break;
		case 'F':
			break;
	}
}

void CloudsVisualSystemYellowTail::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemYellowTail::selfMouseDragged(ofMouseEventArgs& data){
//    int touchId = data.button;
//	
//    map<int,Gesture *>::iterator G = gestureForTouch.find(touchId);
//    assert(G != gestureForTouch.end());
//    Gesture *gesture = G->second;
//    
//    if (gesture->distanceToLastPoint((float)data.x, (float)data.y) > minimumTravelForNewPoint) {
//        
//        float distanceThresholdForAdding = minimumTravelForNewPoint * 8.0;
//        if (gesture->pointCount > 10){ distanceThresholdForAdding = minimumTravelForNewPoint * 2.0;}
//        gesture->addPointAndHalfwayPoint((float)data.x, (float)data.y, distanceThresholdForAdding);
//        
//        gesture->smooth(22.0);
//        gesture->compile();
//    }
}

void CloudsVisualSystemYellowTail::selfMouseMoved(ofMouseEventArgs& data){
    
}

void CloudsVisualSystemYellowTail::selfMousePressed(ofMouseEventArgs& data){
//    int touchId = data.button;
//	
//	/*
//	 if (neverTouchedBefore) {
//	 clearGestures();
//	 neverTouchedBefore = false;
//	 }
//	 */
//	
//    if (touchId%2 == 0){
//        mNewGestureStyle = GestureStyleTraveling;
//    } else {
//        mNewGestureStyle = GestureStyleInPlace;
//    } 
//    
//	Gesture *newGesture = new Gesture(ofGetWidth(), ofGetHeight());
//	newGesture->clear();
//	newGesture->clearPolygons();
//	newGesture->addPoint((float)data.x, (float)data.y);
//	newGesture->setStyle(mNewGestureStyle);
//    
//    potentialGestures.insert(newGesture);
//    
//    gestureForTouch[touchId] = newGesture;
}

void CloudsVisualSystemYellowTail::selfMouseReleased(ofMouseEventArgs& data){
//    int touchId = data.button;
//	
//    map<int,Gesture *>::iterator G = gestureForTouch.find(touchId);
//    
//	assert(G != gestureForTouch.end());
//    
//	Gesture *gesture = G->second;
//    
//    gestureForTouch.erase(G);
//    potentialGestures.erase(find(potentialGestures.begin(), potentialGestures.end(), gesture));
//    
//    if (gesture->isValid()) {
//        gestures.push_back(gesture);
//    } else {
//        delete gesture;
//    }
}

//--------------------------------------------------------------
void CloudsVisualSystemYellowTail::selfInteractionMoved(CloudsInteractionEventArgs& args){
    
}

//--------------------------------------------------------------
void CloudsVisualSystemYellowTail::selfInteractionStarted(CloudsInteractionEventArgs& args){
    int touchId = args.playerId;
	
	/*
	 if (neverTouchedBefore) {
	 clearGestures();
	 neverTouchedBefore = false;
	 }
	 */
	
//    if (touchId%2 == 0){
        mNewGestureStyle = GestureStyleTraveling;
//    } else {
//        mNewGestureStyle = GestureStyleInPlace;
//    } 
    
	Gesture *newGesture = new Gesture(ofGetWidth(), ofGetHeight());
	newGesture->clear();
	newGesture->clearPolygons();
	newGesture->addPoint((float)args.position.x, (float)args.position.y);
	newGesture->setStyle(mNewGestureStyle);
    
    potentialGestures.insert(newGesture);
    
    gestureForTouch[touchId] = newGesture;
}

//--------------------------------------------------------------
void CloudsVisualSystemYellowTail::selfInteractionDragged(CloudsInteractionEventArgs& args){
    int touchId = args.playerId;
	
    map<int,Gesture *>::iterator G = gestureForTouch.find(touchId);
	//JG removed asserts as they are a stability problem
//    assert(G != gestureForTouch.end());
	if(G == gestureForTouch.end()){
		return;
	}
    Gesture *gesture = G->second;
    
    if (gesture->distanceToLastPoint((float)args.position.x, (float)args.position.y) > minimumTravelForNewPoint) {
        
        float distanceThresholdForAdding = minimumTravelForNewPoint * 8.0;
        if (gesture->pointCount > 10){ distanceThresholdForAdding = minimumTravelForNewPoint * 2.0;}
        gesture->addPointAndHalfwayPoint((float)args.position.x, (float)args.position.y, distanceThresholdForAdding);
        
        gesture->smooth(22.0);
        gesture->compile();
    }
}

//--------------------------------------------------------------
void CloudsVisualSystemYellowTail::selfInteractionEnded(CloudsInteractionEventArgs& args){
    int touchId = args.playerId;
	
    map<int,Gesture *>::iterator G = gestureForTouch.find(touchId);
    	//JG removed asserts as they are a stability problem
	//assert(G != gestureForTouch.end());
	if(G == gestureForTouch.end()){
		return;
	}
    
	Gesture *gesture = G->second;
    
    gestureForTouch.erase(G);
    potentialGestures.erase(find(potentialGestures.begin(), potentialGestures.end(), gesture));
    
    if (gesture->isValid()) {
        gestures.push_back(gesture);
    } else {
        delete gesture;
    }
}

//--------------------------------------------------------------
void CloudsVisualSystemYellowTail::updateGeometry(){
    for (set<Gesture *>::iterator gesture = potentialGestures.begin(); gesture != potentialGestures.end(); ++gesture) {
        (*gesture)->compile();
    }
    for (deque<Gesture *>::iterator gesture = gestures.begin(); gesture != gestures.end(); ++gesture) {
        (*gesture)->advance();
	}
    for (vector<Gesture *>::iterator gesture = fadingGestures.begin(); gesture != fadingGestures.end(); ++gesture) {
        (*gesture)->advance();
    }
}

//--------------------------------------------------------------
void CloudsVisualSystemYellowTail::clearGestures() {
    for (deque<Gesture *>::iterator gesture = gestures.begin(); gesture != gestures.end(); ++gesture) {
		cout << " adding fade gesture " << endl;
		fadingGestures.push_back(*gesture);
    }
    gestures.clear();
}


void CloudsVisualSystemYellowTail::selfSetupGui(){

	
}

void CloudsVisualSystemYellowTail::selfGuiEvent(ofxUIEventArgs &e){

	if(e.widget->getName() == "SOME BUTTON" && ((ofxUIButton*)e.widget)->getValue() ){
		//do buttony stuff
	}
}

void CloudsVisualSystemYellowTail::selfSetupSystemGui(){
	
}

void CloudsVisualSystemYellowTail::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemYellowTail::selfSetupRenderGui(){
	
}

void CloudsVisualSystemYellowTail::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemYellowTail::selfSetupTimeline(){
	
}

void CloudsVisualSystemYellowTail::selfSetupTimelineGui(){
	
}

void CloudsVisualSystemYellowTail::selfTimelineGuiEvent(ofxUIEventArgs &e){
	
}
