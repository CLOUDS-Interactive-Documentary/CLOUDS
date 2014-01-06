
#include "CloudsVisualSystemVerletForm.h"
#include <string>
#include <map>
#include <vector>

/**
 * - Fix geometry (GRIDRECT, GRIDCYL)
 * - Use normals for new activity
 */

ofVec3f bMin,bMax,bCenter;
ofVec3f axisY=ofVec3f(0,1,0);
ofVec3f axisX=ofVec3f(1,0,0);
ofVec3f axisZ=ofVec3f(0,0,1);
MWTerrain terrain;
float radMin=0.1f;
int fixChance=40;
float lastNMod=0.1f;

void CloudsVisualSystemVerletForm::mwPreset() {
	int type=(int)(ofRandom(GRIDCYL+1));
	while(type==gridType) type=(int)(ofRandom(GRIDCYL+1));
	gridType=type;

//	gridType=GRIDCYL;

	if(doAutoGenerate) {
		colorMod=ofRandom(1.f,15.f);
		gridSize = (int)(ofRandom(45,80)*(ofGetFrameNum()>20 && fpsMod<1 ? fpsMod : 1));
	}
	if(!doAutoGenerate && gridSizeF>11) gridSize=gridSizeF;
	else gridSizeF=gridSize;

	getCameraRef().setOrientation(ofVec3f(0,0,0));

	gridDoStitch=(gridType!=GRIDRECT && ofRandom(100)>30);

	printf("mwPreset - grid=%d gridType=%d col=%d\n",
			gridSize,gridType,
			(int)colorIndex);

	springStrength = ofRandom(1,7)/100.0f;
	springDampening = 0.1f;
	springExtend = ofRandom(0.5f,4);

	colorIndex=ofRandom(0,800);
	clothWidth = ofRandom(450,550);
	clothHeight = ofRandom(0.6f,0.8f)*clothWidth;


	modelRot=ofVec3f(0.5f,0,0);
	modelRot.rotate(ofRandom(360),axisY);
	modelRot.rotate(ofRandom(-90,90),axisX);
	modelRotMax=0.1f;

	modelRotD=modelRot;

	camCenterOffs=ofVec3f(ofRandom(0.05f,0.15f)*clothWidth,0,0);
//	camCenterOffs.rotate(ofRandom(360),axisZ);
	
	stickyNum=gridSize*gridSize;
	stickyNum=(int)(stickyNum*(ofRandom(0.2f,2.5f)/100.0f));
	stickyNum=(stickyNum<10 ? 10 : stickyNum);
	stickyNum=(stickyNum>50 ? 50 : stickyNum);
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
		v.y*=1+a*0.2f;;
		a=1+a;
		v.x*=a;
		v.z*=a;


		v.y=1+ofNoise(d*5);
		v.y*=clothHeight;
	}
	else {
		a=v.x/clothWidth;
		float sineMod=sin(a*HALF_PI+colorIndex/900.f*HALF_PI*0.5f)+0.15f;
		float b=atan2(-v.z,v.y);
		a=ofNoise(a*25,b*25)*sineMod;
		a=a*a*2.5f+0.75f;
		v.set(v.x,v.y*a,v.z*a);


//		v.rotate(90,axisX);
		return v;
	}

	return v;

}

