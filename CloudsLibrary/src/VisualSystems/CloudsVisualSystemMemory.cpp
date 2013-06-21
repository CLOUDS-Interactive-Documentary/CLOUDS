//
//  CloudsVisualSystemMemory.cpp
//  VSMemory
//
//  Created by Patricio Gonzalez Vivo on 6/20/13.
//
//

#include "CloudsVisualSystemMemory.h"


string CloudsVisualSystemMemory::getSystemName()
{
	return "Memory";
}

void CloudsVisualSystemMemory::selfSetup()
{
    ofEnableAlphaBlending();
    
    blockScale = 1.0;
    blockWidth = 4;
    blockHeight = 8;
    margin = 2;
    blocksTotal = 10000;
    
    data = new unsigned char[blocksTotal];
    
    generateBlocks(data,blocksTotal);
}

void CloudsVisualSystemMemory::selfSetupSystemGui()
{
    sysGui->addSlider("block_width", 0.001, 20, &blockWidth);
    sysGui->addSlider("block_height", 0.001, 20, &blockHeight);
    sysGui->addSlider("block_scale", 0.001, 10, &blockScale);
    sysGui->addSlider("margin", 0.001, 10, &margin);
}

void CloudsVisualSystemMemory::selfSetupRenderGui()
{
    
}

void CloudsVisualSystemMemory::guiSystemEvent(ofxUIEventArgs &e)
{
    generateBlocks(data,blocksTotal);
}

void CloudsVisualSystemMemory::selfKeyPressed(ofKeyEventArgs & args){
    
}

void CloudsVisualSystemMemory::selfUpdate()
{
    for(float i = 0; i < blocks.size()-1; i++){
        if (int(blocks[i].value) > int(blocks[i+1].value)-127){
            swap(blocks[i].value, blocks[i+1].value);
            swap(blocks[i].color, blocks[i+1].color);
            
            blocks[i].bSelected = true;
        } else {
            blocks[i].bSelected = false;
        }
    }
}

void CloudsVisualSystemMemory::generateBlocks( unsigned char * _data, int _nElements ){
    
    int xMargin = 10;//(blockWidth*blockScale*2.5)
    int yMargin = 10;//(blockHeight*blockScale*1.5)
    
    int width = ofGetWidth()-xMargin*2.0;
    int height = ofGetHeight()-yMargin*2.0;
    
    int xBlocks = (float)width/((blockWidth+margin)*blockScale);
    int yBlocks = (float)height/((blockHeight+margin)*blockScale);
    
    blocks.clear();
    int index = 0;
    for (int j = 0; j < yBlocks; j++) {
        for (int i = 0; i < xBlocks; i++){
            
            if (index < blocksTotal ){
                
                int x = xMargin + ((margin + blockWidth)*blockScale)*i ;
                int y = yMargin + ((margin + blockHeight)*blockScale)*j ;
                
                if ( y > (ofGetHeight()+margin+blockHeight*blockScale))
                    break;
                
                Block block;
                block.x = x;
                block.y = y;
                block.width = blockWidth*blockScale;
                block.height = blockHeight*blockScale;
                block.color = ofColor((unsigned char)_data[index]);
                block.value = _data[index];
                block.bSelected = false;
                
                blocks.push_back(block);
                
            } else {
                break;
            }
            
            index++;
        }
    }
}

void CloudsVisualSystemMemory::draw(ofEventArgs & args)
{
    ofPushStyle();
    if(bRenderSystem)
    {
        drawBackground();
        
        for (int i = 0; i < blocks.size(); i++) {
            blocks[i].draw();
        }
    }
    
    ofPopStyle();
	
    //	timeline->draw();
}


void CloudsVisualSystemMemory::selfSetupGuis()
{
    
}

void CloudsVisualSystemMemory::selfAutoMode()
{
    
}


void CloudsVisualSystemMemory::selfDrawBackground()
{
    
}

void CloudsVisualSystemMemory::selfDrawDebug()
{
    
}

void CloudsVisualSystemMemory::selfSceneTransformation()
{
    
}

void CloudsVisualSystemMemory::selfExit()
{
    
}

void CloudsVisualSystemMemory::selfBegin()
{
    
}

void CloudsVisualSystemMemory::selfEnd()
{
    
}

void CloudsVisualSystemMemory::selfKeyReleased(ofKeyEventArgs & args)
{

}

void CloudsVisualSystemMemory::mouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemMemory::mouseMoved(ofMouseEventArgs &args)
{
    
}

void CloudsVisualSystemMemory::mousePressed(ofMouseEventArgs &args)
{

}

void CloudsVisualSystemMemory::mouseReleased(ofMouseEventArgs &args)
{

}

void CloudsVisualSystemMemory::selfSetupGui()
{
    
}

void CloudsVisualSystemMemory::selfGuiEvent(ofxUIEventArgs &e)
{
}

void CloudsVisualSystemMemory::guiRenderEvent(ofxUIEventArgs &e)
{
    
}