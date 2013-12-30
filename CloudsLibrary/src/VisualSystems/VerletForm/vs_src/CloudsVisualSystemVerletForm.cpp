
#include "CloudsVisualSystemVerletForm.h"

void CloudsVisualSystemVerletForm::mwUpdate() {
	vector<int> dead;

	for(int i=0; i<ppActive.size(); i++) {
		MWParticle* pt=ppActive.at(i);
		pt->stateCnt--;

		if(ofGetFrameNum()%100==0 && i%50==0) printf("%d pt.stateCnt %d\n",
			i,pt->stateCnt);

		if(pt->stateCnt<0) {
			dead.push_back(i);
			printf("DEAD: %d %d %d\n",i,pt->stateCnt,(pt->p->isFixed() ? 1 : 0));
			if(pt->p->isFixed()) pt->p->makeFree();
			else pt->p->makeFixed();
		}
		else if(pt->state>0) pt->p->moveBy(pt->vD);
	}

	if(ofGetFrameNum()%100==0 || dead.size()>0) {
		printf("dead %d pp %d ppActive %d >> ",
			dead.size(),pp.size(),ppActive.size());
	}

	while(dead.size()>0 && ppActive.size()>10) {
		int id=dead.back();
		dead.pop_back();
		MWParticle* pt=ppActive.at(id);
		ppActive.erase(ppActive.begin()+id);
		pp.push_back(pt);
	}

	if(ofGetFrameNum()%100==0) {
		printf("pp %d ppActive %d\n",pp.size(),ppActive.size());
	}
}

void CloudsVisualSystemVerletForm::mwNewActivity(int id,int state) {
	activityCnt=ofRandom(30,90);//*fpsMod;

	if(id<0) id=ofRandom(pp.size());

	MWParticle* pt=pp.at(id);
	pp.erase(pp.begin()+id);

	pt->state=state;
	pt->stateCnt=ofRandom(60,180);//*fpsMod;

	pt->p->makeFixed();

	if(pt->state>0) {
		pt->vD=ofVec3f(0,ofRandom(0.1f,0.2f)*clothWidth,0)*0.5f;
		if(pt->p->getPosition().y>0) pt->vD.y*=-1;

		ofVec3f axis=ofVec3f(1,0,0);
		pt->vD.rotate(ofRandom(-30,30), axis);
		axis.set(0,1,0);
		pt->vD.rotate(ofRandom(-30,30), axis);
		pt->vD/=pt->stateCnt;
		pt->state=state;
	}

	ppActive.push_back(pt);

	printf("%d mwNewActivity - pp %d ppActive %d\n",
		ofGetFrameNum(),pp.size(),ppActive.size());

}

float CloudsVisualSystemVerletForm::rndExtend () {
  int r=ofRandom(0.8f,1.0f)*springExtend;
  return r*0.5f*((ofRandom(100)>50 ? clothWidth : clothHeight)/gridSize) ;
}

ofVec3f axis=ofVec3f(0,1,0);

void CloudsVisualSystemVerletForm::mwMakeParticle(Particle3D* pt,int x,int y) {
	MWParticle* ppt=new MWParticle();
	ppt->p=pt;
	ppt->gridx=x;
	ppt->gridy=y;

	pp.push_back(ppt);
}

void CloudsVisualSystemVerletForm::mwFix(MWParticle &pt) {
	if(pt.p->isFixed()) {
		pt.p->makeFree();
	}
	else pt.p->makeFixed();

	ofVec3f v=ofVec3f(ofRandom(1),ofRandom(1),0)*0.1f;
	mwGridPt(v,0);

	pt.p->moveBy(v);
}

void CloudsVisualSystemVerletForm::mwGridPt (ofVec3f &o,int type) {  // function:

	if(type==0) { // circular
		float a=o.y*360.0f;
		o.set(clothWidth*0.5f*(o.x+0.1f),0,o.z);
		o.rotate(a, axis);
	}
	else if(type>0) { // grid
		o.set(clothWidth*(o.x-0.5f),0,clothHeight*(o.y-0.5f));
	}
	o.y=ofRandom(0.1f,0.3f)*(ofRandom(100)<50 ? clothHeight : -clothHeight);

}

std::vector< std::vector<ofVec3f> > CloudsVisualSystemVerletForm::mwGrid(int type) {
	vector< vector<ofVec3f> > stack;

	for(int i=0; i<gridSize; i++) {
		vector<ofVec3f> vl;
		for(int j=0; j<gridSize; j++) {
			ofVec3f v=ofVec3f(j/(gridSize-1),i/(gridSize-1),0);
			mwGridPt(v,type);
			vl.push_back(v);
		}

		stack.push_back(vl);
	}

	return stack;
}

int CloudsVisualSystemVerletForm::mwGetRndID(int edge) {
	int mx=gridSize-1;
	MWParticle* pt;
	int id=-1;

	if(edge<0) {
		do {
			id=ofRandom(pp.size());
			printf("mwGetRndID(%d) - id=%d\n",edge,id);
			pt=pp.at(id);
			if(pt->gridx==0 || pt->gridx==mx ||
				pt->gridy==0 || pt->gridy==mx) id=-1;
		} while(id<0);
	}
	else {
		do {
			id=ofRandom(pp.size());
			printf("mwGetRndID(%d) - id=%d\n",edge,id);
			pt=pp.at(id);
			if(!(pt->gridx==0 || pt->gridx==mx ||
				pt->gridy==0 || pt->gridy==mx)) id=-1;
		} while(id<0);
	}

	return id;
}

