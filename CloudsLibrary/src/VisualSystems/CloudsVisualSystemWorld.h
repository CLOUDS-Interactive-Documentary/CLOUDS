//
//  CloudsVisualSystemWorld.h
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/13/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"

#include "wStar.h"
#include "wPoint.h"
#include "wParticle.h"
#include "wSatellite.h"

class CloudsVisualSystemWorld : public CloudsVisualSystem {
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
    void        loadVbo(ofVboMesh &_vbo, string _file);
    float       wireSphereScale, solidSphereScale;
    float       wireSphereAlpha, solidSphereAlpha;
    float       coastAlpha, riversAlpha;
    ofVboMesh   coastVbo;
    ofVboMesh   riversVbo;

    //  Cities
    //
    void loadWorldPoints(string _file);
    vector< wPoint > worldPoints;
    float   pointNoisePeaks;
    float   rippleThreshold;
    
    //  Flocking particles
    //
//    void loadParticles( string _file );
    vector<wParticle*> particles;
    ofPoint globalOffset;
    float   nMaxPoints;
    float   initialForce;
    float   density,gravity,repulsion;
    float   turbulence,neigbordhood,independence;
    
    //  Stars/constelations
    //
    void loadStarts( string _file);
    vector< wStar* > stars;
    vector< string > constelations;
    string  selectedConstelation;
    float   constelationMin,constelationMax, constelationRnd;
    
    //  Satellites
    //
    vector< wSatellite > satellites;
};