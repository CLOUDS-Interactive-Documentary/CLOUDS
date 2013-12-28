#include "CloudsVisualSystemReplicator.h"

CloudsVisualSystemReplicator::CloudsVisualSystemReplicator(){

}

string CloudsVisualSystemReplicator::getSystemName(){
	return "Replicator";
}

void CloudsVisualSystemReplicator::selfSetup(){
	local_time = ofGetElapsedTimef() + ofRandom(10000);
    
    // sound
    synth.setOutputGen(buildSynth());
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
#if (OF_VERSION_MINOR < 8)
	glDisable(GL_DEPTH_TEST);
#else
	ofDisableDepthTest();
#endif
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
    // sound
    ofAddListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemReplicator::audioRequested);
    
    for (int i=0; i<2; i++)
    {
        if (playSample[i]) {
            soundTriggers[i].trigger();
        }
    }
}

void CloudsVisualSystemReplicator::selfEnd(){
    
    ofRemoveListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemReplicator::audioRequested);
    
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
    soundGui = new ofxUISuperCanvas("Sound", gui);
	soundGui->copyCanvasStyle(gui);
	soundGui->copyCanvasProperties(gui);
	soundGui->setName("Sound");
	soundGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    soundGui->addToggle(soundFiles[0], &playSample[0]);
    soundGui->addToggle(soundFiles[1], &playSample[1]);
    
    guis.push_back(soundGui);
	guimap[soundGui->getName()] = soundGui;
    ofAddListener(soundGui->newGUIEvent, this, &CloudsVisualSystemReplicator::selfGuiEvent);
}

void CloudsVisualSystemReplicator::selfGuiEvent(ofxUIEventArgs &e){
    for (int i=0; i<2; i++)
    {
        if (e.widget->getName() == soundFiles[i]) {
            ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
            playSample[i] = toggle->getValue();
            if (toggle->getValue() == true) {
                soundTriggers[i].trigger();
            }
        }
    }
}


void CloudsVisualSystemReplicator::selfSetupSystemGui(){

}

void CloudsVisualSystemReplicator::guiSystemEvent(ofxUIEventArgs &e){

}


void CloudsVisualSystemReplicator::selfSetupRenderGui(){

}

void CloudsVisualSystemReplicator::guiRenderEvent(ofxUIEventArgs &e){
	
}

Generator CloudsVisualSystemReplicator::buildSynth()
{
    string strDir = GetCloudsDataPath()+"sound/textures/";
    ofDirectory sdir(strDir);
    
    SampleTable samples[2];
    
    int nSounds = sizeof(soundFiles) / sizeof(string);
    for (int i=0; i<nSounds; i++)
    {
        string strAbsPath = sdir.getAbsolutePath() + "/" + soundFiles[i];
        samples[i] = loadAudioFile(strAbsPath);
    }
    
    Generator sampleGen1 = BufferPlayer().setBuffer(samples[0]).loop(1).trigger(soundTriggers[0]);
    Generator sampleGen2 = BufferPlayer().setBuffer(samples[1]).loop(1).trigger(soundTriggers[1]);
    
    return sampleGen1 * 0.7f + sampleGen2 * 1.0f;
}

void CloudsVisualSystemReplicator::audioRequested(ofAudioEventArgs& args)
{
    synth.fillBufferOfFloats(args.buffer, args.bufferSize, args.nChannels);
}