void CloudsVisualSystemVerletForm::mwGridSticky() {
	int n=ofRandom(30,50);
	n=gridSize*gridSize;
	n=ofRandom(n/100,n/10);
	int nhalf=n/2;

	vector<int> dead;

	printf("%d mwGridSticky - pp %d ppActive %d | n=%d\n",
		ofGetFrameNum(),pp.size(),ppActive.size(),n);

	while(n>0) {
		int id=mwGetRndID(n>nhalf ? -1 : 1);
		mwNewActivity(id,-1);
		n--;
	}

	printf("%d mwGridSticky - pp %d ppActive %d\n",
		ofGetFrameNum(),pp.size(),ppActive.size());
}

//////////////////////////////////

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
	
	springStrength = .01;
	springDampening = 1;
	
	colorIndex=ofRandom(800);
	clothWidth = ofRandom(300,800);
	clothHeight = ofRandom(300,800);
	
	gridSize = ofRandom(35,60);

}

string CloudsVisualSystemVerletForm::getSystemName(){
	return "VerletForm";
}

void CloudsVisualSystemVerletForm::selfSetup(){
	fpsMod=1;
	currentRotAngle = 0;
	currentCamDistance = 100;
	
}

void CloudsVisualSystemVerletForm::generateMesh(){
	
	cout << "GENERATING MESH " << endl;

	colors = initColors( int(colorIndex) );

	clearElements();
	
	physics.setWorldSize(ofVec3f(-clothWidth*2,-clothWidth*2,-clothWidth*2),
						 ofVec3f(clothWidth*2,clothHeight*2,clothWidth*2));
	physics.setSectorCount(1);
	physics.setDrag(1.0);
	physics.setGravity(-0.001);
		

	ofMesh baseMesh;
	float gridStepX = (float) ((clothWidth) / gridSize);
	float gridStepY = (float) ((clothHeight) / gridSize);
		
	float ext=springExtend*2;
	float str=springStrength*0.1f;

	int gridType=(ofRandom(100)>49 ? 1 : 0);

	vector< vector<ofVec3f> > stack = mwGrid(gridType);
	//pp=vector<MWParticle>;

	for (int i = 0; i < gridSize; i++){
		vector<ofVec3f> vl=stack.at(i);

		particles.push_back(vector<Particle3D*>());
		
		for (int j = 0; j < gridSize; j++){

			//ofVec3f position = ofVec3f(j * gridStepX - (clothWidth / 2), 0.0, i * gridStepY - clothHeight/2) ;
			ofVec3f position=vl.at(j);

			Particle3D* particle = physics.makeParticle(position);
			particle->setMass(.2);
			particles[i].push_back( particle );

			mwMakeParticle(particle,i,j);
			
//			particleToMeshIndex[particle] = baseMesh.getNumVertices();
			meshIndexToParticle[baseMesh.getNumVertices()] = particle;
			
			baseMesh.addVertex( position );
			
			if (j > 0){
				physics.makeSpring( particles[i][j - 1], particles[i][j], str, rndExtend());
			}
		}
	}
	
	printf("generateMesh() - pp size %d\n", int(pp.size()) );

	for (int j = 0; j < gridSize; j++){
		for (int i = 1; i < gridSize; i++){
			physics.makeSpring(particles[i - 1][j], particles[i][j], str, rndExtend());
		}
	}

	// connect wrap-around edge
	//if(gridType>0) for (int j = 0; j < gridSize; j++){
	//	int y=gridSize-1;
	//	physics.makeSpring(particles[j][0], particles[j][y], str, rndExtend());
	//}

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

	mwGridSticky();

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
	clothGui->addSlider("SPRING EXTEND", 0.001, 5, &springExtend);

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
	
	float fpsModNow=ofGetFrameRate();
	fpsMod=fpsMod*0.9f+(fpsModNow/60.0f)*0.1f;
	
	activityCnt--;
	if(activityCnt<0) mwNewActivity(-1,1);
	
	mwUpdate();
	
	//compute the center of mesh
	ofVec3f meshCenter = mesh.getCentroid();
	
	//set the position to the middle plus some rotation around the center
	float camDistance = ofMap(GetCloudsInputY(), 0, getCanvasHeight(), 300, 700,true);
	float camAngle = ofMap(GetCloudsInputX(), 0, getCanvasWidth(), -180, 180,true);
	
	currentRotAngle += (camAngle - currentRotAngle)*.1;
	currentCamDistance += (camDistance - currentCamDistance)*.1;
	
	ofVec3f camTargetPosition  = meshCenter + ofVec3f(0,0,currentCamDistance).getRotated(currentRotAngle, ofVec3f(0,1,0));
	ofVec3f camCurrentPosition = cam.getPosition();
	cam.setPosition( camCurrentPosition + (camTargetPosition - camCurrentPosition) * .4);
	cam.lookAt(meshCenter);
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

	//MOVED YOUR STUFF TO UPDATE
	ofEnableAlphaBlending();
	
	int id=ofRandom(pp.size());

	//mwFix(pp.at(id));
	
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
//	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	mesh.draw();
	mat->end();
	
	for(int i = 0; i < 3; i++){
		if(auxLights[i].enabled) auxLights[i].light.disable();
	}
	
}

void CloudsVisualSystemVerletForm::selfExit(){
	clearElements();

}

void CloudsVisualSystemVerletForm::selfBegin(){
	shouldRegenerateMesh = true;
}

void CloudsVisualSystemVerletForm::selfEnd(){
}

void CloudsVisualSystemVerletForm::clearElements(){
	
	//memorieeesss....
	for(int i = 0; i < pp.size(); i++){
		delete pp[i];
	}
	pp.clear();
	for(int i = 0; i < ppActive.size(); i++){
		delete ppActive[i];
	}
	ppActive.clear();

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



