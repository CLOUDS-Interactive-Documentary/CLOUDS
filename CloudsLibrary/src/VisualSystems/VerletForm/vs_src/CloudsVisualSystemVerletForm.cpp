
#include "CloudsVisualSystemVerletForm.h"
#include <string>
#include <map>
#include <vector>

/**
 * - Fix geometry (GRIDRECT, GRIDCYL)
 * - Use normals for new activity
 */

ofVec3f axisY=ofVec3f(0,1,0);
ofVec3f axisX=ofVec3f(1,0,0);
ofVec3f axisZ=ofVec3f(0,0,1);
MWTerrain terrain;
float radMin=0.1f;
int fixChance=40;

float lastNMod=0.5f;
float lastTMod=1.f;

float camCenterOffsD=0;

void CloudsVisualSystemVerletForm::mwPreset() {
	clothWidth =600;
	clothHeight=clothWidth*0.6666;
	currentCamDistance = 1500;

	int type=(int)(ofRandom(GRIDCYL+1));
	while(type==gridType) type=(int)(ofRandom(GRIDCYL+1));
	gridType=type;
	gridTypeF=gridType;
	gridDoStitch=(gridType!=GRIDRECT && ofRandom(100)>30);

	if(doLoRes)
		gridSize = (int)(ofRandom(8,13)*(ofGetFrameNum()>20 && fpsMod<1 ? fpsMod : 1))*4;
	else gridSize = (int)(ofRandom(12,20)*(ofGetFrameNum()>20 && fpsMod<1 ? fpsMod : 1))*4;
	gridSizeF=gridSize;

	colorIndex=ofRandom(0,900);
	colorStrategy=(colorStrategy+1)%3;
	colorStrategyF=colorStrategy;

	colorMod=ofRandom(2.f,15.f);
	if(rndBool(80)) colorMod=ofRandom(2.f,5.f);

	springStrength = ofRandom(1,6)/100.0f;
	springExtend = ofRandom(0.5f,1.5f);
	if(rndBool(80)) springExtend=ofRandom(2.f,5.f);


	stickyNum=gridSize*gridSize;
	stickyNum=(int)(stickyNum*(ofRandom(0.2f,2.5f)/100.0f));
	stickyNum=(stickyNum<10 ? 10 : stickyNum);
	stickyNum=(stickyNum>50 ? 50 : stickyNum);

	activityMod=ofRandom(0.5f,1);
	if(gridType==GRIDRECT) activityMod=ofRandom(0.25f,0.4f);

	vnormalMod=ofRandom(0.9f,1.5f);

	lineMesh=false;
//	} 
//	else {
//		colorIndex=(int)colorIndex;
//		gridType=(int)gridTypeF;
//		gridSize=(int)gridSizeF;
//		colorStrategy=(int)colorStrategyF;
//	}

	bbMult=0.5f;

	printf("mwPreset - grid=%d gridType=%d col=%d\n",
			gridSize,gridType,
			(int)colorIndex);

	modelRot=ofVec3f(ofRandom(360),ofRandom(360),ofRandom(360));

	for(int i=0; i<3; i++) modelRotD[i]=rndSigned(5,15)+90;


	camCenterOffs=ofVec3f(ofRandom(0.05f,0.15f)*clothWidth,0,0);
//	camCenterOffs.rotate(ofRandom(360),axisZ);
	
	fixChance=(int)ofRandom(20,60);

	mwCreateLights();

	terrain.type=(int)ofRandom(3);
	for(int i=0; i<7; i++) terrain.h[i]=rndSigned(0.2f,0.5f);
	terrain.h[0]=(ofRandom(100)>50 ? 1-terrain.h[0] : terrain.h[0]*0.25f);
	terrain.h[1]=(terrain.h[0] < 0.5f ? 1-terrain.h[1] : terrain.h[1]*0.25f);

	terrain.h[6]=(terrain.h[0]<0.5f ? 1-terrain.h[6] : terrain.h[0]*0.25f);

	terrain.h[2]=-abs(terrain.h[2]);
	terrain.h[4]=-abs(terrain.h[4]);
}

void CloudsVisualSystemVerletForm::mwFix(MWParticle &pt,bool fix) {
	if(!fix && pt.p->isFixed()) {
		pt.p->makeFree();
		fixCnt--;
	}
	else if(fix && !pt.p->isFixed()) {
		pt.p->makeFixed();
		fixCnt++;
	}

//	if(MWDEBUG && ofGetFrameNum()%50==0) {
//		printf("mwFix - fixCnt %d/%d\n",fixCnt,pp.size());
//	}
}


ofVec3f CloudsVisualSystemVerletForm::terrainMod(ofVec3f &v) {
	float a=0,d=0;


	if(gridType<GRIDCYL) {
		a=(atan2(-v.z,v.x)/PI+1)*0.5f;

//		d=sqrt(v.x*v.x+v.z*v.z)/(clothWidth);
		d=bezierPoint(terrain.h,a);

		a=2*ofNoise(5*d);
		v.z*=1+a*0.2f;;
		a=1+a;
		v.x*=a;
		v.y*=a;


		v.y=1+ofNoise(d*5);
		v.y*=clothHeight;
	}
	else {
		a=v.x/clothWidth;
		a=(atan2(-v.z,v.x)/PI+1)*0.5f;
		d=bezierPoint(terrain.h,a);
		float sineMod=sin(a*HALF_PI+colorIndex/900.f*HALF_PI*0.5f)+0.15f;
		float b=atan2(-v.z,v.y);
		a=ofNoise(a*25,b*25)*sineMod;
		a=a*a*2.5f+0.75f;
		v.set(v.x*a,v.y*a,v.z);


//		v.rotate(90,axisX);
		return v;
	}

	return v;

}

bool CloudsVisualSystemVerletForm::rndBool(float prob) {
	return ofRandom(100)>prob;
}

ofColor CloudsVisualSystemVerletForm::rndColor() {
	return colors[(int)ofRandom(colors.size()-1)];
}

	float CloudsVisualSystemVerletForm::rndSigned(float a,float b) {
		float v=ofRandom(a,b);
		return ofRandom(100)<50 ? v : -v;
	}


