
#include "CloudsVisualSystemVerletForm.h"
#include <string>
#include <map>
#include <vector>

ofVec3f bMin,bMax,bCenter;
ofVec3f axisY=ofVec3f(0,1,0);
ofVec3f axisX=ofVec3f(1,0,0);

void CloudsVisualSystemVerletForm::mwPreset() {
	lastFixY=0;

	gridType=(int)(ofRandom(GRIDCYL+1));
//	gridType=(ofRandom(100)<50 ? GRIDCIRC : GRIDRECT);

//	gridType=GRIDCIRC;//GRIDRECT;
	gridSize = (int)ofRandom(35,60);
	gridSize = gridSizeF;

	colorIndex=ofRandom(900);

	printf("mwPreset - grid=%d gridType=%d col=%d\n",
			gridSize,gridType,
			(int)colorIndex);

	pp.clear();
	ppActive.clear();


	springStrength = ofRandom(1,4)/100.0f;
	springDampening = 0.1f;

	springExtend = ofRandom(0.5f,3);
	colorIndex=ofRandom(0,800);
	clothWidth = ofRandom(600,1000);
	clothHeight = ofRandom(0.4f,0.6f)*clothWidth;
	
	stickyNum=gridSize*gridSize;
	stickyNum=(int)(stickyNum*(ofRandom(0.5f,5)/100.0f));

	mwCreateLights();
}

void CloudsVisualSystemVerletForm::mwLights() {
	ofFloatColor black=ofFloatColor(0,0,0);
	ofSetGlobalAmbientColor(ofColor(0,0,0));
	ofSetSmoothLighting(false);

	for(int i = 0; i < 4; i++){
		AuxLight &a=auxLights[i];
		if(i>0) {
//			a.position.rotate(ofMap(i,0,3,0.1f,0.5f),axis);
			a.position.rotate(a.spinSpeed,a.spinAxis);
		}
		a.light.setPosition(a.position);
		a.light.setDiffuseColor(a.color);

		if(i>0) {
			a.light.setSpecularColor(auxLights[(i+1)%3+1].color);
			a.light.setAmbientColor(black);
		}
		else {
			a.light.setSpecularColor(a.color);
			a.light.setAmbientColor(black);
		}
		a.light.enable();

	}

}

void CloudsVisualSystemVerletForm::mwUpdate() {
	vector<int> dead;

	float fpsModNow=ofGetFrameRate();
	fpsMod=fpsMod*0.9f+(fpsModNow/60.0f)*0.1f;

	activityCnt--;
	//if(activityCnt<0 && ppActive.size()<100) mwNewActivity(-1,1);

	int nn=gridSize/5;
	for(int i=0; i<pp.size(); i+=nn) {
		MWParticle& pt=pp.at(i);
		ofVec3f pv=pt.p->getPosition();

		bMin.x=pv.x<bMin.x ? pv.x : bMin.x;
		bMin.y=pv.x<bMin.y ? pv.y : bMin.y;
		bMin.z=pv.x<bMin.z ? pv.z : bMin.z;

		bMax.x=pv.x>bMax.x ? pv.x : bMax.x;
		bMax.y=pv.x>bMax.y ? pv.y : bMax.y;
		bMax.z=pv.x>bMax.z ? pv.z : bMax.z;
	}

	ofVec3f c=bMin+bMax;
	c*=0.5;

	for(int i=0; i<ppActive.size(); i++) {
		MWParticle& pt=ppActive.at(i);
		pt.stateCnt--;

		if(pt.state==FIXEDMOVING) {
			pt.p->moveBy(pt.vD,true);
		}

		if(ofGetFrameNum()%1000==0 && i%50==0)
			printf("%d pt.stateCnt %d\n",
				i,pt.stateCnt);

		if(pt.stateCnt<0) {
			dead.push_back(i);
//			printf("DEAD: %d %d %d\n",i,pt.stateCnt,(pt.p->isFixed() ? 1 : 0));
//			if(pt.p->isFixed()) pt.p->makeFree();
//			else pt.p->makeFixed();
		}
	}

	if(ofGetFrameNum()%100==0 || dead.size()>0) {
		printf("dead %d pp %d ppActive %d >> ",
			dead.size(),pp.size(),ppActive.size());
	}

	while(dead.size()>0 && ppActive.size()>10) {
		int id=dead.back();
		dead.pop_back();
		MWParticle& pt=ppActive.at(id);
		ppActive.erase(ppActive.begin()+id);
		pp.push_back(pt);
	}

	if(ofGetFrameNum()%1000==0) {
		printf("pp %d ppActive %d\n",pp.size(),ppActive.size());
	}
}