string CloudsVisualSystemVerletForm::hexStr(ofColor &c) {
    ostringstream ret;
	string s="[";
//    int hex=c.getHex();
//
//	ret << std::hex << std::setfill('0') << std::setw(2) << (std::uppercase) << (hex & 0xff);
//	ret << std::hex << std::setfill('0') << std::setw(2) << (std::uppercase) << (hex >> 8 & 0xff);
//	ret << std::hex << std::setfill('0') << std::setw(2) << (std::uppercase) << (hex >> 16 & 0xff);;
//
	s.append(""+c.r);
	s.append(",");
	s.append(""+c.g);
	s.append(",");
	s.append(""+c.b);
	s.append("]");
	return s;
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
	ofSetSmoothLighting(false);

	for(int i = 0; i < LIGHTS; i++) {
		AuxLight &a=auxLights[i];
		ofColor c=(colorLightEnabled ? a.color : ofColor::white);

		if(i>1 && colorLightEnabled) {
			a.cnt++;

			if(a.cnt>-1) {
				float t=ofMap(a.cnt, 0,a.cntGoal, 0,1);
				c=ofColor(c).lerp(a.colorGoal,t);
				if(a.cnt==a.cntGoal) mwNewLightColor(a);
			}

			a.position.rotate(
					a.spinSpeed/fpsMod,a.spinAxis);
		}

		a.light.setPosition(a.position);

		ofFloatColor cf=ofFloatColor(c);
		a.light.setDiffuseColor(cf);
		a.light.setSpecularColor(cWhite);
		a.light.setAmbientColor(cBlack);
//
//		if(i>1) {
//			a.light.setSpecularColor(auxLights[(i+1)%3+1].color);
//		}
//		else {
//		}
		a.light.enable();

	}

} 

float camCenterOffsD=0;

void CloudsVisualSystemVerletForm::mwUpdateCamera() {
	//compute the center of mesh
	ofVec3f meshCenter = mesh.getCentroid();


	if(!cursorIsOverGUI()) {

		ofVec3f pos=ofVec3f(
				GetCloudsInputX()-getCanvasWidth()/2,
				GetCloudsInputY()-getCanvasHeight()/2,0);

		if(abs(pos.x-mousePos.x)>1 || abs(pos.y-mousePos.y)>1) {
			camCenterOffsD=ofClamp(camCenterOffsD+rndSigned(0.1f,0.3f), -1,1);
			camCenterOffs.rotate(camCenterOffsD,axisY);

		//	meshCenter+=camCenterOffs;
			//set the position to the middle plus some rotation around the center

			float camDistance =
					ofMap(pos.length(), 0, getCanvasWidth()*0.5f,
							1200+clothWidth/2, 100,true);
			currentCamDistance += (camDistance - currentCamDistance)*.4;

			float camAngle = ofMap(GetCloudsInputY(), 0, getCanvasHeight(), -180, 180,true);
			currentRotAngle.x += (camAngle - currentRotAngle.x)*.1;

			camAngle = ofMap(GetCloudsInputX(), 0, getCanvasWidth(), -180, 180,true);
			currentRotAngle.y += (camAngle - currentRotAngle.y)*.1;

			mousePos.set(pos);
		}

		ofVec3f camTargetPosition  = meshCenter +
//				camCenterOffs+
				ofVec3f(0,0,currentCamDistance).
				getRotated(currentRotAngle.x,currentRotAngle.y,0);
		// ofVec3f(0.5f,1,0));
		ofVec3f camCurrentPosition = cam.getPosition();

		cam.setPosition( camCurrentPosition + (camTargetPosition - camCurrentPosition) * .1);

		cam.lookAt(meshCenter);
	}


	modelRotMax=0.2f/fpsMod;
	float min=modelRotMax*0.005f;
	float max=modelRotMax*0.015f;

	do {
		modelRotD.x=modelRotD.x+rndSigned(min,max);
	} while(abs(modelRotD.x)<max);

	do {
		modelRotD.y=modelRotD.y+rndSigned(min,max);
	} while(abs(modelRotD.y)<max);

	do {
		modelRotD.z=modelRotD.z+rndSigned(min,max);
	} while(abs(modelRotD.z)<max);

	modelRotD.x=(modelRotD.x>modelRotMax ? modelRotMax : (modelRotD.x<-modelRotMax ? -modelRotMax : modelRotD.x));
	modelRotD.y=(modelRotD.y>modelRotMax ? modelRotMax : (modelRotD.y<-modelRotMax ? -modelRotMax : modelRotD.y));
	modelRotD.z=(modelRotD.z>modelRotMax ? modelRotMax : (modelRotD.z<-modelRotMax ? -modelRotMax : modelRotD.z));
	modelRot+=modelRotD;


}

