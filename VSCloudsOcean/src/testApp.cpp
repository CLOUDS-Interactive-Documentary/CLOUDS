#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    glEnable(GL_DEPTH_TEST);

	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableSmoothing();
    ofEnableAlphaBlending();
	ofBackground(0);
//	fbo.allocate(6144, 768, GL_RGB, 4);
	
	ofSetWindowPosition(0, 0);
//	ofSetWindowShape(fbo.getWidth(), fbo.getHeight());

//	cam.setScale(1,-1,1);
	cam.speed = 1.5;
	cam.setFov(30);

	cam.setup();
	cam.useArrowKeys = false;
	cam.usemouse = true;
	cam.autosavePosition = false;
	cam.cameraPositionFile = "camera1.xml";
	cam.loadCameraPosition();

	oceanTileSizeX = 200;
	oceanTileSizeY = 200;

//	ocean = new ofxOcean();
    ocean.size = ofVec3f(oceanTileSizeX, 1.0, oceanTileSizeY);
    ocean.windSpeed = 32;
    ocean.setup();
    //all other ocean params are set per frame



	//renderer = new ofxOceanRenderer();
	
	renderer.shaderLocation = "";
	renderer.setup(&ocean, 9, 9);
	
//	scaleToView = false;
//	editingHandles = false;
//	editingTextureRatios = false;
//	draggingCorner = false;
//	currentScreen = NULL;

//	loadScreens();

//	flashTime = 0;
//	lastSwitchedTime = 0;
//	autoMode = false;
//	hideEverything = false;
	
	ofToggleFullscreen();
}

/*
void testApp::createMoods(){
	contours = new ofxOceanContourGenerator();
	contours->ocean = ocean;
	contours->tileSize = ofVec2f(500, 2000);
	contours->step = 8;
	contours->fft = fft;


	 OceanContourMood* m = new OceanContourMood();
	 m->name = "basic";
	 m->thicknessA = .4;
	 m->thicknessB = 1.;
	 m->velocityA = ofVec2f(1.0, 0);
	 m->velocityB = ofVec2f(.5, 0);
	 m->targetDensity = 300;
	 m->baseColor = ofFloatColor(.6,.6,.6);
	 m->accentColor = ofFloatColor(1.0, .35, 0);
	 moods.push_back( m );

	contours->currentMood = m;

	m = new OceanContourMood();
	m->name = "thicksparse";
	m->thicknessA = 2.0;
	m->thicknessB = 1;
	m->velocityA = ofVec2f(1.0, 0);
	m->velocityB = ofVec2f(.3, 0);
	m->targetDensity = 100;
	m->baseColor = ofFloatColor(0.0, 20/255.0,182/255.0);
	m->accentColor = ofFloatColor(1.0,1.0,1.0);
	moods.push_back( m );


	m = new OceanContourMood();
	m->name = "thinsparse";
	m->thicknessA = .3;
	m->thicknessB = .1;
	m->velocityA = ofVec2f(2.0, 0);
	m->velocityB = ofVec2f(.5, 0);
	m->targetDensity = 150;
	m->baseColor = ofFloatColor(.75, .75, .75);
	m->accentColor = ofFloatColor(.8,.0, 1.0);
	moods.push_back( m );

	m = new OceanContourMood();
	m->name = "thindense";
	m->thicknessA = .2;
	m->thicknessB = .5;
	m->velocityA = ofVec2f(2.0, 0);
	m->velocityB = ofVec2f(.5, 0);
	m->targetDensity = 300;
	m->baseColor = ofFloatColor(1.0, .35, 0);
	m->accentColor = ofFloatColor(1.0,1.0,0.0);
	moods.push_back( m );

	contours->generate();

}
*/

