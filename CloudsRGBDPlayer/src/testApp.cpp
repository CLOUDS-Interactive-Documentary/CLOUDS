/**
 * Example - Mesh Builder
 * This example shows how to create a RGBD Mesh on the CPU
 *
 *
 * James George 2012 
 * Released under the MIT License
 *
 * The RGBDToolkit has been developed with support from the STUDIO for Creative Inquiry and Eyebeam
 */

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(25);
    
    //set up the game camera
    cam.setup();
    cam.speed = 20;
    cam.autosavePosition = true;
    cam.targetNode.setPosition(ofVec3f());
    cam.targetNode.setOrientation(ofQuaternion());
    cam.targetXRot = -180;
    cam.targetYRot = 0;
    cam.rotationZ = 0;    
    
	xsimplify = 1;
    ysimplify = 1;
    xshift = 0;
    yshift = 0;
    
    gui.setup("tests");
    gui.add(xshift.setup("xshift", ofxParameter<float>(), -.15, .15));
    gui.add(yshift.setup("yshift", ofxParameter<float>(), -.15, .15));
    gui.add(xsimplify.setup("xsimplify", ofxParameter<float>(), 1, 8));
    gui.add(ysimplify.setup("ysimplify", ofxParameter<float>(), 1, 8));
    gui.add(flipTexture.setup("flip texture", ofxParameter<bool>()));
        
    gui.loadFromFile("defaultSettings.xml");
    
    if(xsimplify < 1){
        xsimplify = 1;
    }
    if(ysimplify < 1){
        ysimplify = 1;
    }
    
    player.loadMovie("Higa/movie.mov");
    renderer.setup("Higa");
    player.play();
    
    renderer.setTexture( player );
}

//--------------------------------------------------------------
void testApp::update(){
    player.update();
    
    //copy any GUI changes into the mesh
    renderer.shift = ofVec2f(xshift,yshift);
    renderer.simplify = ofVec2f(xsimplify,ysimplify);
    renderer.bFlipTexture = flipTexture;
    
    renderer.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    
    cam.begin();
    ofSetColor(255);
    glEnable(GL_DEPTH_TEST);
    renderer.drawMesh();
    glDisable(GL_DEPTH_TEST);
    cam.end();

    gui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == ' '){
        //player.togglePlay();
    }
    if(key == 'r'){
        renderer.reloadShader();
    }
}

//--------------------------------------------------------------
void testApp::exit(){
    gui.saveToFile("defaultSettings.xml");
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
	
}
