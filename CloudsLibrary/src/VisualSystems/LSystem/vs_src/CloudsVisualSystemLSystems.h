//
//  CloudsVisualSystemLSys.h
//  VSLSys
//
//  Created by Patricio Gonzalez Vivo on 5/25/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"

#include "LSystem.h"
#include "LNode.h"

class CloudsVisualSystemLSystems : public CloudsVisualSystem {
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
    
    //  Grid
    //
    void        buildGrid();
    ofVboMesh   grid;
    float       gridSize;
    float       gridResolution;
    float       gridCrossSize;
    float       gridCrossWidth;
    float       gridCrossAlpha;
    
    //  LSystem
    //
    void    buildLSystem();
    void    lineTo(ofPoint &_pnt);
    void    addNode(ofPoint &_pnt);
    int     isNode(ofPoint &_pnt);
    void    addBranch(ofMesh &_mesh, int _index, float _relativeTime, float _speed);
    
    vector<ofPolyline>  lsysLines;
    vector<LNode>       lsysNodes;
    
    ofMesh  lsysOriginal;
    string  lsysAxiom, lsysRule1, lsysRule2;
    float   lsysAngle, lsysScale, lsysDepth;
    float   lsysOriginalAlpha;
    
    //  Growing
    ofMesh  lsysGrowing;
    float   lsysGrowingBorn, lsysGrowingSpeed;
    float   lsysGrowingAlpha;
    vector<ofPoint>  dots;
    float   dotSize, dotAlpha;
    
    //  Flow
    float   lsysFlowSpeed;
    float   lsysFlowLenght;
    float   lsysFlowAlpha;
    
    //  Post
    //
    ofShader    postShader;
    ofTexture   postTexture;
    float       postChromaDist;
    float       postGrainDist;
    
    float   time;
};
