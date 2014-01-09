//
//  CloudsVisualSystemWorld.cpp
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/13/13.
//
//

#include "CloudsVisualSystemWorld.h"

void CloudsVisualSystemWorld::selfSetDefaults()
{
    nMaxPoints = 1000;
    pointNoisePeaks = 0.0;
    rippleThreshold = 1.0;
    initialForce = 1.0;
    density = 0.9;
    gravity = 1.0;
    repulsion = 1.0;
    turbulence = 1.0;
    neigbordhood = 1.0;
    independence = 1.0;
    
    coastAlpha = 0.3921;
    riversAlpha = 0.3921;
    
    wireSphereScale = 0.9333;
    solidSphereScale = 0.8666;
    wireSphereAlpha = 0.0784;
    solidSphereAlpha = 1.0;
    
    constelationMin = 0.035;
    constelationMax = 0.4;
    constelationRnd = 10;
    
    nMaxSatellites = 10;
    satLinksDist = 0;
    satLinksAlpha = 1.0;
}

string CloudsVisualSystemWorld::getSystemName()
{
	return "World";
}

void CloudsVisualSystemWorld::selfSetup()
{
    
    haloShader.load(getVisualSystemDataPath()+"shaders/backlight");
    
    postShader.load("",getVisualSystemDataPath()+"shaders/postprocess.fs");
    ofLoadImage(postTexture, getVisualSystemDataPath()+"images/7.jpg");

}

void CloudsVisualSystemWorld::selfBegin()
{

	cam.reset();
	
	citiesMesh.clear();
	citiesMesh.setMode(OF_PRIMITIVE_POINTS);
	
    //  Load Globe paths
    //
    loadVbo(coastVbo, "simple-coast.txt");
    loadVbo(riversVbo, "simple-rivers.txt");
    
    //  Load cities points
    //
    loadCities( "simple-cities.txt" );
    loadSecCities( "airports.txt");
    
    //  Load Stars
    //
    loadStarts( "constelations.txt" );
    
//	satellitesMesh.clear();
//	satellitesMesh.setMode(OF_PRIMITIVE_LINES);
	
    for(int i = 0; i < 1000; i++ ){
        wSatellite *newSat = new wSatellite();
//		newSat->satelliteMesh = &satellitesMesh;
		
		ofVec3f position = ofVec3f(ofRandom(-0.01,0.01),ofRandom(-0.01,0.01),0.0);
        newSat->place(400, position);
		
        satellites.push_back( newSat );

    }
    
    globalOffset.set(0,0,0);
}

void CloudsVisualSystemWorld::selfEnd()
{
    coastVbo.clear();
    riversVbo.clear();
    cities.clear();
    secCities.clear();
    
    for(int i = particles.size()-1; i >= 0; i--){
        delete particles[i];
        particles.erase(particles.begin()+i);
    }
    
    for(int i = stars.size()-1; i >= 0; i--){
        delete stars[i];
        stars.erase(stars.begin()+i);
    }
    
    for(int i = satellites.size()-1; i >= 0; i--){
        delete satellites[i];
        satellites.erase(satellites.begin()+i);
    }
}

void CloudsVisualSystemWorld::selfExit()
{
    
}

void CloudsVisualSystemWorld::loadVbo(ofVboMesh &_vbo, string _file){
    _vbo.setMode(OF_PRIMITIVE_LINES);
    
    ifstream fileIn;
	
    string filePath = getVisualSystemDataPath()+_file;
    ofBuffer buffer = ofBufferFromFile(filePath);
    
    int lineCount = 0;
    ofPoint prevPoint;
    
    bool bFirst = true;
    while(!buffer.isLastLine()) {
        string temp = buffer.getNextLine();
        
        if(temp.length() != 0) {
            
            vector<string> values = ofSplitString(temp, " ");
            
            
            if ( values[0] == "segment"){
                bFirst = true;
                
            } else {
                ofPoint newPoint = ofPoint(ofToFloat(values[0]),
                                           ofToFloat(values[1]),
                                           ofToFloat(values[2]));
                if (!bFirst){
                    _vbo.addVertex(prevPoint);
                    _vbo.addVertex(newPoint);
                } else {
                    bFirst = false;
                }
                
                prevPoint = newPoint;
            }
            
            lineCount++;
        }
    }
}

