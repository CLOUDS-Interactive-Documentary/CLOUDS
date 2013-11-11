//
//  CloudsVisualSystemMarchingCubes.cpp
//

#include "CloudsVisualSystemMarchingCubes.h"
#include "CloudsRGBDVideoPlayer.h"


string surfaceTypes[] = { "noise", "spheres", "sine^2"};
float elapsedTime;
bool bPause = false;

void CloudsVisualSystemMarchingCubes::selfSetupGui()
{
	
	
}


void CloudsVisualSystemMarchingCubes::selfGuiEvent(ofxUIEventArgs &e)
{
	
}

void CloudsVisualSystemMarchingCubes::selfSetupSystemGui(){
	
}

void CloudsVisualSystemMarchingCubes::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemMarchingCubes::selfSetupRenderGui(){

}

void CloudsVisualSystemMarchingCubes::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemMarchingCubes::selfSetup(){
    glEnable(GL_DEPTH_TEST);
	
	differentSurfaces = 0;
	drawGrid = true;
	mc.setup();
	mc.setResolution(32,16,32);
	mc.scale.set( 500, 250, 500 );
    
	mc.setSmoothing( false );
    
	normalShader.load(getVisualSystemDataPath("MarchingCubes") + "shaders/normalShader");
	
}

void CloudsVisualSystemMarchingCubes::selfPresetLoaded(string presetPath){
	
}

void CloudsVisualSystemMarchingCubes::selfBegin(){
}



void CloudsVisualSystemMarchingCubes::selfUpdate(){
    
	if(!bPause) elapsedTime = ofGetElapsedTimef();
	
	if(differentSurfaces == 0){
		//NOISE
		float noiseStep = elapsedTime * .5;
		float noiseScale = .06;
		float noiseScale2 = noiseScale * 2.;
		for(int i=0; i<mc.resX; i++){
			for(int j=0; j<mc.resY; j++){
				for(int k=0; k<mc.resZ; k++){
					//noise
					float nVal = ofNoise(float(i)*noiseScale, float(j)*noiseScale, float(k)*noiseScale + noiseStep);
					if(nVal > 0.)	nVal *= ofNoise(float(i)*noiseScale2, float(j)*noiseScale2, float(k)*noiseScale2 + noiseStep);
					mc.setIsoValue( i, j, k, nVal );
				}
			}
		}
	}
	else if(differentSurfaces == 1){
		//SPHERES
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
	else if(differentSurfaces == 2){
		//SIN
		float sinScale = .5;
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
    //	mc.update(threshold);
	
}

void CloudsVisualSystemMarchingCubes::selfDraw(){
    
    float elapsedTime = ofGetElapsedTimef();
    ofSetWindowTitle( ofToString( ofGetFrameRate() ) );
	
	camera.begin();
	
	//draw the mesh
	normalShader.begin();
    
	wireframe?	mc.drawWireframe() : mc.draw();
	
	normalShader.end();
	
	//draw the voxel grid
	if(drawGrid)	mc.drawGrid();
	
	camera.end();
	
	string info = "fps:" + ofToString(ofGetFrameRate()) +
	+ "\nnum vertices:" + ofToString(mc.vertexCount, 0)
	+ "\nthreshold:" + ofToString(mc.threshold)
    + "\n' ' changes surface type, currently " + surfaceTypes[differentSurfaces]
	
    + "\n's' toggles smoothing"
    + "\n'w' toggles wireframe"
    + "\n'f' flips normals"
    + "\n'g' toggles draw grid"
    + "\n'p' toggles pause"
    + "\n'up/down' +- threshold";
    
	ofDrawBitmapString(info, 20, 20);
	
}


void CloudsVisualSystemMarchingCubes::selfDrawDebug(){
	
}

void CloudsVisualSystemMarchingCubes::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}

void CloudsVisualSystemMarchingCubes::selfEnd(){
}

void CloudsVisualSystemMarchingCubes::selfExit()
{

}

void CloudsVisualSystemMarchingCubes::selfKeyPressed(ofKeyEventArgs &args){

switch (args.key) {
    case 'w':
        wireframe = !wireframe;
        break;
        
    case 'f':
        mc.flipNormals();
        break;
        
    case 's':
        mc.setSmoothing( !mc.getSmoothing() );
        break;
        
    case 'g':
        drawGrid = !drawGrid;
        break;
        
    case ' ':
        differentSurfaces++;
        if(differentSurfaces>=3){
            differentSurfaces = 0;
        }
        break;
        
    case 'p':
        bPause = !bPause;
        break;
        
    case OF_KEY_UP:
        mc.threshold += .03;
        break;
    case OF_KEY_DOWN:
        mc.threshold -= .03;
        break;
        
    default:
        break;
}

	
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


