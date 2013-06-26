//
//  CloudsVisualSystemWorld.cpp
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/13/13.
//
//

#include "CloudsVisualSystemWorld.h"

string CloudsVisualSystemWorld::getSystemName()
{
	return "World";
}

void CloudsVisualSystemWorld::selfSetup()
{
    ofSetCircleResolution(60);
//    loadParticles("simple-rivers.txt");
    
    loadPath(coastVbo, "simple-coast.txt");
    loadPath(riversVbo, "simple-rivers.txt");
    
//    loadPoints( points, "airports.txt");
    loadPoints( "simple-cities.txt" );
    
    loadStarts( "constelations.txt" );
    globalOffset.set(0,0,0);
    
    nMaxPoints = 1000;
}

void CloudsVisualSystemWorld::loadParticles( string _file ){
    ifstream fileIn;
	
    string filePath = getDataPath()+"visualsystems/World/"+_file;
    ofBuffer buffer = ofBufferFromFile(filePath);
    
    wParticle *lastParticle = NULL;
    wParticle *newParticle = NULL;
    
    while(!buffer.isLastLine()) {
        string temp = buffer.getNextLine();
        
        if(temp.length() != 0) {
            
            vector<string> values = ofSplitString(temp, " ");
            
            if ( values[0] == "segment"){
                
                newParticle = NULL;
                lastParticle = NULL;
                
            } else {
                
                newParticle = new wParticle();
                newParticle->set( ofPoint(ofToFloat(values[0]),
                                          ofToFloat(values[1]),
                                          ofToFloat(values[2]) ) );
                newParticle->loc = *newParticle;
                newParticle->bTrail = false;
                
                if ( lastParticle != NULL){
                    newParticle->connect = lastParticle;
                }
                
                particles.push_back(newParticle);
                lastParticle = newParticle;
            }
        }
    }
}

void CloudsVisualSystemWorld::loadPath(ofVboMesh &_vbo, string _file){
    _vbo.setMode(OF_PRIMITIVE_LINES);
    
    ifstream fileIn;
	
    string filePath = getDataPath()+"visualsystems/World/"+_file;
    ofBuffer buffer = ofBufferFromFile(filePath);
    
    int lineCount = 0;
    ofPoint prevPoint;
    
    bool bFirst = true;
    while(!buffer.isLastLine()) {
        string temp = buffer.getNextLine();
        
        if(temp.length() != 0) {
            
            vector<string> values = ofSplitString(temp, " ");
            
            
            if ( values[0] == "segment"){
                bFirst = true;
                
            } else {
                ofPoint newPoint = ofPoint(ofToFloat(values[0]),
                                           ofToFloat(values[1]),
                                           ofToFloat(values[2]));
                if (!bFirst){
                    _vbo.addVertex(prevPoint);
                    _vbo.addVertex(newPoint);
                } else {
                    bFirst = false;
                }
                
                prevPoint = newPoint;
            }
            
            lineCount++;
        }
    }
}

void CloudsVisualSystemWorld::loadPoints(string _file){
    string filePath = getDataPath()+"visualsystems/World/"+_file;
    ofBuffer buffer = ofBufferFromFile(filePath);
    
    while(!buffer.isLastLine()) {
        string temp = buffer.getNextLine();
        
        if(temp.length() != 0) {
            vector<string> values = ofSplitString(temp, "|");
            
            wPoint worldPoint;
            worldPoint.place(ofToFloat(values[1]),ofToFloat(values[2]));
            worldPoint.noisePeaks = &pointNoisePeaks;
            
            worldPoints.push_back(worldPoint);
        }
    }
}

void CloudsVisualSystemWorld::loadStarts( string _file){
    string filePath = getDataPath()+"visualsystems/World/"+_file;
    ofBuffer buffer = ofBufferFromFile(filePath);
    
    while(!buffer.isLastLine()) {
        string temp = buffer.getNextLine();
        
        if(temp.length() != 0) {
            vector<string> values = ofSplitString(temp, ",");
            
            wParticle *pA = new wParticle();
            pA->place(ofToFloat(values[2])*(360/24)-180,
                      ofToFloat(values[1]),
                      6000);
            
            wParticle *pB = new wParticle();
            pB->place(ofToFloat(values[4])*(360/24)-180,
                      ofToFloat(values[3]),
                      6000);
            
            pB->connect = pA;
            
            stars.push_back(pA);
            stars.push_back(pB);
        }
    }
}