void CloudsVisualSystemWorld::loadCities(string _file){
    string filePath = getVisualSystemDataPath()+_file;
    ofBuffer buffer = ofBufferFromFile(filePath);
    
    while(!buffer.isLastLine()) {
        string temp = buffer.getNextLine();
        
        if(temp.length() != 0) {
            vector<string> values = ofSplitString(temp, "|");
            
            wCity city;
			city.citiesMesh = &citiesMesh;
            city.place(ofToFloat(values[1]),ofToFloat(values[2]));
            city.noisePeaks = &pointNoisePeaks;
            city.noiseThreshold = &rippleThreshold;
            
            cities.push_back(city);
        }
    }
}

void CloudsVisualSystemWorld::loadSecCities(string _file){
    string filePath = getVisualSystemDataPath()+_file;
    ofBuffer buffer = ofBufferFromFile(filePath);
    
    while(!buffer.isLastLine()) {
        string temp = buffer.getNextLine();
        
        if(temp.length() != 0) {
            vector<string> values = ofSplitString(temp, "|");
            
            wCity city;
			city.citiesMesh = &citiesMesh;
            city.place(ofToFloat(values[5]),ofToFloat(values[6]));
            city.size = ofMap(ofToFloat(values[4]), 0, 10000, 0.1, 5.0);
            
            secCities.push_back(city);
        }
    }
}

void CloudsVisualSystemWorld::loadStarts( string _file){
    string filePath = getVisualSystemDataPath()+_file;
    ofBuffer buffer = ofBufferFromFile(filePath);
    
    string lastConstelation = "";
    
    while(!buffer.isLastLine()) {
        string temp = buffer.getNextLine();
        
        if(temp.length() != 0) {
            vector<string> values = ofSplitString(temp, ",");
            
            if ( values[0] != lastConstelation ){
                constelations.push_back(values[0]);
            }
            
            lastConstelation = values[0];
            
            wStar *a = new wStar();
            a->place(ofToFloat(values[2]),
                      ofToFloat(values[1]) );
            a->constName = lastConstelation;
            
            wStar *b = new wStar();
            b->place(ofToFloat(values[4]),
                      ofToFloat(values[3]) );
            b->constName = lastConstelation;
            b->connect = a;
            
            stars.push_back(a);
            stars.push_back(b);
        }
    }
}

void CloudsVisualSystemWorld::selfSetupSystemGui()
{
    sysGui->addLabel("Cities");
    sysGui->addSlider("noisePeaks", 0.0, 500, &pointNoisePeaks);
    sysGui->addSlider("rippleThreshold", 0.0, 1.0, &rippleThreshold);
    
    sysGui->addLabel("Flocking particles");
    sysGui->addSlider("max_number", 0.0, 1000, &nMaxPoints);
    sysGui->addSlider("initialForce", 0.0, 10.0, &initialForce);
    sysGui->addSlider("density", 0.5, 1.0, &density);
    sysGui->addSlider("gravity", 0.0, 1.0, &gravity);
    sysGui->addSlider("repulsion", 0.0, 1.0, &repulsion);
    sysGui->addSlider("turbulence", 0.0, 0.09, &turbulence);
    sysGui->addSlider("neigbordhood", 1, 300, &neigbordhood);
    sysGui->addSlider("independence", 0, 0.01, &independence);

    sysGui->addLabel("Satelite");
    sysGui->addSlider("Satelites_ammount", 0.0, 1000, &nMaxSatellites);
    sysGui->addSlider("Satelites_links_distance", 0.0, 200, &satLinksDist);
    sysGui->addSlider("Satelites_links_alpha", 0.0, 1.0, &satLinksAlpha);
    
    sysGui->addLabel("Arcs");
    sysGui->addSlider("Arcs_ammount", 0.0, 1000, &arcsMax);
}

