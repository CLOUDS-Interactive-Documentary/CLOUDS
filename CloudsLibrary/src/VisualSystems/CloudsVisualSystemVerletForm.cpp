
#include "CloudsVisualSystemVerletForm.h"

CloudsVisualSystemVerletForm::CloudsVisualSystemVerletForm(){
	springStrength = .2;
	springDampening = .1;
	
	clothWidth = 200;
	clothHeight = 200;
	
	gridSizeX = 20;
	gridSizeY = 20;
}

string CloudsVisualSystemVerletForm::getSystemName(){
	return "VerletForm";
}

void CloudsVisualSystemVerletForm::selfSetup(){
	
	
	physics.setWorldSize(ofVec3f(-clothWidth*2,-clothWidth*2,-clothWidth*2), ofVec3f(clothWidth*2,clothHeight*2,clothWidth*2));
	physics.setSectorCount(1);
    physics.setDrag(0.1f);
	physics.setGravity(.9);

//	particles = new Particle[gridSize][gridSize];
	
	float gridStepX = (float) ((clothWidth) / gridSizeX);
	float gridStepY = (float) ((clothHeight) / gridSizeY);
	
	for (int i = 0; i < gridSizeX; i++){
		
		particles.push_back(vector<Particle3D*>());
		
		for (int j = 0; j < gridSizeY; j++){

			particles[i].push_back( physics.makeParticle( ofVec3f(j * gridStepX - (clothWidth / 2), 0.0, i * gridStepY - clothHeight/2) ));
			particles[i][j]->setMass(.2);
			
			if (j > 0){
				physics.makeSpring( particles[i][j - 1], particles[i][j], springStrength, gridStepX);
			}
		}
	}
	
	for (int j = 0; j < gridSizeX; j++){
		for (int i = 1; i < gridSizeY; i++){
			physics.makeSpring(particles[i - 1][j], particles[i][j], springStrength, gridStepY);
		}
	}
	
//	particles[0][0]->makeFixed();
	particles[gridSizeX/2][gridSizeY/2]->makeFixed();
	
}

void CloudsVisualSystemVerletForm::selfSetupGuis(){
	
}

void CloudsVisualSystemVerletForm::selfUpdate(){
	
	physics.update();
	
}

void CloudsVisualSystemVerletForm::selfDrawBackground(){
	
}

void CloudsVisualSystemVerletForm::selfDrawDebug(){
	
}

void CloudsVisualSystemVerletForm::selfSceneTransformation(){
	
}

void CloudsVisualSystemVerletForm::selfDraw(){
	ofMesh points;
	for(int i = 0; i < physics.numberOfParticles(); i++){
		points.addVertex(physics.getParticle(i)->getPosition());
	}
	points.setMode(OF_PRIMITIVE_POINTS);
	
	ofMesh lines;
	for(int i = 0; i < physics.numberOfSprings(); i++){
		lines.addVertex(physics.getSpring(i)->getOneEnd()->getPosition());
		lines.addVertex(physics.getSpring(i)->getTheOtherEnd()->getPosition());
	}
	lines.setMode(OF_PRIMITIVE_LINES);
	
	ofPushStyle();
	ofSetLineWidth(1);
	glPointSize(2);
	lines.draw();
	points.draw();
	ofPopStyle();
}

void CloudsVisualSystemVerletForm::selfExit(){
	
}

void CloudsVisualSystemVerletForm::selfBegin(){
	
}

void CloudsVisualSystemVerletForm::selfEnd(){
	
}

void CloudsVisualSystemVerletForm::selfKeyPressed(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemVerletForm::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemVerletForm::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemVerletForm::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemVerletForm::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemVerletForm::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemVerletForm::selfSetupGui(){
	
}

void CloudsVisualSystemVerletForm::selfGuiEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemVerletForm::selfSetupSystemGui(){
	
}

void CloudsVisualSystemVerletForm::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemVerletForm::selfSetupRenderGui(){
	
}

void CloudsVisualSystemVerletForm::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemVerletForm::selfSetupTimeline(){
	
}

void CloudsVisualSystemVerletForm::selfSetupTimelineGui(){
	
}

void CloudsVisualSystemVerletForm::selfTimelineGuiEvent(ofxUIEventArgs &e){
	
}
