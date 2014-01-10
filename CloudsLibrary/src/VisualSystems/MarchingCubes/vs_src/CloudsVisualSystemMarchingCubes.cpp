//
//  CloudsVisualSystemMarchingCubes.cpp
//

#include "CloudsVisualSystemMarchingCubes.h"
#include "CloudsRGBDVideoPlayer.h"

void CloudsVisualSystemMarchingCubes::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("MARCHING CUBES", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("MarchingCubes");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    customGui->addToggle("PAUSED", &bPaused);
    customGui->addToggle("DRAW GRID", &bDrawGrid);
    customGui->addToggle("RENDER NORMALS", &bRenderNormals);
    customGui->addButton("FLIP NORMALS", false);
    customGui->addToggle("SMOOTHED", &mc.bSmoothed);
    customGui->addToggle("WIREFRAME", &bWireframe);
    customGui->addSlider("THRESHOLD", 0, 1, &mc.threshold);
    
    customGui->addSpacer();
    vector<string> surfaceTypes;
    surfaceTypes.push_back("NOISE");
    surfaceTypes.push_back("SPHERES");
    surfaceTypes.push_back("SINE^2");
    customGui->addRadio("SURFACE TYPES", surfaceTypes);
    
    customGui->addSpacer();
    fgHue = new ofx1DExtruder(0);
    fgHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(fgHue);
    customGui->addSlider("FG HUE", 0.0, 255.0, fgHue->getPosPtr());
    fgSat = new ofx1DExtruder(0);
    fgSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(fgSat);
    customGui->addSlider("FG SAT", 0.0, 255.0, fgSat->getPosPtr());
    fgBri = new ofx1DExtruder(0);
    fgBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(fgBri);
    customGui->addSlider("FG BRI", 0.0, 255.0, fgBri->getPosPtr());
    fgAlpha = new ofx1DExtruder(0);
    fgAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(fgAlpha);
    customGui->addSlider("FG ALPHA", 0.0, 255.0, fgAlpha->getPosPtr());
    customGui->addSpacer();
    customGui->addToggle("Motion", &motion);
    customGui->addToggle("Use Grain", &bNoisy);
    customGui->addToggle("Use Color Grain", &bColorGrain);
    customGui->addSlider("Grain Amnt", 0., 1., &noiseAmt);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemMarchingCubes::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
    
    //fog gui
	fogGui = new ofxUISuperCanvas("FOG", gui);
	fogGui->copyCanvasStyle(gui);
	fogGui->copyCanvasProperties(gui);
	fogGui->setName("FOG");
	fogGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	fogGui->addSpacer();
	fogGui->addSlider("fogDist", 10, 1000, &fogDist);
	fogGui->addSlider("fogExpo", .6, 3., &fogExpo);
	
    //fogGui->addImageSampler("fogColor", &colorMap, 100, 100);
	
	ofAddListener(fogGui->newGUIEvent, this, &CloudsVisualSystemMarchingCubes::selfGuiEvent);
	guis.push_back(fogGui);
	guimap[fogGui->getName()] = fogGui;

}

void CloudsVisualSystemMarchingCubes::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "PAUSED") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            pauseTime = ofGetElapsedTimef();
        }
        else {
            startTime += ofGetElapsedTimef() - pauseTime;
        }
    }
    else if (e.widget->getName() == "FLIP NORMALS" && ((ofxUIToggle *)e.widget)->getValue()) {
        mc.flipNormals();
    }
	
    else if (e.widget->getName() == "NOISE" && ((ofxUIToggle *)e.widget)->getValue()) {
        differentSurfaces = 0;
    }
    else if (e.widget->getName() == "SPHERES" && ((ofxUIToggle *)e.widget)->getValue()) {
        differentSurfaces = 1;
    }
    else if (e.widget->getName() == "SINE^2" && ((ofxUIToggle *)e.widget)->getValue()) {
        differentSurfaces = 2;
    }
    
    else if (e.widget->getName() == "FG HUE") {
        fgHue->setPosAndHome(fgHue->getPos());
	}
    else if (e.widget->getName() == "FG SAT") {
        fgSat->setPosAndHome(fgSat->getPos());
	}
    else if (e.widget->getName() == "FG BRI") {
        fgBri->setPosAndHome(fgBri->getPos());
	}
    else if (e.widget->getName() == "FG ALPHA") {
        fgAlpha->setPosAndHome(fgAlpha->getPos());
    }
}

void CloudsVisualSystemMarchingCubes::selfSetupSystemGui(){
	
}

void CloudsVisualSystemMarchingCubes::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemMarchingCubes::selfSetupRenderGui(){

}

void CloudsVisualSystemMarchingCubes::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemMarchingCubes::selfSetup()
{
    // Set defaults.
    bPaused = false;
    startTime = pauseTime = elapsedTime = 0;
    
    bRenderNormals = true;
	bWireframe = false;
    bDrawGrid = true;
    differentSurfaces = 0;
    
    noiseAmt = .1;
    bNoisy = false;
    bColorGrain = false;
    
    motion  = false;
    
    fgColor.set(128);

	mc.setup();
	mc.setResolution(32, 16, 32);
	mc.scale.set(500, 250, 500);
    
	mc.setSmoothing(false);
    fogDist = 650.f;
    fogExpo = 2.5f;
    reloadShaders();
}

