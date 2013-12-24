//
//  CloudsSVGMesh.h
//  CloudsHUDDesigner
//
//  Created by James George on 12/24/13.
//
//

#pragma once;

#include "ofMain.h"
#include "ofxXmlSettings.h"

class CloudsSVGMesh
{
  public:
	bool setup(string file);
	void draw();
	ofRectangle getBounds();

	//of document
	float getWidth();
	float getHeight();

  protected:
	void recurseSVGTag(ofxXmlSettings& xml, string parentId);
	vector<ofVboMesh> meshes;
	map<string,int> meshIdIndex;
	ofRectangle document;
	ofRectangle bounds;

};