ofVec3f CloudsVisualSystemVerletForm::mwOutlineShape(ofVec3f &v) {
	float a=0;

	if(gridType<GRIDCYL) {
		a=atan2(-v.z,v.x)/PI+1;
		a=ofNoise(a*10)*2.5f+0.3f;
		return v*a;
	}
	else {
		a=v.x/clothWidth;
		a=ofNoise(a*10);
		a=a*a*1.5f+0.1f;
		v.set(v.x,v.y*a,v.z*a);
		return v;
	}

	return v;
}

MWParticle & CloudsVisualSystemVerletForm::mwGetParticle(signed int id,bool fromEdge,bool addToActive) {
	MWParticle& pt=pp.at(0);

	if(id>-1) {
		printf("mwGetParticle id=%d/%d\n",id,pp.size());
		pt=pp[id];
	}
	else {
		while(id<0) {
			id=(int)ofRandom(pp.size());
			pt=pp[id];

			if((fromEdge && pt.isEdge) || (!fromEdge && pt.isEdge))
				id=-1;
		}

		pt=pp[id];
	}

	if(addToActive) {
		pp.erase(pp.begin()+id);
		ppActive.push_back(pt);
	}

	return pt;
}

void CloudsVisualSystemVerletForm::mwNewActivity(MWParticle& pt,signed int state) {
	activityCnt=ofRandom(150,500);//*fpsMod;

	if(state==FREE) {
		pt.p->makeFree();
		pt.vD.set(0,0,0);
		pt.stateCnt=ofRandom(60,180)*25;//*fpsMod;
	}
	else if(state==FIXEDSTATIC) {
		pt.p->makeFixed();

		float y=ofRandom(0.1f,0.25f)*clothHeight;
		if(ofRandom(100)>80) y*=2;

		if(gridType<GRIDCYL) {
			if(pt.isEdge && ofRandom(100)>10) y=-y;
			else if(!pt.isEdge && ofRandom(100)>70) y=-y*0.5f;
			pt.orig.set(
					pt.p->getPosition().x,
					y,
					pt.p->getPosition().z);
		}
		else {
			y/=clothHeight;
			y=(ofRandom(100)>70 ? 1-y : 1+y);

			pt.orig.set(
					pt.p->getPosition().x,
					pt.p->getPosition().y*y,
					pt.p->getPosition().z*y);
		}

		y=y*0.5f+lastFixY*0.5f;
		lastFixY=y;

		if(pt.isEdge) {
			pt.orig*=ofRandom(1,1.5f);
			pt.orig.rotate(ofRandom(-30,30),axisY);
		}

		pt.p->moveTo(pt.orig,true);

		pt.vD.set(0,0,0);
		pt.stateCnt=ofRandom(60,180)*50;//*fpsMod;
	}
	else {
		pt.stateCnt=ofRandom(60,180)*5;//*fpsMod;
		pt.p->makeFixed();

		pt.vD=ofVec3f(0,ofRandom(0.1f,0.2f)*clothWidth,0)*0.5f;
		if(pt.p->getPosition().y>0) pt.vD.y*=-1;

		pt.vD.rotate(ofRandom(-30,30), axisY);
		axisY.set(0,1,0);
		pt.vD.rotate(ofRandom(-30,30), axisY);

		pt.vD/=pt.stateCnt;
	}

	pt.state=state;
	printf("%d mwNewActivity - pp %d ppActive %d\n",
		ofGetFrameNum(),pp.size(),ppActive.size());

}

void CloudsVisualSystemVerletForm::mwMakeParticle(int x,int y,ofVec3f &o) {
	MWParticle ppt;
	ppt.id=(int)gridSize;
	ppt.id=ppt.id*y+x;

	ppt.gridx=x;
	ppt.gridy=y;

//	if(gridType<GRIDCYL) //&& ofRandom(100)>30)
		o=mwOutlineShape(o);

	ppt.orig=ofVec3f(o.x,o.y,o.z);
	ppt.state=FREE;
	ppt.vD=ofVec3f(0,0,0);

	ppt.isEdge=((ppt.gridx==0 || ppt.gridx==gridSize-1 ||
			ppt.gridy==0 || ppt.gridy==gridSize-1) ? true : false);

	pp.push_back(ppt);

	if(pp.size()%gridSize==0) printf("%d %d | %d / %d | mwMakeParticle id=%d x=%d i=%d |  %1.2f, %1.2f, %1.2f \n",
		x,ppt.gridx,
		pp.size(),gridSize,
		ppt.id,x,y,
		ppt.orig.x,ppt.orig.y,ppt.orig.z);
}

