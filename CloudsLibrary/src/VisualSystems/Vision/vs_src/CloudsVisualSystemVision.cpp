//
//  CloudsVisualSystemVision.cpp
//
//  Created by Patricio Gonzalez Vivo on 7/23/13.
//
//

#include "CloudsVisualSystemVision.h"
using namespace ofxCv;
using namespace cv; 

string CloudsVisualSystemVision::getSystemName()
{
	return "Vision";
}

void CloudsVisualSystemVision::selfSetDefaults(){
//    currentMode = OpticalFlow;
    curFlow = &farneback;
    bIs2D = true;
	
    opticalFlowScale = 2;
    movieIndex =0;
    pyrScale = 0.5;
    levels =4;
    winsize = 8;
    iterations =2;
    polyN = 7;
    polySigma = 1.5;
    winSize =32;
    maxLevel =  3;
    maxFeatures = 200;
    qualityLevel = 0.01;
    minDistance = 4;
    colorRange=50;
    flowFirstFrame = true;
    useFarneback = true;
    drawPlayer = true;
    drawThresholded =false;
    
    videoAlpha = 255;
    windowAlpha = 0;
    thresholdAlpha = 0;
    
    bContourTracking = false;
    bOpticalFlow = false;
    bDrawBoxes = false;
    bDrawLines = false;
    bContours =false;
    bDrawHeatMap = false;
    
    boxSat =0;
    boxBright =0;
    boxHue =0;
    boxColor = ofFloatColor::green;
    
    flowLineMultiplier = 1;
	flowDamp = 1.0;
    flowColorMapRange = 50;
    flowLineWidth = 2;
    learningTime = 15;
    thresholdValue =128;
    cvPersistance =3;
    cvMaxDistance = 80;
    cvMinAreaRadius =15;
    cvMaxAreaRadius = 100;
    cvThresholdValue = 25;
    cvUpdateParameters = false;
    lineWidth = 2;
    accumulationCount =0;
    skipFrames = 0;
    contourLifetimeColorRange = 110;
    windowWidth = .1;
    windowHeight = .1;
	
	currentFlowDensity = -1;
	flowDensity = 3;
	differenceHueShift = .7;

}

void CloudsVisualSystemVision::selfSetup()
{


    shader.load(getVisualSystemDataPath() + "heatMapShader");

    //	app
    movieIndex = 0;

    videosDir.listDir(getVisualSystemDataPath(true) + "videos" );
    videosDir.sort();
    for (int i = 0; i < videosDir.size(); i++) {
        movieStrings.push_back(videosDir.getName(i));
    }
    frameIsNew = false;
    loadCurrentMovie();
}

