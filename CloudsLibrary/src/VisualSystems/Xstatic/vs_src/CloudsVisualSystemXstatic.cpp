//
//  CloudsVisualSystemXstatic.cpp
//

#include "CloudsVisualSystemXstatic.h"
#include "CloudsRGBDVideoPlayer.h"
#include "ofxBillboard.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif


static const int kStrideData = 8;

int CloudsVisualSystemXstatic::nParticles = 200;

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemXstatic::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    customGui->setName("Initial Conditions");
    customGui->addButton("Projectile", &PROJECTILEMODE);
    customGui->addButton("Falling", &FALLINGMODE);
    customGui->addButton("Rising", &RISINGMODE);
    customGui->addButton("Frozen", &FROZENMODE);
    customGui->addSpacer();
    customGui->addButton("REGENERATE", &shouldRegenerate);
    customGui->setName("General");
    customGui->addSpacer();
    customGui->addButton("Wraparound", &WRAPAROUNDMODE);
    customGui->addButton("Reflect off Walls", &BOUNCEMODE);
    customGui->addIntSlider("Number of Particles", 0, 2000, &nParticles);
    customGui->addSpacer();
    customGui->setName("Physics");
    customGui->addSlider("Mass", 0, 10, &mass);
    
    customGui->addButton("RESET FORCES TO 0", &shouldReset);
    customGui->addSlider("Gravity y", -6, 6, &gravityY); //up down
    customGui->addSlider("Gravity x", -6, 6, &gravityX); //don't normally need this;
    customGui->addSlider("Gravity z", -6, 6, &gravityZ);
    customGui->addSlider("Topspeed", 0, 10, &topspeed);
    
    customGui->addSlider("Wind strength x", 0, 10.0, &windX);
    customGui->addSlider("Wind strength z", 0, 10.0, &windZ);
    customGui->addIntSlider("Max Brightness", 0, 255, &maxBrightness);
    customGui->addIntSlider("Min Brightness", 0, 255, &minBrightness);
    
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemXstatic::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemXstatic::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
    
    if(e.widget->getName() == "REGENERATE" && ((ofxUIButton*)e.widget)->getValue() ){
		regenerate();
	}
    
    if(e.widget->getName() == "Projectile" && ((ofxUIButton*)e.widget)->getValue() ){
		FALLINGMODE = false;
        FROZENMODE = false;
        RISINGMODE = false;
        regenerate();
	}
    
    if(e.widget->getName() == "Falling" && ((ofxUIButton*)e.widget)->getValue() ){
		FROZENMODE = false;
        PROJECTILEMODE = false;
        RISINGMODE = false;
        regenerate();
	}
    if(e.widget->getName() == "Rising" && ((ofxUIButton*)e.widget)->getValue() ){
		FROZENMODE = false;
        PROJECTILEMODE = false;
        FALLINGMODE = false;
        regenerate();
	}
    
    if(e.widget->getName() == "Frozen" && ((ofxUIButton*)e.widget)->getValue() ){
		FALLINGMODE = false;
        PROJECTILEMODE = false;
        RISINGMODE = false;
        regenerate();
	}
    
   
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemXstatic::selfSetupSystemGui(){
	
}

void CloudsVisualSystemXstatic::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemXstatic::selfSetupRenderGui(){

}

