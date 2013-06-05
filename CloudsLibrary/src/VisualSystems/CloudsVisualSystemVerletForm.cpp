
#include "CloudsVisualSystemVerletForm.h"

vector<ofColor> CloudsVisualSystemVerletForm::initColors() {

	vector<ofColor> colstr;
	colstr.push_back(ofColor::fromHex(0xFFFF00)); colstr.push_back(ofColor::fromHex(0xFF9900)); //0
	colstr.push_back(ofColor::fromHex(0xCABEBD)); colstr.push_back(ofColor::fromHex(0xFFFFFF)); //1
	colstr.push_back(ofColor::fromHex(0x001B2E)); colstr.push_back(ofColor::fromHex(0xA1CDFF)); //2
	colstr.push_back(ofColor::fromHex(0xFF0000)); colstr.push_back(ofColor::fromHex(0x7D0000)); //3
	colstr.push_back(ofColor::fromHex(0x00FF00)); colstr.push_back(ofColor::fromHex(0x003800)); //4
	colstr.push_back(ofColor::fromHex(0x00FFFF)); colstr.push_back(ofColor::fromHex(0x00ABC9)); //5
	colstr.push_back(ofColor::fromHex(0xFF3287)); colstr.push_back(ofColor::fromHex(0xFFA1C1)); //6
	colstr.push_back(ofColor::fromHex(0xFF7700)); colstr.push_back(ofColor::fromHex(0xFF2500)); //7
	colstr.push_back(ofColor::fromHex(0x480081)); colstr.push_back(ofColor::fromHex(0xB477FF)); //8
	colstr.push_back(ofColor::fromHex(0x373737)); colstr.push_back(ofColor::fromHex(0x838383)); //9
	colstr.push_back(ofColor::fromHex(0xFFFFFF)); colstr.push_back(ofColor::fromHex(0xF0F0F0)); //10
	colstr.push_back(ofColor::fromHex(0xCCFFFF)); colstr.push_back(ofColor::fromHex(0x0099FF)); //11
	colstr.push_back(ofColor::fromHex(0x003300)); colstr.push_back(ofColor::fromHex(0x33FF00));  //12

	
	vector<ofColor> col;
	for (int i = 0; i < colstr.size() / 2; i++) {
		if (ofRandom(100) > 30) {
//			int c1 = toColor(colstr[i*2]);
//			int c2 = toColor(colstr[i*2+1]);
			
			ofColor c1 = colstr[i*2];
			ofColor c2 = colstr[i*2+1];
			int n = (int)ofRandom(3, 11);
			
			for(int j = 0; j < n; j++){
				col.push_back( c1.getLerped(c2, ofMap(j, 0, n-1, 0, 1)) );
				//col.push_back( colstr );
			}
		}
	}
	
	return col;
}
//
//void CloudsVisualSystemVerletForm::shuffleColors(vector<int>& cl){
//	int id1, id2, tmp;
//	
//	int colors[] = new int[cl.size()];
//	
//	for(int i=0; i<colors.length; i++){
//		colors[i] = cl.get(i);	
//	}
//	
//	int n=colors.length;
//	for (int i=0; i<n; i++) {
//		do {
//			id1=(int)random(n);
//			id2=(int)random(n);
//		}
//		while (id1==id2);
//		tmp=colors[id2];
//		colors[id2]=colors[id1];
//		colors[id1]=tmp;
//	}
//	
//	cl.clear();
//	for(int cc:colors) cl.add(cc);
//}
//

/*
int interpolate(float fract, int col1, int col2) {
	float r, r2, g, g2, b, b2, a, a2;
	
	r=(col1 >> 16) &0xff;
	r2=((col2 >> 16)&0xff)-r;
	g=(col1 >> 8)&0xff;
	g2=((col2 >> 8)&0xff)-g;
	b=col1&0xff;
	b2=(col2&0xff)-b;
	a=alpha(col1);
	a2=alpha(col2)-a;
	
	col1=color(r + r2 * fract, g + g2 * fract, b + b2 * fract, a+a2*fract);
	return col1;
}


public int toColor(String hex) {
	int alpha=255;
	if (hex.length()==8) {
		alpha=Integer.parseInt(hex.substring(0, 2), 16);
		//			UUtil.log("hex: "+hex+" alpha: "+alpha);
		hex=hex.substring(2);
	}
	
	return (alpha<<24) | Integer.parseInt(hex, 16);
}
*/

CloudsVisualSystemVerletForm::CloudsVisualSystemVerletForm(){

	springStrength = .1;
	springDampening = .1;
	
	clothWidth = 200;
	clothHeight = 200;
	
	gridSize = 25;
}

string CloudsVisualSystemVerletForm::getSystemName(){
	return "VerletForm";
}

void CloudsVisualSystemVerletForm::selfSetup(){
	colors = initColors();

	generateMesh();
//	ofRandomize(colors);

}

