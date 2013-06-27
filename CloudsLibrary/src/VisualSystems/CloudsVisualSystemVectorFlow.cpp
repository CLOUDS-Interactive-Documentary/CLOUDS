
#include "CloudsVisualSystemVectorFlow.h"

//TODO preallocate buffers

//--------------------------------------------------------------
CloudsVisualSystemVectorFlow::CloudsVisualSystemVectorFlow(){
	step = 50;
	particlesPerFrame = 0;
	
}

//--------------------------------------------------------------
string CloudsVisualSystemVectorFlow::getSystemName(){
	return "VectorFlow";
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::initFlowField(){
	maxVertices = generateMaxVerts;
	trailLength = generateTrailLength;
	width = ofGetWidth();
	height = ofGetHeight();
		
	//gives our initial columns
	cout << "creating " << width/int(step) << " " << height/int(step) << " lines" << endl;

	particleMesh.clear();
	particles.clear();
	particleMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	
    cout << "adding vertices" << endl;
    
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

}

void CloudsVisualSystemVectorFlow::selfSetupGuis(){
	
}

void CloudsVisualSystemVectorFlow::selfUpdate(){
	

    
	if(regenerateFlow){
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
		
		float magnitude = getMagnitude(cp.pos.y, cp.pos.x);
		float magnorm = magnitude/maxLength;
		float scaledHue = ofMap(magnorm, 0, 1, ofFloatColor::blue.getHue(), ofFloatColor::red.getHue());
		cp.pos += getDirection(cp.pos.x, cp.pos.y) * magnitude;
		
		particleMesh.setVertex(cp.index, cp.pos); //set the fence post
		particleMesh.setVertex(cp.index+1, cp.pos);
		
		ofFloatColor magnitudeColor = ofFloatColor::fromHsb(scaledHue, 200, 200) ;
		particleMesh.setColor(cp.index+1, magnitudeColor);
	
		particleMesh.setVertex(cp.index + trailLength+1, particleMesh.getVertices()[cp.index+trailLength]); //set the fence post
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
		float length = getMagnitude(vert.x, vert.y);
		lines.setVertex(i+1, vert + getDirection(vert.x, vert.y) * length);
		ofFloatColor c = ofFloatColor::white.getLerped( ofFloatColor::red, (length-5)/maxLength );
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
	float chaossqr = powf(chaos,2);	
	return ofVec3f(0,1,0).getRotated( ofNoise(x/chaossqr + ofGetFrameNum()*speed, y/chaossqr)*360, ofVec3f(0,0,1) );
}

float CloudsVisualSystemVectorFlow::getMagnitude(float x, float y){
	float chaossqr = powf(chaos,2);	
	return maxLength * ofNoise(y/chaossqr + ofGetFrameNum()*speed,x/chaossqr);
}

void CloudsVisualSystemVectorFlow::selfDrawBackground(){
    
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(255);
	ofSetLineWidth(1);
	
	lines.draw();
	particleMesh.draw();
	
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
	rdrGui->addSlider("maxLength", 0, 100, &maxLength);
	rdrGui->addSlider("fieldAlpha", 0, 1.0, &fieldAlpha);

	
//	rdrGui->addSlider("speed", 0., 2., &speed);
//	rdrGui->addSlider("point size", 1., 25., &pointSize);
//	rdrGui->addSlider("scale", 1., 10., &scale);
}

//--------------------------------------------------------------
void CloudsVisualSystemVectorFlow::guiRenderEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "regenerate"){
		regenerateFlow = true;
	}
}

