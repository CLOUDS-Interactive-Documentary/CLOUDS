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

void CloudsVisualSystemVision::selfSetup()
{
    currentMode = OpticalFlow;
    curFlow = &farneback;
    
    scale = 2;
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
    bContourTracking = false;
    videoAlpha = 128;
    windowAlpha = 128;
    
    flowLineMultiplier = 1;
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
    windowWidth = 500;
    windowHeight = 500;
    
    
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    //	app
    movieIndex = 0;
    movieStrings.push_back("union_square_crop.mov");
    movieStrings.push_back("GreenPoint_bike_crop.mov");
    movieStrings.push_back("indianTrafficCrop.mov");
    movieStrings.push_back("unionsq_1 - Wi-Fi_crop.mov");
    movieStrings.push_back("Swarm_EindhovenTest_Watec_two-visitors_Crop.mov");
    
    frameIsNew = false;
    window = ofRectangle(0,0,500,500);
    loadCurrentMovie();
    
    
}

void CloudsVisualSystemVision::selfSetupGui()
{
    opticalFlowGui = new ofxUISuperCanvas("OPTICAL FLOW", gui);
    opticalFlowGui->copyCanvasStyle(gui);
    opticalFlowGui->copyCanvasProperties(gui);
    opticalFlowGui->addSpacer();
    opticalFlowGui->addLabel("VISUAL PARAMS");
    ofxUIButton *drawflowWindowbtn = opticalFlowGui->addToggle("DRAW FLOW WINDOW", &drawFlowWindow);
    opticalFlowGui->addSpacer();
    opticalFlowGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    opticalFlowGui->addSlider("WINDOW WIDTH", 10, 1000, &windowWidth);
    opticalFlowGui->addSlider("WINDOW HEIGHT", 10, 1000, &windowHeight);
    opticalFlowGui->addSlider("FLOW LINE LENGTH", 0.5, 3, &flowLineMultiplier);
    opticalFlowGui->addSlider("FLOW COLOUR MAP RANGE", 10, 200, &flowColorMapRange);
    opticalFlowGui->addSlider("FLOW LINE WIDTH", 1, 10, &flowLineWidth);
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
    
    contourTrackingGui = new ofxUISuperCanvas("CONTOUR TRACKING",gui);
    ofxUIButton *drawContourbtn = contourTrackingGui->addToggle("CONTOUR TRACKING", &drawFlowWindow);
    contourTrackingGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    contourTrackingGui->copyCanvasStyle(gui);
    contourTrackingGui->copyCanvasProperties(gui);
    contourTrackingGui->addSpacer();
    contourTrackingGui->addLabel("VISUAL PARAMS");
    contourTrackingGui->addSpacer();
    contourTrackingGui->addSlider("BOX LINE WIDTH", 1, 10, &lineWidth);
    contourTrackingGui->addLabel("BACKGROUND PARAM");
    contourTrackingGui->addSlider("LEARNING TIME", 0,100,&learningTime);
    contourTrackingGui->addSlider("THRESHOLD VALUE", 0,255  ,&thresholdValue);
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
    guimap[opticalFlowGui->getName()] = contourTrackingGui;
    
}

