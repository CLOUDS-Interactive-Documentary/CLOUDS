
#include "CloudsVisualSystemVerletForm.h"

vector<ofColor> CloudsVisualSystemVerletForm::initColors(int row) {

	//palette
	ofPixels pix;
	vector<ofColor> col;
	
	if( !ofLoadImage(pix, getVisualSystemDataPath() + "Palettes/catenarycolors.png") ){
		ofLogError() << "COULDN'T LOAD COLORS";
		return col;
	}
	
	row = ofClamp(row, 1, pix.getHeight()-1);
	//int randomPalette = int(ofRandom(1, pix.getHeight()));

	for(int i = 1; i < pix.getWidth(); i++){
		ofColor color = pix.getColor(i, row);
		if(color.a == 0) break;
		col.push_back( color );
	}
	cout << "found " << col.size() << " for palette row " << row << endl;
	return col;
}

CloudsVisualSystemVerletForm::CloudsVisualSystemVerletForm(){
	
	springStrength = .1;
	springDampening = .1;
	
	clothWidth = 500;
	clothHeight = 500;
	
	gridSize = 40;

}

string CloudsVisualSystemVerletForm::getSystemName(){
	return "VerletForm";
}

void CloudsVisualSystemVerletForm::selfSetup(){

}

void CloudsVisualSystemVerletForm::generateMesh(){
	
	cout << "GENERATING MESH " << endl;

	colors = initColors( int(colorIndex) );

	clearElements();
	
	physics.setWorldSize(ofVec3f(-clothWidth*2,-clothWidth*2,-clothWidth*2),
						 ofVec3f(clothWidth*2,clothHeight*2,clothWidth*2));
	physics.setSectorCount(1);
	physics.setDrag(1.0);
	physics.setGravity(-.1);
		

	ofMesh baseMesh;
	float gridStepX = (float) ((clothWidth) / gridSize);
	float gridStepY = (float) ((clothHeight) / gridSize);
		
	float ext=springExtend*2;


	for (int i = 0; i < gridSize; i++){
		
		particles.push_back(vector<Particle3D*>());
		
		for (int j = 0; j < gridSize; j++){

			ofVec3f position = ofVec3f(j * gridStepX - (clothWidth / 2), 0.0, i * gridStepY - clothHeight/2) ;
			Particle3D* particle = physics.makeParticle(position);
			particle->setMass(.2);
			particles[i].push_back( particle );
			
//			particleToMeshIndex[particle] = baseMesh.getNumVertices();
			meshIndexToParticle[baseMesh.getNumVertices()] = particle;
			
			baseMesh.addVertex( position );
			
			if (j > 0){
				physics.makeSpring( particles[i][j - 1], particles[i][j], springStrength, rndExtend());
			}
		}
	}
	
	for (int j = 0; j < gridSize; j++){
		for (int i = 1; i < gridSize; i++){
			physics.makeSpring(particles[i - 1][j], particles[i][j], springStrength, rndExtend());
		}
	}
	
	//mesh.clearIndices();;
	int w = gridSize;
	int h = gridSize;
	for (int y = 0; y < gridSize-1; y++){
		for (int x = 0; x < gridSize-1; x++){
			
			ofIndexType a,b,c;
			a = x+y*w;
			b = (x+1)+y*w;
			c = x+(y+1)*w;
			baseMesh.addIndex(a);
			baseMesh.addIndex(b);
			baseMesh.addIndex(c);
			
			a = (x+1)+(y+1)*w;
			b = x+(y+1)*w;
			c = (x+1)+(y)*w;
			baseMesh.addIndex(a);
			baseMesh.addIndex(b);
			baseMesh.addIndex(c);
		}
	}
	
	//now we split it up and add colors	for(int)
	for(int i = 0; i < baseMesh.getNumIndices(); i+=3){
		
		particleToMeshIndices[ meshIndexToParticle[ baseMesh.getIndex(i+0) ] ].push_back(mesh.getNumVertices()+0);
		particleToMeshIndices[ meshIndexToParticle[ baseMesh.getIndex(i+1) ] ].push_back(mesh.getNumVertices()+1);
		particleToMeshIndices[ meshIndexToParticle[ baseMesh.getIndex(i+2) ] ].push_back(mesh.getNumVertices()+2);

		mesh.addVertex( baseMesh.getVertices()[ baseMesh.getIndex(i+0) ] );
		mesh.addVertex( baseMesh.getVertices()[ baseMesh.getIndex(i+1) ] );
		mesh.addVertex( baseMesh.getVertices()[ baseMesh.getIndex(i+2) ] );

		mesh.addColor(colors[ int(ofNoise(mesh.getVertices()[i+0].x, mesh.getVertices()[i+0].z) * (colors.size()-1)) ]);
		mesh.addColor(colors[ int(ofNoise(mesh.getVertices()[i+1].x+100, mesh.getVertices()[i+1].z+100) * (colors.size()-1)) ]);
		mesh.addColor(colors[ int(ofNoise(mesh.getVertices()[i+2].x+200, mesh.getVertices()[i+2].z+200) * (colors.size()-1)) ]);

		mesh.addNormal(ofVec3f(0,1,0));
		mesh.addNormal(ofVec3f(0,1,0));
		mesh.addNormal(ofVec3f(0,1,0));
		
		mesh.addIndex(i+0);
		mesh.addIndex(i+1);
		mesh.addIndex(i+2);
	}
	
	updateNormals();
	
	float rh=clothHeight*0.1f;
	int numConnectionsPerEdge = gridSize/2;
	for(int i = 0; i < numConnectionsPerEdge; i++){
		
		int xIndex = ofRandom(gridSize);
		int yIndex = 0; //ofRandom(gridSize);
		
		particles[xIndex][yIndex]->moveTo( particles[xIndex][yIndex]->getPosition() + ofVec3f(0, ofRandom(-rh,rh), 0) );
		particles[xIndex][yIndex]->makeFixed();
	}
	
	for(int i = 0; i < numConnectionsPerEdge; i++){
		int xIndex = 0;//ofRandom(gridSize);
		int yIndex = ofRandom(gridSize); //ofRandom(gridSize);

		particles[xIndex][yIndex]->moveTo( particles[xIndex][yIndex]->getPosition() + ofVec3f(0, ofRandom(-rh,rh), 0) );
		particles[xIndex][yIndex]->makeFixed();
	}
	
	//fix y back
	for(int i = 0; i < numConnectionsPerEdge; i++){
		int xIndex = gridSize-1;//ofRandom(gridSize);
		int yIndex = ofRandom(gridSize); //ofRandom(gridSize);
		
		particles[xIndex][yIndex]->moveTo( particles[xIndex][yIndex]->getPosition() + ofVec3f(0, ofRandom(-rh,rh), 0) );
		particles[xIndex][yIndex]->makeFixed();
	}

	for(int i = 0; i < numConnectionsPerEdge; i++){
		int xIndex = ofRandom(gridSize); //ofRandom(gridSize);
		int yIndex = gridSize - 1; //ofRandom(gridSize);
		
		particles[xIndex][yIndex]->moveTo( particles[xIndex][yIndex]->getPosition() + ofVec3f(0, ofRandom(-rh,rh), 0) );
		particles[xIndex][yIndex]->makeFixed();
	}

	int cnt=ofRandom(5,25);
	rh*=5;

	for(int i = 0; i < cnt; i ++){
		int xIndex = ofRandom(gridSize); //ofRandom(gridSize);
		int yIndex = ofRandom(gridSize); //ofRandom(gridSize);
		
		particles[xIndex][yIndex]->moveTo( particles[xIndex][yIndex]->getPosition() + ofVec3f(0, ofRandom(-rh,rh), 0) );
		particles[xIndex][yIndex]->makeFixed();
	}
}

