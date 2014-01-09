//
//  CloudsVisualSystemVoro.cpp
//  VSCloudsRezanator
//
//  Created by Patricio Gonzalez Vivo on 5/27/13.
//
//

#include "CloudsVisualSystemVoro.h"
#include "ofxObjLoader.h"

// comparison routine for sort...
bool comparisonFunction(  VoroParticle *a, VoroParticle *b ) {
	return a->x < b->x;
}

string CloudsVisualSystemVoro::getSystemName()
{
	return "Voro";
}

void CloudsVisualSystemVoro::selfSetDefaults()
{
	bUseMaterial = false;
	
	cellsWireThickness = 1.0;
	cellsWireAlpha = 1.0;
	cellsVertexAlpha = 1.0;
	cellsVertexSize = 1.0;
}

void CloudsVisualSystemVoro::selfSetup()
{
    objectLookAt = ofVec3f(0,0,1);
    ofLoadImage(dot, getVisualSystemDataPath()+"images/dot.png");
    
    for(int i = 0; i < 28; i++){
        ofLoadImage(nucles[i], getVisualSystemDataPath()+"images/Cell"+ofToString(i,2,'0')+".png");
    }	
}

void CloudsVisualSystemVoro::selfSetupGuis()
{
    
}

void CloudsVisualSystemVoro::selfSetupSystemGui()
{
    sysGui->addLabel("Particles");
    sysGui->addSlider("Max_Number", 10, 1000, &MaxNumOfParticles);
    sysGui->addSlider("BornSecRate", 0.001, 5, &bornRate);
    sysGui->addSlider("Max_Size", 1, 10, &MaxSize);
    sysGui->addSlider("GrowRate", 0.0, 0.1, &growRate);
    sysGui->addSlider("InitialForce", 0.0, 1.0, &initialForce);
    sysGui->addSlider("Z_Move", 0.0, 1.0, &zMove);
    sysGui->addSlider("Atraction", 0.0, 0.3, &atractionPct);
    sysGui->addSlider("Repulsion", 0.0, 1.0, &repulsionPct);
    sysGui->addSlider("texture_Size", 0.0, 1.0, &tSize);
    sysGui->addButton("Clear", &bClear);
    
    sysGui->addLabel("Voronoi");
    sysGui->addToggle("Container_Periodic", &containerPeriodic);
    sysGui->addSlider("Conteiner_Size", 1.0, 200, &containerSize);
    sysGui->addSlider("Conteiner_Height", 0.1, 200, &containerHeight);
    sysGui->addToggle("Cylinder", &bCyllinder);
    sysGui->addToggle("Sphere", &bSphere);
    
    sysGui->addSlider("Cell_Wall", 0.0, 2.0, &wallThikness);
    sysGui->addSlider("Cell_Alpha", 0.0, 1.0, &cellsAlpha);
    sysGui->addSlider("Cell_Wires_Alpha", 0.0, 1.0, &cellsWireAlpha);
    sysGui->addSlider("Cell_Wires_Thickness", 0.0, 4.0, &cellsWireThickness);
    sysGui->addSlider("Cell_Vertex_Alpha", 0.0, 1.0, &cellsVertexAlpha);
    sysGui->addSlider("Cell_Vertex_Size", 0.0, 4.0, &cellsVertexSize);
	sysGui->addToggle("Cell_Use_Material", &bUseMaterial);
	
}

void CloudsVisualSystemVoro::selfSetupRenderGui(){}

void CloudsVisualSystemVoro::selfBegin()
{
	cam.reset();
	
}

void CloudsVisualSystemVoro::selfEnd()
{
    for(int i = seedParticles.size()-1; i >= 0; i-- ){
        delete seedParticles[i];
        seedParticles.erase(seedParticles.begin()+i);
    }
    seedParticles.clear();
    cellMeshes.clear();
}

