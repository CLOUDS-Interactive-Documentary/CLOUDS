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
    canvas.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    canvas.begin();
    ofClear(0,0);
    canvas.end();
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
    
    sysGui->addSlider("color_lerp",0.0, 1.0, &colorLerp);
    sysGui->addSlider("color_random", 0.0, 0.02, &colorRandom);
    
    sysGui->addSlider("particles_threshold", 5.0, 100.0, &particlesThreshold);
    sysGui->addSlider("particles_turbulence", 0.0, 0.5, &particlesTurbulence);
    sysGui->addSlider("particles_alpha", 0.0, 1.0, &particlesAlpha);
}

void CloudsVisualSystemPaintBrush::selfSetupRenderGui()
{
    
}

void CloudsVisualSystemPaintBrush::guiSystemEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemPaintBrush::selfKeyPressed(ofKeyEventArgs & args){
    
}

void CloudsVisualSystemPaintBrush::selfUpdate()
{
    brush.update();
    
    if (brush.getVel().length() < particlesThreshold){
        ofFloatColor color;
        color.set(1, 0, 0);
        color.setHue( abs( sin(ofGetElapsedTimef()*0.01) ) );
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
}

void CloudsVisualSystemPaintBrush::draw(ofEventArgs & args)
{
    ofPushStyle();
    if(bRenderSystem)
    {
        drawBackground();
        
        canvas.draw(0, 0);
        
        if (bDebug){
            ofSetColor(255);
            brush.drawDebug();
        }
        
    }
    
    ofPopStyle();
	
//	timeline->draw();
}


void CloudsVisualSystemPaintBrush::selfSetupGuis()
{
    
}

void CloudsVisualSystemPaintBrush::selfAutoMode()
{
    
}


void CloudsVisualSystemPaintBrush::selfDrawBackground()
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