//--------------------------------------------------------------
void testApp::update(){

//	if(autoMode){
//	if( (ofGetElapsedTimef() - lastSwitchedTime) > 3*60){
//		int newMood = ofRandom(1, moods.size());
//		if(newMood < moods.size()){
//			contours->currentMood = moods[newMood];
//		}
//
//		float newCamera = ofRandom(5.0f);
//		if(newCamera < 1){
//			cam.cameraPositionFile = "camera1.xml";
//			cam.loadCameraPosition();
//		}
//		else if(newCamera < 2){
//			cam.cameraPositionFile = "camera2.xml";
//			cam.loadCameraPosition();
//		}
//		else if(newCamera < 3){
//			cam.cameraPositionFile = "camera3.xml";
//			cam.loadCameraPosition();
//		}
//		else if(newCamera < 4){
//			cam.cameraPositionFile = "camera4.xml";
//			cam.loadCameraPosition();
//		}
//		else {
//			cam.cameraPositionFile = "camera5.xml";
//			cam.loadCameraPosition();
//		}
//		lastSwitchedTime = ofGetElapsedTimef();
//	}
//	}

//    ocean.waveScale += (targetHeight - ocean->waveScale)*.01;
//    ocean.choppyScale += (targetChoppy - ocean->choppyScale)*.01;

    ocean.waveSpeed = 15;
    ocean.waveScale = 10;
    ocean.choppyScale = 7;

	ocean.setFrameNum(ofGetFrameNum());
    ocean.update();
	renderer.update();

//	contours->update();

//	handleOSC();
}

//--------------------------------------------------------------
void testApp::draw(){

//	if(hideEverything) return;

//	fbo.begin();

//	int flashColor = ofMap(ofGetElapsedTimef(), flashTime, flashTime+.35, 255, 0, true);
//	ofClear(flashColor, flashColor, flashColor, 255);

//	cam.begin(ofRectangle(0, 0, fbo.getWidth(), fbo.getHeight()));
	cam.begin();	
	ofSetColor(255, 255, 255, 255);
	renderer.drawWireframe();
//	contours->draw();

	cam.end();

	//draw grid if we're editing
//	if(editingHandles){
//		ofPushStyle();
//		ofSetColor(0, 255, 0);
//		for(int i = 0; i < fbo.getWidth(); i += 100){
//			ofLine(i, 0, i, fbo.getHeight());
//		}
//
//		for(int i = 0; i < fbo.getHeight(); i += 100){
//			ofLine(0, i, fbo.getWidth(), i);
//		}
//
//		ofSetColor(255, 255, 0, 50);
//		for(int i = 0; i < fbo.getWidth(); i += 25){
//			ofLine(i, 0, i, fbo.getHeight());
//		}
//
//		for(int i = 0; i < fbo.getHeight(); i += 25){
//			ofLine(0, i, fbo.getWidth(), i);
//		}
//
//		ofPopStyle();
//	}
//
//	fbo.end();
//	if(scaleToView){
//		//calculate how to draw onto main screen
//		float widthRatio = ofGetWidth()/fbo.getWidth();
//		float newHeight = fbo.getHeight()*widthRatio;
//		fbo.getTextureReference().draw(0,0,ofGetWidth(), newHeight);
//	}
//	else{
//		splitAndDraw();
//	}

//	if(editingHandles){
//		ofPushStyle();
//		for(int i = 0; i < screens.size(); i++){
//			ofxPSScreen* s = screens[i];
//
//			//draw handles
//			for(int p = 0; p < 4; p++){
//				if(s == currentScreen && p == currentPointDragIndex){
//					ofFill();
//				}
//				else{
//					ofNoFill();
//				}
//				ofSetColor(255, 0, 0);
//				ofCircle(s->dest[p], 10);
//				ofSetColor(255, 255, 0);
//				ofCircle(s->dest[p], 3);
//			}
//
//			ofNoFill();
//			//draw outline
//			if(s == currentScreen){
//				ofSetColor(255, 0, 0);
//			}
//			else{
//				ofSetColor(255, 255, 0);
//			}
//			ofSetLineWidth(2);
//			ofBeginShape();
//			for(int p = 0; p < 4; p++){
//				ofVertex(s->dest[p].x,s->dest[p].y);
//			}
//			ofEndShape(true);
//		}
//
//		ofSetLineWidth(10);
//		for(int i = 0; i < fencePosts.size(); i++){
//			if(fencepostSelected && selectedFencepostIndex == i){
//				ofSetColor(255, 0, 0);
//			}
//			else{
//				ofSetColor(255, 255, 0, 100);
//			}
//			ofLine(fencePosts[i], 0, fencePosts[i], fbo.getHeight());
//		}
//		ofSetLineWidth(1);
//
//		ofPopStyle();
//	}
//	
//	if(drawFFT){
//		fft->draw( mouseX, mouseY );
//	}


}

