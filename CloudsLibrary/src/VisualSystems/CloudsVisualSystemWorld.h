//
//  CloudsVisualSystemWorld.h
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/13/13.
//
//

#pragma once

#include "CloudsVisualSystemRezanator.h"

#include "wPoint.h"
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

    //  Globe
    //
    void        loadPath(ofVboMesh &_vbo, string _file);
    ofVboMesh   coastVbo;
    ofVboMesh   riversVbo;

    //  Cities
    //
    void loadPoints(string _file);
    vector< wPoint > worldPoints;
    float   pointNoisePeaks;
    
    //  Flocking particles
    //
    void loadParticles( string _file );
    vector<wParticle*> particles;
    ofPoint globalOffset;
    float   nMaxPoints;
    float   density,gravity,repulsion;
    float   turbulence,neigbordhood,independence;
    
    //  Stars/constelations
    //
    void loadStarts( string _file);
    vector< wParticle* > stars;
    
};