float CloudsVisualSystemVerletForm::wraplerp(float a, float b, float t, float w) {
	a += (abs(b-a) > w*0.5f) ? ((a < b) ? w : -w) : 0;
	return ofLerp(a, b, t);
}


 float CloudsVisualSystemVerletForm::bezierPoint(float a[], float t) {
	 if(t<0.5f) {
			t=bezierPoint(a[0],a[1],a[2],a[3],t*2);
	}
	else {
		t=ofClamp((t-0.5f)*2,0,1);
		t=bezierPoint(a[3],a[4],a[5],a[6],t);
	}

	 return t;
 }

  float CloudsVisualSystemVerletForm::bezierPoint(float a, float b, float c, float d, float t) {
    float t1 = 1.0f - t;
    return a*t1*t1*t1 + 3*b*t*t1*t1 + 3*c*t*t*t1 + d*t*t*t;
  }

ofVec3f CloudsVisualSystemVerletForm::mwNewMove(MWParticle& pt) {
	ofVec3f v=ofVec3f(0,0,0);
	return v;
}



void CloudsVisualSystemVerletForm::mwLights() {
	ofSetGlobalAmbientColor(ofColor(0,0,0));
	ofSetSmoothLighting(true);

	for(int i = 0; i < LIGHTS; i++) {
		AuxLight &a=auxLights[i];
		ofColor c=(colorLightEnabled || camEnabled? a.color : cGrayRGB);

		if(colorLightEnabled || camEnabled) {
			
			a.cnt++;

			if(a.cnt>-1) {
				float t=ofMap(a.cnt, 0,a.cntGoal, 0,1);
				c=ofColor(c).lerp(a.colorGoal,t);
				if(a.cnt==a.cntGoal) mwNewLightColor(a);
			}

			if(i==0) {
				a.light.setPosition(meshCentroid+a.position);
//				a.light.setPosition(bbC);//cam.getPosition());
			}
			else {
				a.position.rotate(a.spinAxis.y/fpsMod,axisY);
				a.position.rotate(a.spinAxis.z/fpsMod,axisZ);
				a.position.rotate(a.spinAxis.x/fpsMod,axisX);
				ofVec3f pos=(meshCentroid+a.position);

				a.light.setPosition(a.position);
				a.light.lookAt(meshCentroid,axisY);
			}
		}

		if(i>0) {
			ofColor cf=ofColor(c);
//			if(i==1) cf=ofColor(0,255,0,0);
//			if(i==2) cf=ofColor(0,0,255);
//			if(i==3) cf=ofColor(255,0,0);

			float b=cf.getBrightness();
			ofColor cfs=ofColor(cf);
			cfs.setBrightness(min(255.f,c.getBrightness()*1.5f));
			cf.setBrightness(b*0.75f);

			a.light.setDiffuseColor(cf);
			a.light.setSpecularColor(cfs);
			a.light.setAmbientColor(ofColor(5,5,5));
		}

		a.light.enable();
		//ofSetColor(a.light.getSpecularColor());
		//a.light.draw();

	}

} 


void CloudsVisualSystemVerletForm::mwUpdateCamera() {
	//compute the center of mesh
	mwMeshBB();
	camC.set(bbC.x,bbC.y,bbC.z);

	if(!cursorIsOverGUI()) {

		ofVec3f pos=ofVec3f(
				GetCloudsInputX()-getCanvasWidth()/2,
				GetCloudsInputY()-getCanvasHeight()/2,0);
//		pos=ofVec3f(
//						getCanvasWidth()/2,
//						getCanvasHeight()/2,0);
		mousePos.set(pos);
	}

//	mousePos.set(0,0);

//	bbC.z+=modelTrans;

	float camDistance =
			ofMap(mousePos.length(), 0, getCanvasWidth()*0.7f,
					800, 100,true);

	float m=ofMap(bbDim,clothWidth*0.5f,clothWidth*3, 0,1,true);
	bbMult+=(m-bbMult)*0.1f;

	modelTrans+=(ofMap(bbMult, 0,1, -400,400)-modelTrans)*0.1f;

	if(colorLightEnabled) camDistance+=modelTrans+bbMultTmp;

	currentCamDistance += (camDistance - currentCamDistance)*.01;

	float camAngle = ofMap(mousePos.y, 0, getCanvasHeight(), 0, 360,true);
//	float camRotMod=camDistance/800.f;
//	camAngle=atan2(-(mousePos.y-getCanvasHeight()/2),mousePos.x-getCanvasWidth()/2);
//	camAngle*=camRotMod;

	currentRotAngle.x=wraplerp(currentRotAngle.x,camAngle,0.01f,360);

	camAngle = ofMap(mousePos.x, 0, getCanvasWidth(), 0, 360,true);
//	camAngle=-camAngle;
	currentRotAngle.y=wraplerp(currentRotAngle.y,camAngle,0.01f,360);

	ofVec3f camCurrentPosition = cam.getPosition();

	ofVec3f camTargetPosition  = bbC +
			(ofVec3f(0.1f,0.1f,1)*currentCamDistance);
			//.getRotated(
			//		currentRotAngle.x+modelRot.x,
			//		currentRotAngle.y+modelRot.y,modelRot.z);

	cam.setPosition( camCurrentPosition +
			(camTargetPosition - camCurrentPosition) * .025);

	cam.lookAt(camC);

	modelRotMax=0.2f/fpsMod;
	float min=modelRotMax*0.005f;
	float max=modelRotMax*0.015f;

	for(int i=0; i<3; i++) {
		int val=(int)(i==0 ? modelRot.x :
				(i==1 ? modelRot.y : modelRot.z));

		float sign=(modelRotD[i]<0 ? -1 : 1);

		if(val<55) modelRotD[i]+=max;
		else if(val<305) modelRotD[i]-=max;
		else modelRotD[i]=ofClamp(
				modelRotD[i]+rndSigned(min,max),
				-modelRotMax,modelRotMax);

//		modelRotD[i]=(modelRotD[i]>modelRotMax ? modelRotMax :
//				(modelRotD[i]<-modelRotMax ? -modelRotMax : modelRotD[i]));
	}

	modelRot+=ofVec3f(modelRotD[0],modelRotD[1],modelRotD[2]);
}