void CloudsVisualSystemVerletForm::generateMesh(){
	particleToMeshIndex.clear();
	
//	for(int i = 0; i < physics.numberOfSprings(); i++){
//		physics.getSpring(i)->kill();
//	}
//	for(int i = 0; i < physics.numberOfParticles(); i++){
//		physics.getParticle(i)->kill();
//	}

	particles.clear();
	physics.clear();
	mesh.clear();
	
	physics.setWorldSize(ofVec3f(-clothWidth*2,-clothWidth*2,-clothWidth*2),
						 ofVec3f(clothWidth*2,clothHeight*2,clothWidth*2));
	physics.setSectorCount(1);
    
	physics.setDrag(1.0);
	
	physics.setGravity(-.1);
	
	float gridStepX = (float) ((clothWidth) / gridSize);
	float gridStepY = (float) ((clothHeight) / gridSize);
	
	for (int i = 0; i < gridSize; i++){
		
		particles.push_back(vector<Particle3D*>());
		
		for (int j = 0; j < gridSize; j++){

			ofVec3f position = ofVec3f(j * gridStepX - (clothWidth / 2), 0.0, i * gridStepY - clothHeight/2) ;
			Particle3D* particle = physics.makeParticle(position);
			particle->setMass(.2);
			particles[i].push_back( particle );
			
			particleToMeshIndex[particle] = mesh.getNumVertices();
			
			mesh.addVertex( position );
			
			if (j > 0){
				physics.makeSpring( particles[i][j - 1], particles[i][j], springStrength, gridStepX*3);
			}
		}
	}
	
	for (int j = 0; j < gridSize; j++){
		for (int i = 1; i < gridSize; i++){
			physics.makeSpring(particles[i - 1][j], particles[i][j], springStrength, gridStepY*3);
		}
	}
	
	int w = gridSize;
	int h = gridSize;
	for (int y = 0; y < gridSize-1; y++){
		for (int x = 0; x < gridSize-1; x++){
			
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
	
	int numConnectionsPerEdge = gridSize/2;
	for(int i = 0; i < numConnectionsPerEdge; i++){
		
		int xIndex = ofRandom(gridSize);
		int yIndex =  0; //ofRandom(gridSize);
		
		particles[xIndex][yIndex]->moveTo( particles[xIndex][yIndex]->getPosition() + ofVec3f(0, ofRandom(-50, 50), 0) );
		particles[xIndex][yIndex]->makeFixed();
	}
	
	for(int i = 0; i < numConnectionsPerEdge; i++){
		int xIndex = 0;//ofRandom(gridSize);
		int yIndex = ofRandom(gridSize); //ofRandom(gridSize);

		particles[xIndex][yIndex]->moveTo( particles[xIndex][yIndex]->getPosition() + ofVec3f(0, ofRandom(-50, 50), 0) );
		particles[xIndex][yIndex]->makeFixed();
	}
	
	//fix y back
	for(int i = 0; i < numConnectionsPerEdge; i++){
		int xIndex = gridSize-1;//ofRandom(gridSize);
		int yIndex = ofRandom(gridSize); //ofRandom(gridSize);
		
		particles[xIndex][yIndex]->moveTo( particles[xIndex][yIndex]->getPosition() + ofVec3f(0, ofRandom(-50, 50), 0) );
		particles[xIndex][yIndex]->makeFixed();
	}

	for(int i = 0; i < numConnectionsPerEdge; i++){
		int xIndex = ofRandom(gridSize); //ofRandom(gridSize);
		int yIndex = gridSize - 1; //ofRandom(gridSize);
		
		particles[xIndex][yIndex]->moveTo( particles[xIndex][yIndex]->getPosition() + ofVec3f(0, ofRandom(-50, 50), 0) );
		particles[xIndex][yIndex]->makeFixed();
	}

	
	for(int i = 0; i < mesh.getVertices().size(); i++){
//		mesh.addColor(ofColor(ofRandom(255),
//							  ofRandom(255),
//							  ofRandom(255)));
		mesh.addColor(colors[ int(ofNoise(mesh.getVertices()[i].x, mesh.getVertices()[i].z) * (colors.size()-1)) ]);
	}
	
	for(int i = 0; i < 3; i ++){
		particles[ofRandom(gridSize)][ofRandom(gridSize)]->makeFixed();
	}
}

void CloudsVisualSystemVerletForm::selfSetupGuis(){
	
    clothGui = new ofxUISuperCanvas("CLOTH", gui);
    clothGui->copyCanvasStyle(gui);
    clothGui->copyCanvasProperties(gui);
    clothGui->setName("ClothSettings");
    clothGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	clothGui->addButton("REGENERATE", &shouldRegenerateMesh);
	
	clothGui->addSlider("CLOTH WIDTH", 10, 1000, &clothWidth);
	clothGui->addSlider("CLOTH HEIGHT", 10, 1000, &clothHeight);
	
	clothGui->addSlider("GRID SIZE", 10, 100, &gridSize);
	
	clothGui->addSlider("SPRING STRENGTH", .001, 1.0, &springStrength);
	
	ofAddListener(clothGui->newGUIEvent, this, &CloudsVisualSystemVerletForm::selfGuiEvent);
	
    guis.push_back(clothGui);
    guimap[clothGui->getName()] = clothGui;
	
}

void CloudsVisualSystemVerletForm::selfUpdate(){
	
	physics.update();
	
	//set mesh vertices to particle position
	for(int i = 0; i < physics.numberOfParticles(); i++){
		Particle3D* particle = physics.getParticle(i);
		mesh.getVertices()[ particleToMeshIndex[ particle ] ].set(particle->getPosition());
	}
}

void CloudsVisualSystemVerletForm::selfDrawBackground(){	

}

void CloudsVisualSystemVerletForm::selfDrawDebug(){
	
}

void CloudsVisualSystemVerletForm::selfSceneTransformation(){
	
}

void CloudsVisualSystemVerletForm::selfDraw(){

	
	
	glShadeModel(GL_FLAT);
	
	mesh.draw();
	
	ofPushStyle();
	
	int x = 0;
	int y = 0;
	for(int c = 0; c < colors.size(); c++ ){
		
		ofSetColor( colors[c] );
		
		ofRect(x,y,19,19);
		x+=20;
		
		if(x > ofGetWidth()-20) {
			x=0;
			y+=20;
		}
	}
	
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
	cout << "GUI EVENT " << e.widget->getName() << endl;
	if(e.widget->getName() == "REGENERATE"){
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
