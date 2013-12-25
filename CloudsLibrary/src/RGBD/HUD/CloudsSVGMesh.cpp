//
//  CloudsSVGMesh.cpp
//  CloudsHUDDesigner
//
//  Created by James George on 12/24/13.
//
//

#include "CloudsSVGMesh.h"

string strrephack(string input, string searchStr, string replaceStr){
	ofStringReplace(input, searchStr, replaceStr);
	return input;
}

bool CloudsSVGMesh::load(string file){
	ofxXmlSettings svg;
	if(!svg.loadFile(file)){
		ofLogError("CloudsSVGMesh::setup") << "Couldn't load svg " << file << (ofFile(file).exists() ? "." : ". The file does not exist.");
		return false;
	}
	
	sourceFileName = file;
	
	//example header
//	<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" width="1440px"
//	height="900px" viewBox="0 0 1440 900" enable-background="new 0 0 1440 900" xml:space="preserve">
	document = ofRectangle(ofToInt(strrephack(svg.getAttribute("svg", "x", "0px"), "px", "")),
						   ofToInt(strrephack(svg.getAttribute("svg", "y", "0px"), "px", "")),
						   ofToInt(strrephack(svg.getAttribute("svg", "width", "0px"), "px", "")),
						   ofToInt(strrephack(svg.getAttribute("svg", "height", "0px"), "px", "")));
	
	svg.pushTag("svg");
	recurseSVGTag(svg, "");
	svg.popTag();//svg
	
	//calculate various bounds
	int totalVerts = 0;
	if(meshes.size() > 0){
		//find the bounds
		bounds = ofRectangle(meshes[0].mesh.getVertices()[0].x,
							 meshes[0].mesh.getVertices()[0].y,
							 0,0);
		for(int i = 0; i < meshes.size(); i++){
			meshes[i].bounds = ofRectangle(meshes[i].mesh.getVertices()[0].x,
										   meshes[i].mesh.getVertices()[0].y,
										   0,0);
			for(int v = 0; v < meshes[i].mesh.getNumVertices(); v++){
				bounds.growToInclude(meshes[i].mesh.getVertices()[v]);
				meshes[i].bounds.growToInclude(meshes[i].mesh.getVertices()[v]);
				totalVerts++;
			}
		}
		cout << "Bounds for SVG " << file << " is " << bounds.getMin() << " - " << bounds.getMax() << endl;
		cout << "Found " << meshes.size() << " meshes with " << totalVerts << " vertices" << endl;		
	}
}

