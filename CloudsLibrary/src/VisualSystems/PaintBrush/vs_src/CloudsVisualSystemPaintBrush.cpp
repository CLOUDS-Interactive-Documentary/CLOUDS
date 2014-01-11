//
//  CloudsVisualSystemPaintBrush.cpp
//  VSPaintBrush
//
//  Created by Patricio Gonzalez Vivo on 6/13/13.
//
//

#include "CloudsVisualSystemPaintBrush.h"

string CloudsVisualSystemPaintBrush::getSystemName()
{
	return "PaintBrush";
}

void CloudsVisualSystemPaintBrush::selfSetup()
{
	bIs2D = true;
        
    canvasSrc.allocate(getSharedRenderTarget().getWidth(),
					   getSharedRenderTarget().getHeight(), GL_RGBA32F_ARB);
    canvasSrc.begin();
    ofClear(0,0);
    canvasSrc.end();

    canvasDst.allocate(getSharedRenderTarget().getWidth(),
					   getSharedRenderTarget().getHeight(), GL_RGBA32F_ARB);
    canvasDst.begin();
    ofClear(0,0);
    canvasDst.end();
    
    bUseColorMap = false;
    
    // ColorMaps
    string colorMapPath = getVisualSystemDataPath() + "colorMaps/";
    ofDirectory dir;
    dir.listDir(colorMapPath);
    dir.sort();
    string tempName;
    ofImage tempImage;
    for (int i = 0; i < dir.numFiles(); i++) {
        tempName = dir.getName(i);
        tempImage.loadImage(dir.getPath(i));
		colorMapNames.push_back(tempName);
        colorMapPixelsMap[tempName] = new ofPixels(tempImage.getPixelsRef());
        if (colorMap == NULL) {
            colorMap = colorMapPixelsMap[tempName];
        }
	}
    mapX = mapY = 0;
    bMapForward = true;
    
    fadeAmount = 1.0f;
	
	
//	bClearBackground = false;
}

void CloudsVisualSystemPaintBrush::selfSetupSystemGui()
{
    sysGui->addSlider("brush_width",0, 50, &brushWidth);
    sysGui->addSlider("number_of_lines", 1, 50, &brushNumber);
    sysGui->addSlider("line_width", 1, 10, &lineWidth);
    sysGui->addSlider("damp", 0, 0.5, &brushDamp);
    sysGui->addSlider("k", 0.0, 0.5, &brushK);
    
    sysGui->addSlider("repulsion_rad", 0, 20, &brushRepRad);
    sysGui->addSlider("repulsion_pct", 0.0, 1.0, &brushRepPct);
    
    sysGui->addSlider("particles_threshold", 5.0, 100.0, &particlesThreshold);
    sysGui->addSlider("particles_turbulence", 0.0, 0.5, &particlesTurbulence);
    sysGui->addSlider("particles_alpha", 0.0, 1.0, &particlesAlpha);
    
    sysGui->addSlider("color_hue", 0.0, 1.0, &colorHue);
    sysGui->addSlider("color_lerp",0.0, 1.0, &colorLerp);
    sysGui->addSlider("color_random", 0.0, 0.02, &colorRandom);
    
    sysGui->addSlider("fade_amount", 0.0f, 0.5f, &fadeAmount);
    
    sysGui->addToggle("use color map", &bUseColorMap);
    sysGui->addLabel("color maps");
	sysGui->addRadio("color map", colorMapNames);
}

void CloudsVisualSystemPaintBrush::selfSetupRenderGui()
{
    
}

void CloudsVisualSystemPaintBrush::guiSystemEvent(ofxUIEventArgs &e)
{
    if (e.widget->getKind() == OFX_UI_WIDGET_TOGGLE && e.getToggle()->getValue()) {
		string name = e.getName();
        for (map<string, ofPixels *>::iterator it = colorMapPixelsMap.begin(); it != colorMapPixelsMap.end(); it++) {
			if (it->first == name) {
				colorMap = it->second;
			}
		}
	}
}

void CloudsVisualSystemPaintBrush::selfKeyPressed(ofKeyEventArgs & args){
    
}

void CloudsVisualSystemPaintBrush::selfUpdate()
{
    
    for (map<int, Brush *>::iterator it = brushes.begin(); it != brushes.end(); it++) {
        Brush * brush = it->second;
        brush->update();
        
        if (brush->getVel().length() < particlesThreshold) {
            ofFloatColor color;
            
            if (bUseColorMap) {
                color = colorMap->getColor(mapX, mapY);
                if (bMapForward) {
                    ++mapX;
                    if (mapX >= colorMap->getWidth()) {
                        ++mapY;
                        if (mapY >= colorMap->getHeight()) {
                            // back to start
                            mapX = 0;
                            mapY = 0;
                            bMapForward = true;
                        }
                        else {
                            // next line, going backwards
                            mapX = colorMap->getWidth() - 1;
                            bMapForward = false;
                        }
                    }
                }
                else {
                    --mapX;
                    if (mapX < 0) {
                        ++mapY;
                        if (mapY >= colorMap->getHeight()) {
                            // back to start
                            mapX = 0;
                            mapY = 0;
                            bMapForward = true;
                        }
                        else {
                            // next line, going forward
                            mapX = 0;
                            bMapForward = true;
                        }
                    }
                }
            }
            else {
                color.set(1, 0, 0);
                color.setHue(colorHue);
            }

            brush->setColor(color, colorLerp, colorRandom);
        }
        
        if (brush->bDown) {
            brush->addParticles(particles, particlesThreshold, particlesAlpha);
        }
    }
    
	drawPaint();
	
	//cout << "Mouse pressed?" << GetCloudsInputPressed() << endl;
	
	//	ofPopStyle()
}