void CloudsVisualSystemVision::selfSetupGui()
{
    opticalFlowGui = new ofxUISuperCanvas("OP FLOW", gui);
    opticalFlowGui->copyCanvasStyle(gui);
    opticalFlowGui->copyCanvasProperties(gui);
    opticalFlowGui->setName("OP FLOW");
 
    ofxUIToggle *toggle = opticalFlowGui->addToggle("ENABLE",&bOpticalFlow);

    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    opticalFlowGui->resetPlacer();
    opticalFlowGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    opticalFlowGui->addWidgetToHeader(toggle);
    
    opticalFlowGui->addSpacer();
    opticalFlowGui->addToggle("DRAW FLOW WINDOW", &bDrawFlowWindow);
    opticalFlowGui->addSlider("WINDOW TINT", 0, 255, &windowAlpha);
	opticalFlowGui->addIntSlider("FLOW DENSITY", 1, 5, &flowDensity);
	opticalFlowGui->addSlider("FLOW ALPHA", 0, 1.0, &flowLineAlpha);
    opticalFlowGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    opticalFlowGui->addSlider("WINDOW WIDTH", .0, 1., &windowWidth);
    opticalFlowGui->addSlider("WINDOW HEIGHT", 0., 1., &windowHeight);
    opticalFlowGui->addSlider("FLOW LINE LENGTH", 0.5, 8, &flowLineMultiplier);
    opticalFlowGui->addSlider("FLOW COLOUR MAP RANGE", 10, 1000, &flowColorMapRange);
    opticalFlowGui->addSlider("FLOW LINE WIDTH", 1, 10, &flowLineWidth);
    opticalFlowGui->addSlider("FLOW LINE DAMP", 0., 1.0, &flowDamp);
	
    opticalFlowGui->addSpacer();
    opticalFlowGui->addLabel("OPTICAL FLOW PARAMS");
    opticalFlowGui->addSlider("PYRSCALE", .5, 0.9, &pyrScale);
    opticalFlowGui->addSlider("LEVELS",  1, 8, &levels);
    opticalFlowGui->addSlider("WINSIZE",  4, 64, &winsize);
    opticalFlowGui->addSlider("ITERATIONS",1, 8, &iterations);
    opticalFlowGui->addSlider("POLYN",5, 7, &polyN);
    opticalFlowGui->addSlider("POLYSIGMA", 1.1, 1.1, &polySigma);

    ofAddListener(opticalFlowGui->newGUIEvent, this, &CloudsVisualSystemVision::selfGuiEvent);
    guis.push_back(opticalFlowGui);
    guimap[opticalFlowGui->getName()] = opticalFlowGui;
    
    
    contourTrackingGui = new ofxUISuperCanvas("CONTOUR",gui);
    contourTrackingGui->copyCanvasStyle(gui);
    contourTrackingGui->copyCanvasProperties(gui);
    contourTrackingGui->setName("CONTOUR"); 
    ofxUIToggle *ContourBtn = contourTrackingGui->addToggle("ENABLE",&bContourTracking);
    
    ContourBtn->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    contourTrackingGui->resetPlacer();
    contourTrackingGui->addWidgetDown(ContourBtn, OFX_UI_ALIGN_RIGHT, true);
    contourTrackingGui->addWidgetToHeader(ContourBtn);
    
    contourTrackingGui->addSpacer();
    contourTrackingGui->addLabel("VISUAL PARAMS");
    contourTrackingGui->addSpacer();
    contourTrackingGui->addToggle("DRAW LINES",&bDrawLines);
    contourTrackingGui->addToggle("DRAW NUMBERS",&bNumbers);
    contourTrackingGui->addToggle("LIFESPAN COLOR",&bLifeTime);
    contourTrackingGui->addToggle("DRAW BOXES",&bDrawBoxes);
    contourTrackingGui->addToggle("DRAW CONTOURS",&bContours);
	
    contourTrackingGui->addSlider("BOX H", 0.0,1.0,&boxHue);
    contourTrackingGui->addSlider("BOX S", 0.0,1.0,&boxSat);
    contourTrackingGui->addSlider("BOX B", 0.0,1.0,&boxBright);
    contourTrackingGui->addSlider("BOX LINE WIDTH", 1, 10, &lineWidth);
    contourTrackingGui->addSlider("LIFETIME COLOUR RANGE", 0,255  ,&contourLifetimeColorRange);
    
    contourTrackingGui->addLabel("TRACKER PARAM");
    contourTrackingGui->addSpacer();
    contourTrackingGui->addSlider("PERSISTANCE", 0,100,&cvPersistance);
    contourTrackingGui->addSlider("MAXDISTANCE", 0,100  ,&cvMaxDistance);
    
    contourTrackingGui->addLabel("CONTOUR FINDER PARAMS");
    contourTrackingGui->addSpacer();
    contourTrackingGui->addSlider("MIN AREA RADIUS", 0,50,&cvMinAreaRadius);
    contourTrackingGui->addSlider("MAX AREA RADIUS",0,255,&cvMaxAreaRadius);
    contourTrackingGui->addSlider("THRESHOLD VALUE", 0, 255, &cvThresholdValue);
    contourTrackingGui->addButton("UPDATE CV PARAMS", false);
    
    ofAddListener(contourTrackingGui->newGUIEvent, this, &CloudsVisualSystemVision::selfGuiEvent);
	
    guis.push_back(contourTrackingGui);
    guimap[contourTrackingGui->getName()] = contourTrackingGui;
    
    
    thresholdGui = new ofxUISuperCanvas("BG DIFF",gui);
    thresholdGui->copyCanvasStyle(gui);
    thresholdGui->copyCanvasProperties(gui);
    thresholdGui->setName("THRESHOLD");
    
    ofxUIToggle *ThresholdBtn = thresholdGui->addToggle("DRAW",&drawThresholded);
    ThresholdBtn->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    thresholdGui->resetPlacer();
    thresholdGui->addWidgetDown(ThresholdBtn, OFX_UI_ALIGN_RIGHT, true);
    thresholdGui->addWidgetToHeader(ThresholdBtn);

    
    thresholdGui->addSlider("LEARNING TIME", 0,100,&learningTime);
    thresholdGui->addSlider("THRESHOLD VALUE", 0,255  ,&thresholdValue);
    ofAddListener(contourTrackingGui->newGUIEvent, this, &CloudsVisualSystemVision::selfGuiEvent);
    guis.push_back(thresholdGui);
    guimap[thresholdGui->getName()] = thresholdGui;
}


