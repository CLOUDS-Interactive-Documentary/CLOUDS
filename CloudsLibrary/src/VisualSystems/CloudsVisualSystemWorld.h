//
//  CloudsVisualSystemWorld.h
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/13/13.
//
//

#pragma once

#include "CloudsVisualSystemRezanator.h"
#include "wParticle.h"

class CloudsVisualSystemWorld : public CloudsVisualSystemRezanator {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    void selfDraw();
    void selfExit();
    void selfBegin();
	void selfEnd();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
protected:
    void loadPath(ofVboMesh &_vbo, string _file);
    void loadPoints(vector<ofPoint> &_points, string _file);
    void loadParticles( string _file );
    
    //  Globe
    //
    ofVboMesh   coastVbo;
    ofVboMesh   riversVbo;

    //  Spikes
    //
    vector<wParticle*> particles;
    float   pointNoisePeaks;
    
    //  Flocking points
    //
    vector< ofPoint > points;
    ofPoint globalOffset;
    float   nMaxPoints;
    float   density,gravity,repulsion;
    float   turbulence,neigbordhood,independence;
    
    
};