//void testApp::splitAndDraw(){
//
//	fbo.getTextureReference().bind();
//
//	for(int i = 0; i < screens.size(); i++){
//		ofPushMatrix();
//
//		ofxPSScreen* screen = screens[i];
//		findHomography(screen->source, screen->dest,screen->correctionMatrix);
//
//		glMultMatrixf(screen->correctionMatrix);
//
//		glBegin(GL_QUADS);
//
//		glTexCoord2f(fencePosts[screen->leftPostIndex], 0);
//		glVertex2f(screen->source[0].x, screen->source[0].y);
//
//		glTexCoord2f(fencePosts[screen->rightPostIndex], 0);
//		glVertex2f(screen->source[1].x, screen->source[1].y);
//
//		glTexCoord2f(fencePosts[screen->rightPostIndex], fbo.getHeight());
//		glVertex2f(screen->source[2].x, screen->source[2].y);
//
//		glTexCoord2f(fencePosts[screen->leftPostIndex], fbo.getHeight());
//		glVertex2f(screen->source[3].x, screen->source[3].y);
//
//		glEnd();
//
//		ofPopMatrix();
//	}
//
//	fbo.getTextureReference().unbind();
//}

//void testApp::saveScreens(){
//
//	ofxXmlSettings savedsettings;
//	savedsettings.addTag("screens");
//	savedsettings.pushTag("screens");
//
//	for(int i = 0; i < 6; i++){
//		ofxPSScreen* screen = screens[i];
//		savedsettings.addTag("screen");
//		savedsettings.pushTag("screen", i);
//		savedsettings.addTag("dest");
//		savedsettings.pushTag("dest");
//		for(int p = 0; p < 4; p++){
//			savedsettings.addValue("x", screen->dest[p].x);
//			savedsettings.addValue("y", screen->dest[p].y);
//		}
//		savedsettings.popTag();//dest
//		savedsettings.popTag();//screen;
//	}
//	savedsettings.popTag();//screens
//
//	savedsettings.addTag("posts");
//	savedsettings.pushTag("posts");
//	for(int i = 0; i < 7; i++){
//		savedsettings.addValue("post", fencePosts[i]);
//	}
//	savedsettings.popTag();//posts;
//
//	savedsettings.saveFile("screens.xml");
//}
//
//void testApp::loadScreens(){
//	ofxXmlSettings savedsettings;
//	if(savedsettings.loadFile("screens.xml")){
//		//parse em out
//		savedsettings.pushTag("screens");
//		for(int i = 0; i < 6; i++){
//			savedsettings.pushTag("screen", i);
//			ofxPSScreen* s = new ofxPSScreen();
//
//			savedsettings.pushTag("dest");
//			for(int p = 0; p < 4; p++){
//				s->dest[p] = ofVec2f(savedsettings.getValue("x", 0.0, p),
//									 savedsettings.getValue("y", 0.0, p));
//			}
//			savedsettings.popTag();//dest
//			savedsettings.popTag();//screen
//
//			screens.push_back( s );
//		}
//		savedsettings.popTag(); //screens
//
//		savedsettings.pushTag("posts");
//		for(int i = 0; i < 7; i++){
//			fencePosts.push_back(savedsettings.getValue("post", .0, i));
//		}
//		savedsettings.popTag();
//	}
//	else {
//		for(int i = 0; i < 7; i++){
//			fencePosts.push_back(i/6.0 * fbo.getWidth());
//		}
//
//		//make new ones
//		for(int i = 0; i < 6; i++){
//			ofxPSScreen* s = new ofxPSScreen();
//			s->leftPostIndex = i;
//			s->rightPostIndex = i+1;
//			s->dest[0] = ofVec2f(fencePosts[s->leftPostIndex], 0);
//			s->dest[1] = ofVec2f(fencePosts[s->rightPostIndex], 0);
//			s->dest[2] = ofVec2f(fencePosts[s->rightPostIndex], fbo.getHeight());
//			s->dest[3] = ofVec2f(fencePosts[s->leftPostIndex], fbo.getHeight());
//			screens.push_back( s );
//		}
//	}
//
//	for(int i = 0; i < 6; i++){
//		ofxPSScreen* s = screens[i];
//		s->source[0] = ofPoint(i/6.0*fbo.getWidth(), 0);
//		s->source[1] = ofPoint((i+1)/6.0*fbo.getWidth(), 0);
//		s->source[2] = ofPoint((i+1)/6.0*fbo.getWidth(), fbo.getHeight());
//		s->source[3] = ofPoint(i/6.0*fbo.getWidth(), fbo.getHeight());
//		s->leftPostIndex = i;
//		s->rightPostIndex = i+1;
//	}
//}