void CloudsVisualSystemWorld::selfSetupRenderGui()
{
    rdrGui->addLabel("Solid Sphere");
    rdrGui->addSlider("Solid_Sphere_Scale", 0.0, 1.25, &solidSphereScale);
    rdrGui->addSlider("Solid_Sphere_Alpha", 0.0, 1.0, &solidSphereAlpha);
    
    rdrGui->addLabel("Wire Sphere");
    rdrGui->addSlider("Wire_Sphere_Scale", 0.0, 1.25, &wireSphereScale);
    rdrGui->addSlider("Wire_Sphere_Alpha", 0.0, 1.0, &wireSphereAlpha);
    
    rdrGui->addLabel("Halo Sphere");
    rdrGui->addSlider("Halo_Sphere_Scale",0.0,1.25,&haloSphereScale);
    rdrGui->addSlider("Halo_Sphere_Alpha", 0.0, 1.0, &haloSphereAlpha);
    
    rdrGui->addLabel("World Map");
    rdrGui->addSlider("Rivers", 0.0, 1.0, &riversAlpha);
    rdrGui->addSlider("Coast", 0.0, 1.0, &coastAlpha);
    rdrGui->addSlider("Cities", 0.0, 1.0, &citiesAlpha);
    
    rdrGui->addLabel("Stars & Constelations");
    rdrGui->addSlider("Constelation_Min", 0.0, 1.0, &constelationMin);
    rdrGui->addSlider("Constelation_Max", 0.0, 1.0, &constelationMax);
    rdrGui->addSlider("Constelation_Randomizer", 0.0, 100, &constelationRnd);
    
    rdrGui->addLabel("Arcs");
    rdrGui->addSlider("Arcs_Alpha", 0.0, 1.0, &arcsAlpha);
    
    rdrGui->addLabel("Post-Process");
    rdrGui->addSlider("Chroma_Distortion", -1.0, 1.0, &postChromaDist);
    rdrGui->addSlider("Grain_Distortion", 0.0, 1.0, &postGrainDist);
}

void CloudsVisualSystemWorld::guiSystemEvent(ofxUIEventArgs &e)
{
    
    string name = e.widget->getName();
    
    if ( name == "Satelites_ammount" ){
//        for(int i = satellites.size()-1; i >= 0; i--){
//            delete satellites[i];
//            satellites.erase(satellites.begin()+i);
//        }
//        
//        for(int i = 0; i < 1000; i++ ){
//            wSatellite *newSat = new wSatellite();
//			newSat->satelliteMesh = &satellitesMesh;
//            newSat->place(400, ofVec3f(ofRandom(-0.01,0.01),ofRandom(-0.05,0.05),0.0));
//            satellites.push_back( newSat );
//        }
    }
}

void CloudsVisualSystemWorld::selfKeyPressed(ofKeyEventArgs & args){
    
}