void CloudsVisualSystemVerletForm::mwMeshBB() {
	vector<ofVec3f> vl=mesh.getVertices();
	ofVec3f bb[] = {ofVec3f(),ofVec3f(),ofVec3f()};
	ofVec3f v;

	bbMin=ofVec3f(50000,50000,50000);
	bbMax=ofVec3f(-50000,-50000,-50000);

	int step=4,cnt=0,mxStep;
	float w=clothWidth*1.5f;
	mxStep=vl.size()-gridSize;

	if(vl.size() > 0) {
		for(unsigned int i = 0; i < vl.size(); i+=step) {
			v=vl[i];
//			if(abs(bbC.x-v.x)>w ||
//				abs(bbC.y-v.y)>w ||
//				abs(bbC.z-v.z)>w) {
//			}
//			else {
				cnt++;

				if(i>0 && i%gridSize==0) v=vl[i-1];

				bbMin.set(
						min(bbMin.x,v.x),
						min(bbMin.y,v.y),
						min(bbMin.z,v.z));
				bbMax.set(
						max(bbMax.x,v.x),
						max(bbMax.y,v.y),
						max(bbMax.z,v.z));
//			}
		}

		ofVec3f D=bbMax-bbMin;


		float dim;
//		dim=abs(bbMax.x-bbMin.x);
//		dim+=abs(bbMax.y-bbMin.y);
//		dim+=abs(bbMax.z-bbMin.z);

		bbDim=(D.x+D.y+D.z)/3.f;
		double len=D.length();

		meshCentroid=mesh.getCentroid();
//		bbC=bbC+(((bbMax+bbMin)*0.5f-bbC)*0.01f);
		bbC=bbC+(meshCentroid-bbC)*0.1f;
	}

	char buffer [50];

//	bbC=mesh.getCentroid();
//	Particle3D* ppt=pp[pp.size()/2].p;
//	bbC.set(ppt->getPosition().x,ppt->getPosition().y,ppt->getPosition().z);

	sprintf (buffer, "%d %d | %.2f | %d",
			(int)bbDim,(int)currentCamDistance,
			bbMult,(int)modelTrans);
	bbLabel[0]->setLabel(buffer);

	sprintf (buffer, "%d %d %d",
			(int)bbC.x,(int)bbC.y,(int)bbC.z);
	bbLabel[1]->setLabel(buffer);

	sprintf (buffer, "%d %d %d",
			(int)camC.x,(int)camC.y,(int)camC.z);
	bbLabel[2]->setLabel(buffer);

	sprintf (buffer, "d %d | mt %d",
			(int)currentCamDistance,(int)modelTrans);
	bbLabel[3]->setLabel(buffer);

	if(MWDEBUG && ofGetFrameNum()%200==0) {
		printf("mwMeshBB %d %d %d | %d %d %d | %d %d %d\n",
				(int)bbC.x,(int)bbC.y,(int)bbC.z,
				(int)bbMin.x,(int)bbMin.y,(int)bbMin.z,
				(int)bbMax.x,(int)bbMax.y,(int)bbMax.z);
	}
	clothGui->update();
}

void CloudsVisualSystemVerletForm::mwUpdate() {
	if(doAutoGenerate) {
		if(ofGetFrameNum()>100000) exit();
		else if(ofGetFrameNum()>100 && ofGetFrameNum()%2500==0) shouldRegenerateMesh=true;
	}


	float fpsModNow=ofGetFrameRate()/60.0f;
	fpsMod=ofClamp(
			fpsMod*0.95f+(fpsModNow*0.05f),
			0.25f,2.5f);

	if(ofGetFrameNum()%500==0 && MWDEBUG) printf("%d fpsMod %1.2f | fixCnt=%d | activityCnt=%d | drag=%.4f\n",
			ofGetFrameNum(),fpsMod,
			fixCnt,(int)activityCnt,
			(100.f-fpsMod*0.4f)/100.f);

//	physics.setDrag((100.f-fpsMod*0.4f)/100.f);
	gravCnt++;
	if(gravCnt>-1) {
		if(gravCnt==gravCntGoal) mwNewGravity();
		else {
			float t=ofMap(gravCnt,0,gravCntGoal, 0,1);
			ofVec3f v=ofVec3f(grav);
			v.interpolate(gravGoal,sin(t*HALF_PI));
			physics.setGravity(v);
		}
	}

	activityCnt--;

	if(ofGetFrameNum()%20==0 && fixCnt>20) {
			int freed=0;
			int cnt=fixCnt/10;//ofRandom(5,fixCnt/2);
			while((cnt--)>0) {
				MWParticle& pt=mwGetParticle(false);
				if(pt.p->isFixed()) {
					mwFix(pt,false);
					freed++;
					pt.stateCnt=0;
					pt.state=FREE;
				}
			}

			if(freed>0 && MWDEBUG)
				printf("%d | fixCnt=%d FREED=%d\n",
										ofGetFrameNum(),fixCnt,freed);
	}

	if(activityCnt<0) {
		int cnt=ofRandom(3,7);
		int ptMin=pp.size()/10;


		cnt=ppActive.size()<ptMin ? ofRandom(3,7) *
				(ppActive.size()<ptMin/5 ? 3 : 1) : 2;

		fixChance=10+(fixCnt/2);
		fixChance=(fixChance>90 ? 90 : fixChance);

		bool free=rndBool(100-fixChance);// || ppActive.size()>ptMin;

		while((cnt--)>0) {
			int tries=0;

			MWParticle& pt=mwGetParticle(ofRandom(100)>70);
			while(free && pt.p->isFree() && (tries++)<100) {
				pt=mwGetParticle(false);
			}

			mwNewActivity(pt,free ? FREE :
					(ofRandom(100)>70 ? FIXEDMOVING : FIXEDSTATIC));
		}

		if(MWDEBUG) {
				MWParticle& pt=ppActive.at(ofClamp((int)ofRandom(10)*10,0,ppActive.size()-1));
				ofVec3f gv=ofVec3f(physics.getGravity().x,
					physics.getGravity().y,
					physics.getGravity().z);
				float g=gv.length();

				printf(
						"%d activityCnt=%d | id=%d stateCnt %d | pp=%d/%d/%d | %1.2f | grav=%.4f\n",
						ofGetFrameNum(),activityCnt,
						pt.id,pt.stateCnt,pp.size(),ppActive.size(),fixCnt,
						fpsMod,
						g);
			}

		activityCnt=ofRandom(20,50)*fpsMod*activityMod;
	}

	string s="";

	for(int i=0; i<ppActive.size(); i++) {

		MWParticle& pt=ppActive.at(i);
		pt.stateCnt--;
		if(pt.stateCnt>-1) {
			if(pt.stateCnt>0 && pt.state>FREE) {
				pt.vD.rotate(0.5f,axisX);
				if(pt.speed<1) {
					pt.speed+=0.02f;
					pt.p->moveBy(pt.vD*pt.speed,true);
				}
				else pt.p->moveBy(pt.vD,true);
			}
		}
		else if(pt.stateCnt<-50) {
			if(rndBool(fixChance)) {
				mwFix(pt,false);
			}
			else pt.stateCnt=20;

			int tmp=ppActive.size();

			if(tmp>0) {
				ppActive.erase(ppActive.begin()+i);
			}
		}
	}

	if(MWDEBUG && ofGetFrameNum()%500==0 && ppActive.size()>0) {
		MWParticle& pt=ppActive.at(ofClamp((int)ofRandom(10)*10,0,ppActive.size()-1));
		printf(
				"id=%d stateCnt %d | pp=%d/%d | %1.2f\n",
				pt.id,pt.stateCnt,pp.size(),ppActive.size(),fpsMod);
	}


	if(MWDEBUG && ofGetFrameNum()%500==0) {
		printf("pp %d/%d\n",pp.size(),ppActive.size());
	}
}

