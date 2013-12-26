#include "testApp.h"

static int      kMaxHistoryPts = 400;
static float    kLerpRatio     = 0.5f;
static string   kOscSendHost   = "localhost";
static int      kOscSendPort   = 3030;
    
//--------------------------------------------------------------
void testApp::setup()
{
    ofBackground(0);
    ofSetVerticalSync(true);
    
    kinect.initSensor(0);
    
	kinect.initColorStream(640, 480);
	kinect.initDepthStream(320, 240, true);
	kinect.initSkeletonStream(true);
    
	kinect.start();

	skeletonIdx = -1;
    maxFramesNotFound = 15;
    numFramesNotFound = maxFramesNotFound;

    oscSender.setup(kOscSendHost, kOscSendPort);
    oscSendRate = 1;  // 60 samples/sec @ 60fps
}

//--------------------------------------------------------------
void testApp::update()
{
    kinect.update();
    
	if (kinect.isNewSkeleton()) {
        numFramesNotFound = 0;

        if (skeletonIdx != -1 && kinect.getSkeletons().at(skeletonIdx).find(NUI_SKELETON_POSITION_HEAD) == kinect.getSkeletons().at(skeletonIdx).end()) {
            // We lost our tracked skeleton.
            skeletonIdx = -1;
        }

        if (skeletonIdx == -1) {
            // Look for a new skeleton to track.
            int nearestSkeletonIdx = -1;
            float nearestSkeletonZ = FLT_MAX;
		    for (int i = 0; i < kinect.getSkeletons().size(); i++) {
                // If a skeleton is valid...
                if (kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD) != kinect.getSkeletons().at(i).end()) {
                    // and the nearest...
                    float currSkeletonZ = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second.getStartPosition().z;
				    if (nearestSkeletonIdx == -1 || nearestSkeletonZ > currSkeletonZ) {
                        // track it!
                        nearestSkeletonIdx = i;
                    }
                }
            }

            skeletonIdx = nearestSkeletonIdx;
        }
    }
    else {
        ++numFramesNotFound;
        if (numFramesNotFound == maxFramesNotFound) {
            // Reset tracked skeleton.
            skeletonIdx = -1;
        }
    }

    // Gather new data points.
    if (skeletonIdx != -1 && numFramesNotFound == 0) {
        SkeletonBone headBone = kinect.getSkeletons().at(skeletonIdx).find(NUI_SKELETON_POSITION_HEAD)->second;
		SkeletonBone leftHandBone = kinect.getSkeletons().at(skeletonIdx).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
		SkeletonBone rightHandBone = kinect.getSkeletons().at(skeletonIdx).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
		
        head = head.getInterpolated(headBone.getStartPosition(), kLerpRatio);
        leftHand = leftHand.getInterpolated(leftHandBone.getStartPosition(), kLerpRatio);
        rightHand = rightHand.getInterpolated(rightHandBone.getStartPosition(), kLerpRatio);
    }
    else {
        // No data this frame.
        head = head.getInterpolated(ofVec3f::zero(), kLerpRatio);
        leftHand = leftHand.getInterpolated(ofVec3f::zero(), kLerpRatio);
        rightHand = rightHand.getInterpolated(ofVec3f::zero(), kLerpRatio);
    }
    
    qHeads.push_back(head);
    while (qHeads.size() > kMaxHistoryPts)
        qHeads.pop_front();
    qLeftHands.push_back(leftHand);
    while (qLeftHands.size() > kMaxHistoryPts)
        qLeftHands.pop_front();
    qRightHands.push_back(rightHand);
    while (qRightHands.size() > kMaxHistoryPts)
        qRightHands.pop_front();

    if (ofGetFrameNum() % oscSendRate == 0) {
        // Send data to OSC receiver.
        ofxOscMessage m;
        m.setAddress("/clouds/kinect");
        m.addIntArg(skeletonIdx);
        if (skeletonIdx != -1) {
            m.addFloatArg(head.x);
            m.addFloatArg(head.y);
            m.addFloatArg(head.z);

            m.addFloatArg(leftHand.x);
            m.addFloatArg(leftHand.y);
            m.addFloatArg(leftHand.z);

            m.addFloatArg(rightHand.x);
            m.addFloatArg(rightHand.y);
            m.addFloatArg(rightHand.z);
        }
        oscSender.sendMessage(m);
    }
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofSetColor(255);
    
    debug(0, 0, 320, 240);

    cam.begin();
    {
        graph(0, 0, 0, 200, 200, 200);
        
        // Draw axes.
        static float axisLength = 20;
        ofSetColor(ofColor::red);
        ofLine(0, 0, 0, axisLength, 0, 0);
        ofSetColor(ofColor::green);
        ofLine(0, 0, 0, 0, axisLength, 0);
        ofSetColor(ofColor::blue);
        ofLine(0, 0, 0, 0, 0, axisLength);
    }
    cam.end();
}

//--------------------------------------------------------------
void testApp::debug(int x, int y, int width, int height)
{
    ofDisableAlphaBlending(); // Kinect alpha channel is default 0;
    kinect.draw(x, y, width, height);
    ofEnableAlphaBlending();

    if (skeletonIdx != -1) {
        ofPushMatrix();
        {
            ofTranslate(x, y);
            ofScale(width / 640.0f, height / 480.0f);
        
            kinect.drawSkeleton(skeletonIdx);
        }
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void testApp::graph(int x, int y, int z, int width, int height, int depth)
{
	ofPushMatrix();
    {
        ofTranslate(x, y, z);
        ofScale(width, height, depth);
        
        ofPolyline pLine;
        
        // Draw a base plane.
        //ofSetColor(128);
        //ofLine(0, 1, kMaxHistoryPts, 1);
        
        // Draw the head line.
        pLine.clear();
        for (int i = 0; i < qHeads.size(); i++) {
            pLine.addVertex(qHeads[i]);
        }
        for (int i = qHeads.size(); i < kMaxHistoryPts; i++) {
            pLine.addVertex(ofVec3f::zero());
        }
        ofSetColor(255, 0, 0, 192);
        pLine.draw();
        
        // Draw the left hand line.
        pLine.clear();
        for (int i = 0; i < qLeftHands.size(); i++) {
            pLine.addVertex(qLeftHands[i]);
        }
        for (int i = qLeftHands.size(); i < kMaxHistoryPts; i++) {
            pLine.addVertex(ofVec3f::zero());
        }
        ofSetColor(0, 255, 0, 192);
        pLine.draw();
        
        // Draw the right hand line.
        pLine.clear();
        for (int i = 0; i < qRightHands.size(); i++) {
            pLine.addVertex(qRightHands[i]);
        }
        for (int i = qRightHands.size(); i < kMaxHistoryPts; i++) {
            pLine.addVertex(ofVec3f::zero());
        }
        ofSetColor(0, 0, 255, 192);
        pLine.draw();
    }
	ofPopMatrix();
    
    ofSetColor(255);
    //ofRect(x, y, width, height);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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