void CloudsVisualSystemWorld::selfSetupSystemGui()
{
    sysGui->addLabel("Spikes");
    sysGui->addSlider("pointsNoisePeaks", 0.0, 500, &pointNoisePeaks);
    
    sysGui->addLabel("Flocking particles");
    sysGui->addSlider("max_number", 0.0, 1000, &nMaxPoints);
    sysGui->addSlider("density", 0.9, 1.0, &density);
    sysGui->addSlider("gravity", 0.0, 1.0, &gravity);
    sysGui->addSlider("repulsion", 0.0, 1.0, &repulsion);
    sysGui->addSlider("turbulence", 0.0, 0.09, &turbulence);
    sysGui->addSlider("neigbordhood", 1, 300, &neigbordhood);
    sysGui->addSlider("independence", 0, 0.01, &independence);

}

void CloudsVisualSystemWorld::selfSetupRenderGui()
{

}

void CloudsVisualSystemWorld::guiSystemEvent(ofxUIEventArgs &e)
{

}


void CloudsVisualSystemWorld::selfKeyPressed(ofKeyEventArgs & args){
    
}

void CloudsVisualSystemWorld::selfUpdate()
{
    ofSetColor(255);

    if ( (worldPoints.size() > 2) && (particles.size() < nMaxPoints-2) ){
        int randomIndex = ofRandom(worldPoints.size()-1);
        
        wParticle *newParticle = new wParticle();
        newParticle->set( worldPoints[randomIndex] );
        newParticle->loc = *newParticle;
        newParticle->color = worldPoints[randomIndex].color;
        newParticle->bTrail = true;
        particles.push_back(newParticle);
    }
    
    while (particles.size() > nMaxPoints) {
        delete particles[0];
        particles.erase(particles.begin());
    }
    
    for(int i = 0; i < particles.size(); i++){
        particles[i]->addRepulsionForce( ofPoint(0,0,0), 400, repulsion);
        particles[i]->addAttractionForce( ofPoint(0,0,0), 1000, gravity);
        particles[i]->applyFlockingForce(&globalOffset,neigbordhood,independence);
        particles[i]->update(density);
    }
    
    globalOffset += ofPoint(turbulence/neigbordhood,
							turbulence/neigbordhood,
							turbulence/neigbordhood);
    
    for(int i = 0; i < worldPoints.size(); i++){
        worldPoints[i].update();
    }
}

void CloudsVisualSystemWorld::selfDraw()
{
    mat->begin();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    ofPushMatrix();
    ofPushStyle();
    
    ofScale(0.3,0.3,0.3);
    
    ofRotateY(ofGetFrameNum()*0.01);
    
    //  Render globe
    //
    ofFill();
    ofSetColor(20,20);
	ofDrawSphere(0, 0, 290);
    //
    ofNoFill();
    ofSetColor(255,20);
	ofDrawSphere(0, 0, 295);
    //
    ofSetLineWidth(1.5);
    ofSetColor(255);
    coastVbo.drawWireframe();
    //
    ofSetLineWidth(0.2);
    ofSetColor(0,140,200);
    riversVbo.drawWireframe();
    ofSetColor(255,100);
    
    //  Particles
    //
    for(int i = 0; i < particles.size(); i++){
        particles[i]->draw();
    }
    
    int randomCity = ofRandom(worldPoints.size());
    if ( !worldPoints[randomCity].bRipple){
        worldPoints[randomCity].bRipple = true;
    }
    //  Spikes
    //
    for(int i = 0; i < worldPoints.size(); i++){
        worldPoints[i].draw();
    }
    
    //  Stars
    //
//    for(int i = 0; i < stars.size(); i++){
//        stars[i]->draw();
//    }
    
    ofPopStyle();
    ofPopMatrix();
    
    glDisable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);
    mat->end();
}


void CloudsVisualSystemWorld::selfSetupGuis()
{
    
}

void CloudsVisualSystemWorld::selfAutoMode()
{
    
}


void CloudsVisualSystemWorld::selfDrawBackground()
{
    
}

void CloudsVisualSystemWorld::selfDrawDebug()
{
    
}

void CloudsVisualSystemWorld::selfSceneTransformation()
{
    
}

void CloudsVisualSystemWorld::selfExit()
{
    
}

void CloudsVisualSystemWorld::selfBegin()
{
    
}

void CloudsVisualSystemWorld::selfEnd()
{
    
}

void CloudsVisualSystemWorld::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemWorld::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemWorld::selfMouseMoved(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemWorld::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemWorld::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemWorld::selfSetupGui()
{
    
}

void CloudsVisualSystemWorld::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemWorld::guiRenderEvent(ofxUIEventArgs &e)
{
    
}