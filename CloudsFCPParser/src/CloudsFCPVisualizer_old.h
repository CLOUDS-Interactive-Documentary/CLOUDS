#pragma once

#include "MSAPhysics2D.h"
#include "ofMain.h"
#include "CloudsFCPParser.h"

typedef struct {
    msa::physics::Particle2D* particle;
	string keyword;
} Node;

typedef struct {
	msa::physics::Spring2D* spring;
	vector<ClipMarker> clips;
} Edge;

class CloudsFCPVisualizer {
  public:
    CloudsFCPVisualizer();
    CloudsFCPParser* database;
	
    void setup();
    
    void setupPhysics();
    void createClusterPhysics();
    
    void updatePhysics();
    void drawPhysics();

    void exportForGraphviz();
    
    void drawGrid();
 
    map< pair<string,string>, int > sharedClips;
    
    int width,height;
    msa::physics::World2D physics;

    ofTrueTypeFont font;
    ofTrueTypeFont largFont;
	
	void addTagToPhysics(string tag);
	
    void mousePressed(ofMouseEventArgs& args);
    void mouseMoved(ofMouseEventArgs& args);
    void mouseDragged(ofMouseEventArgs& args);
    void mouseReleased(ofMouseEventArgs& args);
    
    void keyPressed(ofKeyEventArgs& args);
    void keyReleased(ofKeyEventArgs& args);

	map<msa::physics::Particle2D*, string> particleName;
	map<string, msa::physics::Particle2D*> particlesByTag;
	
	set< pair<msa::physics::Particle2D*,msa::physics::Particle2D*> > springs;
	map< msa::physics::Spring2D*, int> clipsInSpring;
	set< msa::physics::Spring2D*> linkSprings;
	
	msa::physics::Particle2D* centerNode;
	
	void windowResized(ofResizeEventArgs& args);
	
	void clear();
	
	ofRectangle totalRectangle;
	float currentScale;
	ofVec2f currentTop;

	string selectionTitle;
	vector<ClipMarker> selectedClips;
	vector<CloudsLink> selectedLinks;
	
  protected:
	ofColor hoverColor;
	ofColor selectedColor;
	ofColor nodeColor;
	ofColor lineColor;
	
	ofVec2f graphPointForScreenPoint(ofVec2f screenPoint);
	ofVec2f screenPointForGraphPoint(ofVec2f graphPoint);
	
    msa::physics::Particle2D* selectedParticle;
    msa::physics::Particle2D* hoverParticle;
	msa::physics::Particle2D* particleNearPoint(ofVec2f point);
	
	msa::physics::Spring2D* selectedSpring;
	msa::physics::Spring2D* hoverSpring;
	msa::physics::Spring2D* springNearPoint(ofVec2f point);
	
	float cursorRadius;
	float minRadius, maxRadius;
	float minMass, maxMass;
	float radiusForNode(float mass);
	
	bool hasParticle(string tagName);
};