void CloudsVisualSystemVision::clearAccumulation(){
    for(int j=0; j<accumulation.height; j++){
        for( int i=0; i<accumulation.width; i++){
            accumulation.setColor(i, j, ofFloatColor(0));
        }
    }
}
void CloudsVisualSystemVision::updateImagesForNewVideo(){
    
    imitate(previousHeatMap, *player);
    imitate(diff, *player);
    accumulation.allocate(player->width, player->height, OF_IMAGE_COLOR);
    
}
void CloudsVisualSystemVision::resetFlowField(){
    
    farneback.resetFlow();
    flowMesh.clear();
    
    flowMesh.setMode(OF_PRIMITIVE_LINES);
    cout<<"resetting flow lines: "<<player->getWidth()<<" , "<<player->getHeight()<<endl;
    for( int j=0; j<player->getHeight(); j +=10){
        for( int i=0; i<player->getWidth(); i += 10 ){
            flowMesh.addVertex(ofVec3f(i, j,0));
            flowMesh.addVertex(ofVec3f(i, j,0));
            flowMesh.addColor(ofColor::white);
            flowMesh.addColor(ofColor::white);
        }
    }
    
}
void CloudsVisualSystemVision::populateOpticalFlowRegions(){
    int rectWidth =20;
    int rectHeight = 20;
    for( int j=0; j<player->getHeight(); j +=rectHeight){
        for( int i=0; i<player->getWidth(); i += rectWidth){
            if (i +rectWidth -1>player->width) {
                rectWidth =player->width- i +1;
            }
            
            if (j +rectHeight -1>player->height) {
                rectHeight =player->height- j +1;
            }
            flowRegions.push_back(ofRectangle(i, j, rectWidth, rectHeight));
        }
    }
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
    
    if (frameIsNew) {
        
        int width = player->getWidth()/scale;
        int height = player->getHeight()/scale;
        player->getPixelsRef().resizeTo(resizeToPixels);
        farneback.calcOpticalFlow(resizeToPixels);
        
        window.setFromCenter(mouseX, mouseY, windowWidth, windowHeight);
        
        for( int i = 0; i < flowMesh.getVertices().size(); i+=2){
            if(window.inside(flowMesh.getVertex(i))){
                ofVec2f pos = farneback.getFlowOffset(flowMesh.getVertex(i).x/scale, flowMesh.getVertex(i).y/scale );
                
                pos *= flowLineMultiplier;
                pos.x += flowMesh.getVertex(i).x;
                pos.y += flowMesh.getVertex(i).y;
                flowMesh.setVertex(i+1, ofVec3f( pos.x,pos.y,0));
                
                float mag =flowMesh.getVertex(i).distance(flowMesh.getVertex(i+1));
                
                float scaledHue = ofMap(mag,0, colorRange, ofFloatColor::blue.getHue(), ofFloatColor::red.getHue());
                ofFloatColor magnitudeColor = ofFloatColor::fromHsb(scaledHue, 128, 128 ) ;
                flowMesh.setColor(i+1,magnitudeColor);
            }
            else{
                flowMesh.setColor(i,0);
                flowMesh.setColor(i+1,0);
            }
        }
        flowFirstFrame = false;
    }
}

void CloudsVisualSystemVision::updateHeatMap(){
    
    if(frameIsNew){
        accumulationCount++;
        // take the absolute difference of prev and cam and save it inside diff
        toCv(accumulation) += toCv(previousHeatMap) -toCv(*player) ;
        
        absdiff(previousHeatMap, *player, diff);
        for(int i =0; i< diff.width; i++ ){
            for(int j =0; j<diff.height; j++){
                ofColor c = diff.getColor(i, j);
                float b = c.getBrightness();
                
                if(b > 10 ){
                    float scaledHue = ofMap(b ,0, 255, ofFloatColor::blue.getHue(), ofFloatColor::red.getHue());
                    ofFloatColor magnitudeColor = ofFloatColor::fromHsb(scaledHue, 128, 128 ) ;
                    diff.setColor(i, j, magnitudeColor);
                }
            }
        }
        diff.update();
        copy(*player, previousHeatMap);
        for(int i =0; i< accumulation.width; i++ ){
            for(int j =0; j<accumulation.height; j++){
                
                if(! (accumulationCount%5) ){
                    accumulation.setColor(i, j, ofColor::black);
                    accumulationCount =0;
                }
                else{
                    ofColor c = accumulation.getColor(i, j);
                    float b = c.getBrightness();
                    if(b > 50 ){
                        float scaledHue = ofMap(b ,0, 255, ofFloatColor::blue.getHue(), ofFloatColor::orange.getHue());
                        ofFloatColor magnitudeColor = ofFloatColor::fromHsb(scaledHue, 200, 200) ;
                        accumulation.setColor(i, j, magnitudeColor);
                    }
                }
            }
        }
        diffMean = mean(toCv(accumulation));
        diffMean *= Scalar(20);
        accumulation.reloadTexture();
    }
}

void CloudsVisualSystemVision::updateContourTracking(){
    if(frameIsNew){
        
        background.update(*player, thresholded);
        thresholded.update();
        blur(thresholded, 5);
        contourFinder.findContours(thresholded);
        tracker.track(contourFinder.getBoundingRects());
    }
}

void CloudsVisualSystemVision::getTextures(){
    vector<MyTracker>& followers = tracker.getFollowers();
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
    
    
}

//radio field