void CloudsVisualSystemVerletForm::selfSetupGuis(){
	
    clothGui = new ofxUISuperCanvas("CLOTH", gui);
    clothGui->copyCanvasStyle(gui);
    clothGui->copyCanvasProperties(gui);
    clothGui->setName("ClothSettings");
    clothGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	clothGui->addSlider("COLOR INDEX", 0, 909, &colorIndex);
	
	clothGui->addSlider("CLOTH WIDTH", 10, 1000, &clothWidth);
	clothGui->addSlider("CLOTH HEIGHT", 10, 1000, &clothHeight);
	clothGui->addSlider("GRID SIZE", 10, 100, &gridSize);

	clothGui->addLabel("SPRINGS");
	clothGui->addSlider("SPRING STRENGTH", .001, 1.0, &springStrength);
	clothGui->addSlider("SPRING EXTEND", 1, 20, &springExtend);
	
	clothGui->addButton("REGENERATE", &shouldRegenerateMesh);

	ofAddListener(clothGui->newGUIEvent, this, &CloudsVisualSystemVerletForm::selfGuiEvent);
	
    guis.push_back(clothGui);
    guimap[clothGui->getName()] = clothGui;

	
	for(int i = 0; i < 3; i++){
		
		auxLightGuis[i] = new ofxUISuperCanvas("AUX LIGHT " + ofToString(i), gui);
		auxLightGuis[i]->copyCanvasStyle(gui);
		auxLightGuis[i]->copyCanvasProperties(gui);
		auxLightGuis[i]->setName("AuxLight " + ofToString(i));
		auxLightGuis[i]->setWidgetFontSize(OFX_UI_FONT_SMALL);

		auxLightGuis[i]->addToggle("ENABLE", &auxLights[i].enabled);
		auxLightGuis[i]->addSlider("SPIN RADIUS", 100, 400, &auxLights[i].spinRadius);
		auxLightGuis[i]->addMinimalSlider("SPIN AXIS X", -1, 1, &auxLights[i].spinAxis.x);
		auxLightGuis[i]->addMinimalSlider("SPIN AXIS Y", -1, 1, &auxLights[i].spinAxis.y);
		auxLightGuis[i]->addMinimalSlider("SPIN AXIS Z", -1, 1, &auxLights[i].spinAxis.z);
		auxLightGuis[i]->addSlider("SPIN SPEED", 0, 4, &auxLights[i].spinSpeed);
		
		ofAddListener(auxLightGuis[i]->newGUIEvent, this, &CloudsVisualSystemVerletForm::selfGuiEvent);
		
		guis.push_back(auxLightGuis[i]);
		guimap[clothGui->getName()] = auxLightGuis[i];

	}
}

