//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
// Welcome to the EMPTY CloudsVisualSystem
//
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "ofxJSONElement.h"
#include "Tweeter.h"
#include "ofxFTGL.h"
#include "ofxBillboard.h"
//--------------------------------------------------------------
//--------------------------------------------------------------
class CloudsVisualSystemTwitter : public CloudsVisualSystem
{
  public:
    
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName() {
		return "Twitter";
	}

	//These methods let us add custom GUI parameters and respond to their events
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    void selfSetup();
    void selfBegin();
	void selfPresetLoaded(string presetPath);
    void selfSceneTransformation();
	void selfUpdate();
    void selfDraw();
	void selfDrawRGBD();
	void selfDrawDebug();
	void selfDrawBackground();
	void selfEnd();
    void selfExit();
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void initSystem(string filePath);
    
    //i/o stuff
    void loadJSONData();
    void addUsersFromMentions();
    void createPajekNetwork();
    void parseClusterNetwork(string fileName);
    void createNewGraph();
    

    //data stuff
    int getUserIdByName(string name);
    vector<Tweeter> getTweetersForDate(int index);
    Tweeter& getTweeterByID(vector<Tweeter>& tweeters, int _id );
    string getDateAsString(Date d);
    void drawTweetsForDate(int index);
    void CompareDates(Date d1,Date d2);
    void loadGraphFromPath(string filePath);
    void clearData();

    //text stuff
    ofxJSONElement result;
    ofxFTGLSimpleLayout font;
    ofxFTGLTextAlignment textAlign;
    bool bRenderText;
    float stringWidth;
    float fontSize;
    
    //draw stuff
    void loadMesh();
    void updateMesh();
    void reloadMeshColor();
    void updateMeshFromTweets(int index);
    void drawText(string text, ofVec3f pos);
    bool bRenderMesh;

    //helpers 
    vector<Date> dateIndex;
    map<string,int>dateIndexMap;
    set<pair<int,int> > links;
    map<pair<string, string>, pair<int, int> >lineIndexPairs;
    map<string,int> numberOfMentions;
    int currentDateIndex;
    

    
protected:
    ofTrueTypeFont listFont;
    ofColor listColor;
        
    
    vector<Tweeter> tweeters;
    stringstream ss;
    
    ofVboMesh nodeMesh;
    ofVboMesh edgeMesh;
    ofVec3f min,max;
    
    int refreshRate;
    float edgeDecayRate;

    ofImage sprite;
    ofShader clusterShader;
    float meshExpansion;
    float pointSize;
    ofxUISuperCanvas* clusterGui;
    ofxUISuperCanvas* textGui;

    vector<Tweeter*> activeTweeters;

    float baseHue,baseSat,baseBri,baseAlpha;
    float tweetHue, tweetSat, tweetBri, tweetAlpha;
    
    ofFloatColor baseColor;
    ofFloatColor baseModifier;
    
    ofFloatColor tweetColor;
    ofFloatColor tweetModifier;
    
    ofFloatColor nodeColor;
    ofFloatColor nodeModifier;
    
    ofFloatColor nodeActiveColor;
    ofFloatColor nodeActiveModifier;
    
    ofFloatColor nodeMidpointColor;
    ofFloatColor nodeMidpointModifier;
    
    ofFloatColor nodeActiveMidpointColor;
    ofFloatColor nodeActiveMidpointModifier;
    
    ofFloatColor textColor;
    ofFloatColor textColorModifier;
    
    string currentMeshFilePath;
    float xScale,yScale, zScale;
    
    bool rotateModel;
    
    
    
    
    
    
    
	
};