void CloudsVisualSystemWorld::selfUpdate()
{
    //  Insert Particles if it's need
    //
    if ( (cities.size() > 2) && (particles.size() < nMaxPoints-2) ){
        int randomIndex = ofRandom(cities.size()-1);
        
        wParticle *newParticle = new wParticle();
        newParticle->set( cities[randomIndex]*1.01 );
        newParticle->loc = *newParticle;
        newParticle->vel = *newParticle - ofPoint(0,0,0);
        newParticle->vel.normalize();
        newParticle->vel *= initialForce;
        
        newParticle->color = cities[randomIndex].color;
        newParticle->bTrail = true;
        particles.push_back(newParticle);
    }
    
    //  Delete if they are to much
    //
    while (particles.size() > nMaxPoints) {
        delete particles[0];
        particles.erase(particles.begin());
    }
    
    //  Update
    //
    for(int i = particles.size()-1; i >= 0 ; i--){
        
        if ( particles[i]->bDead ){
            delete particles[i];
            particles.erase(particles.begin()+i);
        } else {
            particles[i]->addRepulsionForce( ofPoint(0,0,0), 500, repulsion);
            particles[i]->addAttractionForce( ofPoint(0,0,0), 1000, gravity);
            particles[i]->applyFlockingForce(&globalOffset,neigbordhood,independence);
            particles[i]->update(density);
        }
    }
    
    globalOffset += ofPoint(turbulence/neigbordhood,
							turbulence/neigbordhood,
							turbulence/neigbordhood);
    
    for(int i = 0; i < cities.size(); i++){
        cities[i].update();
    }
    
    for(int i = 0; i < secCities.size(); i++){
        secCities[i].update();
    }
    
    if ( (int)ofRandom(constelationRnd) == 1){
        selectedConstelation = constelations[ ofRandom(constelations.size()) ];
    }
    
    //  Update satellites
    //    
    for(int i = 0; i < satellites.size(); i++){
        satellites[i]->update();
    }
    
	for(int i = 0; i < nMaxSatellites; i++){
        for(int j = i+1; j < nMaxSatellites; j++){
//			int connectIndex = ofContains(satellites[i]->connectedIndeces, j) ?
//									ofFind(satellites[i]->connectedIndeces, j) : -1;
//			
//			bool linked = connectIndex >= 0;
//			bool shouldBeLinked = satellites[i]->distanceSquared( *satellites[j] ) <= satLinksDist*satLinksDist;
//			
//            if (!linked && shouldBeLinked){
//				satellites[i]->connectedIndecesIndedeces.push_back( satellitesMesh.getNumIndices() );
//				satellites[i]->connectedIndeces.push_back(j);
//				satellitesMesh.addIndex(i);
//				satellitesMesh.addIndex(j);
//			}
//			else if(linked && !shouldBeLinked){
//				int vertexIndex = satellites[i]->connectedIndeces[connectIndex];
//				int indexIndex = satellites[i]->connectedIndecesIndedeces[connectIndex];
//
//				satellitesMesh.getIndices().erase( satellitesMesh.getIndices().begin()+indexIndex+1);
//				satellitesMesh.getIndices().erase( satellitesMesh.getIndices().begin()+indexIndex  );
//				
//				satellites[i]->connectedIndeces.erase(satellites[i]->connectedIndeces.begin()+connectIndex);
//				satellites[i]->connectedIndecesIndedeces.erase(satellites[i]->connectedIndecesIndedeces.begin()+connectIndex);
//									  
//				for(int s = 0; s < nMaxSatellites; s++){
//					for(int sind = 0; sind < satellites[s]->connectedIndecesIndedeces.size(); sind++){
//						if(satellites[s]->connectedIndecesIndedeces[sind] > indexIndex+2){
//							satellites[s]->connectedIndecesIndedeces[sind] -= 2;
//						}
//					}
//				}
//									  
//				for(int ind = 0; ind < satellitesMesh.getNumIndices()-1; ind++){
//					if(satellitesMesh.getIndex(ind)   == i &&
//					   satellitesMesh.getIndex(ind+1) == j)
//					{
//						
//						satellitesMesh.getIndices().erase( satellitesMesh.getIndices().begin()+ind+1);
//						satellitesMesh.getIndices().erase( satellitesMesh.getIndices().begin()+ind  );
//						
//						satellites[i]->connectedIndeces.erase(satellites[i]->connectedIndeces.begin() + locindex);
//						
//						break;
//					}
//				}
			}
		}

	linkMesh.clear();
	linkMesh.setMode(OF_PRIMITIVE_LINES);
	for(int i = 0; i < nMaxSatellites; i++){
        for(int j = i+1; j < nMaxSatellites; j++){
			if (satellites[i]->distanceSquared( *satellites[j] ) <= satLinksDist*satLinksDist ){
				linkMesh.addVertex(*satellites[i]);
				linkMesh.addVertex(*satellites[j]);
			}
		}
    }
	
//	for(int i = nMaxSatellites; i < 1000; i++){
//		//kill any connections on these satellites
//	}
    //  Update Arcs
    //
    if ( (arcs.size() == 0 ) && (arcsMax > 0) ){
        wArc arc;
//		arc.arcMesh = &arcMesh;
        arc.init(cities[ofRandom(cities.size())], cities[ofRandom(cities.size())]);
        arcs.push_back(arc);

    } else if ( arcs.size() > arcsMax ){
        arcs[ofRandom(arcs.size())].kill();
    }
    
    for(int i = arcs.size()-1; i >= 0; i--){
        
        arcs[i].update();
        
        if ( arcs[i].doesDie() ){
            arcs.erase(arcs.begin()+i);
        }
		else if ( arcs[i].doesArrive() && arcs[i].bActive && arcs.size() < arcsMax){
            arcs[i].bActive = false;
            
            wArc arc;
//    		arc.arcMesh = &arcMesh;
			
            arc.init( arcs[ i ].getDst() , cities[ofRandom(cities.size())]);
            arcs.push_back(arc);
            
            arc.init( arcs[ i ].getDst() , cities[ofRandom(cities.size())]);
            arcs.push_back(arc);
        }
    }
}