//void testApp::handleOSC(){
//	while(oscreceiver.hasWaitingMessages()){
//
//		ofxOscMessage m;
//		oscreceiver.getNextMessage(&m);
//
//		cout << "RECEIVED OSC " << m.getAddress() << endl;
//
//		if(m.getAddress() == "/camera1"){
//			cam.cameraPositionFile = "camera1.xml";
//			cam.loadCameraPosition();
//		}
//		if(m.getAddress() == "/camera2"){
//			cam.cameraPositionFile = "camera2.xml";
//			cam.loadCameraPosition();
//		}
//		if(m.getAddress() == "/camera3"){
//			cam.cameraPositionFile = "camera3.xml";
//			cam.loadCameraPosition();
//		}
//		if(m.getAddress() == "/camera4"){
//			cam.cameraPositionFile = "camera4.xml";
//			cam.loadCameraPosition();
//		}
//		if(m.getAddress() == "/camera5"){
//			cam.cameraPositionFile = "camera5.xml";
//			cam.loadCameraPosition();
//		}
//
//		if(m.getAddress() == "/mood1"){
//			contours->currentMood = moods[0];
//		}
//		if(m.getAddress() == "/mood2"){
//			contours->currentMood = moods[1];
//		}
//		if(m.getAddress() == "/mood3"){
//			contours->currentMood = moods[2];
//		}
//		if(m.getAddress() == "/mood4"){
//			contours->currentMood = moods[3];
//		}
//
//		if(m.getAddress() == "/waveheight"){
//			targetHeight = m.getArgAsFloat(0);
//		}
//		if(m.getAddress() == "/wavechoppy"){
//			targetChoppy = m.getArgAsFloat(0);
//		}
//
//		if(m.getAddress() == "/wavespeed"){
//			targetSpeed = m.getArgAsFloat(0);
//		}
//
//		if(m.getAddress() == "/fftscale"){
//			fftScale = m.getArgAsFloat(0);
//		}
//
//		if(m.getAddress() == "/hide"){
//			hideEverything = true;
//		}
//
//		if(m.getAddress() == "/reveal"){
//			hideEverything = false;
//		}
//
//		if(m.getAddress() == "/autoon"){
//			lastSwitchedTime = ofGetElapsedTimef();
//			autoMode = true;
//		}
//
//		if(m.getAddress() == "/autooff"){
//			autoMode = false;
//		}
//
//		if(m.getAddress() == "/flash"){
//			flashTime = ofGetElapsedTimef();
//		}
//	}
//}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

//	if(key == 'b'){
//		flashTime = ofGetElapsedTimef();
//	}

//	if(key == 'v'){
//		scaleToView = !scaleToView;
//		if(scaleToView){
//			ofSetFullscreen(true);
//		}
//	}
//	if(key == ' '){
//		editingHandles = !editingHandles;
//		cam.usemouse = !editingHandles;
//	}
//	if(key == 'f'){
//		drawFFT = !drawFFT;
//	}
//	if(fencepostSelected && selectedFencepostIndex >= 0){
//		if(key == OF_KEY_RETURN){
//			selectedFencepostIndex = (selectedFencepostIndex + 1) % 7;
//		}
//		if(key == OF_KEY_LEFT){
//			fencePosts[selectedFencepostIndex] -= .5;
//		}
//		else if(key == OF_KEY_RIGHT){
//			fencePosts[selectedFencepostIndex] += .5;
//		}
//
//		saveScreens();
//	}
//
//	if(currentScreen != NULL && currentPointDragIndex >= 0){
//		if(key == OF_KEY_RETURN){
//			currentPointDragIndex = (currentPointDragIndex + 1) % 4;
//		}
//
//		if(key == OF_KEY_UP){
//			currentScreen->dest[currentPointDragIndex].y -= .5;
//		}
//		else if(key == OF_KEY_DOWN){
//			currentScreen->dest[currentPointDragIndex].y += .5;
//		}
//		else if(key == OF_KEY_LEFT){
//			currentScreen->dest[currentPointDragIndex].x -= .5;
//		}
//		else if(key == OF_KEY_RIGHT){
//			currentScreen->dest[currentPointDragIndex].x += .5;
//		}
//
//		saveScreens();
//	}

	//1