void CloudsVisualSystemVerletForm::selfPresetLoaded(string presetPath){
	shouldRegenerateMesh = true;
}

void CloudsVisualSystemVerletForm::selfUpdate(){

	gridSize = int(gridSize);
	
	if(shouldRegenerateMesh){
		generateMesh();
		shouldRegenerateMesh = false;
	}
	
	physics.update();
	
	
	//set mesh vertices to particle position
	for(int i = 0; i < physics.numberOfParticles(); i++){
		Particle3D* particle = physics.getParticle(i);
		for(int j = 0; j < particleToMeshIndices[particle].size(); j++ ){
			mesh.getVertices()[ particleToMeshIndices[ particle ][j] ].set(particle->getPosition());
		}
	}
	
	updateNormals();
}

void CloudsVisualSystemVerletForm::updateNormals(){
	for(int i = 0; i < mesh.getNumIndices(); i += 3){
		
		ofVec3f& a = mesh.getVertices()[ mesh.getIndex(i+0) ];
		ofVec3f& b = mesh.getVertices()[ mesh.getIndex(i+1) ];
		ofVec3f& c = mesh.getVertices()[ mesh.getIndex(i+2) ];
		
		ofVec3f normal = (b-a).getCrossed(c-a).normalized();
		
		mesh.setNormal(i+0, normal);
		mesh.setNormal(i+1, normal);
		mesh.setNormal(i+2, normal);
	}
}

void CloudsVisualSystemVerletForm::selfDrawBackground(){	 

}

void CloudsVisualSystemVerletForm::selfDrawDebug(){
	
}

void CloudsVisualSystemVerletForm::selfSceneTransformation(){
	
}

void CloudsVisualSystemVerletForm::selfDraw(){

	ofEnableAlphaBlending();
	
	
	for(int i = 0; i < 3; i++){
		
		if(!auxLights[i].enabled){
			continue;
		}
		
		auxLights[i].currentRot += auxLights[i].spinSpeed;
		ofVec3f source;
		if(auxLights[i].spinAxis.isAligned(ofVec3f(0,1,0))){
			source = ofVec3f(1,0.0,0).getCrossed(auxLights[i].spinAxis).normalized();
		}
		else{
			source = ofVec3f(0,1,0).getCrossed(auxLights[i].spinAxis).normalized();
		}
		ofVec3f newPosition = source.getRotated(auxLights[i].currentRot, auxLights[i].spinAxis) * auxLights[i].spinRadius;
		auxLights[i].light.setPosition( newPosition );
//		auxLights[i].light.setAttenuation(1.0*ofGetMouseX()/ofGetWidth());
		auxLights[i].light.enable();
	}
	
	mat->begin();
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	mesh.draw();
	mat->end();
	
	for(int i = 0; i < 3; i++){
		if(auxLights[i].enabled) auxLights[i].light.disable();
	}
	
}

void CloudsVisualSystemVerletForm::selfExit(){
	
}

void CloudsVisualSystemVerletForm::selfBegin(){
	shouldRegenerateMesh = true;
}

void CloudsVisualSystemVerletForm::selfEnd(){
	clearElements();
}

void CloudsVisualSystemVerletForm::clearElements(){
	mesh.clear();
	meshIndexToParticle.clear();
	particleToMeshIndices.clear();
	
	particles.clear();
	physics.clear();
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

	if(e.widget->getName() == "REGENERATE" && ((ofxUIButton*)e.widget)->getValue() ){
		generateMesh();
	}
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



float rndExtend (int xy) {
  int r=ofRandom(0.4f,1.5f)*springExtend;
  return r*((xy==0 ? clothWidth : clothHeight)/gridSize) ;
}