void CloudsVisualSystemVerletForm::mwUpdate() {
	if(doAutoGenerate &&
			(ofGetFrameNum()>100 && ofGetFrameNum()%25000==0)) shouldRegenerateMesh=true;

	float fpsModNow=ofGetFrameRate()/60.0f;
	fpsMod=ofClamp(
			fpsMod*0.95f+(fpsModNow*0.05f),
			0.25f,2.5f);

	if(ofGetFrameNum()%500==0 && MWDEBUG) printf("%d fpsMod %1.2f | fixCnt=%d | activityCnt=%d | drag=%.4f\n",
			ofGetFrameNum(),fpsMod,
			fixCnt,(int)activityCnt,
			(100.f-fpsMod*0.4f)/100.f);

	physics.setDrag((100.f-fpsMod*0.4f)/100.f);
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

		activityCnt=ofRandom(20,80)*fpsMod;
	}

//		if(ppActive.size()<1) return;

/*
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
*/

	string s="";

	for(int i=0; i<ppActive.size(); i++) {

		MWParticle& pt=ppActive.at(i);
		pt.stateCnt--;
		if(pt.stateCnt>-1) {
			if(pt.stateCnt>0 && pt.state>FREE) {
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
	float a=0;

	if(gridType<GRIDCYL) {
		a=atan2(-v.z,v.x)/PI+1;
		a=ofNoise(a*10)*2.5f+0.3f;
		v=v*a;
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


	ofVec3f v2=ofVec3f(
			pt.p->getPosition().x,
			pt.p->getPosition().y,
			pt.p->getPosition().z);
	pt.orig=pt.orig*0.8f+v2*0.2f;

	float len=pt.orig.length();
	if(len>clothWidth) {
		pt.orig=pt.orig.normalize()*clothWidth;
	}
	else if(len<radMin*2*clothWidth) {
		pt.orig=pt.orig.normalize()*radMin*2*clothWidth;
	}


	ofVec3f vD2=ofVec3f(pt.orig);

	float val=ofRandom(1);

	lastNMod=lastNMod*0.9f+ofRandom(0.4f)*0.1f;

	vD2=vn*(ofMap(val*val, 0,1, 0.1f,0.11f+lastNMod)*
			(rndBool(70) ? -0.25f : 1)*clothHeight);

 //rndSigned(0.15f,0.4f)*clothHeight);
	vD2.rotate(rndSigned(15,30), axisX);
	vD2.rotate(rndSigned(15,30), axisY);

	pt.stateCnt=(int)(ofRandom(60,120)*3*fpsMod);


	if(state==FIXEDSTATIC) {
		pt.vD=pt.orig+vD2*0.2f;
	}
	else {
		pt.vD=pt.orig+vD2;
	}

	mwFix(pt,true);

	pt.state=state;
	pt.vD-=v2;
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
//	printf("========\nmwCreateLights pp=%d / gridSize=%d\n",
//		pp.size(),gridSize);

	ofColor c[]= {
			ofColor(colors.at(ofRandom(colors.size()-1))),
			ofColor(colors.at(ofRandom(colors.size()-1))),
			ofColor(colors.at(ofRandom(colors.size()-1)))
	};

	float bSum=c[0].getBrightness()+
			c[1].getBrightness()+
			c[2].getBrightness();
	bSum=400.f/bSum;

	for(int i = 0; i < LIGHTS; i++){
		ofVec3f v=ofVec3f(
				ofRandom(0.75f, 1.f)*2*clothWidth,
				-ofRandom(0.25, 0.75)*clothHeight,
				0);
		if(i==2) v.rotate(120,axisX);

		v.rotate(i*120+ofRandom(-15,15), axisY);

		auxLights[i].position=v;
		auxLights[i].enabled=true;

		if(i<2) {
			auxLights[i].position.set(0,clothHeight*5,0);
			auxLights[i].light.setDirectional();
			auxLights[i].color=rndColor();
			shiftHue(auxLights[i].color,rndSigned(60,120));
			auxLights[i].color.setSaturation(10);
			auxLights[i].color.setBrightness(120+i*60);
			auxLights[i].light.setDiffuseColor(ofFloatColor(auxLights[i].color));
			auxLights[i].light.setSpecularColor(cWhite);

			if(i==0)
				auxLights[i].light.setOrientation( ofVec3f(-30, -75, 0) );
			else auxLights[i].light.setOrientation( ofVec3f(75, 45, 0) );
		}
		else {
			// call twice to set old color and get new one
			mwNewLightColor(auxLights[i]);
			mwNewLightColor(auxLights[i]);

			auxLights[i].light.setPointLight();
			auxLights[i].light.setPosition(auxLights[i].position);
			auxLights[i].light.setSpecularColor(cWhite);

			ofVec3f spinAxis=ofVec3f(
					ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1)*0.2f);
			auxLights[i].spinAxis=spinAxis.normalize();
			auxLights[i].spinSpeed=ofRandom(0.2f,1);

		}

		mat->setShininess(ofRandom(100,255));
		mat->setEmissiveColor(ofFloatColor(0,0,0));

//		  lCol1=UColorTool.adjustBrightness(
//		    UColorTool.adjustSaturation(getColor(),0.5f), random(1,1.5f));
	}

}


void CloudsVisualSystemVerletForm::mwNewGravity() {
	gravCnt=-ofRandom(150,300)*fpsMod;
	gravCntGoal=ofRandom(150,300)*fpsMod;

	grav=gravGoal;
	physics.setGravity(grav);

	gravGoal=ofVec3f(0,0,ofRandom(0.001f,0.01f)/(fpsMod<1 ? 1 : fpsMod));
	if(grav.z>0) gravGoal.z=-gravGoal.z;

	gravGoal.rotate(rndSigned(2,15),axisX);
	gravGoal.rotate(ofRandom(360),axisZ);

	if(MWDEBUG) printf("mwNewGravity %.3f,%.3f,%.3f | %.3f,%.3f,%.3f \n",
			grav.x,grav.y,grav.z,
			gravGoal.x,gravGoal.y,gravGoal.z
			);//,hexStr(a.color),hexStr(a.color));
}

void CloudsVisualSystemVerletForm::mwNewLightColor(AuxLight &a) {

	a.cnt=(int)(-ofRandom(150,300)*fpsMod);
	a.cntGoal=(int)(ofRandom(500,900)*fpsMod);

	ofColor cc=rndColor();
	int b=cc.getBrightness()*ofRandom(1.0f,1.5f);
	cc.setBrightness(b>200 ? 200: (b<80 ? 80 : b));
	shiftHue(cc,rndSigned(60,120));

	float sat=a.colorGoal.getSaturation();
	sat=(sat<30 && ofRandom(100)>80 ?
			ofRandom(80,120) : ofRandom(10,30));
	cc.setSaturation(sat);

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

	activityCnt=ofRandom(20,80)*fpsMod;
	fixCnt=0;

	axisY.set(0,1,0);

	float clothWidthHalf=clothWidth*0.5f;
	float hMod=1;
	float p1=0,p2=0,p3=0;

	int type=0;

	if(gridType==GRIDCIRC) {
		hMod=ofRandom(0.5f,1);
		type=(int)ofRandom(1000)%2;
		p1=ofRandom(0.8f,1);
	}

	for(int i=0; i<gridSize; i++) {
		float deg=ofRandom(70,85);
		for(int j=0; j<gridSize; j++) {
			float tx=ofMap(i,0,gridSize-1, -0.5f,0.5f);
			float ty=ofMap(j,0,gridSize-1, -0.5f,0.5f);
			float tz=rndSigned(0.05f,0.35f);

			ofVec3f v=ofVec3f(tx,ty,tz);
			float a=0,b=0;

			if(gridType==GRIDCIRC) {
				tx=((tx+0.5f));
				ty=((ty+0.5f));

				if(j==0) p1=ofRandom(0.8f,1);
				else p1=p1*0.8f+0.2f*ofRandom(0.8f,1);

				if(type==0) {
					v.set(clothWidth*p1,0,0);

					a=-deg+tx*deg*2;
					v.rotate(a,axisZ);
					a=ty*360;
					v.rotate(a,axisY);
					v.set(v.x*hMod,v.y,v.z*hMod);
				}
				else {
					tx=tx*(1-radMin)+radMin;
					v.set(clothWidth*0.5f*p1*tx,0,0);
					v.rotate(ty*360-180,axisZ);
					if(rndBool(30)) tz*=tx;
					v.z=tz*clothHeight;
				}
				//v.set(tx*clothWidth,ty*clothWidth,tz*clothHeight);

			}
			else if(gridType==GRIDCYL) {
				a=ty*360;
				tz=tz*2+radMin*3;
				v.set(tz*clothWidth,tx*clothWidth*2.5f,0);
				v.rotate(a,axisY);
			}
			else {
				a=bezierPoint(terrain.h,tx+0.5f);
				v.set(tx*clothWidth,ty*clothWidth,tz*clothHeight);
				v*=1+a;
			}

//			v=terrainMod(v);
			mwMakeParticle(i,j,v);
		}

	}





	printf("========\nmwGenerate pp=%d / gridSize=%d / gridType=%d\n ",
		pp.size(),gridSize,gridType);
}

/*
void CloudsVisualSystemVerletForm::mwGenerate() {
	printf("========\nmwGenerate pp=%d / gridSize=%d / gridType=%d\n ",
		pp.size(),gridSize,gridType);

	axisY.set(0,1,0);

	float clothWidthHalf=clothWidth*0.5f;

	float degRange=ofRandom(0.6f,1);
	float degTwist=ofRandom(-90,90),degTwistD=ofRandom(1,3);

	radMin=ofRandom(0.1f,0.3f);

	for(int i=0; i<gridSize; i++) {
		degRange=ofRandom(0.6f,1)*0.2f+degRange*0.8f;

		degTwistD=ofClamp(
				degTwistD+(ofRandom(100)<50 ? ofRandom(0.25,1) : -ofRandom(0.25,1)),
				-3,3);
		degTwist+=degTwistD;

		for(int j=0; j<gridSize; j++) {

			float tx=ofMap(i,0,gridSize-1, 0,clothWidth);
			float ty=ofRandom(0.25f,0.5f)*clothHeight;
			float tz=ofMap(j,0,gridSize-1, 0,1);

			ofVec3f v=ofVec3f(tx,ty,tz);


			if(gridType==GRIDCIRC) { // circular
				float a=(tz-0.5f)*360.0f;
				if(ofRandom(100)<50) ty=-ty;
				v.set(tx*(1-radMin)+radMin*clothWidth,
						ty*ofRandom(2,4),0);
				v.rotate(a, axisY);
				float m=ofRandom(0.8f,1.2f);
				v.set(v.x*m,v.y,v.z*m);
			}
			else if(gridType==GRIDCYL) { // circular
				float a=(tz-0.5f)*360.0f+degTwist;//+90*(1-degRange);
				ty=ofRandom(0.1f,0.4f)+radMin;
				ty=ty*clothHeight;
				v.set(-(tx-clothWidthHalf)*1.5f,
						ty,ty);//tz*clothWidth);

				v.rotate(-a, axisX);
			}
			else if(gridType==GRIDRECT) { // grid
				v.set(tx-clothWidthHalf,
						ty,
						tz*clothWidth-clothWidthHalf);
//				v.rotate(0,axisY);
			}
//					(ofRandom(100)<50 ? 1 : -1);
			mwMakeParticle(i,j,v);
		}
	}

	printf("========\nmwGenerate pp=%d / gridSize=%d / gridType=%d\n ",
		pp.size(),gridSize,gridType);
}
*/

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
			mwNewActivity(pt,ofRandom(100)>30 ? FIXEDSTATIC : FIXEDMOVING);
			n--;
		}
	}

	while((n--)>0) {
		bool onEdge=(n>nhalf);
		mwNewActivity(mwGetParticle(onEdge),
			ofRandom(100)>30 ? FIXEDSTATIC : FIXEDMOVING);
	}
}