void CloudsVisualSystemVision::updateImagesForNewVideo(){
    imitate(previousHeatMap, player->getPixelsRef());
    imitate(diff, player->getPixelsRef());
    accumulation.allocate(player->getWidth(), player->getHeight(), OF_IMAGE_COLOR);
}

void CloudsVisualSystemVision::resetFlowField(){
    
    farneback.resetFlow();
    flowMesh.clear();
    
    flowMesh.setMode(OF_PRIMITIVE_LINES);
    flowMesh.setUsage(GL_STREAM_DRAW);
	
    
	cout<<"resetting flow lines: "<<player->getWidth()<<" , "<<player->getHeight()<<endl;
	
    for( int j=0; j<player->getHeight() / opticalFlowScale; j += flowDensity){
        for( int i=0; i<player->getWidth() / opticalFlowScale; i += flowDensity){
            flowMesh.addVertex(ofVec3f(i, j, 0));
            flowMesh.addVertex(ofVec3f(i, j, 0));
            flowMesh.addColor(ofColor::white);
            flowMesh.addColor(ofColor::white);
        }
    }
	
	currentFlowDensity = flowDensity;
}

void CloudsVisualSystemVision::updateOpticalFlowParameters(){
    farneback.setPyramidScale(pyrScale);
    farneback.setNumLevels( levels );
    farneback.setWindowSize( winsize) ;
    farneback.setNumIterations( iterations);
    farneback.setPolyN( polyN) ;
    farneback.setPolySigma( polySigma );
    farneback.setUseGaussian( useFarneback );
}

void CloudsVisualSystemVision::updateOpticalFlow(){

	player->getPixelsRef().resizeTo(opticalFlowPixels);
	farneback.calcOpticalFlow(opticalFlowPixels);

	int flowWidth  = opticalFlowPixels.getWidth();
	int flowHeight = opticalFlowPixels.getHeight();
	
	if(bDrawFlowWindow){

		float screenToFlowScale = flowWidth / videoRect.width;
		float mouseXVideo = (GetCloudsInputX() - videoRect.x) * screenToFlowScale;
		float mouseYVideo = (GetCloudsInputY() - videoRect.y) * screenToFlowScale;
		flowWindow.setFromCenter(mouseXVideo, mouseYVideo,
								 flowWidth*windowWidth,
								 flowHeight*windowHeight);
	}
	else{
		flowWindow = ofRectangle(0,0,flowWidth,flowHeight);
	}

	for( int i = 0; i < flowMesh.getVertices().size(); i+=2){
		
		ofVec2f basePos = flowMesh.getVertex(i);
		if(flowWindow.inside(basePos)){
			ofVec2f pos = farneback.getFlowOffset(basePos.x,basePos.y);
			
			pos *= flowLineMultiplier;
			pos += basePos;
			
			ofVec3f curPos = flowMesh.getVertices()[i+1];
			ofVec3f newPos = ofVec3f(pos.x,pos.y,0);
			ofVec3f dampenedPos = curPos + (newPos - curPos)*flowDamp;
			
			float mag = basePos.distance(dampenedPos);
			
			float scaledHue = ofMap(mag, 0, colorRange,
									ofFloatColor::blue.getHue(),
									ofFloatColor::red.getHue(),true);
			ofFloatColor magnitudeColor = ofFloatColor::fromHsb(scaledHue, 1.0, 1.0, flowLineAlpha);
			flowMesh.setColor(i+1,magnitudeColor);
			flowMesh.setVertex(i+1,dampenedPos);
			
		}
		else{
			flowMesh.setVertex(i+1,basePos);
			flowMesh.setColor(i,ofFloatColor(0,0));
			flowMesh.setColor(i+1,ofFloatColor(0,0));
		}
	}
	

	flowFirstFrame = false;

}