void CloudsVisualSystemMarchingCubes::selfPresetLoaded(string presetPath){
	
}

void CloudsVisualSystemMarchingCubes::reloadShaders(){
    normalShader.load(getVisualSystemDataPath() + "shaders/normalShader");
    styleShader.load(getVisualSystemDataPath() + "shaders/style");
}

void CloudsVisualSystemMarchingCubes::selfBegin(){

}

void CloudsVisualSystemMarchingCubes::selfUpdate()
{    
    fgColor.setHsb(fgHue->getPos(), fgSat->getPos(), fgBri->getPos(), fgAlpha->getPos());

    if (!bPaused) elapsedTime = ofGetElapsedTimef() - startTime;
	
	if (differentSurfaces == 0) {  // NOISE
		float noiseStep = elapsedTime * .5;
		float noiseScale = .06;
		float noiseScale2 = noiseScale * 2.;
		for(int i=0; i<mc.resX; i++){
			for(int j=0; j<mc.resY; j++){
				for(int k=0; k<mc.resZ; k++){
					//noise
					float nVal = ofNoise(float(i)*noiseScale, float(j)*noiseScale, float(k)*noiseScale + (motion ? noiseStep : 0.f) );
					if(nVal > 0.)	nVal *= ofNoise(float(i)*noiseScale2, float(j)*noiseScale2, float(k)*noiseScale2 + (motion ? noiseStep : 0.f) );
					mc.setIsoValue( i, j, k, nVal );
				}
			}
		}
	}
	else if (differentSurfaces == 1) {  // SPHERES
		ofVec3f step = ofVec3f(3./mc.resX, 1.5/mc.resY, 3./mc.resZ) * PI;
		for(int i=0; i<mc.resX; i++){
			for(int j=0; j<mc.resY; j++){
				for(int k=0; k<mc.resZ; k++){;
					float val = sin(float(i)*step.x) * sin(float(j+elapsedTime)*step.y) * sin(float(k+elapsedTime)*step.z);
					val *= val;
					mc.setIsoValue( i, j, k, val );
				}
			}
		}
	}
	else if (differentSurfaces == 2) {  // SIN^2
		static float sinScale = .5f;
		for(int i=0; i<mc.resX; i++){
			for(int j=0; j<mc.resY; j++){
				for(int k=0; k<mc.resZ; k++){
					float val = sin(float(i)*sinScale) + cos(float(j)*sinScale) + sin(float(k)*sinScale + elapsedTime);
					mc.setIsoValue( i, j, k, val * val );
				}
			}
		}
	}
	
	//update the mesh
	mc.update();
    fc = bgColor;
}

void CloudsVisualSystemMarchingCubes::selfDraw()
{
    if (!bRenderNormals && fgColor.a < 255.0f) {
        glDisable(GL_DEPTH_TEST);
    }
    else {
        glEnable(GL_DEPTH_TEST);
    }
	mat->begin();
    glEnable(GL_DEPTH_TEST);
    //draw the mesh
	styleShader.begin();
    styleShader.setUniform1i("bRenderNormals", bRenderNormals);
    styleShader.setUniform4f("fgColor", fgColor.r / 255.0f, fgColor.g / 255.0f, fgColor.b / 255.0f, fgColor.a / 255.0f);
    
    styleShader.setUniform4f("fogColor", fc.r, fc.g, fc.g, fc.a );
	styleShader.setUniform1f("fogDist", fogDist );
	styleShader.setUniform1f("fogExpo", fogExpo );
    styleShader.setUniform1f("grainAmnt", noiseAmt );
	styleShader.setUniform1f("bUseGrain", bNoisy );
	styleShader.setUniform1f("bUseColorGrain", bColorGrain );
    
    {
        bWireframe?	mc.drawWireframe() : mc.draw();
	}
    
	styleShader.end();
	
    mat->end();
	//draw the voxel grid
    
    if (bDrawGrid) mc.drawGrid();
    
    glDisable(GL_DEPTH_TEST);
    
}


void CloudsVisualSystemMarchingCubes::selfDrawDebug(){
	
}

void CloudsVisualSystemMarchingCubes::selfDrawBackground(){

}

void CloudsVisualSystemMarchingCubes::selfEnd(){
}

void CloudsVisualSystemMarchingCubes::selfExit(){

}

void CloudsVisualSystemMarchingCubes::selfKeyPressed(ofKeyEventArgs &args){
	if(args.key == 'r')reloadShaders();
}

void CloudsVisualSystemMarchingCubes::selfKeyReleased(ofKeyEventArgs &args){
	
}

void CloudsVisualSystemMarchingCubes::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMarchingCubes::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMarchingCubes::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMarchingCubes::selfMouseReleased(ofMouseEventArgs& data){
	
}