void CloudsVisualSystemPaintBrush::drawPaint(){
	
    canvasDst.begin();
	ofClear(0,0);
	ofPushStyle();
    glDisable(GL_DEPTH_TEST);

	ofEnableAlphaBlending();
//	ofSetColor(255, 255);
	ofSetColor(255, .999999999*255);
	canvasSrc.draw(0, 0);
	
	ofSetColor(255, 255);
    
    //MA: replaced ofGetMouseX() with GetCloudsInputX()
	ofCircle(GetCloudsInputX(), GetCloudsInputY(), 20);
	
//	ofSetColor(255, 255, 255, fadeAmount*255);
//	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
//	ofRect(0,0,canvasDst.getWidth(),canvasDst.getHeight());
	
	ofDisableAlphaBlending();
	ofEnableAlphaBlending();
	ofSetColor(255);
    for(int i = particles.size()-1; i >= 0 ; i--){
        
        particles[i].addNoise(ofGetElapsedTimef(), particlesTurbulence);
        particles[i].update();
        
        if (particles[i].getVel().length() < 1.0 ){
            particles.erase(particles.begin()+i);
        } else {
            particles[i].draw();
            particles[i].trail.draw();
        }
    }

    vector<int> toRemove;
    for (map<int, Brush *>::iterator it = brushes.begin(); it != brushes.end(); it++) {
        Brush * brush = it->second;
        
        if (brush->bDown) {
            brush->draw();
        }
        else {
            toRemove.push_back(it->first);
        }
    }
	ofPopStyle();

    canvasDst.end();
	swap(canvasSrc, canvasDst);

    while(particles.size() > 500) {
        particles.erase(particles.begin());
    }
	
    // get rid of any idle brushes
    for (int i = 0; i < toRemove.size(); i++) {
        delete brushes[toRemove[i]];
        brushes.erase(toRemove[i]);
    }
}

void CloudsVisualSystemPaintBrush::selfDrawBackground()
{
    ofPushStyle();
    ofSetColor(255);
    glDisable(GL_DEPTH_TEST);
    //ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofEnableAlphaBlending();
    canvasSrc.draw(0, 0);

//    drawPaint();
	
    if (bDebug){
        ofSetColor(255);
        for (map<int, Brush *>::iterator it = brushes.begin(); it != brushes.end(); it++) {
            it->second->drawDebug();
        }
    }
    
    ofPopStyle();
}


void CloudsVisualSystemPaintBrush::selfSetupGuis()
{
    
}

void CloudsVisualSystemPaintBrush::selfAutoMode()
{
    
}

void CloudsVisualSystemPaintBrush::selfDrawDebug()
{
    
}

void CloudsVisualSystemPaintBrush::selfSceneTransformation()
{
    
}

void CloudsVisualSystemPaintBrush::selfExit()
{
    for (map<int, Brush *>::iterator it = brushes.begin(); it != brushes.end(); it++) {
        delete it->second;
	}
    brushes.clear();
    
    for (map<string, ofPixels *>::iterator it = colorMapPixelsMap.begin(); it != colorMapPixelsMap.end(); it++) {
        delete it->second;
	}
    colorMapPixelsMap.clear();
}

void CloudsVisualSystemPaintBrush::selfBegin()
{
    
}

void CloudsVisualSystemPaintBrush::selfEnd()
{
    
}

void CloudsVisualSystemPaintBrush::selfKeyReleased(ofKeyEventArgs & args)
{
    if (args.key == ' '){
//        brush.clear();
        particles.clear();
        
        canvasSrc.begin();
        ofClear(0,0);
        canvasSrc.end();
        
        canvasDst.begin();
        ofClear(0,0);
        canvasDst.end();
    }
}

void CloudsVisualSystemPaintBrush::selfInteractionMoved(CloudsInteractionEventArgs& args){
	
}

void CloudsVisualSystemPaintBrush::selfInteractionStarted(CloudsInteractionEventArgs& args)
{
    Brush * newBrush;
    if (brushes.find(args.playerId) == brushes.end()) {
        // build a new Brush
        newBrush = new Brush();
        brushes[args.playerId] = newBrush;
    }
    else {
        ofLogError("PaintBrush::selfInteractionStarted") << "Brush " << args.playerId << " already exists!";
        newBrush = brushes[args.playerId];
    }
    
	newBrush->init(brushNumber);
    newBrush->setBrushWidth(brushWidth);
    newBrush->setLineWidth(lineWidth);
    
    newBrush->damp = brushDamp;
    newBrush->k = brushK;
    newBrush->repPct = brushRepPct;
    newBrush->repRad = brushRepRad;
    
    newBrush->begin();
    newBrush->set(args.position.x,args.position.y);
}

void CloudsVisualSystemPaintBrush::selfInteractionDragged(CloudsInteractionEventArgs& args)
{
    if (brushes.find(args.playerId) == brushes.end()) {
        ofLogError("PaintBrush::selfInteractionDragged") << "Brush " << args.playerId << " does not exist!";
        return;
    }
    
	brushes[args.playerId]->set(args.position.x,args.position.y);
}

void CloudsVisualSystemPaintBrush::selfInteractionEnded(CloudsInteractionEventArgs& args)
{
    if (brushes.find(args.playerId) == brushes.end()) {
        ofLogError("PaintBrush::selfInteractionEnded") << "Brush " << args.playerId << " does not exist!";
        return;
    }
	
    brushes[args.playerId]->set(args.position.x, args.position.y);
    brushes[args.playerId]->end();
}

void CloudsVisualSystemPaintBrush::selfSetupGui()
{
    
}

void CloudsVisualSystemPaintBrush::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemPaintBrush::guiRenderEvent(ofxUIEventArgs &e)
{
    
}