void CloudsVisualSystemVerletForm::mwCreateLights() {
	printf("========\nmwCreateLights pp=%d / gridSize=%d\n",
		pp.size(),gridSize);

	ofColor c[]= {
			ofColor(255,255,255),
			ofColor(255,255,255),
			ofColor(255,255,255)
	};

	for(int i = 0; i < 4; i++){
		ofVec3f v=ofVec3f(
				ofRandom(0.5f, 0.8f)*2*clothWidth,
				-ofRandom(0.25, 0.5)*clothHeight,
				0);
		v.rotate(ofRandom(-180,180), axisY);

		auxLights[i].position=v;
		auxLights[i].enabled=true;

		ofColor cc=ofColor(colors.at(ofRandom(colors.size()-1)));
		int b=cc.getBrightness()*ofRandom(1.0f,1.5f)*0.75f;
		cc.setBrightness(b>255 ? 255 : b);
		cc.setSaturation(cc.getSaturation()*ofRandom(0.75f,1.25f));

		auxLights[i].color=ofFloatColor(cc);

		if(i==0) {
			auxLights[i].position.set(0,clothHeight*5,0);
			auxLights[i].light.setDirectional();
			auxLights[i].color=ofFloatColor(ofColor(255, 0, 0));
			auxLights[i].light.setDiffuseColor(auxLights[i].color);
			auxLights[i].light.setSpecularColor(ofFloatColor(1,1,1));
			auxLights[i].light.setOrientation( ofVec3f(0, -0.8f, 0.2f) );
		}
		else {
			if(i==2) v.y*=-1;
			auxLights[i].light.setPointLight();
			auxLights[i].light.setPosition(auxLights[i].position);

			ofVec3f spinAxis=ofVec3f(
					ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1));
			auxLights[i].spinAxis=spinAxis.normalize();
			auxLights[i].spinSpeed=ofRandom(0.2f,1);
		}



//		  lCol1=UColorTool.adjustBrightness(
//		    UColorTool.adjustSaturation(getColor(),0.5f), random(1,1.5f));
	}

}

void CloudsVisualSystemVerletForm::mwGenerate() {
	printf("========\nmwGenerate pp=%d / gridSize=%d / gridType=%d\n ",
		pp.size(),gridSize,gridType);

	axisY.set(0,1,0);

	float clothWidthHalf=clothWidth*0.5f;

	float degRange=ofRandom(0.6f,1);

	for(int i=0; i<gridSize; i++) {
		degRange=ofRandom(0.6f,1)*0.2f+degRange*0.8f;

		for(int j=0; j<gridSize; j++) {

			float tx=ofMap(i,0,gridSize-1, 0,clothWidth);
			float ty=ofRandom(0.25f,0.5f)*clothHeight;
			float tz=ofMap(j,0,gridSize-1, 0,1);

			ofVec3f v=ofVec3f(tx,ty,tz);


			if(gridType==GRIDCIRC) { // circular
				float a=(tz-0.5f)*360.0f;
				v.set(tx*0.95+0.05f*clothWidth,ty,0);
				v.rotate(a, axisY);
			}
			else if(gridType==GRIDCYL) { // circular
				float a=(tz-0.5f)*360.0f*degRange;
				v.set(
						-(tx-clothWidthHalf)*1.5f,
						ty+0.1f*clothHeight,
						ty+0.1f*clothHeight);//tz*clothWidth);

				v.rotate(-a, axisX);
			}
			else if(gridType==GRIDRECT) { // grid
				v.set(tx-clothWidthHalf,
						ty,
						tz*clothWidth-clothWidthHalf);
				v.rotate(0,axisY);
			}
//					(ofRandom(100)<50 ? 1 : -1);
			mwMakeParticle(i,j,v);
		}
	}

	printf("========\nmwGenerate pp=%d / gridSize=%d / gridType=%d\n ",
		pp.size(),gridSize,gridType);
}

