#pragma once

#include "ofMain.h"
#include "CloudsVisualSystemManager.h"
#include "ofxXmlSettings.h"

struct VS_PRESET {
    
    std::string name;
    float fps;
    float loadtime;
    
};

struct VS {
    
    std::string name;
    std::vector<VS_PRESET> presets;
    
};


class testApp : public ofBaseApp{
  public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void advanceSystem();
	void shuffleSystemIndices();
	void toLog( const std::string& to_log );
	void logEnd();
    void toSpeedLog( const std::string& vs_name, const std::string& preset, float loadtime, float avg_fps );
    void writeSpeedLog();
    
	float elapsedTime( float& start_time );

	CloudsVisualSystemManager systemManager;
	CloudsVisualSystem* currentSystem;
	vector<int> presetIndices;
	int currentSystemIndex;
	float lastSystemChangedTime;

	ofBuffer log;
        
	char     logFilename[1024];
	float    startTime;
	bool     bForward;
    
    bool     bEnableSpeedLog;
    
    int      vsFrameCount;
    std::string version, card, vendor, speedLogFilenameXML, speedLogFilenameCSV;
    bool     prevLoadedSpeedLog;
    float    vsLoadTime;
    float    vsStartTime;
    float    vsAvgFPS;
    float    vsLastSampleFrame, vsFPSLastSampleTime;
    
	vector< std::pair<string,string> > blackList;
    vector<VS> vs;

};
