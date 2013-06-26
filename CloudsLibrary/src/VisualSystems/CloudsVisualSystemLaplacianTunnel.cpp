
#include "CloudsVisualSystemLaplacianTunnel.h"

int loadMesh(ofVbo &vbo, string path) {
    char* buffer;
    long size;
	
	//cout << "path is " << path << endl;
	
    ifstream data(ofToDataPath(path,true).c_str(), ios::in | ios::binary);
    data.seekg(0, ios::end);
    size = data.tellg();
    data.seekg(0, ios::beg);
    buffer = new char[size];
	
    data.read(buffer, size);
    data.close();
	
    unsigned int *ints = (unsigned int *) buffer;
    int numPts = ints[0];
    int numTriangles = ints[1];
    float *pts = (float *) (ints+2);
    unsigned int * indices = ints + 2 + numPts*6;
    //not sure what is enable or disable by default
    vbo.enableIndices();
    vbo.enableNormals();
    vbo.disableColors();
    vbo.disableTexCoords();
    vbo.setVertexData(pts,3,numPts,GL_STATIC_DRAW,sizeof(float)*3);
    vbo.setNormalData(pts+numPts*3,numPts,GL_STATIC_DRAW,sizeof(float)*3);
    vbo.setIndexData(indices,numTriangles*3,GL_STATIC_DRAW);
	
	//cout << "File " << path << " has " << numTriangles << " triangles " << endl;
	return numTriangles*3;
}

CloudsVisualSystemLaplacianTunnel::CloudsVisualSystemLaplacianTunnel(){
	frameCount = 0;
	fps = 15;
}

string CloudsVisualSystemLaplacianTunnel::getSystemName(){
	return "LaplacianTunnel";
}

void CloudsVisualSystemLaplacianTunnel::selfSetup(){

}

void CloudsVisualSystemLaplacianTunnel::selfSetupGuis(){
	
	cout << "SETUP!" << endl;
	
    customGui = new ofxUISuperCanvas("Settings", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	
    customGui->setName("ExampleCustom");
    customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("num replications", 1, 100, &numReplications);
	customGui->addSlider("replication offset", 0, 500, &replicationOffset);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemLaplacianTunnel::selfGuiEvent);
	
    guis.push_back(customGui);
    guimap[customGui->getName()] = customGui;
}


void CloudsVisualSystemLaplacianTunnel::selfUpdate(){
	
}

void CloudsVisualSystemLaplacianTunnel::selfDrawBackground(){
	
}

void CloudsVisualSystemLaplacianTunnel::selfDrawDebug(){
	
}

void CloudsVisualSystemLaplacianTunnel::selfSceneTransformation(){
	
}

void CloudsVisualSystemLaplacianTunnel::selfDraw(){
	
	if(vbos.size() > 0){
		
		int vboIndex = int( (ofGetElapsedTimef() - startTime) * fps)  % vbos.size() ;

		ofPushMatrix();
		for(int i = 0; i < numReplications; i++){
			
			ofTranslate(0,replicationOffset,0);
			
			vbos[vboIndex]->drawElements(GL_TRIANGLES, indexCount[vboIndex]);
		}
		ofPopMatrix();
	}
	
}

void CloudsVisualSystemLaplacianTunnel::selfExit(){
	
}

void CloudsVisualSystemLaplacianTunnel::selfBegin(){
	
	cout << "*** BEGIN!" << endl;
	
	ofDirectory objs(getVisualSystemDataPath() + "_meshes/");
	objs.allowExt("vbo");
	objs.listDir();

	clear();
	
	int numFiles = objs.numFiles();
	vbos.resize( numFiles );
	indexCount.resize( numFiles );
	for(int i = 0; i < numFiles; i++){
		vbos[i] = new ofVbo();
		indexCount[i] = loadMesh(*vbos[i], objs.getPath( i ) );
	}
	
	startTime = ofGetElapsedTimef();
}

void CloudsVisualSystemLaplacianTunnel::selfEnd(){
	clear();
}

void CloudsVisualSystemLaplacianTunnel::clear(){
	if(vbos.size() > 0){
		for(int i = 0; i < vbos.size(); i++){
			vbos[i]->clear();
			delete vbos[i];
		}
		vbos.clear();
		indexCount.clear();
	}
	
}
void CloudsVisualSystemLaplacianTunnel::selfKeyPressed(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemLaplacianTunnel::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemLaplacianTunnel::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemLaplacianTunnel::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemLaplacianTunnel::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemLaplacianTunnel::selfMouseReleased(ofMouseEventArgs& data){
	
}


void CloudsVisualSystemLaplacianTunnel::selfSetupGui(){
	
}

void CloudsVisualSystemLaplacianTunnel::selfGuiEvent(ofxUIEventArgs &e){
	
}


void CloudsVisualSystemLaplacianTunnel::selfSetupSystemGui(){
	
}

void CloudsVisualSystemLaplacianTunnel::guiSystemEvent(ofxUIEventArgs &e){
	
}


void CloudsVisualSystemLaplacianTunnel::selfSetupRenderGui(){
	
}

void CloudsVisualSystemLaplacianTunnel::guiRenderEvent(ofxUIEventArgs &e){
	
}