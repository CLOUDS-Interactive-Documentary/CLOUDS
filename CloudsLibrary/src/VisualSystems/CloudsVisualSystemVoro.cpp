//
//  CloudsVisualSystemVoro.cpp
//  VSCloudsRezanator
//
//  Created by Patricio Gonzalez Vivo on 5/27/13.
//
//

#include "CloudsVisualSystemVoro.h"

// comparison routine for sort...
bool comparisonFunction(  VoroParticle *a, VoroParticle *b ) {
	return a->x < b->x;
}

string CloudsVisualSystemVoro::getSystemName()
{
	return "Voro";
}

void CloudsVisualSystemVoro::selfSetup()
{
    
}

void CloudsVisualSystemVoro::selfSetupGuis()
{
    
}

void CloudsVisualSystemVoro::selfAutoMode()
{
    
}


void CloudsVisualSystemVoro::selfDrawBackground()
{
    
}

void CloudsVisualSystemVoro::selfDrawDebug()
{
    
}

void CloudsVisualSystemVoro::selfSceneTransformation()
{
    
}

void CloudsVisualSystemVoro::selfUpdate()
{
    cout << 60.0/bornRate << endl;
    //  Born seed particles
    //
    if ( fps%((int)(60.0/bornRate))==0 ){
        VoroParticle *seed = new VoroParticle();
        seed->init( ofPoint(0,0), ofPoint(0,0) );
        seed->size = 2;
        seedParticles.push_back(seed);
    }
    
    //  Kill particles
    //
    while (seedParticles.size()>=MaxNumOfParticles) {
        seedParticles.erase(seedParticles.begin());
    }
    
    //  Sort seed particles:
    //
	sort( seedParticles.begin(), seedParticles.end(), comparisonFunction );
	
    //  Compute seed particles
    //
    for (int i = 0; i < seedParticles.size(); i++){
        seedParticles[i]->addAttractionForce( ofPoint(0,0) ,1500,0.01);
		for (int j = 0; j < i; j++){
            if ( fabs(seedParticles[j]->x - seedParticles[i]->x) >	10) break;
            seedParticles[i]->addRepulsionForce( seedParticles[j], 0.7);
		}
        seedParticles[i]->update();
	}
    
    fps++;
}

void CloudsVisualSystemVoro::selfDraw()
{
    mat->begin();
    ofPushMatrix();
    
    glEnable(GL_DEPTH_TEST);
    
    for (int i = 0; i < seedParticles.size(); i++){
        seedParticles[i]->draw();
	}
    
    glDisable(GL_DEPTH_TEST);
    
    ofPopMatrix();
    mat->end();
}

void CloudsVisualSystemVoro::billBoard()
{
    
}

void CloudsVisualSystemVoro::selfExit()
{
    
}

void CloudsVisualSystemVoro::selfBegin()
{
    
}

void CloudsVisualSystemVoro::selfEnd()
{
    
}

void CloudsVisualSystemVoro::selfKeyPressed(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemVoro::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemVoro::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemVoro::selfMouseMoved(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemVoro::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemVoro::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemVoro::selfSetupGui()
{
    
}

void CloudsVisualSystemVoro::selfGuiEvent(ofxUIEventArgs &e)
{
    
}


void CloudsVisualSystemVoro::selfSetupSystemGui()
{
    sysGui->addLabel("Particles");
    sysGui->addSlider("Max_Number", 10, 300, &MaxNumOfParticles);
    sysGui->addSlider("BornSecRate", 0.001, 60, &bornRate);
}

void CloudsVisualSystemVoro::guiSystemEvent(ofxUIEventArgs &e)
{

}

void CloudsVisualSystemVoro::selfSetupRenderGui()
{
    
}

void CloudsVisualSystemVoro::guiRenderEvent(ofxUIEventArgs &e)
{
    
}