ofVec3f CloudsVisualSystemVerletForm::mwOutlineShape(ofVec3f &v) {
	float a=0,a2=0;


	if(gridType<GRIDCYL) {
		a=atan2(-v.z,v.x)/PI+1;
		a=ofNoise(a*10)*2.5f+0.3f;
		a2=(1+a)*0.5f;
		v.set(v.x*a2,v.y*a2,v.z*a);
//		v.y*=terrainMod(v.x,v.z);

	}
	else {
		a=v.x/clothWidth;
		float sineMod=sin(a*2*HALF_PI+colorIndex/900.f*HALF_PI)+0.15f;
		float b=atan2(-v.z,v.y);
		a=ofNoise(a*25,b*25)*sineMod;
		a=a*a*2.5f+0.1f;
		v.set(v.x*a,v.y*a,v.z);


//		v.rotate(90,axisX);
		return v;
	}

	return v;
}

int lastID=0;
int lastCnt=0;

MWParticle & CloudsVisualSystemVerletForm::mwGetParticle(bool fromEdge) {
	MWParticle& pt=pp.at(0);
	signed int id=-1,tries=0;

	int mx=6;

	if(lastID>0 && lastCnt>20) {
		lastID=0;
		lastCnt=0;
	}

	while(id<0 && (tries++)<500) {
		id=(int)ofRandom(pp.size());

		if(lastID>0) {
			int offsx=(int)rndSigned(1,mx);
			int offsy=(int)rndSigned(1,mx);

			offsx+=offsy*gridSize+id;
			if(offsx>-1 && offsx<pp.size()-1) id=offsx;
			else id=-1;
		}


		if(id>-1) {
			pt=pp[id];

			if(tries<500) {
				if((fromEdge && !pt.isEdge)) id=-1;

				if(id>-1) {
					for(int i=0; i<ppActive.size() && id>-1; i++) {
						if(ppActive[i].id==id) id=-1;
					}
				}
			}
		}

	}

	lastID=id;
	lastCnt++;

	return pt;
}

void CloudsVisualSystemVerletForm::mwNewActivity(MWParticle& pt,signed int state) {
	if(state==FREE && pt.p->isFixed()) {
		mwFix(pt,false);
		return;
	}
	else state=(rndBool(70) ? FIXEDMOVING : FIXEDSTATIC);

	pt.vD.set(0,0,0);

	ofVec3f vn=mesh.getNormal(particleToMeshIndices[pt.p][0]);
	pt.v=ofVec3f(
			pt.p->getPosition().x,
			pt.p->getPosition().y,
			pt.p->getPosition().z);

	pt.orig=pt.orig*0.8f+pt.v*0.2f;

	float len=pt.orig.length();
	if(len>clothWidth) {
		pt.orig=pt.orig.normalize()*clothWidth;
	}
	else if(len<radMin*2*clothWidth) {
		pt.orig=pt.orig.normalize()*radMin*2*clothWidth;
	}

	pt.state=state;

	lastTMod=lastTMod*0.9f+ofRandom(1,3)*0.1f;
	pt.stateCnt=(int)(ofRandom(60,120)*lastTMod*fpsMod);

	float val=ofRandom(1);
	lastNMod=lastNMod*0.9f+ofRandom(0.25f,1)*0.1f;

	ofVec3f vD2=ofVec3f(pt.orig);

	vD2=vn*(vnormalMod*ofMap(1-sin(lastNMod), 0,1, 0.1f,1)*
			(rndBool(70) ? -0.35f : 1)*
			clothWidth);
//	vD2=vn*(vnormalMod*ofMap(1-sin(HALF_PI*val*val), 0,1, 0.1f,0.11f+lastNMod)*
//			(rndBool(70) ? -0.35f : 1)*
//			clothWidth);

 //rndSigned(0.15f,0.4f)*clothHeight);
	vD2.rotate(rndSigned(2,10)*vnormalMod, axisX);
	vD2.rotate(rndSigned(2,10)*vnormalMod, axisY);

	pt.vD=pt.orig+vD2;

	len=pt.vD.length();
	if(len>clothWidth*1.25f) {
		len=(clothWidth*1.25f)/len;
		pt.vD=pt.orig+vD2*len;
	}

	mwFix(pt,true);


	pt.vD-=pt.v;
	if(state==FIXEDSTATIC) pt.vD*=0.1f;
	pt.vD/=pt.stateCnt;

	pt.speed=0;

	for(int i=0; i<ppActive.size(); i++) if(pt.id==ppActive[i].id) {
		return;
	}

	ppActive.push_back(pt);
	if(MWDEBUG && ppActive.size()>0 && ppActive.size()%25==0) printf("%d mwNewActivity - pp %d/%d state %d stateCnt=%d\n",
		ofGetFrameNum(),pp.size(),ppActive.size(),
		pt.state,pt.stateCnt);



}

void CloudsVisualSystemVerletForm::mwMakeParticle(int x,int y,ofVec3f &o) {
	MWParticle ppt;
	ppt.id=(int)gridSize;
	ppt.id=ppt.id*y+x;

	ppt.gridx=x;
	ppt.gridy=y;
	ppt.gridV=ofVec3f(x,y,0)/(float)(gridSize-1);
	ppt.v=ofVec3f(0,0,0);

//	if(gridType<GRIDCYL) //&& ofRandom(100)>30)
	o=mwOutlineShape(o);

	ppt.orig=ofVec3f(o.x,o.y,o.z);
	ppt.state=FREE;
	ppt.stateCnt=0;
	ppt.vD=ofVec3f(0,0,0);

	ppt.colID=int(ofNoise(
			colorMod*ppt.gridV.x,
			colorMod*ppt.gridV.y)*(colors.size()-1));

	ppt.isEdge=((ppt.gridx==0 || ppt.gridx==gridSize-1 ||
			ppt.gridy==0 || ppt.gridy==gridSize-1) ? true : false);
	if(gridDoStitch && gridType!=GRIDRECT &&
			(ppt.gridy==gridSize-1 || ppt.gridy==0)) ppt.isEdge=false;

	pp.push_back(ppt);

//	if(pp.size()%gridSize==0) printf("%d %d | %d / %d | mwMakeParticle id=%d x=%d i=%d |  %1.2f, %1.2f, %1.2f \n",
//		x,ppt.gridx,
//		pp.size(),gridSize,
//		ppt.id,x,y,
//		ppt.orig.x,ppt.orig.y,ppt.orig.z);
}

