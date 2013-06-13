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
    cout << "Load paths" << endl;
    loadPath( coast, "simple-coast.txt");
    loadPath( borders, "simple-borders.txt");
    loadPath( rivers, "simple-rivers.txt");
}

void CloudsVisualSystemWorld::loadPath(vector<ofPolyline> &_path, string _file){
    
    ifstream fileIn;
	
    string filePath = getDataPath()+"visualsystems/World/"+_file;
    ofBuffer buffer = ofBufferFromFile(filePath);

    int lineCount = 0;
    ofPolyline newPoly;
    while(!buffer.isLastLine()) {
        string temp = buffer.getNextLine();
        
        if(temp.length() != 0) {
            
            vector<string> values = ofSplitString(temp, " ");
            
            if ( values[0] == "segment"){
                
                _path.push_back( newPoly );
                newPoly.clear();
            } else {
                newPoly.addVertex(ofPoint(ofToFloat(values[0]),
                                          ofToFloat(values[1]),
                                          ofToFloat(values[2])));
            }
            
            lineCount++;
        }
    }
}

void CloudsVisualSystemWorld::selfSetupSystemGui()
{

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
    
    ofFill();
    ofSetColor(20,20);
	ofDrawSphere(0, 0, 290);
    
    ofNoFill();
    ofSetColor(255,20);
	ofDrawSphere(0, 0, 300);
    
    ofSetLineWidth(1.5);
    ofSetColor(255);
    for(int i = 0; i < coast.size(); i++){
        coast[i].draw();
    }
    
    ofSetLineWidth(0.5);
    ofSetColor(150);
    for(int i = 0; i < borders.size(); i++){
        borders[i].draw();
    }
    
    ofSetLineWidth(0.2);
    ofSetColor(0,70,100);
    for(int i = 0; i < rivers.size(); i++){
        rivers[i].draw();
    }
    
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