//	if(key == '1'){
//		cam.cameraPositionFile = "camera1.xml";
//		cam.saveCameraPosition();
//	}
//	else if(key == '2'){
//		cam.cameraPositionFile = "camera2.xml";
//		cam.saveCameraPosition();
//	}
//	else if(key == '3'){
//		cam.cameraPositionFile = "camera3.xml";
//		cam.saveCameraPosition();
//	}
//	else if(key == '4'){
//		cam.cameraPositionFile = "camera4.xml";
//		cam.saveCameraPosition();
//	}
//	else if(key == '5'){
//		cam.cameraPositionFile = "camera5.xml";
//		cam.saveCameraPosition();
//	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

//	if(editingHandles){
//		if(currentScreen != NULL && draggingCorner){
//			currentScreen->dest[currentPointDragIndex] = ofVec2f(x,y) + dragOffset;
//		}
//		else if(fencepostSelected){
//			fencePosts[selectedFencepostIndex] = x + fencepostSelectOffset;
//		}
//	}

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
//	if(editingHandles){
//
//		ofHideCursor();
//
//		fencepostSelected = false;
//		currentScreen = NULL;
//		currentPointDragIndex = -1;
//		draggingCorner = false;
//
//		ofVec2f mousePoint(x,y);
//		//attempt to click point
//		for(int i = 0; i < screens.size(); i++){
//			for (int p = 0; p < 4; p++) {
//				if(mousePoint.distance(screens[i]->dest[p]) < 20){
//					currentScreen = screens[i];
//					currentPointDragIndex = p;
//					dragOffset = screens[i]->dest[p] - mousePoint;
//					draggingCorner = true;
//					return;
//				}
//			}
//		}
//
//
//		//check for fencepost
//		for(int i = 0; i < fencePosts.size(); i++){
//			if(fabs(mousePoint.x - fencePosts[i]) < 10){
//				selectedFencepostIndex = i;
//				fencepostSelected = true;
//				fencepostSelectOffset = fencePosts[i] - mousePoint.x;
//				return;
//			}
//		}
//
//		//attempt to click square
//		for(int i = 0; i < screens.size(); i++){
//			if(insidePolygon(screens[i]->dest, 4, mousePoint)){
//				currentScreen = screens[i];
//				return;
//			}
//		}
//	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
//	if(editingHandles){
//		saveScreens();
//		ofShowCursor();
//	}

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

