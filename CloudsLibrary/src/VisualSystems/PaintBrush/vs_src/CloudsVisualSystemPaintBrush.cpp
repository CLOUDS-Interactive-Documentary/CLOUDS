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
    canvas.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    canvas.begin();
    ofClear(0,0);
    canvas.end();
    
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
    ofPushStyle();
    ofSetColor(255);
    glDisable(GL_DEPTH_TEST);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    brush.update();
    
    if (brush.getVel().length() < particlesThreshold) {
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
        
        brush.setColor(color, colorLerp, colorRandom);
    }
    
    brush.addParticles(particles, particlesThreshold, particlesAlpha);
    
    canvas.begin();
    
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
    
    brush.draw();
    
    canvas.end();
    
    while(particles.size()>500){
        particles.erase(particles.begin());
    }
    
    ofPopStyle();
}

void CloudsVisualSystemPaintBrush::selfDrawBackground()
{
    ofPushStyle();
    ofSetColor(255);
    glDisable(GL_DEPTH_TEST);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    canvas.draw(0, 0);
    
    if (bDebug){
        ofSetColor(255);
        brush.drawDebug();
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
        brush.clear();
        canvas.begin();
        ofClear(0,0);
        canvas.end();
        particles.clear();
    }
}

void CloudsVisualSystemPaintBrush::mouseDragged(ofMouseEventArgs& data)
{
    brush.set(data.x,data.y);
    
    //  If is slow take color?
    //
//    brush.pickColorFrom( canvas.getTextureReference(), 0.5, 0.25 );
}

void CloudsVisualSystemPaintBrush::mouseMoved(ofMouseEventArgs &args)
{
    
}

void CloudsVisualSystemPaintBrush::mousePressed(ofMouseEventArgs &args)
{
    ofPoint mouse(args.x,args.y);
    
    brush.init(brushNumber);
    brush.setBrushWidth(brushWidth);
    brush.setLineWidth(lineWidth);
    
    brush.damp = brushDamp;
    brush.k = brushK;
    brush.repPct = brushRepPct;
    brush.repRad = brushRepRad;
    
    brush.begin();
    brush.set(mouse);
}

void CloudsVisualSystemPaintBrush::mouseReleased(ofMouseEventArgs &args)
{
    brush.set(args.x, args.y);
    brush.end();
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