void CloudsVisualSystemVerletForm::mwGridSticky() {
	int n=ofRandom(30,50);
	n=stickyNum;
	int nhalf=ofRandom(n/2,n*0.9f);

	if(gridType==GRIDRECT) {
		int corners[] = {0, gridSize-1, 
			pp.size()/2,
			pp.size()-1, pp.size()-(gridSize)};

		for(int i=0; i<5; i++) if(ofRandom(100)>50) {
			MWParticle &pt=mwGetParticle(corners[i],false,false);
			mwNewActivity(pt,FIXEDSTATIC);

		}
	}

	while((n--)>0) {
		bool onEdge=(n>nhalf);
		mwNewActivity(mwGetParticle(-1,onEdge,false),FIXEDSTATIC);

		printf("%d mwGridSticky - pp %d ppActive %d | n=%d onEdge=%d\n",
		ofGetFrameNum(),pp.size(),ppActive.size(),n,
		(onEdge ? 1 : 0));

//		pp.at(id).p->makeFixed();
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
}

string CloudsVisualSystemVerletForm::getSystemName(){
	return "VerletForm";
}

void CloudsVisualSystemVerletForm::selfSetup(){
	fpsMod=1;
	for(int i=0; i<4; i++) {
		auxLights[i].light=ofLight();
	}
}

void CloudsVisualSystemVerletForm::generateMesh(){
	
	cout << "GENERATING MESH " << endl;

	colors = initColors( int(colorIndex) );

	mwPreset();

	bMin=ofVec3f(0,0,0);
	bMax=ofVec3f(0,0,0);

	clearElements();
	
	physics.setWorldSize(
		ofVec3f(-clothWidth*2,-clothWidth*2,-clothWidth*2),
		ofVec3f(clothWidth*2,clothHeight*2,clothWidth*2));
	physics.setSectorCount(1);
	physics.setDrag(1.0);
	physics.setGravity(-0.001);
		

	ofMesh baseMesh;
	float ext=springExtend;
	float str=springStrength*0.1f;

	mwGenerate();

	int id=0;

	for (int i = 0; i < gridSize; i++){

		particles.push_back(vector<Particle3D*>());
		
		for (int j = 0; j < gridSize; j++){
			MWParticle &theParticle=pp[id];

			Particle3D* particle = physics.makeParticle(theParticle.orig);
			particle->setMass(.2);
			particles[i].push_back( particle );
			pp[id++].p=particle;
			
//			particleToMeshIndex[particle] = baseMesh.getNumVertices();
			meshIndexToParticle[baseMesh.getNumVertices()] = particle;
			
			baseMesh.addVertex( particle->getPosition() );
			
		}
	}
	


	printf("generateMesh() - pp size %d grid=%d\n",pp.size(),(int)gridSize);

	for (int j = 0; j < gridSize; j++){
		for (int i = 0; i < gridSize; i++){
			if (i > 0) {
				physics.makeSpring(
						particles[i - 1][j],
						particles[i][j], str,
						ext*ofRandom(0.5f,1.25f));
			}
			if (j > 0){
				physics.makeSpring(
						particles[i][j - 1],
						particles[i][j], str,
						ext*ofRandom(0.5f,1.25f));
			}
		}
	}

	if(gridType==GRIDCYL) {
		int y1=gridSize-1,y2=0;

		for (int i = 0; i < gridSize; i++){
//						physics.makeSpring(
//								particles[i][y2],
//								particles[i][y1], str,
//								ext*ofRandom(0.5f,1.25f));
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
	
	//clothGui->addSlider("COLOR INDEX", 0, 909, &colorIndex);
	
	clothGui->addSlider("CLOTH WIDTH", 10, 1000, &clothWidth);
	clothGui->addSlider("CLOTH HEIGHT", 10, 1000, &clothHeight);
	clothGui->addSlider("GRID SIZE", 10, 100, &gridSizeF);
	clothGui->addSlider("STICKY", 10, 100, &stickyNum);

	clothGui->addLabel("SPRINGS");
	clothGui->addSlider("SPRING STRENGTH", .001, 1.0, &springStrength);
	clothGui->addSlider("SPRING EXTEND", 0.001, 5, &springExtend);

	clothGui->addButton("REGENERATE", &shouldRegenerateMesh);

	ofAddListener(clothGui->newGUIEvent, this, &CloudsVisualSystemVerletForm::selfGuiEvent);
	
    guis.push_back(clothGui);
    guimap[clothGui->getName()] = clothGui;

/*
	
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
*/
}

void CloudsVisualSystemVerletForm::selfPresetLoaded(string presetPath){
	shouldRegenerateMesh = true;
}

void CloudsVisualSystemVerletForm::selfUpdate(){
	//mwUpdate();

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
	
	mwLights();
	
	mat->begin();
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	mesh.draw();
	mat->end();

	for(int i = 0; i < 4; i++){
		auxLights[i].light.disable();
	}

/*
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
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	mesh.draw();
	mat->end();
	
	for(int i = 0; i < 3; i++){
		if(auxLights[i].enabled) auxLights[i].light.disable();
	}
*/
	
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
	pp.clear();
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