void CloudsVisualSystemVision::updateContourTracking(){
	background.update(player->getPixelsRef(), thresholded);
	thresholded.update();
	blur(thresholded, 5);
	contourFinder.findContours(thresholded);
    tracker.track(contourFinder.getBoundingRects());
    contours = contourFinder.getPolylines();
}

void CloudsVisualSystemVision::updateCVParameters(){

    //  background subtraction
    background.setDifferenceMode(RunningBackground::ABSDIFF);
    background.setLearningTime(learningTime);
    background.setThresholdValue(thresholdValue);
    
    contourFinder.setMinAreaRadius(cvMinAreaRadius);
    contourFinder.setMaxAreaRadius(cvMaxAreaRadius);
    contourFinder.setThreshold(cvThresholdValue);
    
    //how many frames before we give up on it
    tracker.setPersistence(cvPersistance);
    // an object can move up to 50 pixels per frame
    tracker.setMaximumDistance(cvMaxDistance);
    
    boxColor.setHsb(boxHue, boxSat, boxBright);
}

//radio field

void CloudsVisualSystemVision::selfPresetLoaded(string presetPath){

	ofxUIDropDownList* d = (ofxUIDropDownList*)rdrGui->getWidget("VIDEO");
    cout<<"IM in self preset loaded"<<endl;
//	vector<int>& selected = d->getSelectedIndeces();
//    for (int i =0 ; i<selected.size(); i++) {
//        cout<<"name of videos : "<< selected[i]<<endl;
//    }
    vector<ofxUILabelToggle*> t =  d->getToggles();
//	if(selected.size() > 0){
//        
//		loadMovieWithName( d->getToggles()[ selected[0] ]->getName() );
//	}
    
    for (int i =0; i<t.size(); i++) {
        if (t[i]->getValue()) {
            cout<<"LOADING MOVIE : "<<t[i]->getName()<<endl;
            loadMovieWithName( t[i]->getName() );
        }
    }
	
    
//    //LOADING MOVIE
//    ofxUIRadio* r = (ofxUIRadio*)rdrGui->getWidget("VIDEO");
//    
//    vector<ofxUIToggle*> t = r->getToggles();
//    string movieName;
//    for(int j = 0; j < t.size(); j++){
//        if(t[j]->getValue()) {
//            movieName = t[j]->getName();
//            
//			cout << "LOADING MOVIE :"<<movieName<<endl;
//			
//			for(int i = 0; i < movieStrings.size(); i++){
//				if (movieStrings[i] == movieName) {
//					loadMovieAtIndex(i);
//					break;
//				}
//			}
//			break;
//        }
//    }
}


void CloudsVisualSystemVision::selfBegin()
{
    
    
}

void CloudsVisualSystemVision::selfEnd()
{
    
}

void CloudsVisualSystemVision::selfExit()
{
}

void CloudsVisualSystemVision::selfSetupSystemGui()
{
    
}