void CloudsVisualSystemVision::selfPresetLoaded(string presetPath){
    cout<<"LOADED PRESET: "<<presetPath<<endl;
    
    ofxUIRadio* r =(ofxUIRadio* ) rdrGui->getWidget("VIDEO");
    
    vector<ofxUIToggle* >t = r->getToggles();
    string movieName;
    for(int j=0; j<t.size();j++){
        if(t[j]->getValue()){
            movieName = t[j]->getName();
            
            cout<<"LOADING MOVIE :"<<movieName<<endl;
        }
    }
    for(int i =0; i<movieStrings.size(); i++){
        if (movieStrings[i] ==movieName) {
            cout<<"HERE "<<movieStrings[i] <<endl;
            loadMovieAtIndex(i);
        }
    }
    
    ofxUIRadio* modeRadio =(ofxUIRadio* ) rdrGui->getWidget("MODE");
    vector<ofxUIToggle* >modes = modeRadio->getToggles();
    
    for(int k=0; k<modes.size();k++){
        if(modes[k]->getValue()){
            if(modes[k]->getName() == "OPTICAL FLOW" ){
                setMode(OpticalFlow);
            }
            else if(modes[k]->getName() == "CONTOUR TRACKING" ){
                setMode(ContourTracking);
            }
            else if(modes[k]->getName() == "ABS DIFF HEAT MAP"){
                setMode(HeatMap);
            }
            else if(modes[k]->getName() == "DRAW THRESHOLDED"){
                drawThresholded = true;
                drawPlayer = false;
                drawDiff = false;
            }
            
        }
        
    }
    
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
    /*
     rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
     ofxUIButton *loadbtn = rdrGui->addButton("OPTICAL FLOW", false);
     rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
     ofxUIButton *updatebtn = rdrGui->addToggle("CONTOUR TRACKING", false);
     ofxUIButton *diffbtn = rdrGui->addToggle("DRAW DIFF", &drawDiff);
     rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
     rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
     ofxUIButton *drawthresholdedbtn = rdrGui->addToggle("DRAW THRESHOLDED", &drawThresholded);
     ofxUIButton *nextVideoButton = rdrGui->addToggle("NEXT VIDEO", false);
     rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
     ofxUIButton *prevVideoButton = rdrGui->addToggle("PREVIOUS VIDEO", false);
     */
    vector<string> modes;
    modes.push_back("OPTICAL FLOW");
    modes.push_back("CONTOUR TRACKING");
    modes.push_back("ABS DIFF HEAT MAP");
    modes.push_back("DRAW THRESHOLDED");
    
    rdrGui->addSpacer();
    rdrGui->addLabel("PLAY MODES");
    rdrGui->addRadio("MODE", modes);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    ofxUIButton *drawplayerbtn = rdrGui->addToggle("DRAW PLAYER", &drawPlayer);
    ofxUIButton *drawflowWindowbtn = rdrGui->addToggle("DRAW FLOW WINDOW", &drawFlowWindow);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    ofxUIButton *clearthresholdbtn = rdrGui->addToggle("CLEAR DIFF", false);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    rdrGui->addLabel("VIDEOS");
    rdrGui->addRadio("VIDEO", movieStrings);
    rdrGui->addSlider("VIDEO ALPHA", 0, 255, &videoAlpha);
    rdrGui->addSlider("FLOW WINDOW ALPHA", 0, 255, &windowAlpha);
    rdrGui->autoSizeToFitWidgets();
    ofAddListener(rdrGui->newGUIEvent, this, &CloudsVisualSystemVision::selfGuiEvent);
    
}

void CloudsVisualSystemVision::selfUpdate(){
    player->update();
    frameIsNew = player->isFrameNew();
    
    if(currentMode == ContourTracking){
        
        updateContourTracking();
    }
    else if(currentMode == OpticalFlow){
        
        updateOpticalFlow();
    }
    else if(currentMode == HeatMap){
        
        updateHeatMap();
    }
}

void CloudsVisualSystemVision::selfDrawBackground()
{
    
    ofSetColor(128,videoAlpha);
    
    if(drawPlayer){
        player->draw(0,0,ofGetWidth(),ofGetHeight());
    }
    if(drawThresholded){
        flowMesh.draw();
        thresholded.draw(0,0, ofGetWidth(), ofGetHeight());
    }
    
    if(currentMode == ContourTracking){
        ofPushMatrix();
        ofScale(ofGetWidth()/player->getWidth(),ofGetHeight()/player->getHeight());
        contourFinder.draw();
        vector<MyTracker>& followers = tracker.getFollowers();
        for(int i = 0; i < followers.size(); i++) {
            float b = followers[i].getLifeTime();
            
            followers[i].draw(lineWidth);
            
        }
        ofPopMatrix();
        
    }
    else if(currentMode == OpticalFlow){
        ofTexture tex = player->getTextureReference();
        if(drawFlowWindow){
            
            ofPushMatrix();
            ofPushStyle();
            ofSetColor(200,windowAlpha);
            ofScale(ofGetWidth()/player->getWidth(),ofGetHeight()/player->getHeight());
            tex.drawSubsection(mouseX-window.width/2 , mouseY-window.height/2, window.width, window.height, mouseX-window.width/2, mouseY-window.height/2);
            ofSetLineWidth(flowLineWidth);
            flowMesh.draw();
            ofPopStyle();
            ofPopMatrix();
        }
        else{
            ofPushMatrix();
            ofPushStyle();
            ofSetColor(200,windowAlpha);
            ofScale(ofGetWidth()/player->getWidth(),ofGetHeight()/player->getHeight());
            tex.draw(0,0);
            ofSetLineWidth(flowLineWidth);
            flowMesh.draw();
            ofPopStyle();
            ofPopMatrix();
        }
        
    }
    else if(currentMode == HeatMap){
        
        diff.draw(0, 0,ofGetWidth(),ofGetHeight());
        
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
    }
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
    
    mouseX = ofMap(data.x, 0, ofGetWidth(), 0, player->getWidth());
    mouseY = ofMap(data.y, 0, ofGetHeight(), 0, player->getHeight()) ;
}

