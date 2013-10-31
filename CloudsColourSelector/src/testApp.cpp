#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
    /*
     if(ofDirectory("../../../CloudsData/").exists()){
     parser.parseLinks("../../../CloudsData/links/clouds_link_db.xml");
     parser.setup("../../../CloudsData/fcpxml/");
     parser.parseClusterMap("../../../CloudsData/gephi/CLOUDS_test_5_26_13.SVG");
     }
     else{
     cout << "SETTING UP IN DATA DIRECTORY" << endl;
     parser.parseLinks("clouds_link_db.xml");
     parser.setup("xml");
     }
     */
    if(! player.loadMovie("Zach_Fucking_boring.mov")){
        cout<<"Movie not loaded"<<endl;
    }
    img.allocate(player.getWidth(), player.getHeight(), OF_IMAGE_COLOR_ALPHA);
    player.play();
    threshold = 0.5;
    
    gui = new ofxUISuperCanvas("COLOUR SELECTOR", OFX_UI_FONT_MEDIUM);
    gui->addSpacer();
    gui->addSpacer();
    gui->addSlider("THRESHOLD", 0.0, 2.0, &threshold);
    gui->addSlider("HUE THRESHOLD", 0.0, 1.0, &hueThreshold);
    gui->addSlider("SATURATION THRESHOLD", 0.0, 1.0, &satThreshold);
    gui->addSlider("BRIGTHNESS THRESHOLD", 0.0, 1.0, &brightThreshold);
    gui->addSpacer();
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    
    gui->loadSettings("GUI/guiSettings.xml");
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
    //	string name = e.widget->getName();
    //	int kind = e.widget->getKind();
}

//--------------------------------------------------------------
void testApp::update(){
    
    player.update();
    
}

//--------------------------------------------------------------
void testApp::draw(){
    player.draw(0, 0,player.getWidth()/2,player.getHeight()/2);
    
    img.draw(player.width/2,0, player.getWidth()/2, player.getHeight()/2);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if(key == OF_KEY_UP){
        threshold += 0.2;
        cout<<"Threshold updated : "<<threshold<<endl;
    }
    else if(key == OF_KEY_DOWN){
        threshold-= 0.2;
        cout<<"Threshold updated : "<<threshold<<endl;
    }
    else if (key == ' '){
        samplePoint.set(mouseX, mouseY);
        checkColorDistance();
    }
    else if(key == 'f'){
        ofToggleFullscreen();
    }
}

//--------------------------------------------------------------
void testApp::exit(){
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
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
void testApp::checkColorDistance(){
    ofFloatColor c =player.getPixelsRef().getColor(samplePoint.x,samplePoint.y);
        ofVec3f sample = ofVec3f(c.getHue() ,c.getSaturation(),c.getBrightness() );
    ofPixels pixels = player.getPixelsRef();

    cout<<"HSB :"<< sample<<endl;
    for( int j=0; j < player.getHeight(); j++){
        for (int i =0 ; i<player.getWidth();i++) {
            
            ofFloatColor currentColour = pixels.getColor(i, j);
            
            //            ofVec3f current = ofVec3f(currentColour.getHue() ,currentColour.getSaturation() ,currentColour.getBrightness());
            
            float hue = abs(c.getHue() - currentColour.getHue());
            float sat = abs(c.getSaturation() - currentColour.getSaturation());
            float bright = abs(c.getBrightness() - currentColour.getBrightness());
            
            if(hue < hueThreshold && sat < satThreshold && bright < brightThreshold){
                ofFloatColor col;
                col.setHsb(currentColour.getHue(),currentColour.getSaturation(),currentColour.getBrightness() );
                img.setColor(i, j, col);
            }
            else{
                img.setColor(i,j,ofFloatColor(0,0,0));
            }
            /*
             
             if(sample.distance(current) < threshold){
             if(ofGetKeyPressed('1')){
             cout<<"distance : "<<sample.distance(current)<<endl;
             }
             
             ofFloatColor col;
             col.setHsb(currentColour.getHue(),currentColour.getSaturation(),currentColour.getBrightness() );
             img.setColor(i, j, col);
             
             }
             else{
             img.setColor(i, j, ofFloatColor(0,0,0) );
             }
             */
        }
    }
    img.update();
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