//
//  CloudsVisualSystemWorld.h
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/13/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"

#include "wArc.h"
#include "wStar.h"
#include "wCity.h"
#include "wParticle.h"
#include "wSatellite.h"

class CloudsVisualSystemWorld : public CloudsVisualSystem {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
	void selfSetDefaults();
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    void selfDraw();
    void selfPostDraw();
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
    ofShader    haloShader;
    
    float       wireSphereScale, solidSphereScale, haloSphereScale;
    float       wireSphereAlpha, solidSphereAlpha, haloSphereAlpha;
    float       coastAlpha, riversAlpha;
    ofVboMesh   coastVbo;
    ofVboMesh   riversVbo;

    //  Cities
    //
    void loadCities(string _file);
    void loadSecCities(string _file);
    vector< wCity > cities;
    vector< wCity > secCities;
    float   citiesAlpha;
    
    float   pointNoisePeaks;
    float   rippleThreshold;
    ofVboMesh citiesMesh;
    
    //  Flocking particles
    //
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
    vector< wSatellite* > satellites;
    float   satLinksAlpha;
    float   satLinksDist;
    float   nMaxSatellites;
    float   blinkingAlpha;
    float   blinkingSpeed;
    //ofVboMesh satellitesMesh;
	ofMesh linkMesh;
	
    //  Arcs
    //
    vector<wArc>    arcs;
    float           arcsAlpha;
    float           arcsMax;

    //  Post
    //
    ofShader    postShader;
    ofTexture   postTexture;
    float       postChromaDist;
    float       postGrainDist;
};