void CloudsVisualSystemVerletForm::mwCreateLights() {
	if(MWDEBUG) printf("========\nmwCreateLights pp=%d / gridSize=%d\n",
		pp.size(),gridSize);

	    ofVec3f xax(1, 0, 0);
	    ofVec3f yax(0, 1, 0);
	    ofVec3f zax(0, 0, 1);



	mat->setShininess(ofRandom(100,255));
	mat->setEmissiveColor(ofFloatColor(0,0,0));

	for(int i = 0; i < LIGHTS; i++){
		AuxLight &a=auxLights[i];

		a.position=ofVec3f(0,0,clothWidth*0.5f*(i!=0 ? -1.f : 1.f));
		if(i==0) {
			a.position.rotate(-ofRandom(15,30),axisX);
			a.position.rotate(ofRandom(-30,30),axisY);
			a.light.setDiffuseColor(ofColor(100,100,100));

			int ch=(int)ofRandom(999)%3;
			a.light.setSpecularColor(
					ofColor(ch==0?200:255, ch==1?200:255, ch==2?200:255));
			a.light.setAmbientColor(ofColor(0,0,0));
		}
		if(i==1) {
			a.position.rotate(-60,axisX);
			a.position.rotate(0,axisY);
		}
		if(i==2) {
			a.position.rotate(0,axisX);
			a.position.rotate(120,axisY);
		}
		if(i==3) {
			a.position.rotate(60,axisX);
			a.position.rotate(240,axisY);
		}

		a.enabled=true;

		if(i==2) a.color=ofColor(0,0,0);
		else a.color=ofColor(255,255,255);

		mwNewLightColor(a);
		mwNewLightColor(a);

		a.light.setPosition(bbC+a.position);
		if(i>0) {
			a.light.setDirectional();
			a.light.lookAt(bbC,axisY);
			//a.light.setAttenuation(0.1f,0,0);
		}
		else a.light.setPointLight();

//	    ofQuaternion q;
//	    ofVec3f rot=ofVec3f(0,90,0);
//	    q.makeRotate(rot.x, xax, rot.y, yax, rot.z, zax);
//
//        a.light.setOrientation(q);

		ofVec3f spinAxis=ofVec3f(
				ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1)*0.2f);

		a.spinSpeed=ofRandom(0.2f,1);
		a.spinAxis=spinAxis.normalize()*a.spinSpeed;

//		if(i>1) a.spinAxis=auxLights[1].spinAxis;
	}


	if(MWDEBUG) printf("========\nmwCreateLights DONE | pp=%d / gridSize=%d\n",
		pp.size(),gridSize);

}

void CloudsVisualSystemVerletForm::mwNewGravity() {
	gravCnt=-ofRandom(150,300)*fpsMod;
	gravCntGoal=ofRandom(150,300)*fpsMod;

	grav=gravGoal;
	physics.setGravity(grav);

	gravGoal=ofVec3f(0,0,ofRandom(0.001f,0.01f)/(fpsMod<1 ? 1 : fpsMod));
	if(grav.z>0) gravGoal.z=-gravGoal.z;

	gravGoal.rotate(rndSigned(2,15),axisX);
	gravGoal.rotate(rndSigned(2,15),axisY);
//	gravGoal.rotate(ofRandom(360),axisZ);

	if(MWDEBUG) printf("mwNewGravity %.3f,%.3f,%.3f | %.3f,%.3f,%.3f \n",
			grav.x,grav.y,grav.z,
			gravGoal.x,gravGoal.y,gravGoal.z
			);//,hexStr(a.color),hexStr(a.color));
}

void CloudsVisualSystemVerletForm::mwNewLightColor(AuxLight &a) {

	float prev=a.color.getSaturation();

	a.cnt=(int)(-ofRandom(150,350)*fpsMod);
	a.cntGoal=(int)(ofRandom(60,120)*fpsMod);

	ofColor cc=rndColor();
	cc.setBrightness(200);

	float sat=cc.getSaturation();

	if(a.light.getIsDirectional() &&
			(prev<150 && rndBool(30))) {
		sat=ofRandom(200,255);
		cc*=0.6f;
		shiftHue(cc,rndBool(50) ? -80 : 80);
		cc.setSaturation(sat);
	}
	else {
		sat=ofRandom(25,60);
		cc.set(sat,sat,sat);
	}


	a.color=a.colorGoal;
	a.colorGoal=cc;

	if(MWDEBUG) printf("mwNewLightColor cnt=%d | %d,%d,%d | %d,%d,%d\n",
				a.cnt,
				(int)a.color.r,(int)a.color.g,(int)a.color.b,
				(int)a.colorGoal.r,(int)a.colorGoal.g,(int)a.colorGoal.b);
}

void CloudsVisualSystemVerletForm::shiftHue(ofColor &cc,float mod) {
	float hue=cc.getHue()+mod;
	cc.setHue(hue>255 ? hue-255 : (hue<0 ? hue+255 : hue));
}