/*
void testApp::gaussian_elimination(float *input, int n){
	// ported to c from pseudocode in
	// http://en.wikipedia.org/wiki/Gaussian_elimination
	float * A = input;
	int i = 0;
	int j = 0;
	int m = n-1;
	while (i < m && j < n)
	{
		// Find pivot in column j, starting in row i:
		int maxi = i;
		for(int k = i+1; k<m; k++)
		{
			if(fabs(A[k*n+j]) > fabs(A[maxi*n+j]))
			{
				maxi = k;
			}
		}
		if (A[maxi*n+j] != 0)
		{
			//swap rows i and maxi, but do not change the value of i
			if(i!=maxi)
				for(int k=0; k<n; k++)
				{
					float aux = A[i*n+k];
					A[i*n+k]=A[maxi*n+k];
					A[maxi*n+k]=aux;
				}
			//Now A[i,j] will contain the old value of A[maxi,j].
			//divide each entry in row i by A[i,j]
			float A_ij=A[i*n+j];
			for(int k=0; k<n; k++)
			{
				A[i*n+k]/=A_ij;
			}
			//Now A[i,j] will have the value 1.
			for(int u = i+1; u< m; u++)
			{
				//subtract A[u,j] * row i from row u
				float A_uj = A[u*n+j];
				for(int k=0; k<n; k++)
				{
					A[u*n+k]-=A_uj*A[i*n+k];
				}
				//Now A[u,j] will be 0, since A[u,j] - A[i,j] * A[u,j] = A[u,j] - 1 * A[u,j] = 0.
			}
			i++;
		}
		j++;
	}

	//back substitution
	for(int i=m-2; i>=0; i--)
	{
		for(int j=i+1; j<n-1; j++)
		{
			A[i*n+m]-=A[i*n+j]*A[j*n+m];
			//A[i*n+j]=0;
		}
	}
}


void testApp::findHomography(ofVec2f src[4], ofVec2f dst[4], float homography[16]) {

	// create the equation system to be solved
	//
	// from: Multiple View Geometry in Computer Vision 2ed
	//       Hartley R. and Zisserman A.
	//
	// x' = xH
	// where H is the homography: a 3 by 3 matrix
	// that transformed to inhomogeneous coordinates for each point
	// gives the following equations for each point:
	//
	// x' * (h31*x + h32*y + h33) = h11*x + h12*y + h13
	// y' * (h31*x + h32*y + h33) = h21*x + h22*y + h23
	//
	// as the homography is scale independent we can let h33 be 1 (indeed any of the terms)
	// so for 4 points we have 8 equations for 8 terms to solve: h11 - h32
	// after ordering the terms it gives the following matrix
	// that can be solved with gaussian elimination:
	float P[8][9]=
	{
		{-src[0].x, -src[0].y, -1,   0,   0,  0, src[0].x*dst[0].x, src[0].y*dst[0].x, -dst[0].x }, // h11
		{  0,   0,  0, -src[0].x, -src[0].y, -1, src[0].x*dst[0].y, src[0].y*dst[0].y, -dst[0].y }, // h12
		{-src[1].x, -src[1].y, -1,   0,   0,  0, src[1].x*dst[1].x, src[1].y*dst[1].x, -dst[1].x }, // h13
		{  0,   0,  0, -src[1].x, -src[1].y, -1, src[1].x*dst[1].y, src[1].y*dst[1].y, -dst[1].y }, // h21
		{-src[2].x, -src[2].y, -1,   0,   0,  0, src[2].x*dst[2].x, src[2].y*dst[2].x, -dst[2].x }, // h22
		{  0,   0,  0, -src[2].x, -src[2].y, -1, src[2].x*dst[2].y, src[2].y*dst[2].y, -dst[2].y }, // h23
		{-src[3].x, -src[3].y, -1,   0,   0,  0, src[3].x*dst[3].x, src[3].y*dst[3].x, -dst[3].x }, // h31
		{  0,   0,  0, -src[3].x, -src[3].y, -1, src[3].x*dst[3].y, src[3].y*dst[3].y, -dst[3].y }, // h32
	};


	gaussian_elimination(&P[0][0],9);


	// gaussian elimination gives the results of the equation system
	// in the last column of the original matrix.
	// opengl needs the transposed 4x4 matrix:
	float aux_H[]= { P[0][8],P[3][8],0,P[6][8], // h11  h21 0 h31
		P[1][8],P[4][8],0,P[7][8], // h12  h22 0 h32
		0      ,      0,0,0,       // 0    0   0 0
		P[2][8],P[5][8],0,1
	};      // h13  h23 0 h33

	for(int i=0; i<16; i++) homography[i] = aux_H[i];
}


bool testApp::insidePolygon(ofVec2f *polygon, int N, ofVec2f p) {
   int counter = 0;
   int i;
   double xinters;
   ofPoint p1,p2;

   p1 = polygon[0];
   for (i=1;i<=N;i++) {
	   p2 = polygon[i % N];
	   if (p.y > MIN(p1.y,p2.y)) {
		   if (p.y <= MAX(p1.y,p2.y)) {
			   if (p.x <= MAX(p1.x,p2.x)) {
				   if (p1.y != p2.y) {
					   xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
					   if (p1.x == p2.x || p.x <= xinters)
						   counter++;
				   }
			   }
		   }
	   }
	   p1 = p2;
   }
	return counter % 2 != 0;
}
*/
