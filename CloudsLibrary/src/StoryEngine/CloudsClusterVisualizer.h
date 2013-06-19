//
//  CouldsClusterVisualizer.h
//  CloudsFCPParser
//
//  Created by Surya Mattu on 12/06/13.
//
//

#ifndef __CloudsFCPParser__CloudsClusterVisualizer__
#define __CloudsFCPParser__CloudsClusterVisualizer__

#include <iostream>
//#include "ofxXmlSettings.h"
//#include "CloudsClip.h"
//#include "CloudsLink.h"
//#include <set>
#include "MSAPhysics2D.h"
#include "ofMain.h"
#include "CloudsFCPParser.h"
#include "CloudsEvents.h"
#include "ParticleEdge.h"


class CloudsClusterVisualiser {
public:
    CloudsClusterVisualiser();
    
    void setup(CloudsFCPParser& database);
    
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
    
    void addAllClipsWithAttraction();
    void addTagToPhysics(string tag);
    void clipBegan(CloudsStoryEventArgs& args);
    
    //void addLinksToPhysics(CloudsClip& center, vector<CloudsClip>& connections, vector<float>& scores);
    
    void mousePressed(ofMouseEventArgs& args);
    void mouseMoved(ofMouseEventArgs& args);
    void mouseDragged(ofMouseEventArgs& args);
    void mouseReleased(ofMouseEventArgs& args);
    
    void keyPressed(ofKeyEventArgs& args);
    void keyReleased(ofKeyEventArgs& args);
    
    map<msa::physics::Particle2D*, CloudsClip> particleToClip;
    map<msa::physics::Particle2D*, string> particleName;
    map<string, msa::physics::Particle2D*> particlesByTag;
    
    map< pair<msa::physics::Particle2D*,msa::physics::Particle2D*>, msa::physics::Spring2D*> springs;
    map< msa::physics::Spring2D*, vector<string> > keywordsInSpring;
    set< msa::physics::Spring2D*> linkSprings;
    set< msa::physics::Spring2D*> suppressedSprings;
    map< msa::physics::Particle2D*, int> particleBirthOrder;
    
    vector< CloudsClip > currentOptionClips;
    vector< msa::physics::Particle2D* > currentOptionParticles;
    vector< CloudsClip > pathByClip;
    vector< msa::physics::Particle2D* > pathByParticles;
    vector< msa::physics::Spring2D* > pathBySprings;
    map< msa::physics::Spring2D*, float > springScores;
    vector<string> clipLog;
    
    map<ParticleEdge*, float> edgeScores;
    map<pair<msa::physics::Particle2D*,msa::physics::Particle2D*>,ParticleEdge*> edgePairs;
    vector<ParticleEdge*> edges;
    map<ParticleEdge*,vector<string> > keywordsinEdges;
    set<ParticleEdge*> linkEdges;
    set<ParticleEdge*> suppressedEdges;
    vector<ParticleEdge*> pathByEdges;
    
    string currentTopic;
    
    //	set<string> allTags;
    //	vector<string, float> tagRadius;
    //	vector<string, ofVec2f> tagCenter;
    
    msa::physics::Particle2D* centerNode;
    
    void windowResized(ofResizeEventArgs& args);
    
    void storyBegan(CloudsStoryEventArgs& clips);
    void clear();
    
    ofRectangle totalRectangle;
    float currentScale;
    ofVec2f currentTop;
    
    ofRectangle zoomRectangle;
    bool zoomView;
    string selectionTitle;
    vector<CloudsClip> selectedClips;
    
    bool isEdgeSelected();
    bool isSelectedEdgeLink();
    
    CloudsClip getEdgeSource();
    CloudsClip getEdgeDestination();
    void linkedEdge();
    void unlinkEdge();
    
    bool isSelectedEdgeSuppressed();
    void suppressEdge();
    void unsuppressEdge();
    
    void linkLastEdge();
    void suppressLastEdge();
    
    void linkPathEdge(int pathIndex);
    void suppressPathEdge(int pathIndex);
    
    bool getPathChanged();
    
    float springStrength;
    float restLength;
    float repulsionForce;
    float minRadius, maxRadius;
    float zoom;
    
protected:
    
    CloudsFCPParser* database;
    
    ofColor visitedColor;
    ofColor abandonedColor;
    ofColor hoverColor;
    ofColor selectedColor;
    ofColor nodeColor;
    ofColor currentNodeColor;
    ofColor lineColor;
    ofColor traceColor;
    ofColor suppressedColor;
    
    ofVec2f graphPointForScreenPoint(ofVec2f screenPoint);
    ofVec2f screenPointForGraphPoint(ofVec2f graphPoint);
    
    msa::physics::Particle2D* selectedParticle;
    msa::physics::Particle2D* hoverParticle;
    msa::physics::Particle2D* particleNearPoint(ofVec2f point);
    
    msa::physics::Spring2D* selectedSpring;
    msa::physics::Spring2D* hoverSpring;
    msa::physics::Spring2D* springNearPoint(ofVec2f point);
    
    ParticleEdge* selectedEdge;
    ParticleEdge* hoverEdge;
    ParticleEdge* edgeNearPoint(ofVec2f point);
    
    map< msa::physics::Particle2D*, ofVec2f > dampendPositions;
    map<ParticleEdge*,ofVec2f> dampenedEdgePositions;
    float cursorRadius;
    float minMass, maxMass;
    float minScore, maxScore;
    float radiusForNode(float mass);
    
    bool pathChanged;
    bool hasParticle(string tagName);
    
};
#endif /* defined(__CloudsFCPParser__CouldsClusterVisualizer__) */
