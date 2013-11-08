
#pragma once

#include "ofMain.h"

#include "CloudsVisualSystem.h"

#include "Gesture.h"
#include "GestureRenderer.h"
#include "GestureRendererInPlace.h"
#include "GestureRendererTraveling.h"
#include "Gesture.h"
#include "YtPolygon.h"

#include <deque>
#include <set>


#define MAX_GESTURES    20
#define MAX_TOUCHES     11

class CloudsVisualSystemYellowTail : public CloudsVisualSystem {
  public:
	
	CloudsVisualSystemYellowTail();
	
	string getSystemName();
	
    void selfSetup();
    void selfSetupGuis();
    void selfPresetLoaded(string presetPath);
	
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
    
    void selfSetupTimeline();
    void selfSetupTimelineGui();
    void selfTimelineGuiEvent(ofxUIEventArgs &e);
 
	GestureStyle     getNewGestureStyle() { return mNewGestureStyle; }
	void    setNewGestureStyle(GestureStyle newGestureStyle) { mNewGestureStyle = newGestureStyle; }
	int     gestureCount() { return gestures.size(); }
	void	clearGestures();
	
  protected:
	
	ofxUISuperCanvas* customGui;
	
	void	updateGeometry();
	bool	bUpdatingGeometry;
	
	int		softMaxGestures;
	int		softMaxTouches;
	
	set<Gesture *> potentialGestures;
	deque<Gesture *> gestures;
	vector<Gesture *> fadingGestures;
	map<int, Gesture *> gestureForTouch;
	
	int		minimumTravelForNewPoint;
	
	int     previousActiveGestureCount;
	bool    neverTouchedBefore;
	
	GestureStyle mNewGestureStyle;

};