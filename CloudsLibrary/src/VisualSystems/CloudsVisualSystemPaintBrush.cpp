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
    iconSize = 100;
    
    canvas.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    canvas.begin();
    ofClear(0,0);
    canvas.end();
    
    palette.path = getDataPath()+"visualsystems/PaintBrush/";
    palette.setup();
    palette.loadPalette(getDataPath()+"visualsystems/PaintBrush/settings.xml");
    palette.setVisible(false);
    
    paletteBtn.set(0, 0, iconSize+30, iconSize+30);
    paletteBtn.setImage(getDataPath()+"visualsystems/PaintBrush/icon_pallete.png");
    ofAddListener( paletteBtn.clickPressed, this, &CloudsVisualSystemPaintBrush::showPalette );
    cleanBtn.set(ofGetWidth()-iconSize,ofGetHeight()-iconSize,iconSize,iconSize);
    ofAddListener( cleanBtn.clickPressed, this, &CloudsVisualSystemPaintBrush::cleanCanvas );
    cleanBtn.setImage(getDataPath()+"visualsystems/PaintBrush/icon_close.png");
    
    bDebug  = false;
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
    palette.update();
    
    canvas.begin();
    if ( palette.getVisible() ){
        if ( ofGetMouseY() > palette.getY() ){
            brush.draw();
        }
    } else {
        brush.draw();
    }
    canvas.end();
    
    if ( palette.getVisible() ){
        if ( ofGetMouseY() < palette.getY() ){
            palette.begin();
            brush.draw();
            palette.end();
        }
    }
}

void CloudsVisualSystemPaintBrush::draw(ofEventArgs & args)
{
    ofPushStyle();
    if(bRenderSystem)
    {
        //  TEMPORAL FIX
        //
        drawBackground();
        
        canvas.draw(0, 0);
        
        paletteBtn.draw();
        cleanBtn.draw();
        
        palette.draw();
        
        if (bDebug){
            ofSetColor(255);
            brush.drawDebug();
        }
        
    }
    
    ofPopStyle();
	
    cleanBtn.set(ofGetWidth()-iconSize,ofGetHeight()-iconSize,iconSize,iconSize);
    
//	timeline->draw();
}

void CloudsVisualSystemPaintBrush::showPalette(int &_n){
    if (!palette.getVisible()){
        palette.clear();
        brush.clear();
        palette.setVisible(true);
    }
}

void CloudsVisualSystemPaintBrush::cleanCanvas(int &_n){
    brush.clear();
    canvas.begin();
    ofClear(0,0);
    canvas.end();
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
    
}

void CloudsVisualSystemPaintBrush::mouseDragged(ofMouseEventArgs& data)
{
    if ( palette.getVisible() ){
        brush.pickColorFrom( palette.getTextureReference(),colorLerp,colorRandom );
    }
    brush.set(data.x,data.y);
}

void CloudsVisualSystemPaintBrush::mouseMoved(ofMouseEventArgs &args)
{
    
}

void CloudsVisualSystemPaintBrush::mousePressed(ofMouseEventArgs &args)
{
    ofPoint mouse(args.x,args.y);
    
    if ( !cleanBtn.checkOver(mouse) && !paletteBtn.checkOver(mouse)){
        
        if (palette.getVisible()){
            if (  mouse.y > palette.getY() ){
                brush.clear();
                palette.setVisible(false);
            }
        }
        
        paletteBtn.checkOver(mouse);
        cleanBtn.checkOver(mouse);
        
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