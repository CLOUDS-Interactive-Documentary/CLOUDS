
#include "CloudsVisualSystemVerletForm.h"

CloudsVisualSystemVerletForm::CloudsVisualSystemVerletForm(){
	springStrength = .5;
	springDampening = .1;
	
	clothWidth = 200;
	clothHeight = 200;
	
	gridSizeX = 50;
	gridSizeY = 50;
}

string CloudsVisualSystemVerletForm::getSystemName(){
	return "VerletForm";
}

void CloudsVisualSystemVerletForm::selfSetup(){
	
	physics.setWorldSize(ofVec3f(-clothWidth,-clothWidth,-clothWidth),
						 ofVec3f(clothWidth,clothHeight,clothWidth));
	//physics.setWorldSize(ofVec3f(-clothWidth*2,-clothWidth*2,-clothWidth*2), ofVec3f(clothWidth*2,clothHeight*2,clothWidth*2));
	physics.setSectorCount(1);
    physics.setDrag(0.1f);
	physics.setGravity(9.8);
	
	float gridStepX = (float) ((clothWidth) / gridSizeX);
	float gridStepY = (float) ((clothHeight) / gridSizeY);
	
	for (int i = 0; i < gridSizeX; i++){
		
		particles.push_back(vector<Particle3D*>());
		
		for (int j = 0; j < gridSizeY; j++){

			ofVec3f position = ofVec3f(j * gridStepX - (clothWidth / 2), 0.0, i * gridStepY - clothHeight/2) ;
			Particle3D* particle = physics.makeParticle(position);
			particle->setMass(.2);
			particles[i].push_back( particle );
			
			particleToMeshIndex[particle] = mesh.getNumVertices();
			
			mesh.addVertex( position );
			
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
	
	
	int w = gridSizeX;
	int h = gridSizeY;
	for (int y = 0; y < gridSizeY-1; y++){
		for (int x = 0; x < gridSizeX-1; x++){
			ofIndexType a,b,c;
			a = x+y*w;
			b = (x+1)+y*w;
			c = x+(y+1)*w;
			mesh.addIndex(a);
			mesh.addIndex(b);
			mesh.addIndex(c);
			
			a = (x+1)+(y+1)*w;
			b = x+(y+1)*w;
			c = (x+1)+(y)*w;
			mesh.addIndex(a);
			mesh.addIndex(b);
			mesh.addIndex(c);
		}
	}
	
	
	//particles[gridSizeX/2][gridSizeY/2]->makeFixed();
	for(int i = 0; i < 15; i++){
		particles[ofRandom(gridSizeX)][ofRandom(gridSizeY)]->makeFixed();
	}
	
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
	
	
	for(int i = 0; i < physics.numberOfParticles(); i++){
		Particle3D* particle = physics.getParticle(i);
		mesh.getVertices()[ particleToMeshIndex[ particle ] ].set(particle->getPosition());
//		points.addVertex(physics.getParticle(i)->getPosition());
	}
	
	mesh.drawWireframe();
	/*
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
	 */
	
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