void CloudsVisualSystemVision::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemVision::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemVision::setMode(CVMode mode){
    
    switch (mode) {
        case OpticalFlow:
            cout<<"setting mode to optical flow"<<endl;
            currentMode = OpticalFlow;
            drawDiff = false;
            drawThresholded = false;
            break;
            
        case ContourTracking:
            currentMode = ContourTracking;
            drawDiff = false;
            drawThresholded = false;
            drawPlayer = true;
            break;
            
        case HeatMap:
            drawDiff = true;
            currentMode = HeatMap;
            drawThresholded = false;
            drawPlayer =false;
            break;
            
        default:
            break;
    }
}

void CloudsVisualSystemVision::selfGuiEvent(ofxUIEventArgs &e)
{
    
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    cout<<kind<<" : "<<name<<endl;
    ofxUIRadio* r = (ofxUIRadio*)e.widget;
    ofxUIButton* b  = (ofxUIButton*) e.widget;
    
    
    
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
    else if (name == "OPTICAL FLOW"){
        setMode(OpticalFlow);
    }
    else if( name == "CONTOUR TRACKING" ){
        setMode(ContourTracking);
        
    }
    else if (name == "DRAW PLAYER"){
        drawPlayer = b->getValue();
        drawThresholded = false;
        drawDiff = false;
    }
    else if( name == "DRAW THRESHOLDED"){
        drawThresholded = b->getValue();
        drawPlayer =false;
        drawDiff = false;
    }
    else if( name == "ABS DIFF HEAT MAP"){
        setMode(HeatMap);
        
    }
    else if( name == "CLEAR DIFF"){
        b->setValue(false);
        clearAccumulation();
    }
    else if(name == "FLOW WINDOW"){
        drawFlowWindow = b->getValue();
    }


    if (kind == OFX_UI_WIDGET_TOGGLE){
        thresholded.clear();
        background.reset();
        updateImagesForNewVideo();
        ofxUIToggle* t = (ofxUIToggle*)e.widget;
        cout<<t->getName()<<endl;
        for(int i =0; i<movieStrings.size(); i++){
            if (movieStrings[i] == t->getName()) {
                loadMovieAtIndex(i);
            }
        }
    }
    
}

void CloudsVisualSystemVision::loadCurrentMovie(){
    player = ofPtr<ofVideoPlayer>(new ofVideoPlayer());
    if(player->loadMovie(getVisualSystemDataPath() + movieStrings[ movieIndex ])){
        resizeToPixels.allocate(player->getWidth()/scale,player->getHeight()/scale,
                                player->getPixelsRef().getImageType());
        player->play();
    }
    else{
        cout<<"Not Playing"<<endl;
    }
    
    cout<<"Player dimensions (new) :"<< player->getWidth()<<" , "<<player->getHeight() <<endl;
    updateCVParameters();
    populateOpticalFlowRegions();
    updateImagesForNewVideo();
    resetFlowField();
}

void  CloudsVisualSystemVision::loadMovieAtIndex(int index){
    movieIndex = index;
    player = ofPtr<ofVideoPlayer>(new ofVideoPlayer());
    if(player->loadMovie(getVisualSystemDataPath() +movieStrings[ movieIndex ])){
        resizeToPixels.allocate(player->getWidth()/scale,player->getHeight()/scale,
                                player->getPixelsRef().getImageType());
        player->play();
    }
    else{
        cout<<"Not Playing"<<endl;
    }
    //    videoRect.alignTo(screenRect);
    cout<<"Player dimensions (new) :"<< player->getWidth()<<" , "<<player->getHeight() <<endl;
    updateCVParameters();
    populateOpticalFlowRegions();
    updateImagesForNewVideo();
    resetFlowField();
}
void CloudsVisualSystemVision::guiRenderEvent(ofxUIEventArgs &e)
{
    
}