void CloudsVisualSystemXstatic::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemXstatic::regenerate(){
    
    data = new GLfloat[nParticles * kStrideData];
    
    particles.clear();
    
    gravityX = 0.0;
    gravityY = 0.0;
    gravityZ = 0.0;
    windX = 0.0;
    windZ = 0.0;
    topspeed = 2.0;
    
    for(int i = 0; i<nParticles; i++){
        
        
        if (PROJECTILEMODE == 1){
        particles.push_back( Particle(mass, ofRandom(0,2) * 4 - 2, ofRandom(0,2) * 4 - 2, ofRandom(0,2) * 4 - 2, minBrightness, maxBrightness)); // ricocheting particles
            WRAPAROUNDMODE = false;
            BOUNCEMODE = true;
        }
        
        if (FALLINGMODE == 1){
         particles.push_back(Particle(mass, 0, ofRandom(-.5,-.3), 0, minBrightness, maxBrightness)); // falling particles
            WRAPAROUNDMODE = true;
            BOUNCEMODE = false;
         }
        
        if (RISINGMODE == 1){
         particles.push_back( Particle(mass, 0, ofRandom(.4,.6), 0, minBrightness, maxBrightness)); // rising particles
            gravityY = -0.0;
            WRAPAROUNDMODE = true;
            BOUNCEMODE = false;
        }
        if (FROZENMODE == 1){
        particles.push_back( Particle(mass, 0, 0, 0, minBrightness, maxBrightness)); // standstill
        }
    }
    
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemXstatic::selfSetup(){
	
    gravityX = 0.0;
    gravityY = 0.0;
    gravityZ = 0.0;
    windX = 0.0;
    windZ = 0.0;
    topspeed = 2.0;
    
    PROJECTILEMODE = true;
    FALLINGMODE = false;
    RISINGMODE = false;
    FROZENMODE = false;
    BOUNCEMODE = true; 
    WRAPAROUNDMODE = false;
    //terminal velocity
    //speed = .005; // noise speed
    
    for(int i = 0; i<nParticles; i++){

       // particles.push_back(Particle(mass, 0, 0, 0, minBrightness, maxBrightness)); // falling particles
        
        if (PROJECTILEMODE == 1){
            particles.push_back( Particle(mass, ofRandom(0,2) * 4 - 2, ofRandom(0,2) * 4 - 2, ofRandom(0,2) * 4 - 2, minBrightness, maxBrightness)); // ricocheting particles
            WRAPAROUNDMODE = false;
            BOUNCEMODE = true;
        }
        
        if (FALLINGMODE == 1){
            gravityY = -0.1;
            particles.push_back(Particle(mass, 0, 0, 0, minBrightness, maxBrightness)); // falling particles
            WRAPAROUNDMODE = true;
            BOUNCEMODE = false;
        }
        
        if (RISINGMODE == 1){
            particles.push_back( Particle(mass, 0, ofRandom(.5,.6), 0, minBrightness, maxBrightness)); // rising particles
            gravityY = 0.1;
            WRAPAROUNDMODE = true;
            BOUNCEMODE = false;
        }
        if (FROZENMODE == 1){
            particles.push_back( Particle(mass, 0, 0, 0, minBrightness, maxBrightness)); // standstill
            gravityY = 0.0;
            gravityX = 0.0;
            gravityZ = 0.0;
        }

    }
    
    data = new GLfloat[nParticles * kStrideData];
    ofDisableArbTex();
    tex.loadImage(getVisualSystemDataPath() + "spark.png");
    shader.load(getVisualSystemDataPath() + "shaders/particles");
    ofEnableArbTex();
    
    
  	
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemXstatic::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemXstatic::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemXstatic::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemXstatic::selfUpdate()
{
    
    if(shouldReset==1){
    
        gravityX = 0.0;
        gravityY = 0.0;
        gravityZ = 0.0;
        windX = 0.0;
        windZ = 0.0;
    }
    
    for (int i = 0; i < nParticles; i++) {
        
        // CALCULATE WIND
        ofVec3f wind;
        float fakeWindX = ofSignedNoise(particles[i].location.x * .005, particles[i].location.z * .005, ofGetElapsedTimef() * 0.6); //speed = .005
        
        wind.set(fakeWindX * 0.0004 + ofSignedNoise(particles[i].uniqueVal, particles[i].location.y * 0.006) * (windX/10000.0),
                0,
                 ofSignedNoise(particles[i].uniqueVal, particles[i].location.x * 0.006) * (windZ/10000.0));
        
        cout << "fakeWindX = " << fakeWindX << endl;
        
        particles[i].applyForce(wind);
        
        // CALCULATE GRAVITY
       
         ofVec3f gravity;
         gravity.set(gravityX/10.0, gravityY/10.0, gravityZ/10.0);
        
        particles[i].applyForce(gravity);
         

        particles[i].update(topspeed);
        if(BOUNCEMODE==true){
            particles[i].checkEdges();
        }
        if(WRAPAROUNDMODE==true){
            particles[i].verticalWraparound();
        }
        data[i * kStrideData + 0] = particles[i].location.x;
        data[i * kStrideData + 1] = particles[i].location.y;
        data[i * kStrideData + 2] = particles[i].location.z;
        data[i * kStrideData + 3] = *particles[i].mass;
        data[i * kStrideData + 4] = particles[i].brightness / 255.0f;
        data[i * kStrideData + 5] = particles[i].brightness / 255.0f;
        data[i * kStrideData + 6] = particles[i].brightness / 255.0f;
        data[i * kStrideData + 7] = 1.0f;
    }
    
    vbo.setVertexData(&data[0], 4, nParticles, GL_STREAM_DRAW, kStrideData * sizeof(GLfloat));
    vbo.setColorData(&data[4], nParticles, GL_STREAM_DRAW, kStrideData * sizeof(GLfloat));
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemXstatic::selfDraw()
{
    glDisable(GL_DEPTH_TEST);
 
    ofSetColor(255);
    


    for(int i = 0; i < nParticles; i++){
        
    particles[i].display();
      
    shader.begin();
    ofEnablePointSprites();
    tex.getTextureReference().bind();
    {
        vbo.draw(GL_POINTS, 0, nParticles);

    }
    tex.getTextureReference().unbind();
    ofDisablePointSprites();
    shader.end();
    
    ofNoFill();
    ofSetColor(255);
	ofBox(0, 0, 0, 300);
    }   
}

// draw any debug stuff here
void CloudsVisualSystemXstatic::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemXstatic::selfDrawBackground(){
	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemXstatic::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemXstatic::selfExit()
{
    delete [] data;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemXstatic::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemXstatic::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemXstatic::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemXstatic::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemXstatic::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemXstatic::selfMouseReleased(ofMouseEventArgs& data){
	
}