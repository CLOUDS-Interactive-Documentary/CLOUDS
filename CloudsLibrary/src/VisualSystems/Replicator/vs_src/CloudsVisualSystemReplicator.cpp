#include "CloudsVisualSystemReplicator.h"

CloudsVisualSystemReplicator::CloudsVisualSystemReplicator(){

}

string CloudsVisualSystemReplicator::getSystemName(){
	return "Replicator";
}

void CloudsVisualSystemReplicator::selfSetup(){
	local_time = ofGetElapsedTimef() + ofRandom(10000);
}

void CloudsVisualSystemReplicator::selfSetupGuis(){
}


void CloudsVisualSystemReplicator::selfUpdate(){
	Replicator::Grid2D(100, 100, 2000, 2000).apply(repl);
	
	// centering
	for (int i = 0; i < repl.size(); i++)
	{
		Replicator::Node &n = repl[i];
		n.m.glTranslate(-1000, -1000, 0);
	}
	
	// rectsize
	for (int i = 0; i < repl.size(); i++)
	{
		Replicator::Node &n = repl[i];
		n.param.w = 10 + (sin(local_time + i * 0.1)) * 10;
		n.param.h = 10 + (cos(local_time + i * 0.5)) * 10;
	}
	
	// rotate
	for (int i = 0; i < repl.size(); i++)
	{
		Replicator::Node &n = repl[i];
		n.param.rx = (local_time * 1.1) * n.param.x;
		n.param.ry = (local_time * 3.1) * n.param.y;
	}
	
	// noise
	for (int i = 0; i < repl.size(); i++)
	{
		Replicator::Node &n = repl[i];
		float t = local_time * 0.1;
		n.m.glTranslate(ofSignedNoise(n.param.x, 0, t) * 20,
						ofSignedNoise(0, n.param.y, t) * 20,
						ofSignedNoise(t, 0, i) * 20);
	}
	
	// pow
	for (int i = 0; i < repl.size(); i++)
	{
		Replicator::Node &n = repl[i];
		ofVec3f v = n.m.getTranslation();
		float d = v.length() / 500;
		d = d * d * d * d;
		float m = sin(local_time * 0.01) * 10;
		n.m.glTranslate(-v.x * sin(d - m) * 0.9,
						-v.y * sin(d - m) * 0.9,
						cos(d) * 500);
	}
	
	local_time += ofGetLastFrameTime();
}

void CloudsVisualSystemReplicator::selfDrawBackground(){

}

void CloudsVisualSystemReplicator::selfDrawDebug(){
	
}

void CloudsVisualSystemReplicator::selfSceneTransformation(){

}

void CloudsVisualSystemReplicator::selfDraw(){
	ofPushMatrix();
	
	ofPushStyle();

	ofNoFill();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	ofSetLineWidth(2);
	ofSetColor(255, 70);
	
	ofSetRectMode(OF_RECTMODE_CENTER);
	ofDisableDepthTest();
	
	ofRotateZ(ofSignedNoise(1, 0, 0, local_time * 0.01) * 30);
	ofRotateY(ofSignedNoise(0, 1, 0, local_time * 0.01) * 180);
	ofRotateX(ofSignedNoise(0, 0, 1, local_time * 0.01) * 180);
	
	ofTranslate(0, 0, ofSignedNoise(local_time * 0.05) * 300);
	
	for (int i = 0; i < repl.size(); i++)
	{
		Replicator::Node &n = repl[i];
		n.beginTransform();
		
		ofRotateX(n.param.rx);
		ofRotateY(n.param.ry);
		
		ofRect(0, 0, n.param.w, n.param.h);

		n.endTransform();
	}
	
	ofSetLineWidth(1);
	ofSetColor(255, 40);
	
	glBegin(GL_LINES);
	for (int i = 0; i < repl.size(); i++)
	{
		Replicator::Node &n = repl[i];
		ofVec3f v(0);
		v = v * n.m;
		glVertex3fv(v.getPtr());
	}
	glEnd();
	
	ofPopStyle();
	
	ofPopMatrix();
}

void CloudsVisualSystemReplicator::selfExit(){

}

void CloudsVisualSystemReplicator::selfBegin(){

}

void CloudsVisualSystemReplicator::selfEnd(){

}


void CloudsVisualSystemReplicator::selfKeyPressed(ofKeyEventArgs & args){

}

void CloudsVisualSystemReplicator::selfKeyReleased(ofKeyEventArgs & args){

}


void CloudsVisualSystemReplicator::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemReplicator::selfMouseMoved(ofMouseEventArgs& data){

}

void CloudsVisualSystemReplicator::selfMousePressed(ofMouseEventArgs& data){

}

void CloudsVisualSystemReplicator::selfMouseReleased(ofMouseEventArgs& data){

}


void CloudsVisualSystemReplicator::selfSetupGui(){

}

void CloudsVisualSystemReplicator::selfGuiEvent(ofxUIEventArgs &e){

}


void CloudsVisualSystemReplicator::selfSetupSystemGui(){

}

void CloudsVisualSystemReplicator::guiSystemEvent(ofxUIEventArgs &e){

}


void CloudsVisualSystemReplicator::selfSetupRenderGui(){

}

void CloudsVisualSystemReplicator::guiRenderEvent(ofxUIEventArgs &e){
	
}