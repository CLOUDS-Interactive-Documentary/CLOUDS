//
//  Circle.cpp
//  CirclePacking
//
//  Created by Jonathan Minard on 12/9/13.
//
//

#include "Circle.h"

ofxFTGLSimpleLayout Circle::Font;

Circle::Circle()
{
	growth = hue = hue2= sat = brightness = 0;
    
    x = y = r = alpha = 0;
       
}

Circle::Circle(float _x, float _y, float _r, string _label, ofFloatColor _p, ofFloatColor _s, float _a)
{
    x = _x; //this->x
    y = _y;
    r = _r;
    label = _label;
	
	//cout << "creating circle at " << ofVec2f(x,y) <<endl;

    
    growth = ofRandom(-.05, .05);
    hue = ofRandom(_p.r, _s.r);
    sat = ofRandom(_p.g, _s.g);
    brightness = ofRandom(_p.b, _s.b);
    alpha = _a;
}

float Circle::distance(float _x1, float _y1, float _x2, float _y2)
{
    return sqrt((_x1-_x2)*(_x1-_x2)+(_y1-_y2)*(_y1-_y2));
}

float Circle::getOffset(float _x, float _y)
{
    return ofDist(this->x, this->y, _x, _y); // ofDist
}

bool Circle::contains(float _x, float _y)
{
    return ofDist(this->x, this->y, _x, _y) <= this->r;
}

bool Circle::intersect(Circle _circle)
{
    float d = ofDist(this->x, this->y, _circle.x, _circle.y);
    return d <= (this->r + _circle.r);
}

void Circle::draw()
{
    r -= .01;
   //cout << "drawing at " << ofVec2f(x,y) <<endl;
    ofPushStyle();
    color1.setHsb(hue, sat, brightness);
    color1.a = alpha;
    ofSetColor(color1);
    ofSetCircleResolution(100);
    ofCircle(x, y, r);
    ofPopStyle();
    
}
void Circle::checkFontSize(string s, float x, float y){
    ofRectangle rect = Font.getStringBoundingBox(s, x, y);
    if (rect.width > r) {
        float n = rect.width/r;
        
        int size = Font.getSize();
        if(size > 10){
            Font.setSize(20 - (int)n);
        }
        else{
            Font.setSize(20);
        }
    }
}
void Circle::drawCompanies()
{

	ofPushStyle();
    r += (growth + (ofRandom(-.08,.08)));

    std::ostringstream ostr; //output string stream
    int num = int(r) * 2;
    ostr << num;
    std::string number = ostr.str();
    
    ofPushStyle();
    color1.setHsb(hue, sat, brightness);
    color1.a = alpha;

    ofSetColor(color1);
    ofSetCircleResolution(100);
    ofCircle(x, y, r);
    ofPopStyle();
    
    if(r > 25){
        ofPushStyle();
        ofSetColor(255);
        checkFontSize(label, x, y);
        ofRectangle rect = Font.getStringBoundingBox(label, x, y);
        float _x =  x- rect.width/2;
        Font.drawString(label, _x, y);

        ofSetColor(100, 230,230);
        checkFontSize("$"+number+" B", x, y+20);
        rect = Font.getStringBoundingBox("$"+number+" B", x, y);
        _x = x-  rect.width/2;
        Font.drawString("$"+number+" B", _x, y+20);
        ofPopStyle();
	}
	ofPopStyle();
}
    
void Circle::drawHashtags(){
        
    ofPushStyle();
    color1.setHsb(hue, sat, brightness);
    color1.a = alpha;
    ofSetColor(color1);
    //ofSetCircleResolution(100);
    //ofFill();
    //ofCircle(x, y, r);
        
        
    if (r<100){
        //ofSetColor(255);
        // checkFontSize(label, x, y);
        // ofRectangle rect = Font.getStringBoundingBox(label, x, y);
        //  float _x =  x- rect.width/2;
        Font.setSize(r/4);
        Font.drawString(label, x, y);
       
    }
    if (r>100){
        Font.setSize(25);
        Font.drawString(label, x, y);
    }
	ofPopStyle();
}