void CloudsVisualSystemVision::selfSetupRenderGui()
{

    
    ofxUIToggle *toggle = rdrGui->addToggle("PLAYER",&drawPlayer);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    rdrGui->resetPlacer();
    rdrGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    rdrGui->addWidgetToHeader(toggle);

    ofxUIToggle *AbsDiffBtn = rdrGui->addToggle("ABS DIFF HEAT MAP",&bDrawHeatMap);

    rdrGui->addSlider("VIDEO TINT", 0, 255, &videoAlpha);
    rdrGui->addSlider("THRESHOLD TINT", 0, 255, &thresholdAlpha);
    rdrGui->addSlider("DIFF TINT", 0, 255, &diffAlpha);
    rdrGui->addSlider("FLOW WINDOW TINT", 0, 255, &windowAlpha);
	rdrGui->addSlider("DIFFERENCE HUE",	0, 1.0, &differenceHueShift);
    rdrGui->addDropDownList("VIDEO", movieStrings);
    rdrGui->autoSizeToFitWidgets();
    ofAddListener(rdrGui->newGUIEvent, this, &CloudsVisualSystemVision::selfGuiEvent);
    
}

void CloudsVisualSystemVision::selfUpdate(){

	if(player == NULL){
        ofLogError("CloudsVisualSystemVision::selfUpdate") << "Player is not initialised";
        return;
    }
    
	if(currentFlowDensity != flowDensity ){
		resetFlowField();
	}
	
	//AVFoundation loads videos asynchronously.
	//Using this condition to update the system settings once the video is loaded.
	
	if( player->isLoaded() && player->getPixelsRef().isAllocated() ){
		lastPixels = player->getPixelsRef();
	}
	
	player->update();
	
	frameIsNew = player->isFrameNew();
    if(frameIsNew && lastPixels.isAllocated() ){
		if(!prev.isAllocated()){
			prev.allocate(lastPixels);
		}
		prev.loadData(lastPixels);
    }

	if(bNewVideoLoaded && player->getPixelsRef().isAllocated() ){
		player->setLoopState(OF_LOOP_NORMAL);
		updateSettingsForNewVideo();
		bNewVideoLoaded = false;
		opticalFlowPixels.allocate(player->getWidth()/opticalFlowScale,
								player->getHeight()/opticalFlowScale,
								OF_IMAGE_COLOR);
		cout<<"UPDATED VIDEO SETTINGS"<<endl;
	}
	else{
			    
		if(frameIsNew && ! bNewVideoLoaded){
			if(drawThresholded){
				background.update(player->getPixelsRef(), thresholded);
				thresholded.update();
				blur(thresholded, 5);
			}
			
			if(bContourTracking){
				updateContourTracking();
			}
			
			if(bOpticalFlow){
				updateOpticalFlow();
			}
		}
	}
}