void CloudsVisualSystemVoro::selfDrawBackground()
{
//    ofPushStyle();
//    int width = ofGetWidth();
//    int height = ofGetHeight();
//
//    background.begin();
//    background.setUniform1f("time", ofGetElapsedTimef());
//    background.setUniform2f("mouse", xRot->getPos()*PI, yRot->getPos()*PI);
//    background.setUniform2f("resolution", width, height);
//    background.setUniform1f("starsAlpha", 0.5);
//    
//    glBegin(GL_QUADS);
//    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
//    glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
//    glTexCoord2f(width, height); glVertex3f(width, height, 0);
//    glTexCoord2f(0,height);  glVertex3f(0,height, 0);
//    glEnd();
//    background.end();
//    
//    ofPopStyle();
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
//            seed->size = 0.1;
            seed->nucle = &nucles[ (int)(ofGetElapsedTimef()*10)%28 ];
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
    for (int i = 0; i < seedParticles.size(); i++){
		for (int j = 0; j < i; j++){
            seedParticles[i]->addRepulsionForce( seedParticles[j], repulsionPct);
		}
	}
    
    for (int i = seedParticles.size()-1; i >=0 ; i--){
//        if(seedParticles[i]->size < MaxSize){
//            seedParticles[i]->size += growRate;
//        }
        seedParticles[i]->addAttractionForce( ofPoint(0,0) ,200,atractionPct);
        seedParticles[i]->update();
	}
    
    //  Compute Voronoi
    //
    if(cellsAlpha>0.0 || cellsWireAlpha>0.0 || cellsVertexAlpha>0.0){
        
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
        
        getCellsFromContainer(con,wallThikness,cellMeshes);
		
		if( ofGetKeyPressed('[') ){
			//export for Bradley
			ofMesh combinedMesh;
			cout << "mesh frame " << ofGetFrameNum() << ": mesh count # "  << cellMeshes.size() << endl;
			for(int i = 0; i < cellMeshes.size(); i++){
				int startIndex = combinedMesh.getVertices().size();
				for(int v = 0; v < cellMeshes[i].getVertices().size(); v++){
					combinedMesh.addVertex( cellMeshes[i].getVertices()[v] );
				}
				for(int n = 0; n < cellMeshes[i].getNormals().size(); n++){
					combinedMesh.addNormal( cellMeshes[i].getNormals()[n] );
				}
				for(int in = 0; in < cellMeshes[i].getIndices().size(); in++){
					combinedMesh.addIndex(startIndex + cellMeshes[i].getIndices()[in] );
				}
			}
			char filename[1024];
			sprintf( filename, "meshes/cell_mesh_%05d.obj", ofGetFrameNum() );
			//combinedMesh.save( filename );
			ofxObjLoader::save(filename, combinedMesh);
		}
    }

    fps++;
}

void CloudsVisualSystemVoro::selfDraw()
{

	if(bUseMaterial){
		mat->begin();
	}
    
    ofPushMatrix();
    ofPushStyle();
    glEnable(GL_DEPTH_TEST);

    if (cellsVertexAlpha > 0.0){
		glPointSize(cellsVertexSize);
        for(int i = 0; i < cellMeshes.size(); i++){
            ofSetColor(155,cellsVertexAlpha*255.0);
            cellMeshes[i].drawVertices();
        }
    }
    
    
    if(cellsAlpha > 0.0 || cellsWireAlpha > 0.0){
		ofSetLineWidth(cellsWireThickness);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		
        for(int i = 0; i < cellMeshes.size(); i++){
            ofSetColor(255,cellsAlpha*255.0);
            
            if(cellsAlpha){
                ofSetColor(155,cellsAlpha*100.0);
                cellMeshes[i].drawFaces();
            }
            
            if(cellsWireAlpha){
                ofSetColor(155,cellsWireAlpha*100.0);
                cellMeshes[i].drawWireframe();
            }
        }
    }
    

    
    glDisable(GL_DEPTH_TEST);
	ofPopStyle();
    ofPopMatrix();
    

	if(bUseMaterial){
		mat->end();
	}
}


//void CloudsVisualSystemVoro::billBoard()
//{
//    ofVec3f objToCam = cam.getGlobalPosition();
//    objToCam.normalize();
//    float theta = objectLookAt.angle(objToCam);
//    ofVec3f axisOfRotation = objToCam.crossed(objectLookAt);
//    axisOfRotation.normalize();
//    
//    glRotatef(-zRot->getPos(), 0.0, 0.0, 1.0);
//    glRotatef(-yRot->getPos(), 0.0, 1.0, 0.0);
//    glRotatef(-xRot->getPos(), 1.0, 0.0, 0.0);
//    glRotatef(-theta, axisOfRotation.x, axisOfRotation.y, axisOfRotation.z);
//}

void CloudsVisualSystemVoro::selfExit()
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

void CloudsVisualSystemVoro::guiSystemEvent(ofxUIEventArgs &e)
{

}

void CloudsVisualSystemVoro::guiRenderEvent(ofxUIEventArgs &e)
{
    
}
