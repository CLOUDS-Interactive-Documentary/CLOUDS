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
} SVGMesh;

class CloudsSVGMesh
{
  public:
	bool load(string file);
	void draw();
	ofRectangle getBounds();

	string sourceFileName;
	
	//of document
	float getWidth();
	float getHeight();
	vector<SVGMesh>& getMeshes();
	
  protected:
	void recurseSVGTag(ofxXmlSettings& xml, string parentId);
	vector<SVGMesh> meshes;
	map<string,int> meshIdIndex;
	
	ofRectangle document;
	ofRectangle bounds;

};