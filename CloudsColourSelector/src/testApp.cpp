#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	
    if(! player.loadMovie("Zach_Fucking_boring.mov")){
        cout<<"Movie not loaded"<<endl;
    }
    img.allocate(player.getWidth(), player.getHeight(), OF_IMAGE_COLOR_ALPHA);
    fboBlurOnePass.allocate(img.getWidth(), img.getHeight());
    fboBlurTwoPass.allocate(img.getWidth(), img.getHeight());
    player.play();
    threshold = 0.5;
    
    shaderBlurX.load("simpleBlurHorizontal");
    shaderBlurY.load("simpleBlurVertical");
    
    ofEnableAlphaBlending();
    blur = false;
    
    gui = new ofxUISuperCanvas("COLOUR SELECTOR", OFX_UI_FONT_MEDIUM);
    gui->addSpacer();
    gui->addSpacer();
//    gui->addSlider("THRESHOLD", 0.0, 1.0, &threshold);
    gui->addSlider("THRESHOLD_LOWER", 0.0, 1.0, &thresholdLower);
    gui->addSlider("THRESHOLD_UPPER", 0.0, 1.0, &thresholdUpper);
    gui->addSlider("HUE WEIGHT", 0.0, 1.0, &hueWeight);
    gui->addSlider("SATURATION WEIGHT", 0.0, 1.0, &satWeight);
    gui->addSlider("BRIGTHNESS WEIGHT", 0.0, 1.0, &brightWeight);
    gui->addSpacer();
    gui->addSlider("BLUR AMOUNT", 0.0, 10.0, &blurAmount);
    gui->addToggle("BLUR", &blur);
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
    
    if(blur){
//        float blur = ofMap(mouseX, 0, ofGetWidth(), 0, 10, true);
        
        //----------------------------------------------------------
        fboBlurOnePass.begin();
        
        shaderBlurX.begin();
        shaderBlurX.setUniformTexture("src_tex_unit0", img, 0 );
        shaderBlurX.setUniform1f("blurAmnt", blurAmount);
        
        img.draw(0,0);
        
        shaderBlurX.end();
        
        fboBlurOnePass.end();
        
        //----------------------------------------------------------
        fboBlurTwoPass.begin();
        
        shaderBlurY.begin();
        shaderBlurY
        .setUniformTexture("src_tex_unit0", img, 0 );
        shaderBlurY.setUniform1f("blurAmnt", blurAmount);
        
        fboBlurOnePass.draw(0, 0);
        
        shaderBlurY.end();
        
        fboBlurTwoPass.end();
        
        //----------------------------------------------------------
        ofSetColor(ofColor::white);
        fboBlurTwoPass.draw(player.width/2,0, player.getWidth()/2, player.getHeight()/2);
    }
    else{
        img.draw(player.width/2,0, player.getWidth()/2, player.getHeight()/2);
        
    }
 
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
            float weightedD = weightedDistance(sample, current, weights);
            
            
            if(  weightedD> thresholdLower && weightedD<thresholdUpper){

                float alpha = ofxTween::map(weightedD, thresholdLower, thresholdUpper, 0, 255, true,easing,ofxTween::easeOut);
                ofFloatColor col;
                col.setHsb(currentColour.getHue(),currentColour.getSaturation(),currentColour.getBrightness() );
                col.a =alpha;
//                col.set(alpha);
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