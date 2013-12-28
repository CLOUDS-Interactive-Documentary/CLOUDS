#pragma once

#include "ofMain.h"
#include "ofxKinectCommonBridge.h"
#include "ofxOsc.h"

class testApp : public ofBaseApp
{
	public:
		void setup();
		void update();
		void draw();

        void debug(int x, int y, int width, int height);	
        void graph(int x, int y, int z, int width, int height, int depth);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        ofxOscSender oscSender;
        int oscSendRate;
        
        ofxKinectCommonBridge kinect;

        int skeletonIdx;
        int numFramesNotFound;
        int maxFramesNotFound;

		ofVec3f head, leftHand, rightHand;
        deque<ofVec3f> qHeads;
        deque<ofVec3f> qLeftHands;
        deque<ofVec3f> qRightHands;
		
        ofEasyCam cam;
};