void CloudsVisualSystemVision::selfDrawBackground()
{
    if (player == NULL) {
        ofLogError("CloudsVisualSystemVision::selfDrawBackground") << "Player is not initialised";
        return;
    }
	if(!player->isLoaded() || !player->getPixelsRef().isAllocated() ){
		ofLogError("CloudsVisualSystemVision::selfDrawBackground") << "Player is not loaded";
		
		return;
	}
	
    ofPushStyle();
	ofPushMatrix();
	
	videoRect = ofRectangle(0,0, player->getWidth(), player->getHeight());
	ofRectangle screenRect(0,0, getCanvasWidth(), getCanvasHeight());
	videoRect.scaleTo(screenRect);

	float playerWidth  = player->getWidth();
	float playerHeight = player->getHeight();
	ofTranslate(videoRect.x, videoRect.y);
	ofScale(videoRect.width/playerWidth,
			videoRect.height/playerHeight);

    if(drawPlayer){
		ofPushStyle();
		ofSetColor(videoAlpha);
		player->draw(0,0);
		ofPopStyle();
    }
	
    if(drawThresholded){
        if( thresholded.isAllocated() ){
			ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_SCREEN);
			ofSetColor(thresholdAlpha);        
			thresholded.draw(0,0, playerWidth, playerHeight);
			ofPopStyle();
		}
		else{
			ofLogError("CloudsVisualSystemVision::selfDrawBackground") << "Draw threshold turned on but image not prepared";
		}
    }
    
    if(bContourTracking){

//        if(bContours){
//            contourFinder.draw();
//        }
        vector<MyTracker>& followers = tracker.getFollowers();
        for(int i = 0; i < followers.size(); i++) {
            float b = followers[i].getLifeTime();
            followers[i].draw(lineWidth, bLifeTime, contourLifetimeColorRange, bDrawBoxes, bDrawLines, bNumbers, boxColor);
        }

        
        if (bContours) {
            for (int j=0; j<contours.size(); j++) {
                contours[j].draw();
            }
        }


    }
    
    if(bOpticalFlow){

        ofTexture& tex = player->getTextureReference();
		if(tex.isAllocated()){
			ofPushStyle();
			ofDisableAlphaBlending();
//			ofEnableBlendMode(OF_BLENDMODE_SCREEN);
			ofSetColor(windowAlpha, 255);
			
			ofPushMatrix();
			ofScale(opticalFlowScale,opticalFlowScale);
	
			if(bDrawFlowWindow){
				ofPushStyle();
				
				float mouseX = ofMap(GetCloudsInputX()-videoRect.x,0,videoRect.width, 0, opticalFlowPixels.getWidth(), true)-flowWindow.width/2;
				float mouseY = ofMap(GetCloudsInputY()-videoRect.y,0,videoRect.height, 0, opticalFlowPixels.getHeight(), true)-flowWindow.height/2;
				
				tex.drawSubsection(mouseX,mouseY,
								   flowWindow.width, flowWindow.height,
								   mouseX*opticalFlowScale,
								   mouseY*opticalFlowScale,
								   flowWindow.width*opticalFlowScale,flowWindow.height*opticalFlowScale);
				ofPopStyle();
			}
			
			ofSetLineWidth(flowLineWidth);
			ofEnableBlendMode(OF_BLENDMODE_SCREEN);
			flowMesh.draw();
			ofEnableAlphaBlending();
			ofPopMatrix();
			
			ofPopStyle();

		}
		else{
			ofLogError("CloudsVisualSystemVision::selfDrawBackground") << "Video texture not allocated for optical flow";
		}
    }
    
    if(bDrawHeatMap){
		ofMesh m;
		ofVec2f a = ofVec2f(0,0);
		ofVec2f b = ofVec2f(player->getWidth(),0);
		ofVec2f c = ofVec2f(0,player->getHeight());
		ofVec2f d = ofVec2f(player->getWidth(),player->getHeight());
		
		m.addTexCoord(a);
		m.addVertex(a);
		
		m.addTexCoord(b);
		m.addVertex(b);
		
		m.addTexCoord(c);
		m.addVertex(c);

		m.addTexCoord(d);
		m.addVertex(d);
		
		m.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

		shader.begin();
		//shader.setUniformTexture("thresholdedImage", thresholded, 0);
		shader.setUniformTexture("previousFrame", prev, 1);
		shader.setUniformTexture("currentFrame", player->getTextureReference(), 2);
		
		m.draw();
		
		shader.end();


        /*
        ofPushStyle();
        ofSetColor(128,diffAlpha);

        diff.draw(0, 0,playerWidth,playerHeight);
        
        float diffRed = diffMean[0];
        float mapRed = ofMap(diffRed, 0, 512, 0, accumulation.width,true);
        float diffGreen = diffMean[1];
        float mapGreen = ofMap(diffGreen, 0, 512, 0, accumulation.width,true);
        float diffBlue = diffMean[2];
        float mapBlue = ofMap(diffBlue, 0, 512, 0, accumulation.width,true);
        
        ofSetColor(255, 0, 0);
        ofRect(0,0, mapRed, 10);
        ofSetColor(0, 255, 0);
        ofRect(0,10, mapGreen, 10);
        ofSetColor(0, 0, 255);
        ofRect(0, 20,  mapBlue, 10);

        ofPopStyle();
        
        */

    }
	
	ofPopMatrix();
    ofPopStyle();
    
}

void CloudsVisualSystemVision::selfDraw()
{
    
}


void CloudsVisualSystemVision::selfSetupGuis()
{
    
}