void CloudsSVGMesh::recurseSVGTag(ofxXmlSettings& xml, string parentId){

	ofMesh strokeMesh;
	ofMesh fillMesh;
	//find geo on this layer
	//<line display="inline" opacity="0.49" fill="none" stroke="#FFFFFF" stroke-miterlimit="10" x1="87.313" y1="612.5" x2="749.313" y2="612.5"/>
	int numLineTags = xml.getNumTags("line");
	for(int l = 0; l < numLineTags; l++){
		strokeMesh.addVertex(ofVec3f(xml.getAttribute("line", "x1", 0., l),
									 xml.getAttribute("line", "y1", 0., l), 0));
		strokeMesh.addVertex(ofVec3f(xml.getAttribute("line", "x2", 0., l),
									 xml.getAttribute("line", "y2", 0., l), 0));

		string hexColor = xml.getAttribute("line", "stroke", "#FFFFFF", l);
		hexColor.erase(hexColor.begin());//kill the #
		ofColor color = ofColor::fromHex( ofHexToInt(hexColor) );
		if(xml.attributeExists("line","opacity",l)){
			color.a = xml.getAttribute("line", "opacity", 0., l)*255.;
		}

		strokeMesh.addColor(color);
		strokeMesh.addColor(color);
	}
	
	//sample rect tag
	//<rect x="519.813" y="144.5" display="inline" opacity="0.02" fill="none" stroke="#FFFFFF" stroke-miterlimit="10" width="29" height="18"/>
	int numRectTags = xml.getNumTags("rect");
	for(int r = 0; r < numRectTags; r++){
		ofRectangle rect(xml.getAttribute("rect", "x", 0., r),
						 xml.getAttribute("rect", "y", 0., r),
						 xml.getAttribute("rect", "width", 0., r),
						 xml.getAttribute("rect", "height", 0., r));

		//pull the vertices out as vec3s		
		ofVec3f a = rect.getTopLeft();
		ofVec3f b = rect.getTopRight();
		ofVec3f c = rect.getBottomRight();
		ofVec3f d = rect.getBottomLeft();
		
		string fillProperty = xml.getAttribute("rect", "fill", "none",r);
		bool isLines = fillProperty == "none";
		
		string hexColor = xml.getAttribute("rect", (isLines ? "stroke" : "fill"), "#FFFFFF", r);
		hexColor.erase(hexColor.begin());//kill the #
		ofColor color = ofColor::fromHex( ofHexToInt(hexColor));
		if(xml.attributeExists("line","opacity",r)){
			color.a = xml.getAttribute("rect", "opacity", 0., r)*255.;
		}

		if(isLines){

			//we may wish to make this a line loop....
			//convert to line segments
			strokeMesh.addVertex(a);
			strokeMesh.addVertex(b);
			
			strokeMesh.addVertex(b);
			strokeMesh.addVertex(c);
			
			strokeMesh.addVertex(c);
			strokeMesh.addVertex(d);
			
			strokeMesh.addVertex(d);
			strokeMesh.addVertex(a);
			
			for(int i = 0; i < 8; i++){
				strokeMesh.addColor(color);
			}
		}
		else{
			
			
			fillMesh.addVertex(a);
			fillMesh.addVertex(b);
			fillMesh.addVertex(d);

			fillMesh.addVertex(b);
			fillMesh.addVertex(d);
			fillMesh.addVertex(c);

			for(int i = 0; i < 6; i++){
				fillMesh.addColor(color);
			}
		}
	}
	
	//if we added go throw it in the group, and give it a name
	if(strokeMesh.getNumVertices() > 0){
		SVGMesh m;
		m.id = parentId;
		if(m.id != ""){
			meshIdIndex[m.id] = meshes.size();
		}
		m.mesh = ofVboMesh(strokeMesh);
		m.mesh.setMode(OF_PRIMITIVE_LINES);
		m.mesh.setUsage(GL_STATIC_DRAW);
		m.fill = false;
		meshes.push_back(m);
	}
	
	if(fillMesh.getNumVertices() > 0){
		SVGMesh m;
		m.id = parentId;
		if(m.id != ""){
			meshIdIndex[m.id] = meshes.size();
		}
		m.mesh = ofVboMesh(fillMesh);
		m.mesh.setMode(OF_PRIMITIVE_TRIANGLES);
		m.mesh.setUsage(GL_STATIC_DRAW);
		m.fill = true;
		meshes.push_back(m);
	}
	
	//search other g tags
	int numGTags = xml.getNumTags("g");
	for(int i = 0; i < numGTags; i++){
		//is g tag displayed?
		if(xml.attributeExists("g", "display", i) &&
		   xml.getAttribute("g", "display", "", i) == "none")
		{
			//skipit
			continue;
		}
		//does g tag have id;
		if(xml.attributeExists("g", "id", i)){
			parentId = xml.getAttribute("g", "id", "",i);
		}
		xml.pushTag("g",i);
		recurseSVGTag(xml, parentId);
		xml.popTag();
	}
}

void CloudsSVGMesh::draw(){
	for(int i = 0; i < meshes.size(); i++){
		meshes[i].mesh.draw();
	}
}

vector<SVGMesh>& CloudsSVGMesh::getMeshes(){
	return meshes;
}

ofRectangle CloudsSVGMesh::getBounds(){
	return bounds;
}
float CloudsSVGMesh::getWidth(){
	return document.width;
}
float CloudsSVGMesh::getHeight(){
	return document.height;
}
