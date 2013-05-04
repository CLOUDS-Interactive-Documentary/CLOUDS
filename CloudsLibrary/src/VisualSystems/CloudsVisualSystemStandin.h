
#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
#include <set>


class CloudsFakeVS {
  public:
	vector<string> keywords;
	string name;
	string filePath;
	ofImage image;
	ofVideoPlayer player;
	void draw(){
		if(image.isAllocated()){
			image.draw(0,0);
		}
		else if(player.isLoaded()){
			player.draw(0,0);
		}
	}
};

class CloudsVisualSystemStandin : public CloudsVisualSystem {
  public:
	
	CloudsVisualSystemStandin();
	
	virtual void setup();
	
	virtual bool isReleventToKeyword(string keyword);
	virtual string getSystemName();
	
	//these events are registered only when running the simulation
	virtual void update(ofEventArgs & args);
	virtual void draw(ofEventArgs & args);
	
	//INTERACTION EVENTS -- registered only
	virtual void keyPressed(ofKeyEventArgs & args);
	virtual void keyReleased(ofKeyEventArgs & args);
	
	virtual void mouseDragged(ofMouseEventArgs & args);
	virtual void mouseMoved(ofMouseEventArgs & args);
	virtual void mousePressed(ofMouseEventArgs & args);
	virtual void mouseReleased(ofMouseEventArgs & args);
	
	//application exit, clean up and don't crash
	virtual void exit(ofEventArgs & args);

	
  protected:
	
	virtual void begin();
	virtual void end();

	bool systemSelected;
	CloudsFakeVS* currentVisualSystem;
	vector<CloudsFakeVS> fakeVisualSystems;
	set<string> allKeywords;
	map<string, vector<CloudsFakeVS> > keywordToSystems;
	
	
};