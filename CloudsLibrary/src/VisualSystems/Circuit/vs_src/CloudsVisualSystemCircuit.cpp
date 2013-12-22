//
//  CloudsVisualSystemCircuit.cpp
//

#include "CloudsVisualSystemCircuit.h"

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemCircuit::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Blip Speed", 0, 10, &blipSpeed);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemCircuit::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemCircuit::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

void CloudsVisualSystemCircuit::generateCircuit(){
	
	heightMap.loadImage(getVisualSystemDataPath() + "images/circuits.jpeg");
    heightMap.setImageType(OF_IMAGE_GRAYSCALE);
    int height = heightMap.getHeight();
    int width = heightMap.getWidth();
	for (int y = 0; y < height; y++){
		for (int x = 0; x<width; x++){
            if(heightMap.getPixelsRef().getColor(x, y).getBrightness() > 128){
                heightMap.getPixelsRef().setColor(x, y, ofColor(255));
            }
            else{
                heightMap.getPixelsRef().setColor(x, y, ofColor(0));
            }
        }
    }
    
    contourFinder.setSimplify(false);
    contourFinder.setMinArea(2);
    contourFinder.setThreshold(100);
    //contourFinder.findContours(cam);
    contourFinder.findContours(heightMap);
    //contourLines.reserve(contourFinder.getPolylines().size());
	lineMesh.clear();
    for(int i = 0; i < contourFinder.getPolylines().size(); i++){
		
        contourLines.push_back(ofPolyline());
		if(contourFinder.getPolyline(i).size() == 0){
			continue;
		}
				
        for(int p = 0; p < contourFinder.getPolyline(i).size(); p++){
			ofVec3f linePoint = contourFinder.getPolyline(i).getVertices()[p];
			linePoint.z = linePoint.y;
			linePoint.y = 0;
			
            contourLines[i].addVertex(linePoint);
			
			Blip b;
            b.lineIndex = i;
            b.vertIndex = p;
			b.meshIndex = blipMesh.getVertices().size();
			
            if(ofRandomuf() > .5){
                b.color = ofFloatColor::white;
            }else{
                b.color = ofFloatColor::black;
            }
			blipMesh.addColor(b.color);
			blipMesh.addVertex(ofVec3f(0,0,0));
            blips.push_back(b);
        }
        
		contourLines[i].close();

		//set the handle
		lineMesh.addColor(ofFloatColor(0,0));
		int numVertices = contourLines[i].getVertices().size();
		lineMesh.addVertex(contourLines[i].getVertices()[0]);
		for(int v = 0; v < contourLines[i].getVertices().size(); v++){
			lineMesh.addColor(ofFloatColor(1,1));
			lineMesh.addVertex(contourLines[i].getVertices()[v]);
		}
		//set the handle
		lineMesh.addColor(ofFloatColor(0,0));
		lineMesh.addVertex(contourLines[i].getVertices()[numVertices-1]);
    }
	
    lineMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	
    contourFinder.setSimplify(true);
    contourFinder.findContours(heightMap);
	
	//add one vertex to the mesh for each pixel
	for (int y = 0; y < height; y++){
		for (int x = 0; x<width; x++){
            
            float brightness = heightMap.getPixelsRef().getColor(x, y).getBrightness();
            if(brightness == 0){
                elevated.push_back(heightMesh.getVertices().size());
            }
			
			heightMesh.addVertex(ofVec3f(x,0,y));
            // this replicates the pixel array within the camera bitmap...
            heightMesh.addTexCoord(ofVec2f(x,y));
            float brightUp = brightness;
            float brightDown = brightness;
            float brightLeft = brightness;
            float brightRight = brightness;
            if(x<width-1){
                brightRight = heightMap.getPixelsRef().getColor(x+1, y).getBrightness();
            }
            if(x>1){
                brightLeft = heightMap.getPixelsRef().getColor(x-1, y).getBrightness();
            }
            if(y<height-1){
                brightDown = heightMap.getPixelsRef().getColor(x, y+1).getBrightness();
            }
            if(y>1){
                brightUp = heightMap.getPixelsRef().getColor(x, y-1).getBrightness();
            }
            
            ofVec3f upPos = ofVec3f(x,brightUp*.05,y-1);
            ofVec3f downPos = ofVec3f(x,brightDown*.05,y+1);
            ofVec3f leftPos = ofVec3f(x-1,brightLeft*.05,y);
            ofVec3f rightPos = ofVec3f(x+1,brightRight*.05,y);
            
            heightMesh.addNormal((upPos - downPos).getCrossed(leftPos-rightPos).normalize());
		}
	}
	
	for (int y = 0; y<height-1; y++){
		for (int x=0; x<width-1; x++){
			heightMesh.addIndex(x+y*width);				// 0
			heightMesh.addIndex((x+1)+y*width);			// 1
			heightMesh.addIndex(x+(y+1)*width);			// 10
			
			heightMesh.addIndex((x+1)+y*width);			// 1
			heightMesh.addIndex((x+1)+(y+1)*width);		// 11
			heightMesh.addIndex(x+(y+1)*width);			// 10
		}
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemCircuit::selfSetupSystemGui(){
	
}

void CloudsVisualSystemCircuit::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemCircuit::selfSetupRenderGui(){

}

void CloudsVisualSystemCircuit::guiRenderEvent(ofxUIEventArgs &e){
	
}

//This is called whenever a new preset is loaded, before selfSetup()
//use it to ensure all your simple variables are initialized to an
//acceptable default state
void CloudsVisualSystemCircuit::selfSetDefaults(){
	blipSpeed = 3.0;
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemCircuit::selfSetup(){
	generateCircuit();
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemCircuit::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemCircuit::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemCircuit::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemCircuit::selfUpdate(){
	
    for(int i = 0; i < blips.size(); i++){
        ofPolyline& line = contourLines[ blips[i].lineIndex ];
        int numVerts = line.getVertices().size();
        blips[i].vertIndex = fmod(blips[i].vertIndex + blipSpeed, numVerts);
        int up = int(ceil(blips[i].vertIndex)) % numVerts;
        int dn = int(floor(blips[i].vertIndex)) % numVerts;
        float alpha = blips[i].vertIndex - dn;
        blips[i].pos = line.getVertices()[dn].getInterpolated(line.getVertices()[up], alpha);
		blipMesh.getVertices()[blips[i].meshIndex].set(blips[i].pos);
    }
    
//    if(lastElevation  != currentElevation){
//        for(int i = 0; i < elevated.size(); i++){
//            heightMesh.getVertices()[ elevated[i] ].y = -currentElevation;
//        }
//        lastElevation = currentElevation;
//    }

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemCircuit::selfDraw(){
	
	
    ofSetColor(255);
    ofEnableAlphaBlending();
	lineMesh.draw();
	
//	blipMesh.clear();
    glDisable(GL_DEPTH_TEST);
    float blipAlpha = 1.0;
	
	glPointSize(4);
	
//    for(int i = 0; i < blips.size(); i++){
//        blipMesh.se(blips[i].pos);
////        blipMesh.addColor(blips[i].color);
//    }
    
    blipMesh.drawVertices();

}

// draw any debug stuff here
void CloudsVisualSystemCircuit::selfDrawDebug(){
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemCircuit::selfDrawBackground(){

}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemCircuit::selfEnd(){
		
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemCircuit::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemCircuit::selfKeyPressed(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemCircuit::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemCircuit::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCircuit::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCircuit::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCircuit::selfMouseReleased(ofMouseEventArgs& data){
	
}