//////////////////////////////////

void CloudsVisualSystemVerletForm::initColors(int row) {

	//palette
	ofPixels pix;
	
	if( !ofLoadImage(pix, getVisualSystemDataPath() + "Palettes/catenarycolors.png") ){
		ofLogError() << "COULDN'T LOAD COLORS";
	}
	
	row=row%pix.getHeight();

	row = ofClamp(row, 1, pix.getHeight()-1);
	//int randomPalette = int(ofRandom(1, pix.getHeight()));

	float chance=ofRandom(70,90);

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
	currentCamDistance = 800;
	mousePos== ofVec3f(0,0,0);

	camEnabled=true;
	colorLightEnabled=true;

	gridSizeF=10;
	lastGenerated=-100;
	colorMod=ofRandom(0.5f,15.f);

	colorStrategy=ofRandom(3);

	grav=ofVec3f();
	gravGoal=ofVec3f();

	cBlack=ofFloatColor(0,0,0);
	cWhite=ofFloatColor(0.4f,0.4f,0.4f);

	for(int i=0; i<LIGHTS; i++) {
		auxLights[i].light=ofLight();
		auxLights[i].color=ofColor(255,0,0);
		auxLights[i].colorGoal=ofColor(255,0,0);
	}
}

void CloudsVisualSystemVerletForm::generateMesh(){
	
	colorIndex=(int)ofRandom(900);
	cout << "GENERATING MESH | colorIndex " << colorIndex << endl;

	colors.clear();
	initColors( int(colorIndex) );
	while(colors.size()<50)
		initColors( int(colorIndex+ofRandom(50,500)));
//	initColors( int(colorIndex+ofRandom(50,500))%colors.size());


	clearElements();
	mwPreset();


	bMin=ofVec3f(0,0,0);
	bMax=ofVec3f(0,0,0);

	
	physics.setWorldSize(
		ofVec3f(-clothWidth*4,-clothWidth*4,-clothWidth*4),
		ofVec3f(clothWidth*4,clothHeight*4,clothWidth*4));
	physics.setSectorCount(1);
//	physics.setDrag(0.98f);
	physics.setNumIterations(20);
//	physics.setTimeStep(0.0000010);

//	gravGoal=ofVec3f(0,0,rndSigned(1,3)/1000.f);
//	gravGoal.rotate(rndSigned(2,10),axisX);
//	gravGoal.rotate(ofRandom(360),axisZ);
//
	mwNewGravity();

		

	ofMesh baseMesh;
	float ext=(clothWidth/gridSize)*springExtend;
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

	// connect wrap-around edge
	//if(gridType>0) for (int j = 0; j < gridSize; j++){
	//	int y=gridSize-1;
	//	physics.makeSpring(particles[j][0], particles[j][y], str, rndExtend());
	//}

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

	colorStrategy=(colorStrategy+1)%3;

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
	clothGui->addSlider("SPRING STRENGTH", .001, 1.0, &springStrength);
	clothGui->addSlider("SPRING EXTEND", 0.001, 5, &springExtend);
	clothGui->addSlider("COLOR MOD", 0.001, 15, &colorMod);

	clothGui->addButton("REGENERATE", &shouldRegenerateMesh);
	clothGui->addToggle("CAM ENABLED", &camEnabled);
	clothGui->addToggle("COLOR LIGHTS", &colorLightEnabled);
	clothGui->addToggle("AUTO-REGENERATE", &doAutoGenerate);


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
	if(shouldRegenerateMesh){
		shouldRegenerateMesh = false;
		if(ofGetFrameNum()-lastGenerated>10) {
			generateMesh();
			lastGenerated=ofGetFrameNum();
		}
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
	
	if(camEnabled) mwUpdateCamera();
	mwUpdate();
	mwLights();
	
	ofPushMatrix();
	ofRotateX(modelRot.x);
	ofRotateZ(modelRot.z);
	ofRotateY(modelRot.y);

	mat->begin();
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	mesh.draw();
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
	particleToMeshIndices.clear();
	
	particles.clear();
	physics.clear();
}

void CloudsVisualSystemVerletForm::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key=='n' && ((GetKeyState( VK_CONTROL) & 0x80) > 0)) shouldRegenerateMesh=true;
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



