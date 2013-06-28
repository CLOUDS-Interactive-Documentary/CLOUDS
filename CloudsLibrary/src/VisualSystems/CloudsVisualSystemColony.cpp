//
//  CloudsVisualSystemColony.cpp
//  VSColony
//
//  Created by Patricio Gonzalez Vivo on 6/26/13.
//
//

#include "CloudsVisualSystemColony.h"

string CloudsVisualSystemColony::getSystemName()
{
	return "Colony";
}

void CloudsVisualSystemColony::selfSetup()
{
    noiseShader.load("", getDataPath()+"shaders/VisualSystems/Colony/liquidNoise.fs");
    foodFbo.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    noiseZoom = 100.0;
    
    
   }

void CloudsVisualSystemColony::selfSetupSystemGui()
{
    sysGui->addSlider("noiseZoom", 0.0, 1000, &noiseZoom);
}

void CloudsVisualSystemColony::selfSetupRenderGui()
{

}

void CloudsVisualSystemColony::guiSystemEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemColony::guiRenderEvent(ofxUIEventArgs &e)
{

}

void CloudsVisualSystemColony::selfKeyPressed(ofKeyEventArgs & args){

}


void CloudsVisualSystemColony::selfUpdate()
{
	
	if(!noise.isAllocated() || noise.getWidth() != ofGetWidth() || noise.getHeight() != ofGetHeight()){
		noise.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_GRAYSCALE);
		
		for(int x = 0; x < noise.getWidth(); x++){
			for(int y = 0; y < noise.getHeight(); y++){
				noise.setColor(x,y,ofColor(ofNoise(x/200.,y/200.0) * 255, 255) );

			}
		}
		
		noise.reloadTexture();
	}
	
    //  Process food texture
    //
    int width = foodFbo.getWidth();
    int height = foodFbo.getHeight();
    /*
    ofSetColor(255);
    foodFbo.begin();
    noiseShader.begin();
    noiseShader.setUniform1f("time", ofGetElapsedTimef());
    noiseShader.setUniform1f("zoom", noiseZoom);
    noiseShader.setUniform2f("resolution", (float)width, (float)height);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
    glTexCoord2f(width, height); glVertex3f(width, height, 0);
    glTexCoord2f(0,height);  glVertex3f(0,height, 0);
    glEnd();
    noiseShader.end();
    foodFbo.end();
    
    ofPixels pixels;
    foodFbo.getTextureReference().readToPixels(pixels);
    */
	
    //  Update cells position and life
    //
    for (int i = 0; i < cells.size(); i++) {
        
        cells[i]->applyFlock(cells);
        cells[i]->applyBorders();
        cells[i]->update();
        
//        cells[i]->feedCellWidth( pixels );
		cells[i]->feedCellWidth( noise.getPixelsRef() );
    }
    
    for (int i = cells.size()-1; i >= 0; i--){
        if (cells[i]->shouldReplicate){
    
            colonyCell *newCell = new colonyCell();
            newCell->set(*cells[i]);
            cells.push_back(newCell);
            
            cells[i]->shouldReplicate = false;
            cells[i]->hasReplicated = true;
        }
        
        if (cells[i]->dead == true){
            delete cells[i];
            cells.erase( cells.begin() + i);
        }
    }
    
}

void CloudsVisualSystemColony::draw(ofEventArgs & args)
{
    ofPushStyle();
    if(bRenderSystem)
    {
        drawBackground();
    
        ofSetColor(255,255,255);
		noise.draw(0,0);
//        foodFbo.draw(0, 0);
        
        for (int i = 0; i < cells.size(); i++) {
            cells[i]->draw();
        }
        
    }
    
    ofPopStyle();

}


void CloudsVisualSystemColony::selfSetupGuis()
{
    
}

void CloudsVisualSystemColony::selfAutoMode()
{
    
}


void CloudsVisualSystemColony::selfDrawBackground()
{
    
}

void CloudsVisualSystemColony::selfDrawDebug()
{
    
}

void CloudsVisualSystemColony::selfSceneTransformation()
{
    
}

void CloudsVisualSystemColony::selfExit()
{
    
}

void CloudsVisualSystemColony::selfBegin()
{
    for (int i = 0; i < 200; i++) {
        colonyCell *newCell = new colonyCell();
        newCell->set(ofRandomWidth(),ofRandomHeight());
        cells.push_back(newCell);
    }
}

void CloudsVisualSystemColony::selfEnd()
{
    for (int i = cells.size()-1; i >= 0; i--){
        delete cells[i];
        cells.erase(cells.begin()+i);
    }
}

void CloudsVisualSystemColony::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemColony::mouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemColony::mouseMoved(ofMouseEventArgs &args)
{
    
}

void CloudsVisualSystemColony::mousePressed(ofMouseEventArgs &args)
{
    
}

void CloudsVisualSystemColony::mouseReleased(ofMouseEventArgs &args)
{
    
}

void CloudsVisualSystemColony::selfSetupGui()
{
    
}

void CloudsVisualSystemColony::selfGuiEvent(ofxUIEventArgs &e)
{
    
}