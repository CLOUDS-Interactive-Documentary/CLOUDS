//
//  CloudsVisualSystemLSys.h
//  VSLSys
//
//  Created by Patricio Gonzalez Vivo on 5/25/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"

#include "ofxUI.h"
#include "LSystemReconstructor.h"

class CloudsVisualSystemLSys : public CloudsVisualSystem {
public:
	CloudsVisualSystemLSys();
	~CloudsVisualSystemLSys();
    
    void update(ofEventArgs & args);
    void draw(ofEventArgs & args);
	
    void exit(ofEventArgs & args);
    
	void keyPressed(ofKeyEventArgs & args);
	void keyReleased(ofKeyEventArgs & args);
	
	void mouseDragged(ofMouseEventArgs & args);
	void mouseMoved(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
    
    string getSystemName();
    
protected:
    void begin();
	void end();
    void reBuildLSys();
    
    LSystemReconstructor lsysr;
    
    string  axiom;
    string  rule1;
    string  rule2;
    float   angle;
    float   globalScale;
    float   lsysScale;
    int     lsysDepth;
    
    ofxUICanvas *gui;
	void guiEvent(ofxUIEventArgs &e);

};
