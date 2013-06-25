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
    objectLookAt = ofVec3f(0,0,1);
    ofLoadImage(dot, getDataPath()+"images/dot.png");
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
    
    //  Born & Death
    //
    if (bClear){
        for (int i = seedParticles.size()-1; i >= 0; i--){
            delete seedParticles[i];
            seedParticles.erase(seedParticles.begin()+i);
        }
    } else {
        //  Born seed particles
        //
        if ( fps%(int)(60/bornRate)==0 ){
            VoroParticle *seed = new VoroParticle();
            seed->init(ofPoint(0,0),
                       ofPoint(ofNoise(ofGetElapsedTimef()*0.1, fps*0.03)*2.0-1.0,
                               ofNoise(ofGetElapsedTimef()*0.05, fps*0.07)*2.0-1.0,
                               (ofNoise(ofGetElapsedTimef()*0.01, fps*0.003)-0.5)*zMove)*initialForce );
            seed->size = 0.1;
            seedParticles.push_back(seed);
        }
        
        //  Kill particles
        //
        while (seedParticles.size()>=MaxNumOfParticles) {
            delete seedParticles[0];
            seedParticles.erase(seedParticles.begin());
        }
    }
    
    //  Compute seed particles
    //
//    sort( seedParticles.begin(), seedParticles.end(), comparisonFunction );
    for (int i = 0; i < seedParticles.size(); i++){
		for (int j = 0; j < i; j++){
//            if ( fabs(seedParticles[j]->x - seedParticles[i]->x) >	50.0) break;
            seedParticles[i]->addRepulsionForce( seedParticles[j], repulsionPct);
		}
	}
    
    for (int i = seedParticles.size()-1; i >=0 ; i--){
        if(seedParticles[i]->size < MaxSize){
            seedParticles[i]->size += growRate;
        }
        seedParticles[i]->addAttractionForce( ofPoint(0,0) ,200,atractionPct);
        seedParticles[i]->update();
	}
    
    //  Compute Voronoi
    //
    if(bDrawVoronoiWireFrames || bDrawVoronoi){
        
        cellMeshes.clear();
		voro::container con(-containerSize,containerSize,
                      -containerSize,containerSize,
                      -containerHeight,containerHeight,
                      1,1,1,
                      containerPeriodic,containerPeriodic,containerPeriodic,
                      1);
        
//        if (bSphere){
//            voro::wall_sphere sph(0, 0, 0, containerSize);
//            con.add_wall(sph);
//            bCyllinder = false;
//        }
//        
//        if (bCyllinder){
//			voro::wall_cylinder cyl(0,0,0,0,0,0, containerSize);
//            con.add_wall(cyl);
//            bSphere = false;
//        }
        
        for(int i = 0; i < seedParticles.size(); i++){
            con.put(i, seedParticles[i]->x, seedParticles[i]->y, seedParticles[i]->z);
        }
        
        cellMeshes = getCellsFromContainer(con);
    }
    
    
    fps++;
}

void CloudsVisualSystemVoro::selfDraw()
{
    mat->begin();
    ofPushMatrix();
    
    glEnable(GL_DEPTH_TEST);
    
    if (bDrawGlow){
        glDepthMask(GL_FALSE);
        ofEnableBlendMode(OF_BLENDMODE_ADD);
    }
    
    ofMesh particles;
    particles.setMode(OF_PRIMITIVE_POINTS);
    
    if (bDrawParticles){
        for (int i = 0; i < seedParticles.size(); i++){
            
            if (bDrawGlow){
                ofSetColor(255);
                ofPushMatrix();
                ofSetRectMode(OF_RECTMODE_CENTER);
                
                ofTranslate(*seedParticles[i]);
                billBoard();
                
                ofSetColor(255, 255*(seedParticles[i]->size/MaxSize));
                dot.draw(0,
                         0,
                         seedParticles[i]->size*glowSize,
                         seedParticles[i]->size*glowSize);
                ofSetRectMode(OF_RECTMODE_CORNER);
                ofPopMatrix();
            } else {
//                ofSetColor(255,255,0);
//                seedParticles[i]->drawSphere();
                particles.addVertex( *seedParticles[i] );
            }
        }
    }

    if (bDrawGlow){
        glDepthMask(GL_TRUE);
        ofEnableAlphaBlending();
    } else {
        particles.draw();
    }
    
    if(bDrawVoronoiWireFrames || bDrawVoronoi){
        for(int i = 0; i < cellMeshes.size(); i++){
            ofSetColor(255,cellsAlpha*255.0);
            
            if(bDrawVoronoi){
                ofSetColor(155,cellsAlpha*100.0);
                cellMeshes[i].drawFaces();
            }
            
            if(bDrawVoronoiWireFrames){
                ofSetColor(155,cellsWireAlpha*100.0);
                cellMeshes[i].drawWireframe();
            }
        }
    }
    
    
    glDisable(GL_DEPTH_TEST);
    
    ofPopMatrix();
    mat->end();
}

void CloudsVisualSystemVoro::billBoard()
{
    ofVec3f objToCam = cam.getGlobalPosition();
    objToCam.normalize();
    float theta = objectLookAt.angle(objToCam);
    ofVec3f axisOfRotation = objToCam.crossed(objectLookAt);
    axisOfRotation.normalize();
    
    glRotatef(-zRot->getPos(), 0.0, 0.0, 1.0);
    glRotatef(-yRot->getPos(), 0.0, 1.0, 0.0);
    glRotatef(-xRot->getPos(), 1.0, 0.0, 0.0);
    glRotatef(-theta, axisOfRotation.x, axisOfRotation.y, axisOfRotation.z);
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
    sysGui->addSlider("Max_Number", 10, 1000, &MaxNumOfParticles);
    sysGui->addSlider("BornSecRate", 0.001, 60, &bornRate);
    sysGui->addSlider("Max_Size", 1, 10, &MaxSize);
    sysGui->addSlider("GrowRate", 0.0, 0.1, &growRate);
    sysGui->addSlider("InitialForce", 0.0, 1.0, &initialForce);
    sysGui->addSlider("Z_Move", 0.0, 1.0, &zMove);
    sysGui->addSlider("Atraction", 0.0, 0.3, &atractionPct);
    sysGui->addSlider("Repulsion", 0.0, 1.0, &repulsionPct);
    sysGui->addToggle("Glow",& bDrawGlow);
    sysGui->addSlider("Glow_Size", 0.0, 1.0, &glowSize);
    sysGui->addToggle("DrawParticles", &bDrawParticles);
    sysGui->addButton("Clear", &bClear);
    
    sysGui->addLabel("Voronoi");
    sysGui->addToggle("Container_Periodic", &containerPeriodic);
    sysGui->addSlider("Conteiner_Size", 10, 200, &containerSize);
    sysGui->addSlider("Conteiner_Height", 1, 200, &containerHeight);
    sysGui->addToggle("Cylinder", &bCyllinder);
    sysGui->addToggle("Sphere", &bSphere);
    sysGui->addSlider("Cell_Alpha", 0.0, 1.0, &cellsAlpha);
    sysGui->addToggle("DrawVoronoi", &bDrawVoronoi);
    sysGui->addSlider("Cell_Wires_Alpha", 0.0, 1.0, &cellsWireAlpha);
    sysGui->addToggle("DrawVoronoiWires", &bDrawVoronoiWireFrames);
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