
#include "CloudsVisualSystemVectorFlow.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
CloudsVisualSystemVectorFlow::CloudsVisualSystemVectorFlow(){
	step = 50;
	particlesPerFrame = 0;
	chaos = 0;
	speed = 0;
	sincPosition = ofVec2f(.5,.5);
	sincRadius = 0;
    sincStrength = 1;
}

//--------------------------------------------------------------
string CloudsVisualSystemVectorFlow::getSystemName(){
	return "VectorFlow";
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::initFlowField(){
	maxVertices = generateMaxVerts;
	trailLength = generateTrailLength;
	
	width = getSharedRenderTarget().getWidth();
	height = getSharedRenderTarget().getHeight();
		
	//gives our initial columns
//	cout << "creating " << width/int(step) << " " << height/int(step) << " lines" << endl;

	particles.clear();
	particleMesh.clear();
	particleMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	particleMesh.setUsage(GL_STREAM_DRAW);
	
//    cout << "adding vertices" << endl;
    
	lines.clear();
	lines.setMode(OF_PRIMITIVE_LINES);
	for(int y = 0; y <= height/step; y++){
		for(int x = 0; x <= width/step; x++){
			lines.addVertex( ofVec3f(x*step,y*step,0) );
			lines.addVertex( ofVec3f(x*step,y*step,0) );
			lines.addColor( ofColor::white );
			lines.addColor( ofColor::white );
		}
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::selfSetup(){
	colorMap.loadImage( GetCloudsDataPath() + "colors/defaultColorPalette.png" );
	bIs2D = true;
}

void CloudsVisualSystemVectorFlow::selfSetupGuis(){
	
}

void CloudsVisualSystemVectorFlow::selfUpdate(){
	   
	if(regenerateFlow || width != ofGetWidth() || height != ofGetHeight()){
		regenerateFlow = false;
		initFlowField();
	}
    

	//UPDATE PARTICLES
	for(int i = 0; i < particlesPerFrame; i++){
		addParticle();
	}
	
	for(int i = 0; i < particles.size(); i++){
		FlowParticle& cp = particles[i];
		for(int t = trailLength; t > 0; t--){
			ofIndexType targetIndex = cp.index + t + 1;
			ofIndexType sourceIndex = cp.index + t;
			particleMesh.setVertex(targetIndex, particleMesh.getVertices()[sourceIndex] );
			particleMesh.setColor(targetIndex, particleMesh.getColors()[sourceIndex] );
			particleMesh.getColors()[targetIndex].a = particleMesh.getNormals()[targetIndex].x;
		}
		
		//find magnitude of the field, it'll effect both position and color
		float magnitude = sampleField(cp.pos.y, cp.pos.x) * maxLength;
		float magnorm = magnitude/maxLength;

		//set position
		cp.pos += getDirection(cp.pos.x, cp.pos.y) * magnitude;
		particleMesh.setVertex(cp.index,   cp.pos); //set the fence post
		particleMesh.setVertex(cp.index+1, cp.pos);
		particleMesh.setVertex(cp.index + trailLength+1, particleMesh.getVertices()[cp.index+trailLength]); //set the fence post
		
		//set color
		ofFloatColor magnitudeColor;
		//float scaledHue = ofMap(magnorm, 0, 1, ofFloatColor::blue.getHue(), ofFloatColor::red.getHue());
		if(interpRGB){
			magnitudeColor = startColor.getLerped(endColor, magnorm);
		}
		else {
			float scaledHue = ofMap(magnorm, 0, 1, startColor.getHue(), endColor.getHue());
			float scaledSat = ofMap(magnorm, 0, 1, startColor.getSaturation(), endColor.getSaturation());
			float scaledBri = ofMap(magnorm, 0, 1, startColor.getBrightness(), endColor.getBrightness());
			magnitudeColor = ofFloatColor::fromHsb(scaledHue, scaledSat, scaledBri);
		}
//		ofFloatColor magnitudeColor = ofFloatColor::fromHsb(scaledHue, 200, 200) ;
		particleMesh.setColor(cp.index+1, magnitudeColor);
	}

	ofRectangle screenRect(0,0,width,height);
	for(int i = 0; i < particles.size(); i++){
		FlowParticle& cp = particles[i];
		cp.dead = true;
		for(int v = cp.index+1; v <= cp.index+trailLength; v++){
			ofVec3f& vert = particleMesh.getVertices()[v];
			if(screenRect.inside(vert.x, vert.y)){
				cp.dead = false;
			}
		}
	}
	
	//UPDATE LINES
	for(int i = 0; i < lines.getVertices().size(); i += 2){
		ofVec3f& vert = lines.getVertices()[i];
		float length = sampleField(vert.x, vert.y);
		lines.setVertex(i+1, vert + getDirection(vert.x, vert.y) * length * fieldAmplitude * maxLength);
		ofFloatColor c = ofFloatColor::white.getLerped( ofFloatColor::red, length );
		c.a = fieldAlpha;
		lines.setColor(i+1, c);
	}
}

void CloudsVisualSystemVectorFlow::addParticle(){
		
	for(int i = 0; i < particles.size(); i++){
		FlowParticle& p = particles[i];

		if(p.dead){
			p.dead = false;
			p.pos = ofVec3f( ofRandom(width), ofRandom(height), 0 );
			
			//alpha fence post begin
			particleMesh.setVertex(p.index, p.pos ); //at p.index
			
			for(int i = 0; i < trailLength; i++){
				particleMesh.setVertex( p.index + i, p.pos ); //p.index+1 to p.index+trailLength
			}
			
			//alpha fence post end
			particleMesh.setVertex(p.index+trailLength+1, p.pos ); //p.index+trailLength+1
		
			return;
		}
	}

	if(particleMesh.getVertices().size() < maxVertices){
		
//		cout << "allocating particles " << particleMesh.getVertices().size() << " trail " << trailLength << " " << endl;
		
		FlowParticle p;
		p.pos = ofVec3f( ofRandom(width), ofRandom(height), 0 );
		p.index = particleMesh.getVertices().size();

		//alpha fence post begin
		particleMesh.addVertex( p.pos ); //at p.index
		particleMesh.addColor( ofFloatColor(0,0,0,0) );
		particleMesh.addNormal( ofVec3f(0,0,0) );
		
		for(int i = 0; i < trailLength; i++){
			particleMesh.addVertex( p.pos ); //p.index+1 to p.index+trailLength
			//particleMesh.addColor( ofFloatColor(1,0,float(trailLength-1-i)/trailLength,float(trailLength-1-i)/trailLength) );
			particleMesh.addNormal( ofVec3f(float(trailLength-1-i)/trailLength,0,0) );
			particleMesh.addColor( ofFloatColor(1) );
		}
		
		//alpha fence post end
		particleMesh.addVertex( p.pos ); //p.index+trailLength+1
		particleMesh.addColor( ofFloatColor(0,0,0,0) );
		particleMesh.addNormal( ofVec3f(0,0,0) );
		
		particles.push_back(p);
	}
	
//	cout << "finished with particle add" << endl;
}

ofVec3f CloudsVisualSystemVectorFlow::getDirection(float x, float y){
//	return ofVec3f(0,1,0).getRotated( sampleField(x,y) * 360, ofVec3f(0,0,1) );
	float ssWeight = 0;
	float ssAngle = 0;
	getSincSourceAngle(x,y,ssAngle,ssWeight);
	float anglePercent = sampleField(x, y) * (1. - ssWeight) + ssAngle * ssWeight;
	return ofVec3f(0,1,0).getRotated( anglePercent  * 360,  ofVec3f(0,0,1) );
}


float CloudsVisualSystemVectorFlow::sampleField(float x, float y){
	float chaossqr   = powf(chaos,2);
	float oscillator = sin( oscFrequency*ofGetFrameNum() ) ;
	float sample = (ofNoise(y/chaossqr + ofGetFrameNum()*speed,  x/chaossqr) + (oscillator * .5 + .5)) * .5;
	return sample;
}

void CloudsVisualSystemVectorFlow::getSincSourceAngle(int x, int y, float& angle, float& weight){
	ofVec2f pos(x,y);
//	ofVec2f sincToPos = sincPosition - pos;
	ofVec2f mousePos(ofGetMouseX(),ofGetMouseY());
	ofVec2f sincToPos = mousePos - pos;
	float distSq = sincToPos.lengthSquared();
//	if( distSq < powf(sincRadius, 2) ){
//	angle = ofVec2f(0, 1).angle(sincToPos) / 360.0;;
	float sincRadSq = powf(sincRadius*ofGetWidth(),2);
	if(sincRadius > 0 && distSq < sincRadSq){
//		angle = ofMap( atan2(y,x) - atan2(mousePos.y,mousePos.x), -PI/2.0, PI/2.0, 0, 1.0 ) ;
		angle = ofMap( atan2(sincToPos.y,sincToPos.x), -TWO_PI, TWO_PI, -1.0, 1.0 ) ;
		weight = ofMap(distSq, 0, sincRadSq, sincStrength, 0.0, true);
//		weight = 1.;
	}
//		return
//	}
//	return 0;

}
void CloudsVisualSystemVectorFlow::selfDrawBackground(){
    
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(255);
	ofSetLineWidth(1);
	
	if(blendAdd){
		ofEnableBlendMode(OF_BLENDMODE_ADD);
	}
	else{
		ofEnableAlphaBlending();
	}
	
	lines.draw();
	particleMesh.draw();
	
//	ofSetColor(startColor);
//	ofRect(0,0,100,100);
//	ofSetColor(endColor);
//	ofRect(100,0,100,100);
	
	if(!bClearBackground){
		ofEnableAlphaBlending();
		ofSetColor(0,0,0, 5);
		ofRect(0, 0, ofGetWidth(), ofGetHeight());
	}
	ofPopStyle();

}

void CloudsVisualSystemVectorFlow::selfDrawDebug(){

}

void CloudsVisualSystemVectorFlow::selfSceneTransformation(){
	
}

void CloudsVisualSystemVectorFlow::selfDraw(){
		
}

void CloudsVisualSystemVectorFlow::selfExit(){
	
}

void CloudsVisualSystemVectorFlow::selfBegin(){
	regenerateFlow = true;
}

void CloudsVisualSystemVectorFlow::selfPresetLoaded(string presetPath)
{
	regenerateFlow = true;
}

void CloudsVisualSystemVectorFlow::selfEnd(){
	
}

void CloudsVisualSystemVectorFlow::selfKeyPressed(ofKeyEventArgs & args){

}

void CloudsVisualSystemVectorFlow::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemVectorFlow::selfMouseDragged(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::selfMouseReleased(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::selfSetupGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::selfGuiEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::selfSetupSystemGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::guiSystemEvent(ofxUIEventArgs &e){
}

void CloudsVisualSystemVectorFlow::selfSetupRenderGui(){
	
	rdrGui->addSlider("maxVerts", 10000, 100000, &generateMaxVerts);
	rdrGui->addSlider("trail length", 5, 100, &generateTrailLength);
	rdrGui->addButton("regenerate", false);
	
	rdrGui->addSlider("birthrate", 0, 100, &particlesPerFrame);
	
	rdrGui->addSlider("step", 5, 100, &step);
	rdrGui->addSlider("chaos", 5, 100, &chaos);
	rdrGui->addSlider("speed", 0, .1, &speed);
	rdrGui->addSlider("sincRadius", 0, 1.0, &sincRadius);
    rdrGui->addSlider("sincStrength", 0, 10.0, &sincStrength);
	rdrGui->addSlider("fieldAmplitude", .0, 10.0, &fieldAmplitude);

	rdrGui->addSlider("oscFreq", 0, .1, &oscFrequency);
	
	rdrGui->addSlider("maxLength", 0, 100, &maxLength);
	rdrGui->addSlider("fieldAlpha", 0, 1.0, &fieldAlpha);
	
	rdrGui->addImageSampler("startColor", &colorMap, colorMap.getWidth()/2., colorMap.getHeight()/2. );
	rdrGui->addImageSampler("endColor", &colorMap, colorMap.getWidth()/2., colorMap.getHeight()/2. );
	rdrGui->addToggle("Interpolate RGB", &interpRGB);
	rdrGui->addToggle("Refresh Bg", &bClearBackground);
	rdrGui->addToggle("Blend Add", &blendAdd);

}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::guiRenderEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "regenerate"){
		regenerateFlow = true;
	}
	else if( e.widget->getName() == "startColor"){
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		startColor = sampler->getColor();
	}
	else if( e.widget->getName() == "endColor"){
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		endColor = sampler->getColor();
	}
}