void CloudsVisualSystemVision::selfAutoMode()
{
    
}

void CloudsVisualSystemVision::guiSystemEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemVision::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key == 'R'){
        shader.load(getVisualSystemDataPath() + "heatMapShader");
    }
    
}

void CloudsVisualSystemVision::selfDrawDebug()
{
    
}

void CloudsVisualSystemVision::selfSceneTransformation()
{
    
}

void CloudsVisualSystemVision::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemVision::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemVision::selfMouseMoved(ofMouseEventArgs& data)
{
}

void CloudsVisualSystemVision::selfMousePressed(ofMouseEventArgs& data)
{
}

void CloudsVisualSystemVision::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

//void CloudsVisualSystemVision::setMode(CVMode mode){
//    
//    switch (mode) {
//        case OpticalFlow:
//            cout<<"setting mode to optical flow"<<endl;
//            currentMode = OpticalFlow;
//            break;
//            
//        case ContourTracking:
//            currentMode = ContourTracking;
//            break;
//            
//        case HeatMap:
//            currentMode = HeatMap;
//            break;
//            
//        default:
//            break;
//    }
//}

void CloudsVisualSystemVision::selfGuiEvent(ofxUIEventArgs &e)
{
    
    string name = e.widget->getName();
    int kind = e.widget->getKind();

    ofxUIRadio* r = (ofxUIRadio*)e.widget;
    ofxUIButton* b  = (ofxUIButton*) e.widget;
    ofxUIToggle* t  = (ofxUIToggle*) e.widget;
    
    if(name == "UPDATE CV PARAMS" &&  b->getValue() ){
        b->setValue(false);
        updateCVParameters();
        cout<<"updating CV parameters"<<endl;
    }
    else if(name == "UPDATE FLOW PARAMS"){
        b->setValue(false);
        updateOpticalFlowParameters();
        cout<<"Updating Optical Flow parameters"<<endl;
    }
    else if(name == "BOX H"){
        boxColor.setHue(boxHue);
    }
    else if(name =="BOX S"){
        boxColor.setSaturation(boxSat);
    }
    else if(name == "BOX B"){
        boxColor.setBrightness(boxBright);
    }
    if (e.widget->getParent()->getName() == "VIDEO"){

        thresholded.clear();
        background.reset();
        updateImagesForNewVideo();
		
        ofxUIToggle* t = (ofxUIToggle*)e.widget;
        if(t->getValue())loadMovieWithName( t->getName() );
    }
}

void CloudsVisualSystemVision::loadCurrentMovie(){
	loadMovieAtIndex(movieIndex);
}

void CloudsVisualSystemVision::loadMovieWithName(string name){

	for(int i = 0; i < movieStrings.size(); i++){
		if (movieStrings[i] == name) {
			cout << "Loading movie from GUI " << movieStrings[i] <<" : "<<name<< endl;
			loadMovieAtIndex(i);
			break;
		}
	}
}

void CloudsVisualSystemVision::loadMovieAtIndex(int index){
	if(movieStrings.size() == 0){
		ofLogError("CloudsVisualSystemVision::loadMovieAtIndex") << "No movies. Make sure you have a visualsystems_ignored/Vision/videos folder.";
		return;
	}

    movieIndex = index;

    player = ofPtr<ofxAVFVideoPlayer>(new ofxAVFVideoPlayer());
//	player = ofPtr<ofVideoPlayer>(new ofVideoPlayer());
    if(player->loadMovie(getVisualSystemDataPath(true)+"videos/" + movieStrings[ movieIndex ])){
        player->play();
    }
    else{
        cout<<"Not Playing"<<endl;
    }

    bNewVideoLoaded = true; 
    cout<<"Player dimensions (new) :"<< player->getWidth()<<" , "<<player->getHeight() <<endl;

}
void CloudsVisualSystemVision::updateSettingsForNewVideo(){
    updateCVParameters();
    updateImagesForNewVideo();
    resetFlowField();
}

void CloudsVisualSystemVision::guiRenderEvent(ofxUIEventArgs &e)
{
    
}