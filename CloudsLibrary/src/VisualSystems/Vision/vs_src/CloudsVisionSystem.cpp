//
//  CloudsVisionSystem.cpp
//
//  Created by Patricio Gonzalez Vivo on 7/23/13.
//
//

#include "CloudsVisionSystem.h"
using namespace ofxCv;
using namespace cv;

string CloudsVisionSystem::getSystemName()
{
	return "VisionSystem";
}

void CloudsVisionSystem::selfSetup()
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
    flowFirstFrame = true;
    useFarneback = true;
    drawPlayer = true;
    drawThresholded =false;
    
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
    screenRect = ofRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
}

void CloudsVisionSystem::clearAccumulation(){
    for(int j=0; j<accumulation.height; j++){
        for( int i=0; i<accumulation.width; i++){
            accumulation.setColor(i, j, ofFloatColor(0));
        }
    }
}
void CloudsVisionSystem::updateImagesForNewVideo(){
    
    imitate(previousHeatMap, *player);
    imitate(diff, *player);
    accumulation.allocate(player->width, player->height, OF_IMAGE_COLOR);
    
}
void CloudsVisionSystem::resetFlowField(){
    
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
void CloudsVisionSystem::populateOpticalFlowRegions(){
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
void CloudsVisionSystem::updateOpticalFlowParameters(){
    farneback.setPyramidScale(pyrScale);
    farneback.setNumLevels( levels );
    farneback.setWindowSize( winsize) ;
    farneback.setNumIterations( iterations);
    farneback.setPolyN( polyN) ;
    farneback.setPolySigma( polySigma );
    farneback.setUseGaussian( useFarneback );
}

void CloudsVisionSystem::updateOpticalFlow(){
    
    if (frameIsNew) {
        
        int width = player->getWidth()/scale;
        int height = player->getHeight()/scale;
        //        test.resize(width, height);
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

                float scaledHue = ofMap(mag,0, 50, ofFloatColor::blue.getHue(), ofFloatColor::red.getHue());
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

void CloudsVisionSystem::updateHeatMap(){
    
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

void CloudsVisionSystem::updateContourTracking(){
    if(frameIsNew){
        
        background.update(*player, thresholded);
        thresholded.update();
        blur(thresholded, 5);
        contourFinder.findContours(thresholded);
        tracker.track(contourFinder.getBoundingRects());
    }
}

void CloudsVisionSystem::getTextures(){
    vector<MyTracker>& followers = tracker.getFollowers();
}

void CloudsVisionSystem::updateCVParameters(){
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

void CloudsVisionSystem::selfPresetLoaded(string presetPath){
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
    //TODO: Clean up the paths

}


void CloudsVisionSystem::selfBegin()
{

    
}

void CloudsVisionSystem::selfEnd()
{
    
}

void CloudsVisionSystem::selfExit()
{
}

void CloudsVisionSystem::selfSetupSystemGui()
{


    sysGui->addLabel("OPTICAL FLOW PARAMS");

    sysGui->addSlider("PYRSCALE", .5, 0.9, &pyrScale);
    sysGui->addSlider("LEVELS",  1, 8, &levels);
    sysGui->addSlider("WINSIZE",  4, 64, &winsize);
    sysGui->addSlider("ITERATIONS",1, 8, &iterations);
    sysGui->addSlider("POLYN",5, 7, &polyN);
    sysGui->addSlider("POLYSIGMA", 1.1, 1.1, &polySigma);
    sysGui->addButton("UPDATE FLOW PARAMS", false);
    sysGui->addSlider("WINDOW WIDTH", 100, 1000, &windowWidth);
    sysGui->addSlider("WINDOW HEIGHT", 100, 1000, &windowHeight);
    sysGui->addSlider("FLOW LINE LENGTH", 0.5, 3, &flowLineMultiplier);
    sysGui->addSlider("FLOW COLOUR MAP RANGE", 10, 200, &flowColorMapRange);
    sysGui->addSlider("FLOW LINE WIDTH", 1, 10, &flowLineWidth);
    
    sysGui->autoSizeToFitWidgets();
    
    ofAddListener(sysGui->newGUIEvent, this, &CloudsVisionSystem::selfGuiEvent);
    
}

void CloudsVisionSystem::selfSetupRenderGui()
{
    
    rdrGui->addSpacer();
    rdrGui->addLabel("CV MODES");
    rdrGui->addSlider("BOX LINE WIDTH", 1, 10, &lineWidth);
    rdrGui->addLabel("BACKGROUND PARAM");
    rdrGui->addSlider("LEARNING TIME", 0,100,&learningTime);
    rdrGui->addSlider("THRESHOLD VALUE", 0,255  ,&thresholdValue);
    
    rdrGui->addLabel("TRACKER PARAM");
    rdrGui->addSlider("PERSISTANCE", 0,100,&cvPersistance);
    rdrGui->addSlider("MAXDISTANCE", 0,100  ,&cvMaxDistance);
    
    rdrGui->addLabel("CONTOUR FINDER PARAMS");
    rdrGui->addSlider("MIN AREA RADIUS", 0,50,&cvMinAreaRadius);
    rdrGui->addSlider("MAX AREA RADIUS",0,255,&cvMaxAreaRadius);
    rdrGui->addSlider("THRESHOLD VALUE", 0, 255, &cvThresholdValue);
    sysGui->addButton("UPDATE CV PARAMS", false);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    ofxUIButton *loadbtn = rdrGui->addButton("OPTICAL FLOW", false);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ofxUIButton *updatebtn = rdrGui->addToggle("CONTOUR TRACKING", false);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    ofxUIButton *drawplayerbtn = rdrGui->addToggle("DRAW PLAYER", &drawPlayer);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ofxUIButton *drawthresholdedbtn = rdrGui->addToggle("DRAW THRESHOLDED", &drawThresholded);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    ofxUIButton *diffbtn = rdrGui->addToggle("DRAW DIFF", &drawDiff);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ofxUIButton *clearthresholdbtn = rdrGui->addToggle("CLEAR DIFF", false);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
//    ofxUIButton *nextVideoButton = rdrGui->addToggle("NEXT VIDEO", false);
//    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
//    ofxUIButton *prevVideoButton = rdrGui->addToggle("PREVIOUS VIDEO", false);
    rdrGui->addRadio("VIDEO", movieStrings);

    rdrGui->autoSizeToFitWidgets();
    ofAddListener(rdrGui->newGUIEvent, this, &CloudsVisionSystem::selfGuiEvent);
    
}

void CloudsVisionSystem::selfUpdate(){
    player->update();
    frameIsNew = player->isFrameNew();
    
    if(currentMode == ControurTracking){
        
        updateContourTracking();
    }
    else if(currentMode == OpticalFlow){
        
        updateOpticalFlow();
    }
    else if(currentMode == HeatMap){
        
        updateHeatMap();
    }
}

void CloudsVisionSystem::selfDrawBackground()
{
    
    
    float x =ofGetWidth()/2 -player->getWidth()/2;
    float y =ofGetHeight()/2 -player->getHeight()/2;
    
    ofSetColor(128,128);

    if(drawPlayer){
        player->draw(0,0);
    }
    if(drawThresholded){
        flowMesh.draw();
        thresholded.draw(0,0, thresholded.width, thresholded.height);
    }
    
    if(currentMode == ControurTracking){
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofPushMatrix();
        ofTranslate(-player->getWidth()/2, -player->getHeight()/2);
        contourFinder.draw();
        
        vector<MyTracker>& followers = tracker.getFollowers();
        for(int i = 0; i < followers.size(); i++) {
            float b = followers[i].getLifeTime();
            
            followers[i].draw(lineWidth);
            
        }
        ofPopMatrix();
        ofPopMatrix();
        
    }
    else if(currentMode == OpticalFlow){
        
        ofTexture tex = player->getTextureReference();
        ofPushStyle();
        ofSetColor(200);
        tex.drawSubsection(mouseX-window.width/2 , mouseY-window.height/2, window.width, window.height, mouseX-window.width/2, mouseY-window.height/2);
        ofSetLineWidth(flowLineWidth);
        flowMesh.draw();
        ofPopStyle();

    }
    else if(currentMode == HeatMap){
        
        diff.draw(x, y);
        
        float diffRed = diffMean[0];
        float mapRed = ofMap(diffRed, 0, 512, 0, accumulation.width,true);
        float diffGreen = diffMean[1];
        float mapGreen = ofMap(diffGreen, 0, 512, 0, accumulation.width,true);
        float diffBlue = diffMean[2];
        float mapBlue = ofMap(diffBlue, 0, 512, 0, accumulation.width,true);
        
        ofSetColor(255, 0, 0);
        ofRect(0,0 + accumulation.height -30 , mapRed, 10);
        ofSetColor(0, 255, 0);
        ofRect(0, 0 + accumulation.height -15 , mapGreen, 10);
        ofSetColor(0, 0, 255);
        ofRect(0, 0+ accumulation.height  ,  mapBlue, 10);
    }
    
    
}

void CloudsVisionSystem::selfDraw()
{
    
}


void CloudsVisionSystem::selfSetupGuis()
{
    
}

void CloudsVisionSystem::selfAutoMode()
{
    
}

void CloudsVisionSystem::guiSystemEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisionSystem::selfKeyPressed(ofKeyEventArgs & args){
    
}

void CloudsVisionSystem::selfDrawDebug()
{
    
}

void CloudsVisionSystem::selfSceneTransformation()
{
    
}

void CloudsVisionSystem::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisionSystem::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisionSystem::selfMouseMoved(ofMouseEventArgs& data)
{
    mouseX = data.x ;
    mouseY = data.y ;
}

void CloudsVisionSystem::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisionSystem::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisionSystem::selfSetupGui()
{
    
}

void CloudsVisionSystem::setMode(CVMode mode){

    switch (mode) {
        case OpticalFlow:
            cout<<"setting mode to optical flow"<<endl;
            currentMode = OpticalFlow;
            drawDiff = false;
            drawThresholded = false;
            break;
        
        case ControurTracking:
            currentMode = ControurTracking;
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

void CloudsVisionSystem::selfGuiEvent(ofxUIEventArgs &e)
{
    
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    cout<<kind<<endl;
    ofxUIRadio* r = (ofxUIRadio*)e.widget;
    ofxUIButton* b  = (ofxUIButton*) e.widget;    

    

    if(name == "UPDATE CV PARAMS" &&  b->getValue() ){
        b->setValue(false);
        updateCVParameters();
        cout<<"updating CV parameters"<<endl;
    }
    else if(name == "UPDATE FLOW PARAMS" && b->getValue()){
        b->setValue(false);
        updateOpticalFlowParameters();
        cout<<"Updating Optical Flow parameters"<<endl;
    }
    else if (name == "OPTICAL FLOW" &&  b->getValue()){
        setMode(OpticalFlow);
    }
    else if( name == "CONTOUR TRACKING" &&  b->getValue() ){
        setMode(ControurTracking);
     
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
    else if( name == "DRAW DIFF"){
        drawDiff = true;
        drawThresholded = false;
        drawPlayer =false;

    }
    else if( name == "CLEAR DIFF"){
        b->setValue(false);
        clearAccumulation();
    }
    else if(name == "NEXT VIDEO" && b->getValue()){
        b->setValue(false);
        thresholded.clear();
        background.reset();
        movieIndex = (movieIndex + 1) % movieStrings.size();
        loadCurrentMovie();
    }
    else if(name == "PREVIOUS VIDEO"  && b->getValue()){
        b->setValue(false);
        thresholded.clear();
        background.reset();
        updateImagesForNewVideo();
        resetFlowField();
        
        movieIndex = (movieIndex-1 + movieStrings.size()) % movieStrings.size();
        loadCurrentMovie();
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

void CloudsVisionSystem::loadCurrentMovie(){
    player = ofPtr<ofVideoPlayer>(new ofVideoPlayer());
    if(player->loadMovie(movieStrings[ movieIndex ])){
        resizeToPixels.allocate(player->getWidth()/scale,player->getHeight()/scale,
                                player->getPixelsRef().getImageType());
        player->play();
    }
    else{
        cout<<"Not Playing"<<endl;
    }
    videoRect = ofRectangle(0,0,player->getWidth(),player->getHeight());
    cout<<"Player dimensions (new) :"<< player->getWidth()<<" , "<<player->getHeight() <<endl;
    updateCVParameters();
    populateOpticalFlowRegions();
    updateImagesForNewVideo();
    resetFlowField();
}

void  CloudsVisionSystem::loadMovieAtIndex(int index){
    movieIndex = index;
    player = ofPtr<ofVideoPlayer>(new ofVideoPlayer());
    if(player->loadMovie(movieStrings[ movieIndex ])){
        resizeToPixels.allocate(player->getWidth()/scale,player->getHeight()/scale,
                                player->getPixelsRef().getImageType());
        player->play();
    }
    else{
        cout<<"Not Playing"<<endl;
    }
    videoRect = ofRectangle(0,0,player->getWidth(),player->getHeight());
    //    videoRect.alignTo(screenRect);
    cout<<"Player dimensions (new) :"<< player->getWidth()<<" , "<<player->getHeight() <<endl;
    updateCVParameters();
    populateOpticalFlowRegions();
    updateImagesForNewVideo();
    resetFlowField();
}
void CloudsVisionSystem::guiRenderEvent(ofxUIEventArgs &e)
{
    
}