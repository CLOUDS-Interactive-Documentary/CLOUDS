//
//  CloudsSVGMesh.h
//  CloudsHUDDesigner
//
//  Created by James George on 12/24/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

typedef struct {
	ofVboMesh mesh;
	string id;
	bool fill;
	ofRectangle bounds;
    bool visible;
} SVGMesh;

class CloudsSVGMesh
{
  public:
    CloudsSVGMesh();
	bool loadDirectory(string file);
	bool load(string file);
	void draw();
	ofRectangle getBounds();
    
	string sourceFileName;
	
	//of document
	float getWidth();
	float getHeight();
	vector<SVGMesh>& getMeshes();
	
	//will be null if ID isnt present
	SVGMesh* getMeshByID(string meshId);
    
    float alpha;
	
  protected:
	void recurseSVGTag(ofxXmlSettings& xml, string parentId, float parentOpacity);
	vector<SVGMesh> meshes;
	map<string,int> meshIdIndex;
	
	ofRectangle document;
	ofRectangle bounds;

};