void CloudsVisualSystemWorld::selfDraw()
{
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    ofPushMatrix();
    ofPushStyle();
    ofScale(0.3,0.3,0.3);
    ofRotateY(ofGetFrameNum()*0.01);
    
    //  Render globe
    //
//    mat->begin();
    
    //  SPHERE
    //  -------------------------------------
    //
    //  Wire Sphere
    ofNoFill();
    ofSetColor(255,wireSphereAlpha*255.0);
	ofSphere(0, 0, wireSphereScale*300);
    //  Solid Sphere
    ofFill();
    ofSetColor(20,solidSphereAlpha*255.0);
	ofSphere(0, 0, solidSphereScale*300 );
    //  Hallo
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    haloShader.begin();
    ofSetColor(0, 227, 255,haloSphereAlpha*255.0);
	ofSphere(0, 0, haloSphereScale*300);
	haloShader.end();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    //  WORLD MAP
    //  -------------------------------------
    //
    if (coastAlpha || riversAlpha){
        //  Coast
        ofSetLineWidth(0.5);
        ofSetColor(255,coastAlpha*255.0f);
        coastVbo.drawWireframe();
        //  Rivers
        ofSetLineWidth(0.2);
        ofSetColor(0,140,200,riversAlpha*255.0f);
        riversVbo.drawWireframe();
        ofSetColor(255,100);
    }
    if (citiesAlpha){
        for(int i = 0; i < secCities.size(); i++){
            secCities[i].draw(citiesAlpha*0.9);
        }
        for(int i = 0; i < cities.size(); i++){
            cities[i].draw(citiesAlpha);
        }
		citiesMesh.draw();
    }
    
    //  Particles
    //
    for(int i = 0; i < particles.size(); i++){
        particles[i]->draw();
    }
    
    //  Satellites
    //
//	ofMesh linkMesh;
    for(int i = 0; i < satellites.size(); i++){
        satellites[i]->draw();
//        for(int j = i+1; j < satellites.size() ; j++){
//            if (satellites[i]->distanceSquared( *satellites[j] ) <= satLinksDist*satLinksDist ){
//				linkMesh.addVertex(*satellites[i]);
//				linkMesh.addVertex(*satellites[j]);
//            }
//        }
    }
	
	ofSetColor(255,satLinksAlpha*255.0);
	linkMesh.draw();
	//satellitesMesh.draw();
	
//	linkMesh.setMode(OF_PRIMITIVE_LINES);
//	ofSetColor(255,satLinksAlpha*255.0);
	
//	linkMesh.draw();
    
    //  Arcs
    //
	ofSetColor(255,255*arcsAlpha);
    for(int i = 0; i < arcs.size(); i++){
        ofSetColor(255,255*arcsAlpha);
        arcs[i].draw();
    }
    glPointSize(1.0);
    
    ofPopStyle();
    ofPopMatrix();
    
    //  STARS & CONSTELATIONS ( outside the pushMatrix )
    //
    glPointSize(1.1);
    for(int i = 0; i < stars.size(); i++){
        if (stars[i]->constName == selectedConstelation && constelationRnd >= 1.0 ){
            stars[i]->constAlpha = ofLerp(stars[i]->constAlpha,constelationMax,0.01);
        } else {
            stars[i]->constAlpha = ofLerp(stars[i]->constAlpha,constelationMin,0.01);
        }
        stars[i]->draw();
    }
    
    glDisable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);
    
}

void CloudsVisualSystemWorld::selfPostDraw(){
    postShader.begin();
    postShader.setUniformTexture("tex1", postTexture, 1);
    postShader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
    postShader.setUniform2f("textureResolution", postTexture.getWidth(), postTexture.getHeight());
    postShader.setUniform1f("chromaDist", postChromaDist);
    postShader.setUniform1f("grainDist", postGrainDist);
	
    CloudsVisualSystem::selfPostDraw();
	
    postShader.end();
}


void CloudsVisualSystemWorld::selfSetupGuis()
{
    
}

void CloudsVisualSystemWorld::selfDrawBackground()
{
    
}

void CloudsVisualSystemWorld::selfDrawDebug()
{
    
}

void CloudsVisualSystemWorld::selfSceneTransformation()
{
    
}

void CloudsVisualSystemWorld::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemWorld::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemWorld::selfMouseMoved(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemWorld::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemWorld::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemWorld::selfSetupGui()
{
    
}

void CloudsVisualSystemWorld::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemWorld::guiRenderEvent(ofxUIEventArgs &e)
{
    
}