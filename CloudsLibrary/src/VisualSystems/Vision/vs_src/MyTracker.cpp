//
//  Car.cpp
//  ofApp
//
//  Created by Omer Shapira on 31/03/13.
//
//

#include "MyTracker.h"
using namespace cv;
using namespace ofxCv;

const float dyingTime = 1;

void MyTracker::setup(const cv::Rect& track) {
    
	color.set(129, 192, 87);
    
	cur = toOf(track).getCenter();
	smooth = cur;
    boundingBox = cv::Rect(track.x,track.y,track.width,track.height);

}

void MyTracker::update(const cv::Rect& track) {
    boundingBox.x = track.x;
    boundingBox.y = track.y;
    boundingBox.width = track.width;
    boundingBox.height = track.height;
	cur = toOf(track).getCenter();
	smooth.interpolate(cur, .5);
	all.addVertex(smooth);
}

void MyTracker::kill() {
	float curTime = ofGetElapsedTimef();
	if(startedDying == 0) {
		startedDying = curTime;
	} else if(curTime - startedDying > dyingTime) {
		dead = true;
	}
}

void MyTracker::draw(float lineWidth, bool lifeTime, float lifetimeColorRange,bool drawBoxes, bool drawLine, bool drawNumbers, ofFloatColor boxColor) {
	ofPushStyle();
	
	float size = 16;
	ofSetColor(255);
	if(startedDying) {
		ofSetColor(ofColor::red);
		size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
	}
	ofNoFill();
    if(lifeTime){
        float scaledHue = ofMap(getLifeTime() ,0, lifetimeColorRange, ofFloatColor::blue.getHue(), ofFloatColor::red.getHue());
        ofFloatColor magnitudeColor = ofFloatColor::fromHsb(scaledHue, 128, 128 ) ;
        ofSetColor(magnitudeColor);
    }
    else{
        ofSetColor(boxColor);
    }
    
    if(drawBoxes){
        ofSetLineWidth(lineWidth);
        ofRect(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height);
    }
    
    if(drawLine){
        all.draw();
    }
    if(drawNumbers){
        ofSetColor(255);
        ofDrawBitmapString(ofToString(label), cur);

    }
	
    ofPopStyle();    
}


ofPolyline MyTracker::getLifeSpan(){
    return all;
}
int MyTracker::getLifeTime(){
    return all.getVertices().size();
}



ofVec2f MyTracker::getTangentAtPoint(int history){
    //FIXME this function is written terribly and inneficiently
    
    //TODO find another way to make this safe
    history = (history < 2)? 2: history;
    if (all.getVertices().size() < history) {
        return ofVec2f(0,0);
    }
    
    std::vector<ofPoint> vec = all.getVertices();
    //TODO write this in a cleaner way, try using toCv();
    //We're taking only the last few points
    std::vector<cv::Vec2f> points;
    //There's no efficient way that isn't O(n)
    for (int i = vec.size() - history ; i<vec.size() ; i++){
        points.push_back(cv::Vec2f(vec[i][0],vec[i][1]));
    }
    
    cv::Vec4f line;
    cv::fitLine(points, line, CV_DIST_L2, 0, .01, .01);
    ofVec2f tangent(line[2],line[3]);
    return tangent;
}
