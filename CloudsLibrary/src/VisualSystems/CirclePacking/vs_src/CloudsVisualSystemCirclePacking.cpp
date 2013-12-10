//
//  CloudsVisualSystemCirclePacking.cpp
//

#include "CloudsVisualSystemCirclePacking.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//float CloudsVisualSystemCirclePacking::NCircles = 50;


class Circle {
public:
    
    Circle();
    void init(float _x, float _y, float _r, string _label);
    void draw();
    
    float distance(float _x1, float _y1, float _x2, float _y2);
    float getOffset(float _x, float _y);
    Boolean contains(float _x, float _y);
    Boolean intersect(Circle _circle);
    
    
    float x;
    float y;
    float r;
    string label;
    
};
Circle::Circle(){
    
}

void Circle::init(float _x, float _y, float _r, string _label)
{
    this->x = _x; //this->x
    this->y = _y;
    this->r = _r;
    this->label = _label;
}

float Circle::distance(float _x1, float _y1, float _x2, float _y2)
{
    return sqrt((_x1-_x2)*(_x1-_x2)+(_y1-_y2)*(_y1-_y2));
}

float Circle::getOffset(float _x, float _y)
{
    return ofDist(this->x, this->y, _x, _y); // ofDist
}

Boolean Circle::contains(float _x, float _y)
{
    return ofDist(this->x, this->y, _x, _y) <= this->r;
}

Boolean Circle::intersect(Circle _circle)
{
    float d = ofDist(this->x, this->y, _circle.x, _circle.y);
    return d <= (this->r + _circle.r);
}

void Circle::draw()
{
    ofCircle(x, y, r*2, r*2);
}


//circle packing

class CirclePacker {
    
public:
    CirclePacker();
    void init(float _width, float _height);
    float fast_distance(float _x1, float _y1, float _x2, float _y2);
    void pack();
    void update();
    void draw();
    
    float width, height, padding, xcenter, ycenter;
    vector <Circle> circles; // vector
    
protected:
    
    float damping, iterations;
    
};

CirclePacker::CirclePacker()
{
    
}

void CirclePacker::init(float _width, float _height)
{
    width = _width;
    height = _height;
    xcenter = width/2;
    ycenter = height/2;
    padding = 20;
    damping = 0.01;
    iterations = 1;
}

float CirclePacker::fast_distance(float _x1, float _y1, float _x2, float _y2)
{
    return (_x1 - _x2) * (_x1 - _x2) + (_y1 - _y2) * (_y1 - _y2);
}

void CirclePacker::pack()
{
    
    for (int i = 0; i < circles.size(); i++)
    {
        Circle& c1 = circles[i];
        
        for (int j = i+1; j < circles.size(); j++)
        {
            Circle& c2 = circles[j];   // Circle& c2 = circles[j]
            
            float d = fast_distance(c1.x, c1.y, c2.x, c2.y);
            float r = c1.r + c2.r + padding;
            
            if (d < (r*r))
            {
                float dx = c2.x - c1.x;
                float dy = c2.y - c1.y;
                float droot = sqrt(d);
                
                // proviamo a dare un peso rispetto al centro
                float cd1 = fast_distance(c1.x, c1.y, xcenter, ycenter);
                float cd2 = fast_distance(c1.x, c1.y, xcenter, ycenter);
                
                float total = dx + dy;
                
                float vx = (dx/droot) * (r-droot);
                float vy = (dy/droot) * (r-droot);
                
                c1.x -= vx * cd1/(cd1+cd2);
                c1.y -= vy * cd1/(cd1+cd2);
                c2.x += vx * cd2/(cd1+cd2);
                c2.y += vy * cd2/(cd1+cd2);
            }
        }
    }
    
    // contraction...
    //
    /*
     for (int i = 0; i < circles.size(); i++)
     {
     Circle c = (Circle) circles.get(i);
     float vx = (c.x - xcenter) * damping;
     float vy = (c.y - ycenter) * damping;
     c.x -= vx;
     c.y -= vy;
     }
     //
     */
}

void CirclePacker::update() {
    for (int w=0; w<iterations; w++)
    {
        this->pack();
    }
}
/**
 * Draw all the circles
 */

void CirclePacker::draw()
{
    for (int i = 0; i < circles.size(); i++)
    {
        Circle& c = circles[i];
        if (c.r < 1)
        {
            cout << "I would erase this one if I knew how" << endl;   // circles.erase(circles.begin() + i);
        }
        else
        {
            c.draw();
        }
    }
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemCirclePacking::selfSetupGui(){

	/*
    customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemCirclePacking::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
     
     */
    
    pack.init(400,400);
    
    
    for(int i = 0; i<50; i++){
      
        newCircle.init(ofRandom(300), ofRandom(300), ofRandom(100), "blank");
        pack.circles.push_back(newCircle);
        
        
    }
    
    
}

void CloudsVisualSystemCirclePacking::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemCirclePacking::selfSetupSystemGui(){
	
}

void CloudsVisualSystemCirclePacking::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemCirclePacking::selfSetupRenderGui(){

}

void CloudsVisualSystemCirclePacking::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemCirclePacking::selfSetup(){
	
	
	
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemCirclePacking::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemCirclePacking::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemCirclePacking::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemCirclePacking::selfUpdate(){
    
    pack.update();

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemCirclePacking::selfDraw(){
	
    ofSetColor(255,255,255);
    pack.draw(); 

	
}

// draw any debug stuff here
void CloudsVisualSystemCirclePacking::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemCirclePacking::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemCirclePacking::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemCirclePacking::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemCirclePacking::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemCirclePacking::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemCirclePacking::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCirclePacking::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCirclePacking::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCirclePacking::selfMouseReleased(ofMouseEventArgs& data){
	
}