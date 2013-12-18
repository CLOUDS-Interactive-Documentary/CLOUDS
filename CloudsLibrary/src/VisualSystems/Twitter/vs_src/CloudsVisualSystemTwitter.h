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
	void selfSetDefaults();
	
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
    
    
    //twitter feed
    void drawFeed();
    //i/o stuff
    void loadJSONData(string folderName);
    void addUsersFromMentions();
    void createPajekNetwork(string outputFileName);
    void parseClusterNetwork(string fileName);
    void createNewGraph(string outputFileName, string inputDataFolder);
    

    //data stuff
    int getUserIdByName(string name);
    vector<Tweeter> getTweetersForDate(int index);
    Tweeter& getTweeterByID(int _id );
    string getDateAsString(Date d);
    void drawTweetsForDate(int index);
    void CompareDates(Date d1,Date d2);
    void loadGraphFromPath(string filePath);
    void clearData();
    
    //shader stuff
    void reloadShaders();

    //text stuff
    ofxJSONElement result;
    ofxFTGLSimpleLayout font;
    ofxFTGLTextAlignment textAlign;

    float stringWidth;
    float fontSize;
    int minUserMentions;
    
    //draw stuff
    void loadMesh();
    void updateMeshFromTweets(int index);
    void updateMesh();
    void drawText(string text, ofVec3f pos);
    void drawText2D(string text, ofVec2f pos);

    //helpers 
    vector<Date> dateIndex;
    map<string,int>dateIndexMap;
    set<pair<int,int> > links;
    map<pair<string, string>, pair<int, int> >lineIndexPairs;
    map<string,int> numberOfMentions;
    int currentDateIndex;
    void updateLabelWithCurrentMeshName(string name);

    // Ma boooooools
    bool bRenderMesh;
    bool bRenderText;
    bool bRenderFeed;
    bool bAnimate;

    
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
    ofShader lineShader;
    ofShader pointsShader;
    float meshExpansion;
    float pointSize;
    ofxUISuperCanvas* clusterGui;
    ofxUISuperCanvas* textGui;
    ofxUISuperCanvas* twitterFeedGui;
    
    vector<Tweeter*> activeTweeters;
    vector<string> activeTweets;

    void addColorToGui(ofxUISuperCanvas* gui, string prefix, ofFloatColor& col, bool doAlpha = true);
	
	float edgeInterpolateExponent;
	ofFloatColor getRGBfromHSV(ofFloatColor hsv);
	//this is the base color of the lines close to the nodes
	ofFloatColor lineNodeBaseHSV;
	//this is the base color of the lines at the midpoint
	ofFloatColor lineEdgeBaseHSV;
	
	//this is the pop color of the lines close to the nodes
	ofFloatColor lineNodePopHSV;
	//this is the pop color of the lines at the midpoint
	ofFloatColor lineEdgePopHSV;
    
	//this is the base color of the node
	ofFloatColor nodeBaseColorHSV;
	//this is the pop color of the node
	ofFloatColor nodePopColorHSV;
	   
    ofFloatColor textColor;
    ofFloatColor textColorModifier;
    
    string currentMeshFilePath;
    string currentMeshFileName;
    int theme = 0;
    float xScale,yScale, zScale;
    
    int rotationRate;
    float rotation ;
    float rotationAmount;
    bool rotateModel;
    ofRectangle tweetFeedRect;
    
	
};