void CloudsVisualSystemVerletForm::mwGenerate() {
	printf("========\nmwGenerate pp=%d / gridSize=%d / gridType=%d\n ",
		pp.size(),gridSize,gridType);

	activityCnt=ofRandom(20,50)*fpsMod*activityMod;
	fixCnt=0;

	axisY.set(0,1,0);
	modelTrans=-800;
	bbMultTmp=0;

	float clothWidthHalf=clothWidth*0.5f;
	float hMod=1;
	float p1=0,p2=0,p3=0;

	int type=0;
	radMin=ofRandom(0.1f,0.15f);

	if(gridType==GRIDCIRC) {
		hMod=ofRandom(0.5f,1);
		type=(int)ofRandom(1000)%2;
		p1=ofRandom(0.8f,1);
	}

	for(int i=0; i<gridSize; i++) {
		float deg=ofRandom(70,85);
		for(int j=0; j<gridSize; j++) {
			float tx=ofMap(i,0,gridSize-1, 0.5f,-0.5f);
			float ty=ofMap(j,0,gridSize-1, 0.5f,-0.5f);
			float tz=rndSigned(0.05f,0.35f);

			ofVec3f v=ofVec3f(tx,ty,tz);
			float a=0,b=0;

			if(gridType==GRIDCIRC) {
				tx=((tx+0.5f));
				ty=((ty+0.5f));

				if(j==0) p1=ofRandom(0.6f,1.f);
				else p1=p1*0.8f+0.2f*ofRandom(0.6f,1.f);

				if(type==0) {
					v.set(clothWidth*p1,0,0);

					a=-deg+tx*deg*2;
					v.rotate(a+180,axisZ);
					a=-ty*360;
					v.rotate(a,axisY);
					v.set(v.x*hMod,v.y,v.z*hMod);
				}
				else {
					tx=p1*tx*(1-radMin)+radMin;
					v.set(clothWidth*tx,0,0);
					v.rotate(ty*360,axisZ);
//					if(rndBool(30)) tz*=tx;
					v.z=tz*clothHeight;
				}
				//v.set(tx*clothWidth,ty*clothWidth,tz*clothHeight);

			}
			else if(gridType==GRIDCYL) {
				a=abs(bezierPoint(terrain.h,ty)+0.5f)+radMin;
				//tz=tz*3+radMin;
				v.set(ty*clothWidth*2,a*clothWidth,0);
				v.rotate(tx*360,axisX);
			}
			else {
				a=bezierPoint(terrain.h,tx+0.5f);
				v.set(tx*clothWidth,ty*clothWidth,tz*clothHeight);
				v*=1+a;
//				v=terrainMod(v);
			}

			mwMakeParticle(i,j,v);
		}

	}



	printf("========\nmwGenerate pp=%d / gridSize=%d / gridType=%d\n ",
		pp.size(),gridSize,gridType);
}

void CloudsVisualSystemVerletForm::mwGridSticky() {
	int n=ofRandom(30,50);
	n=stickyNum;
	int nhalf=ofRandom(n*0.2f,n*0.7f);

	if(gridType==GRIDRECT) {
		int corners[] = {
				ofRandom(gridSize/2),
				gridSize-1-ofRandom(gridSize/2),
			pp.size()/2-ofRandom(-gridSize/2,gridSize/2),
			pp.size()-1-ofRandom(gridSize/2),
			pp.size()-(gridSize)+ofRandom(gridSize/2)};

		for(int i=4; i>-1; i--) if(ofRandom(100)>60) {
			MWParticle &pt=pp[corners[i]];
			mwNewActivity(pt,ofRandom(100)>60 ? FIXEDSTATIC : FIXEDMOVING);
			n--;
		}
	}

	while((n--)>0) {
		bool onEdge=(n>nhalf);
		mwNewActivity(mwGetParticle(onEdge),
			ofRandom(100)>60 ? FIXEDSTATIC : FIXEDMOVING);
	}
}



//////////////////////////////////

void CloudsVisualSystemVerletForm::initColors(int row,int cnt) {

	//palette
	ofPixels pix;
	
	if( !ofLoadImage(pix, getVisualSystemDataPath() + "Palettes/catenarycolors.png") ){
		ofLogError() << "COULDN'T LOAD COLORS";
	}
	
	row=row%pix.getHeight();

	row = ofClamp(row, 1, pix.getHeight()-1);
	int rowCnt=0,a=255;

	if(cnt<0) {
		do {
			a=pix.getColor((rowCnt)+1,row).a;
			if(a!=0) {
				colors.push_back(pix.getColor((rowCnt)+1,row));
			}
			rowCnt++;
		} while(a!=0);
		return;
	}

	do {
		a=(pix.getColor((rowCnt)+1,row).a);
		rowCnt++;
	} while(a!=0);



//	return;
	//int randomPalette = int(ofRandom(1, pix.getHeight()));

	float chance=ofRandom(70,90);

	if(cnt>-1) {
		while((cnt--)>0){
			int id=ofRandom(1,rowCnt);
			colors.push_back(pix.getColor(id, row));
		}
	}

	for(int i = 1; i < pix.getWidth(); i++) if(ofRandom(100)>chance || colors.size()<10){
		ofColor color = pix.getColor(i, row);
		if(color.a == 0) break;
		colors.push_back( color );
	}
	cout << "found " << colors.size() << " for palette row " << row << endl;

}

CloudsVisualSystemVerletForm::CloudsVisualSystemVerletForm(){
}

string CloudsVisualSystemVerletForm::getSystemName(){
	return "VerletForm";
}

void CloudsVisualSystemVerletForm::selfSetup(){
	fpsMod=0.5f;
	currentRotAngle = ofVec3f(0,0,0);
	currentCamDistance = -500;
	mousePos== ofVec3f(0,0,0);

	camEnabled=true;
	colorLightEnabled=true;

	gridSizeF=10;
	lastGenerated=-100;
	colorMod=ofRandom(0.5f,15.f);

	colorStrategy=ofRandom(3);

	grav=ofVec3f();
	gravGoal=ofVec3f();
	activityMod=1;
	vnormalMod=1;

	meshCentroid=ofVec3f(0,0,0);

	cBlack=ofFloatColor(0,0,0);
	cWhite=ofFloatColor(0.4f,0.4f,0.4f);
	cGray=ofFloatColor(0.2f,0.2f,0.2f);

	for(int i=0; i<LIGHTS; i++) {
		auxLights[i].light=ofLight();
		auxLights[i].color=ofColor(255,0,0);
		auxLights[i].colorGoal=ofColor(255,0,0);
	}
}

