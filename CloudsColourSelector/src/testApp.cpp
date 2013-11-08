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
//    gui->addSlider("THRESHOLD", 0.0, 1.0, &threshold);
    gui->addSlider("THRESHOLD_LOWER", 0.0, 1.0, &thresholdLower);
    gui->addSlider("THRESHOLD_UPPER", 0.0, 1.0, &thresholdUpper);
    gui->addSlider("HUE THRESHOLD", 0.0, 1.0, &hueThreshold);
    gui->addSlider("SATURATION THRESHOLD", 0.0, 1.0, &satThreshold);
    gui->addSlider("BRIGTHNESS THRESHOLD", 0.0, 1.0, &brightThreshold);
    gui->addSlider("HUE WEIGHT", 0.0, 1.0, &hueWeight);
    gui->addSlider("SATURATION WEIGHT", 0.0, 1.0, &satWeight);
    gui->addSlider("BRIGTHNESS WEIGHT", 0.0, 1.0, &brightWeight);
    gui->addSpacer();
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);

    gui->loadSettings("GUI/guiSettings.xml");
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{

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
    
    if( x<player.getWidth() && y< player.getHeight() ){
        samplePoint.set(mouseX, mouseY);
        checkColorDistance();
        cout<<"color value at point: "<<player.getPixelsRef().getColor(samplePoint.x, samplePoint.y)<<endl;
    }
    
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
            ofVec3f current = ofVec3f(currentColour.getHue() ,currentColour.getSaturation() ,currentColour.getBrightness());
            
/*
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
*/
            
            ofVec3f weights = ofVec3f(hueWeight,satWeight,brightWeight);
//            sample.distance(current)

            float weightedD = weightedDistance(sample, current, weights);
            
            
            if(  weightedD> thresholdLower && weightedD<thresholdUpper){
                if(ofGetKeyPressed('1')){
                    cout<<"distance : "<<weightedDistance(sample, current, weights)<<endl;
                }
                float alpha = ofxTween::map(weightedD, thresholdLower, thresholdUpper, 0, 255, false,easing,ofxTween::easeOut);
//                alpha = map(weightedD,thresholdUpper,)
                ofFloatColor col;
                col.setHsb(currentColour.getHue(),currentColour.getSaturation(),currentColour.getBrightness() );
                col.set(alpha);
                img.setColor(i, j, col);

            }
            else{
                img.setColor(i, j, ofFloatColor(0,0,0) );
            }
            
        }
    }
    img.update();
}

float testApp::weightedDistance(ofVec3f pnt1 ,ofVec3f pnt2,ofVec3f weights){
	float vx = pnt1.x - pnt2.x;
	float vy = pnt1.y - pnt2.y;
	float vz = pnt1.x - pnt2.x;
    
	return (float)sqrt(weights.x*(vx*vx) +
                       weights.y*(vy*vy) +
                       weights.z*(vz*vz) ) ;
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