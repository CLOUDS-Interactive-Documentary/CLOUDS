#include "CloudsVisualSystemReplicator.h"

string CloudsVisualSystemReplicator::getSystemName(){
	return "Replicator";
}

void CloudsVisualSystemReplicator::selfSetup(){
	local_time = ofGetElapsedTimef() + ofRandom(10000);

    tonicSamples.push_back(TonicSample("organ_slower.aif"));
    tonicSamples.push_back(TonicSample("EchoVortex.aif"));
    gain = 0;
    #ifdef TONIC_SOUNDS
    synth.setOutputGen(buildSynth());
    #endif
    
    vboRect.setUsage(GL_DYNAMIC_DRAW);
    vboRect.setMode(OF_PRIMITIVE_LINE_STRIP);
    
    vboLines.setMode(OF_PRIMITIVE_LINES);
    vboLines.setUsage(GL_DYNAMIC_DRAW);
    
    glDisable(GL_LINE_SMOOTH);

    for( int i = 0; i < 100*100; i++ ){
        
        vboRect.addVertex( ofVec3f(0,0,0) );
        vboRect.addColor(ofFloatColor( 1.,0. ));
        vboRect.addVertex( ofVec3f(0,0,0) );
        vboRect.addColor(ofFloatColor( 1.,70./255. ));
        vboRect.addVertex( ofVec3f(0,1,0) );
        vboRect.addColor(ofFloatColor( 1., 70./255. ));
        vboRect.addVertex( ofVec3f(1,1,0) );
        vboRect.addColor(ofFloatColor( 1.,70./255. ));
        vboRect.addVertex( ofVec3f(1,0,0) );
        vboRect.addColor(ofFloatColor( 1.,70./255. ));
        vboRect.addVertex( ofVec3f(0,0,0) );
        vboRect.addColor(ofFloatColor( 1.,70./255. ));
        vboRect.addVertex( ofVec3f(0,0,0) );
        vboRect.addColor(ofFloatColor( 1.,0. ));
        
        if( i%2 == 0 ){
            vboLines.addVertex( ofVec3f(.5,.5,0) );
            vboLines.addColor(ofFloatColor( 1., 40./255. ));
            vboLines.addVertex( ofVec3f(.5,.5,0) );
            vboLines.addColor(ofFloatColor( 1.,40./255. ));
        }
        
    }
    

}

void CloudsVisualSystemReplicator::selfSetupGuis(){
}


void CloudsVisualSystemReplicator::selfUpdate(){
    volumeControl.value(gain);
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
void CloudsVisualSystemReplicator::selfSetDefaults(){
    primaryCursorMode = CURSOR_MODE_CAMERA;
    secondaryCursorMode = CURSOR_MODE_INACTIVE;
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
	
	ofSetLineWidth(1);
	ofSetColor(255, 70);
	
	ofSetRectMode(OF_RECTMODE_CENTER);
#if (OF_VERSION_MINOR < 8)
	ofDisableDepthTest();
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
        
        ofMatrix4x4 transform;
        ofMatrix4x4 rotate;
        transform.scale(n.param.w, n.param.h, 1.);
        rotate.rotate(n.param.rx,1,0,0);
        rotate.rotate(n.param.ry,0,1,0);
        transform *= rotate;
        transform.translate(n.m.getTranslation());
        
        ofVec3f TL = transform.preMult( ofVec3f(-.5,-.5,0) );
        ofVec3f BL = transform.preMult( ofVec3f(-.5,.5,0) );
        ofVec3f BR = transform.preMult( ofVec3f(.5,.5,0) );
        ofVec3f TR = transform.preMult( ofVec3f(.5,-.5,0) );

        vboRect.setVertex(i*7, TL);
        vboRect.setVertex(i*7+1, TL);
        vboRect.setVertex(i*7+2, BL);
        vboRect.setVertex(i*7+3, BR);
        vboRect.setVertex(i*7+4, TR);
        vboRect.setVertex(i*7+5, TL);
        vboRect.setVertex(i*7+6, TL);
        
    }
    
    for (int i = 0; i < repl.size(); i+=2)
    {
        Replicator::Node &n = repl[i];
        Replicator::Node &n2 = repl[i+1];
            
        ofVec3f LS = n.m.preMult(ofVec3f(0,0,0));
        ofVec3f LE = n2.m.preMult(ofVec3f(0,0,0));
        vboLines.setVertex(i, LS);
        vboLines.setVertex(i+1, LE);
        
	}
    
    vboRect.draw();
    vboLines.draw();
    
//	ofSetLineWidth(2);
//	ofSetColor(255, 0, 0, 40);
//    
//	for (int i = 0; i < repl.size(); i+=2)
//	{
//		Replicator::Node &n = repl[i];
//		ofVec3f v(0);
//		v = v * n.m;
//        
//        Replicator::Node &n2 = repl[i+1];
//        ofVec3f v2(0);
//        v2 = v2 * n2.m;
//        
//        ofLine(v, v2);
//	}

	ofPopStyle();
	
	ofPopMatrix();
}

void CloudsVisualSystemReplicator::selfExit(){

}

void CloudsVisualSystemReplicator::selfBegin(){
    // sound
    ofAddListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemReplicator::audioRequested);
    
    for (int i=0; i<tonicSamples.size(); i++)
    {
        if (tonicSamples[i].playSample) {
            tonicSamples[i].soundTrigger.trigger();
        }
    }
}

void CloudsVisualSystemReplicator::selfEnd(){
    
    volumeControl.value(0);
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
    
    soundGui->addToggle(tonicSamples[0].soundFile, &tonicSamples[0].playSample);
    soundGui->addToggle(tonicSamples[1].soundFile, &tonicSamples[1].playSample);
    soundGui->addSlider("Gain", 0, 1, &gain);
    guis.push_back(soundGui);
	guimap[soundGui->getName()] = soundGui;
    ofAddListener(soundGui->newGUIEvent, this, &CloudsVisualSystemReplicator::selfGuiEvent);
    
}

void CloudsVisualSystemReplicator::selfGuiEvent(ofxUIEventArgs &e){
    for (int i=0; i<2; i++)
    {
        if (e.widget->getName() == tonicSamples[i].soundFile) {
            ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
            tonicSamples[i].playSample = toggle->getValue();
            if (toggle->getValue() == true) {
                tonicSamples[i].soundTrigger.trigger();
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

#ifdef TONIC_SOUNDS
Tonic::Generator CloudsVisualSystemReplicator::buildSynth()
{
    string strDir = GetCloudsDataPath(true)+"sound/textures/";
    ofDirectory sdir(strDir);
    
    Tonic::SampleTable samples[2];
    
    for(int i=0; i<tonicSamples.size();i++){
        string strAbsPath = ofToDataPath(strDir + "/" + tonicSamples[i].soundFile, true);
        samples[i] = Tonic::loadAudioFile(strAbsPath);
    }
    
    Tonic::Generator sampleGen1 = Tonic::BufferPlayer().setBuffer(samples[0]).loop(1).trigger(tonicSamples[0].soundTrigger);
    Tonic::Generator sampleGen2 = Tonic::BufferPlayer().setBuffer(samples[1]).loop(1).trigger(tonicSamples[1].soundTrigger);
    
    return (sampleGen1 * 0.7f + sampleGen2 * 1.0f) * volumeControl;
}
#endif

void CloudsVisualSystemReplicator::audioRequested(ofAudioEventArgs& args)
{
    #ifdef TONIC_SOUNDS
    synth.fillBufferOfFloats(args.buffer, args.bufferSize, args.nChannels);
    #endif
}