void CloudsVisualSystemVerletForm::generateMesh(){
	
	cout << "GENERATING MESH | colorIndex " << colorIndex << endl;
	clearElements();

	colors.clear();

	int ci=colorIndex;
//	while(colors.size()<30) {
//		//colorIndex=(int)ofRandom(900);
//		initColors(ci,10);
//		ci=(ci+131)%900;
//	}

	printf("initColors(%d,-1)\n",ci);
	initColors(ci,-1);
	printf("initColors(%d,-1) - %d\n",ci,colors.size());
	initColors(ci+ofRandom(200,450),-1);
	printf("done - initColors(%d,-1) == %d colors\n",ci,colors.size());

	int n=colors.size();
	float b=rndBool(50) ? 81 : 255;

	for(int i=0; i<n; i+=4) {
		ofColor c=ofColor(colors[i]);
		c.setBrightness(b);
		if(b>200) c.setSaturation(100);
		colors.push_back(c);
	}

/*
	ofColor c1=ofColor(255,100,0);
	ofColor c2=ofColor(255,200,0);
	float offs=ofRandom(50,100);
	shiftHue(c1,offs);
	shiftHue(c2,offs);


	ofColor c3=ofColor(255,0,128);
	ofColor c4=ofColor(100,200,200);
	offs=ofRandom(50,100);
	shiftHue(c3,offs);
	shiftHue(c4,offs);

	for(int i=0; i<6; i++) {
		float t=(float)i/5.f;
		colors.push_back(c1.lerp(c2,t));
		colors.push_back(c3.lerp(c4,t));
	}
*/


	mwPreset();

	physics.setWorldSize(
		ofVec3f(-clothWidth*4,-clothWidth*4,-clothWidth*4),
		ofVec3f(clothWidth*4,clothHeight*4,clothWidth*4));
	physics.setSectorCount(1);
	physics.setNumIterations(20);
	mwNewGravity();

		

	ofMesh baseMesh;
	float ext=(clothWidth/gridSize)*0.5f*springExtend;
	float str=springStrength*0.01f;

	mwGenerate();

	int id=0;

	for (int i = 0; i < gridSize; i++){

		particles.push_back(vector<Particle3D*>());
		
		for (int j = 0; j < gridSize; j++){
			MWParticle &theParticle=pp[id++];

			Particle3D* particle = physics.makeParticle(theParticle.orig);
			particle->setMass(1);
			particles[i].push_back( particle );
			theParticle.p=particle;

			
//			particleToMeshIndex[particle] = baseMesh.getNumVertices();
			meshIndexToParticle[baseMesh.getNumVertices()] = particle;
			meshIndexToMWParticle[baseMesh.getNumVertices()] = theParticle;
			theParticle.vertID=baseMesh.getNumVertices();
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
						ext*ofRandom(0.75f,1.25f));
			}
			if (j > 0){
				physics.makeSpring(
						particles[i][j - 1],
						particles[i][j], str,
						ext*ofRandom(0.75f,1.25f));
			}
		}
	}

	if(gridType!=GRIDRECT && gridDoStitch) {
		int y1=gridSize-1,y2=0;

		for (int i = 0; i < gridSize; i++) if(ofRandom(100)>10){
						physics.makeSpring(
								particles[i][y2],
								particles[i][y1], str,
								ext*ofRandom(0.75f,1.25f));
		}
	}

	//mesh.clearIndices();;
	int w = gridSize;
	int h = gridSize;

	int xn=(gridDoStitch ? gridSize : gridSize-1);

	for (int y = 0; y < gridSize-1; y++){
		for (int x = 0; x < xn; x++){

			int x2=(x+1)%gridSize;
			
			ofIndexType a,b,c;
			a = x+y*w;
			b = x2+y*w;
			c = x+(y+1)*w;
			baseMesh.addIndex(a);
			baseMesh.addIndex(b);
			baseMesh.addIndex(c);
			
			a = x2+(y+1)*w;
			b = x+(y+1)*w;
			c = x2+(y)*w;
			baseMesh.addIndex(a);
			baseMesh.addIndex(b);
			baseMesh.addIndex(c);
		}
	}
	

	printf("==============\ncolorIndex=%d colors.size()=%d colorMod=%.2f\n",
			(int)colorIndex,colors.size(),colorMod);
	printf("pp=%d baseMesh.indices=%d %.1f\n",
			pp.size(),
			baseMesh.getNumIndices(),
			(float)baseMesh.getNumIndices()/(float)pp.size());

	int cid[3];
	MWParticle index[3];

	//now we split it up and add colors	for(int)

	if(MWDEBUG) printf("colorStrategy=%d | indices=%d\n",
			colorStrategy,baseMesh.getNumIndices());

	int partID=0;
	float rgb[3];
	for(int i = 0; i < baseMesh.getNumIndices(); i+=3){
		index[0]=meshIndexToMWParticle[ baseMesh.getIndex(i+0) ];
		index[1]=meshIndexToMWParticle[ baseMesh.getIndex(i+1) ];
		index[2]=meshIndexToMWParticle[ baseMesh.getIndex(i+2) ];

		particleToMeshIndices[ meshIndexToParticle[ baseMesh.getIndex(i+0) ] ].push_back(mesh.getNumVertices()+0);
		particleToMeshIndices[ meshIndexToParticle[ baseMesh.getIndex(i+1) ] ].push_back(mesh.getNumVertices()+1);
		particleToMeshIndices[ meshIndexToParticle[ baseMesh.getIndex(i+2) ] ].push_back(mesh.getNumVertices()+2);

		mesh.addVertex( baseMesh.getVertices()[ baseMesh.getIndex(i+0) ] );
		mesh.addVertex( baseMesh.getVertices()[ baseMesh.getIndex(i+1) ] );
		mesh.addVertex( baseMesh.getVertices()[ baseMesh.getIndex(i+2) ] );

		ofColor c=colors[
		                 index[colorStrategy==2 ? 0 :
							 (int)ofRandom(999)%3].colID];

		if(colorStrategy==0) {
			c=colors[index[0].colID];
			rgb[0]=c.r;
			rgb[1]=c.g;
			rgb[2]=c.b; 

			for(int j=1; j<3; j++) {
				c=colors[index[j].colID];
				rgb[0]+=c.r;
				rgb[1]+=c.g;
				rgb[2]+=c.b;
			}
			c=ofColor(rgb[0]*0.33333f,rgb[1]*0.333333f,rgb[2]*0.33333f);

			float b=(float)c.getBrightness()/255.f;
			b=(b<0.5f ? b*b : 1-(1-b)*(1-b));
			c.setBrightness(b*255);
		}
//		else c=

//		if((partID++)%2==1) c=colors[index[2].colID];

		mesh.addColor(c);
		mesh.addColor(c);
		mesh.addColor(c);
//		mesh.addColor(colors[index[0].colID]);
//		mesh.addColor(colors[index[1].colID]);
//		mesh.addColor(colors[index[2].colID]);

		mesh.addNormal(ofVec3f(0,1,0));
		mesh.addNormal(ofVec3f(0,1,0));
		mesh.addNormal(ofVec3f(0,1,0));

		mesh.addIndex(i+0);
		mesh.addIndex(i+1);
		mesh.addIndex(i+2);
	}
	
	updateNormals();

	mwGridSticky();
//	bbC=mesh.getCentroid();
	mwMeshBB();

	for(int i=0; i<5; i++) physics.update();
}

