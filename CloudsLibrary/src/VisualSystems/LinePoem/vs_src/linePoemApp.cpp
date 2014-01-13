#include "linePoemApp.h"
#include "CloudsVisualSystem.h"
#include "CloudsGlobal.h"

#include <fstream>
#include <iterator>
#include <vector>


ofPolyline             PR;
ofPolyline             PR2;


int                    nAngleLines;
int                    curAngleLine;
bool                   bDrawing;
int                    counter;
bool                   bPlaying;
float                  avgRate;
int                    count;




//SYS->getCanvasWidth()

//---------------------------------------------------------------------
void linePoemApp::loadAllDrawings(){
    
    cout << GetCloudsDataPath()  + "visualsystems/LinePoem/" + "allDrawings.dat" << endl;
  
    ofBuffer buf = ofBufferFromFile(GetCloudsDataPath()  + "visualsystems/LinePoem/" + "allDrawings.dat");
    
    char * buff  = buf.getBinaryBuffer();

    int count=buf.size() / (250 * sizeof(int));
    
    cout << count << endl;
    
    
    
    int pos = 0;
    for (int j = 0; j < count; j++){
        
        angleLengthLine line;
        line.setup(250);
        for (int i = 0; i < 250; i++){

            float a;
            int * temp = (int *)&a;
            
            *temp  = (  ((buff[pos]  << 24) & 0xFF000000) + ((buff[pos+1] << 16) & 0x00FF0000) +  ((buff[pos+2] << 8) & 0x0000FF00) +   ((buff[pos+3] << 0) & 0x000000FF));
            pos += 4;
            //cout << a << endl;
            
            line.ang[i].angle = a;

        }
        alines.push_back(line);
    }
    cout << "done " << endl;
}

void linePoemApp::saveAllDrawings(){
//
//    
//    ofBuffer buf;
//    
//    int pos = 0;
//    for (int j = 0; j < alines.size(); j++){
//        for (int i = 0; i < 250; i++){
//        
//            float a = alines[j].ang[i].angle;
//            char * temp = (char *)&a;
//            
//            char vals[4];
//            vals[3] = temp[0];
//            vals[2] = temp[1];
//            vals[1] = temp[2];
//            vals[0] = temp[3];
//            
//            
//            buf.append(vals, 4);
//            
//            
//        }
//    }
//    
//    ofBufferToFile("allDrawings.dat", buf);
//    
}






//--------------------------------------------------------------
void linePoemApp::setup(){
    

    
    ofSetVerticalSync(true);
    ALM        = new angleLengthLineMorpher(250);
    
    nAngleLines    = 150;
    curAngleLine   = 5;

    
    bDrawing       = false;                                // am I drawing?
    counter        = 0;
    
    
    ofPolyline temp;
    for (int i = 0; i < 250; i++){
        temp.addVertex(ofPoint(i*1, 0));
    }
    angleLengthLine line;
    line.setup(250);
    line.convertFromAngleLengthLine(temp);
    ALM->setPres(&line);
    ALM->setTarg(&line);
    
    
    bDrawing = false;
    bPlaying = true;
    loadAllDrawings();
    
    for (int i = 0; i < 100; i++){
        ALM->draw(0.5f + 0.5f * sin(ofGetElapsedTimeMillis() / 1000.0f));
    }
    
    
    //ofSetFrameRate(4);
    
    fbo.allocate( SYS->getCanvasWidth() , SYS->getCanvasHeight(),GL_RGBA,  2 );
    
}

//--------------------------------------------------------------
void linePoemApp::update(){

    if (bPlaying == true && ALM->bAmAnimating == false){
        curAngleLine++;
        if (curAngleLine == alines.size()) curAngleLine = 0;
        ALM->setTarg(&alines[curAngleLine]);
        
    }
    
}

//--------------------------------------------------------------
void linePoemApp::draw(){
    
    
    
    fbo.begin();
    ofBackground(0);
    
    
    ofFill();
    
    if (bDrawing == false) ofSetColor(255);
    else                   ofSetColor(255);
    
    ofSetLineWidth(2.0);
    ofPushMatrix();
    
    float scale = (float)fbo.getWidth() / 1024.0;
    ofScale(scale, scale, 1);
    ALM->draw(0.5f + 0.5f * sin(ofGetElapsedTimeMillis() / 1000.0f));
    ofPopMatrix();
    
    ofSetLineWidth(1);
    fbo.end();
    
    fbo.draw(0,0, SYS->getCanvasWidth(), SYS->getCanvasHeight());
    
    
}

//--------------------------------------------------------------
void linePoemApp::keyPressed(int key){
    
    
    
    
    
}

//--------------------------------------------------------------
void linePoemApp::keyReleased(int key){

}

//--------------------------------------------------------------
void linePoemApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void linePoemApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void linePoemApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void linePoemApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void linePoemApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void linePoemApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void linePoemApp::dragEvent(ofDragInfo dragInfo){ 

}