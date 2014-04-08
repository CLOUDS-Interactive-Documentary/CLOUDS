
#include "ofMain.h"
#include "CloudsFCPParser.h"
class CloudsSVGParser
{
  public:
	
	CloudsSVGParser();
	
	vector<pair<string, ofVec2f> > keywordCentroids;
	CloudsFCPParser* parser;
	void loadFile(string path);
	void exportFile(string path);
	
	void draw();
};