void CloudsVisualSystemVerletForm::selfSetupGuis(){
	
    clothGui = new ofxUISuperCanvas("CLOTH", gui);
    clothGui->copyCanvasStyle(gui);
    clothGui->copyCanvasProperties(gui);
    clothGui->setName("ClothSettings");
    clothGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    bbLabel[0]=clothGui->addLabel("BB0", OFX_UI_FONT_SMALL);
    bbLabel[1]=clothGui->addLabel("BB1", OFX_UI_FONT_SMALL);
    bbLabel[2]=clothGui->addLabel("BB2", OFX_UI_FONT_SMALL);
    bbLabel[3]=clothGui->addLabel("BB3", OFX_UI_FONT_SMALL);
    clothGui->addSpacer(10);
	
	clothGui->addButton("REGENERATE", &shouldRegenerateMesh);
//	clothGui->addToggle("LINE MESH", &lineMesh);
	clothGui->addToggle("GRID STITCH", &gridDoStitch);
    clothGui->addSpacer(10);

	clothGui->addToggle("AUTO-REGENERATE", &doAutoGenerate);
	clothGui->addToggle("LOWRES", &doLoRes);
//	clothGui->addToggle("CAM ENABLED", &camEnabled);
//	clothGui->addToggle("COLOR LIGHTS", &colorLightEnabled);
    clothGui->addSpacer(10);

	//clothGui->addSlider("COLOR INDEX", 0, 909, &colorIndex);
	
//	clothGui->addSlider("bbMultTmp", -1000, 1000, &bbMultTmp);
//	clothGui->addSlider("CLOTH WIDTH", 400, 800, &clothWidth);
	clothGui->addSlider("GRID TYPE", 0, 2, &gridTypeF);
	clothGui->addSlider("GRID SIZE", 40, 100, &gridSizeF);
	clothGui->addSlider("STICKY", 10, 100, &stickyNum);
	clothGui->addSlider("SPRING STRENGTH", .001, 1.0, &springStrength);
	clothGui->addSlider("SPRING EXTEND", 0.001, 5, &springExtend);
    clothGui->addSpacer(10);
	clothGui->addSlider("COLOR INDEX", 0, 900, &colorIndex);
	clothGui->addSlider("COLOR MOD", 0.001, 15, &colorMod);
	clothGui->addSlider("COLOR STRATEGY", 0,2, &colorStrategyF);


	clothGui->addSlider("VNMORMAL MOD", 0.1f,1.5f,&vnormalMod);
	clothGui->addSlider("ACTIVITY MOD", 0.1f,2,&activityMod);



	ofAddListener(clothGui->newGUIEvent, this, &CloudsVisualSystemVerletForm::selfGuiEvent);
	
    guis.push_back(clothGui);
    guimap[clothGui->getName()] = clothGui;
}

void CloudsVisualSystemVerletForm::selfPresetLoaded(string presetPath){
	shouldRegenerateMesh = true;
}

void CloudsVisualSystemVerletForm::selfUpdate(){
	if(shouldRegenerateMesh){
		shouldRegenerateMesh = false;
		if(ofGetFrameNum()-lastGenerated>10) {
			generateMesh();
			lastGenerated=ofGetFrameNum();
		}
	}
	
	physics.update();

	if(lineMesh) {
/*
		int id=0,lineId=0,mx=gridSize;
		ofPolyline pl=ppLine[lineId];
		vector< ofPoint > pvl=pl.getVertices();

		for(int i=0; i<pp.size(); i++) {
			ofVec3f v=pp[i].p->getPosition();


			pvl[id++].set(v);
			if(id==mx && lineId<gridSize-1) {
				ofPolyline pl=ppLine[lineId++];
				pvl=pl.getVertices();
				id=0;
			}

		}
*/
	}
	else {
		//set mesh vertices to particle position
		for(int i = 0; i < physics.numberOfParticles(); i++){
			Particle3D* particle = physics.getParticle(i);
			for(int j = 0; j < particleToMeshIndices[particle].size(); j++ ){
				mesh.getVertices()[ particleToMeshIndices[ particle ][j] ].set(particle->getPosition());
			}
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
	
	mwUpdate();
	
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	mat->begin();
	ofPushMatrix();

	if(camEnabled) mwUpdateCamera();
	ofRotateX(modelRot.x+currentRotAngle.x);
	ofRotateZ(modelRot.z);
	ofRotateY(modelRot.y+currentRotAngle.y);
	mwLights();



	if(lineMesh) {
		ofNoFill();
		for(int i=0; i<pp.size(); i++) {
			if(i%gridSize==0) {
				if(i>0) ofEndShape(false);
				ofBeginShape();
				ofSetColor(colors[pp[i].colID]);
			}
			else {
				pp[i].v.set(
						pp[i].p->getPosition().x,
						pp[i].p->getPosition().y,
						pp[i].p->getPosition().z
						);
				ofVertex(pp[i].v);
			}

		}
		ofEndShape(false);

		int index=0;
		for(int i=0; i<gridSize; i++) {
			index=i;

			ofBeginShape();
			ofSetColor(colors[pp[index].colID]);

			for(int j=0; j<gridSize; j++) {
					ofVertex(pp[index].v);
					index+=gridSize;
			}
			ofEndShape(false);
		}
	}
	else mesh.draw();

//	ofSetColor(255);
	//ofDrawBox(bbC.x,bbC.y,bbC.z, 10,10,10);
//	ofCircle(bbC,50);

	mat->end();
	ofPopMatrix();

	for(int i = 0; i < LIGHTS; i++){
		auxLights[i].light.disable();
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
	pp.clear();
	ppActive.clear();

	mesh.clear();
	meshIndexToParticle.clear();
	meshIndexToMWParticle.clear();
	particleToMeshIndices.clear();
	
	particles.clear();
	physics.clear();
}

void CloudsVisualSystemVerletForm::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key=='n') shouldRegenerateMesh=true;
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
	if(e.widget->getName() == "GRID TYPE"){
		gridTypeF=(int)floor(gridTypeF+0.5f);
		printf("%d GUI - gridType=%f\n",ofGetFrameNum(),gridTypeF);
	}
	if(e.widget->getName() == "COLOR STRATEGY"){
		colorStrategyF=(int)floor(colorStrategyF+0.5f);
		printf("%d GUI - colorStrategyF=%f\n",ofGetFrameNum(),colorStrategyF);
	}


	if(e.widget->getName() == "REGENERATE" && ((ofxUIButton*)e.widget)->getValue() ){
		shouldRegenerateMesh = true;//generateMesh();
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



