#include "CloudsVisualSystemNeurons.h"
#include <GLUT/GLUT.h>

#define DEFAULT_BLUE 0xC4 / 255.0
#define DEFAULT_GREEN 0x9A / 255.0
#define DEFAULT_RED 0x6C / 255.0

#define _N jtn::TreeNode

#pragma mark

//statics

float _C::dotSize = 0.1;
float _C::axonThickness = 0;
float _C::alpha = 0;
float _C::sway = 0;
int _C::nodeMax = 0;
int _C::rootCount = 4;
float _C::danceAmp = 0;
float _C::danceFreq = 0;
float _C::danceOffset = 0;
bool _C::renderNeurons = true;
jtn::Box _C::boundingBox;
float _C::colorMix = 0.0;
bool _C::renderCamPath = true;
void _C::selfSetup(){
    rotation = 0;
    reset();
    readFromFile( "brain1" );
    generateFlythrough();
    
}

void _C::selfSetupGuis(){
    spinSlider = gui->addSlider("Spin Speed",-6.0, 6.0,1);
    dotSizeSlider = gui->addSlider("Dot Size",1, 64, 2);
    nucleusSize = gui->addSlider("Nucleus Size",0, 5, 1);
    axonThicknessSlider = gui->addSlider("Axon Thickness",0, 10, 5);
    alphaSlider = gui->addSlider("Alpha",0, 1, 0.5);
    swaySlider = gui->addSlider("Sway",0, 10, 5);
    nodeMaxSlider = gui->addSlider("Max Nodes",0, 100000, 10000);
    resetButton = gui->addButton("Reset", false, 64,64);
    rootCountSlider = gui->addSlider("Seed Count", 2,64,4);
    danceAmpSlider = gui->addSlider("Dance Amplitude",0,10,5);
    danceFreqSlider = gui->addSlider("Dance Frequency",0,0.5,0.1);
    danceOffsetSlider = gui->addSlider("Dance Offset",0,0.5,0.1);
    saveButton = gui->addButton("Save Neurons", false, 32,32);
    loadButton = gui->addButton("Load Neurons", false, 32,32);

    
    
    generateCamPath = camGui->addButton( "Generate Flythrough",false, 32,32);
    generateRandCam = camGui->addButton( "Generate Random Bounce",false, 32,32);
    tumbleCam = camGui->addButton( "Quatumble",false, 32,32);
    camDuration = camGui->addSlider("Cam Path Duration",0,120,60);

    
    rdrGui->addToggle("Show Neurons", &renderNeurons);
    rdrGui->addSlider("Depth Coloring", 0.0, 1.0, &_C::colorMix);
    rdrGui->addToggle("Show Camera Path", &renderCamPath);
}



void _C::selfUpdate(){
    
    cloudsPathCam.update();
    
    rotation += spinSlider->getScaledValue();
    
    ofCamera& cam = getCameraRef();
    
    _N::terminals.clear();
    vector<_N*>::iterator it;
    for(it=rootNodes.begin();it!=rootNodes.end();it++){
        (*it)->update();
    }
    
    
    // these are separate because children are born in the previous update calls
    // and we want to catch them all
    
    for(it=_N::all.begin();it!=_N::all.end();it++){
        (*it)->updateScreenSpace(cam);
    }
    
    
    updateBoundingBox();
    
    axonThickness = axonThicknessSlider->getScaledValue();
    dotSize = dotSizeSlider->getScaledValue();
    alpha = alphaSlider->getScaledValue();
    sway = swaySlider->getScaledValue();
    nodeMax = nodeMaxSlider->getScaledValue();
    rootCount = rootCountSlider->getScaledValue();
    danceAmp = danceAmpSlider->getScaledValue();
    danceFreq = danceFreqSlider->getScaledValue();
    danceOffset = danceOffsetSlider->getScaledValue();
    
    
}

void _C::updateBoundingBox(){
    
    // reset
    boundingBox.setOppositeExtremes();
    
    // loop through and compare bounds
    vector<_N*>::iterator it;
    for(it=_N::all.begin();it!=_N::all.end();it++){
        boundingBox.stretch( (*it)->screenSpace );
    }
}

void _C::reset(bool createRootNodes){
    
    //free all dynamically allocated memory
    
    vector<_N*>::iterator it;
    for( it=_N::all.begin() ; it!=_N::all.end() ; it++){
        delete *it;
    }
    
    //clear all linked lists
    
    _N::all.clear();
    rootNodes.clear();
	
    
    // lay down new root nodes in an inward facing spherical formation
    
    if(createRootNodes){
        int stepsTheta = sqrt(rootCount);
        int stepsPhi = sqrt(rootCount);
        float incTheta = (PI*2)/(float)stepsTheta;
        float incPhi = (PI)/(float)stepsPhi;
        for(int j=0;j<stepsPhi+1;j++){
            for(int i=0;i<stepsTheta;i++){
                
                //sphere coordinate math
                float x = cos(i*incTheta) * sin(j*incPhi);
                float y = sin(i*incTheta) * sin(j*incPhi);
                float z = cos(j*incPhi);
                
                _N *n = new _N();
                n->generation = 0;
                
                //set the direction to current point scaled out.
                // only works if pivot == origin
                
                n->future.x = n->x = x * 50;
                n->future.y = n->y = y * 50;
                n->future.z = n->z = z * 50;
                n->direction = ofPoint(-x,-y,-z);
                rootNodes.push_back(n);
            }
        }
    }
}

void _C::guiCameraEvent(ofxUIEventArgs &e){
    CloudsVisualSystem::guiCameraEvent(e);
    if(e.widget == generateCamPath && ofGetMousePressed() ) {
        generateFlythrough();
    }else if(e.widget == generateRandCam && ofGetMousePressed() ) {
        generateRandCamBounce();
    }else if(e.widget == camDuration && ofGetMousePressed() ) {
        cloudsPathCam.setDuration(camDuration->getScaledValue());
    }else if(e.widget == tumbleCam && ofGetMousePressed() ) {
        _N::clearPathFlags();
        cloudsPathCam.clear();
    }
}


void _C::selfGuiEvent(ofxUIEventArgs &e){
    if( e.widget->getName()=="Reset" && ofGetMousePressed() ){
        reset();
    }else if(e.widget == saveButton && ofGetMousePressed()){
        writeToFile( "new_brain" );
    }else if(e.widget == loadButton && ofGetMousePressed()){
        cout << ofGetTimestampString() << endl;
        readFromFile( "brain1" );
        cout << ofGetTimestampString() << endl;
    }
}

string _C::getSystemName(){
  return "Neurons";
}

void _C::generateRandCamBounce(){
    //reset cam path.
    cloudsPathCam.clear();

    _N::clearPathFlags();
    
    float s = 50;
    ofVec3f firstPos;
    for(int i=0;i<10.0;i++){
        // add a random point somewhere
        ofVec3f p = ofVec3f(
                            ofRandomf() * s,
                            ofRandomf() * s,
                            ofRandomf() * s
                            );
        cloudsPathCam.addPositionControlVertex( p );
        cloudsPathCam.addTargetControlVertex(ofVec3f());
        
        if(i==0)firstPos = p;
    }
    
    //one more to make it loop.
    
    cloudsPathCam.addPositionControlVertex( firstPos );
    cloudsPathCam.addTargetControlVertex(ofVec3f());
    
    
    // orient cam path to look inward,
    // so we always start out seeing something full
    
    ofCamera& cam = getCameraRef();
    cam.lookAt(ofVec3f());
}

void _C::generateFlythrough(){
    //reset cam path.
    cloudsPathCam.clear();

    _N::clearPathFlags();
    
    // find someone in the youngest possible generation of terminal
    // therefore insuring a long path between a terminal and a root parent.
    
    int youngestGen = 0;
    _N *thisNode = NULL;
    vector<jtn::TreeNode*>::iterator nit = _N::all.begin();
    for(;nit!=_N::all.end();nit++){
        if( (*nit)->isTerminal() ){
            if((*nit)->generation > youngestGen) {
                youngestGen = (*nit)->generation;
                thisNode = (*nit);
            }
        }
    }
    
    
    vector<ofVec3f> pts; //collecting points as i go, so i can add them backwards, later.
    
    ofVec3f firstPoint = *thisNode; // make a copy of the first point for lookAt() later.
    
    float camPosOffset = 1.5;

    while(thisNode != NULL){
		thisNode->isPartOfCamPath = true;
        cloudsPathCam.addPositionControlVertex( *thisNode);
		pts.push_back( ofVec3f( thisNode->x,
								thisNode->y,
								thisNode->z) );
		thisNode = thisNode->parent;
	}
	
//    while(thisNode != NULL && thisNode->parent != NULL  ){
//
//        thisNode->isPartOfCamPath = true;
//        cloudsPathCam.addPositionControlVertex( *thisNode);
//        //cloudsPathCam.addTargetControlVertex(ofVec3f());
//        //cloudsPathCam.addUpControlVertex(ofVec3f(1,0,0));
//        pts.push_back( *thisNode );
//
//        //traverse up the parent
//        
//        if( thisNode->parent != NULL ){
//            thisNode = thisNode->parent;
//        }
//		else{
//            break;
//        }
//    }
    
    // then add the same points in reverse, to scrub back and forth
    // rather than seeing where the loop wraps around.
    
//    vector<ofVec3f>::reverse_iterator pit = pts.rbegin();
	
    //for(; pit != pts.rend(); pit++){
	for(int i = pts.size()-1; i >= 0; i--){
        cloudsPathCam.addPositionControlVertex( pts[i] );
        //cloudsPathCam.addTargetControlVertex(ofVec3f());
        //cloudsPathCam.addUpControlVertex(ofVec3f(1,0,0));
    }

    // orient cam path to look inward,
    // so we always start out seeing something nefull
    ofCamera& cam = getCameraRef();
    cam.lookAt( -firstPoint );
}

ofCamera& _C::getCameraRef(){
    if(cloudsPathCam.getPositionSpline().getControlVertices().size()==0 ){
        //do the overloaded behavior.
        return CloudsVisualSystem::getCameraRef();
    }else{
        //disabled "look around" during the path tour.
        //ogCam.setPosition(  cloudsPathCam.getPosition() - camDistance );
        return cloudsPathCam;
    }
}

/**
    serializes node network into a flat file
 */
void _C::writeToFile(string filename){
    //open outfile
    ofLogNotice() << "Saving neurons to " << filename;
    ofstream fout((ofToDataPath(getVisualSystemDataPath()) + string("sets/") + filename ).c_str(), ios::binary);
    fout << "NeuronNetwork " << 0 // important! increment this version number as needed.
        << endl;
    
    // flatten all neurons to a file
    vector<_N*>::iterator nit = _N::all.begin();
    for(;nit!=_N::all.end();nit++){
        (*nit)->serialize(fout);
    }
    
    fout.close();
    ofLogNotice() << "Done saving.";
}



/**
 unserializes node network from a flat file and into memory.
 */
void _C::readFromFile(string filename){
    
    reset(false);
    
    ofLogNotice() << "Loading neurons from " << filename;
    ifstream fin((ofToDataPath(getVisualSystemDataPath()) + string("sets/") + filename).c_str());
    
    while(fin.good()){
        string token;
        fin >> token;
        if(token=="NeuronNetwork"){ // file header
            float version;
            fin >> version;
            ofLogNotice() << "NeuronNetwork file version " << version;
        }else if(token=="N"){
            _N *n = new _N(fin); // node will parse the file by itself.
        }else if(token==""){
            fin.close();
            break;
        }else{
            ofLogError() << "Unrecognized token: " << token;
        }
    }
    
    if(fin.is_open()){
        fin.close();
    }
    
    // resolve indices to pointers.
    
    vector<_N*>::iterator nit = _N::all.begin();
    for(;nit != _N::all.end();nit++){
        if( ((int)(*nit)->parent) == -1 ){ // if it was stored in the file as -1
            (*nit)->parent = NULL; // convert it back to null.
            rootNodes.push_back(*nit); // collect the parent into the root list.
        }else{
            //otherwise a straight element index.
            (*nit)->parent = _N::all[ (int)((*nit)->parent) ];
        }
        
        // start a fresh list of pointers
        vector<_N*> pointers;
        
        //for all children
        vector<_N*>::iterator cit = (*nit)->children.begin();
        for(;cit != (*nit)->children.end(); cit++){
            
            //record the current pointer for the given index.
            pointers.push_back( _N::all[ (int)(*cit) ] );
        }
        
        //swap lists
        (*nit)->children = pointers;
        
        
    }
    
    ofLogNotice() << "Done loading.";

}




void _C::selfPresetLoaded(string presetPath){
	reset();
}

void _C::selfDrawBackground(){
 //   ofBackgroundGradient(ofColor(0), ofColor(255));
}

void _C::selfDraw(){
	
	//was leaking color states
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	ofPushStyle();
	
    ofPushMatrix();

    //ofTranslate(0,0,camDistance);
    
    //cloudsPathCam.draw();

    //ofSetColor(0,0xff,0);
    //cloudsPathCam.drawPaths(5);

    //ofRotate(rotation,0,0,1);

    //some camera sway
    ofTranslate(
                ofNoise( ofGetFrameNum() * 0.01 , 1000) * _C::sway,
                ofNoise( ofGetFrameNum() * 0.01 , 2000) * _C::sway,
                ofNoise( ofGetFrameNum() * 0.01 , 3000) * _C::sway
    );
    
    
    ofCamera &cam = getCameraRef();
    /*
    
    cam->setGlobalPosition( *rootNodes[0] );
     */
    
    
    if(renderNeurons){
        
        _N::drawMode = GL_LINES;
        ofMesh m;
        // for all root nodes:
        vector<_N*>::iterator it;
        int tCount=0;
        for(it=rootNodes.begin();it!=rootNodes.end();it++){
            
            //draw a nookilus right where the node is.
            ofPushMatrix();
            ofTranslate((*it)->x,(*it)->y,(*it)->z);
            ofSetColor( ofFloatColor( (*it)->r,(*it)->g,(*it)->b,_C::alpha) );
			
			//TODO: use ofSphere as GLUT will leave with of 0.8
            glutSolidSphere(nucleusSize->getScaledValue(),8,8);
            ofPopMatrix();
            
            //tell the thing to draw a line for itself.
            (*it)->draw();
            tCount++;
        }
        
		//TODO: remove all immediate modes and replace with ofMesh
        glPointSize(dotSize);
        //glBegin(GL_POINTS);		
//        glColor3f(1,1,1);
        // for all terminals
        for(it=_N::terminals.begin();it!=_N::terminals.end();it++){
//            glVertex3f( (*it)->x,
//                                    (*it)->y,
//                                    (*it)->z );
			m.addVertex(ofVec3f( (*it)->x,(*it)->y,(*it)->z) );
		}
		
		ofSetColor(255);
		m.drawVertices();
        glEnd();
        
    }

	ofPopMatrix();
	
    ofSetColor(255);
//    stringstream fps;
//    fps << "FPS: " << ofGetFrameRate();
//    cout << fps.str() << endl;
	
	ofPopStyle();
	glPopAttrib();
    
}

//############################################################
#pragma mark

vector<_N*> _N::all;
vector<_N*> _N::terminals;
GLuint _N::drawMode = 0;

int _N::maxDepth = 0;

void _N::updateScreenSpace(ofCamera &cam){
    //cache my world space value for use in draw()
    screenSpace = cam.worldToCamera(  *this );
    //screenSpace =  cam.worldToScreen( cam.cameraToWorld( *this ) );

}

void _N::update(){

    
    
    int n = ofGetFrameNum();
    
    ofPoint temp(future);
    
    temp += ofPoint(
                    ofSignedNoise( n * _C::danceFreq , future.x  * _C::danceFreq , generation * _C::danceOffset ),
                    ofSignedNoise( n * _C::danceFreq , future.y * _C::danceFreq , generation * _C::danceOffset ),
                    ofSignedNoise( n * _C::danceFreq , future.z  * _C::danceFreq, generation * _C::danceOffset )
                    ) * _C::danceAmp;
     
    
    
	x += (temp.x - x)*0.1;
	y += (temp.y - y)*0.1;
	z += (temp.z - z)*0.1;
    
	
	vector<_N*>::iterator it;
	
	for(it=children.begin();it!=children.end();it++){
		(*it)->update();
	}
	
    
    // branching rules
	if( _N::all.size() < _C::nodeMax
			&& (
		 (ofRandomuf() < 0.1 && children.size()==0)
		 ||
		 (ofRandomuf() < 0.001 && children.size()==1)
		 ||
		 (ofRandomuf() < 0.0001 && children.size()>1)
         ||
         (generation == 50)
                    )
		 ){

		_N *n = new _N();
		n->parent = this;
		
		if(parent==NULL){
			n->future.x = n->x = x + direction.x;
			n->future.y = n->y = y + direction.y;
			n->future.z = n->z = z + direction.z;
		}else{
			
			ofPoint v = (this->future) + ((this->future) - (*parent)).normalize()*2;
			
			v = v.getRotated(rot.x , ofVec3f(1,0,0));
			v = v.getRotated(rot.y , ofVec3f(0,1,0));
			v = v.getRotated(rot.z , ofVec3f(0,0,1));
			
			n->future.x = v.x;
			n->future.y = v.y;
			n->future.z = v.z;
 			
			n->x = x;
			n->y = y;
			n->z = z;
			
			n->rot.x = rot.x;
			n->rot.y = rot.y;
			n->rot.z = rot.z;
			
			float s = ofRandomf();
            if(generation==51)s=10;
			n->rot += ofPoint(ofRandomf(),ofRandomf(),ofRandomf())*s;
			n->rot += (ofPoint() - n->rot) * 0.5;
			
			n->r = r + ofRandomf()*0.1;
			n->g = g + ofRandomf()*0.05;
			//n->b = b + ofRandomf()*0.1;
			
			float average = (n->r + n->g + n->b) / 3.0;

			n->r += ( average - n->r ) * 0.01f;
			n->g += ( average - n->g ) * 0.01f;
			n->b += ( average - n->b ) * 0.01f;
			n->generation = generation + 1;
            n->updateMaxDepth();
		}
		children.push_back( n );
	}

	if( isTerminal() ){
		terminals.push_back(this);
	}
    
    
	age++;
}

void _N::updateMaxDepth(){
	if(maxDepth < generation)maxDepth = generation;
}

bool _N::isTerminal(){
	return children.size()==0 || (children.size()==1 && children[0]->age<1);
}

void _N::draw(){
	
	
	//TODO: remove immediate mode calls, replace with persistent meshes
	vector<_N*>::iterator that;
	   
    jtn::PointD worldNormPos = _C::boundingBox.getNormalized( screenSpace );
    
	for(that=children.begin(); that!=children.end();that++){
		
        if(isPartOfCamPath && _C::renderCamPath && ofGetFrameNum() % 8 > 4){
			ofSetColor(255,0,0,255);
        }else{
            ofSetColor(ofFloatColor(ofLerp(worldNormPos.x, r, _C::colorMix),
									ofLerp(worldNormPos.y, g, _C::colorMix),
									ofLerp(worldNormPos.z, b, _C::colorMix),
									_C::alpha));
        }

        
        if(drawMode==GL_LINES){
            ofSetLineWidth( ( 1 - (generation+1) / (float)maxDepth) * _C::axonThickness );
            glBegin(GL_LINES);
        }
        
		if( !(drawMode==GL_POINTS && isTerminal()) )
			glVertex3f(x,y,z);
		
		_N *t = *that;
        
        if(t->isPartOfCamPath && _C::renderCamPath && ofGetFrameNum() % 8 > 4){
            ofSetColor(255,0,0,255);
        }else{
            jtn::PointD worldNormPos2 = _C::boundingBox.getNormalized( t->screenSpace );
            ofSetColor(ofFloatColor(ofLerp(worldNormPos2.x, t->r, _C::colorMix),
									ofLerp(worldNormPos2.y, t->g, _C::colorMix),
									ofLerp(worldNormPos2.z, t->b, _C::colorMix),
									_C::alpha));
        }
        

		
		if( !(drawMode == GL_POINTS && t->isTerminal()) )
			glVertex3f(t->x,t->y,t->z);
        
        if(drawMode==GL_LINES){
            glEnd();
        }
		
		(*that)->draw();
	}
	
}

_N::TreeNode(){
	ident = all.size();
	all.push_back(this);
	parent = NULL;
	r = DEFAULT_RED;
	g = DEFAULT_GREEN;
	b = DEFAULT_BLUE;
	a = 1;
	generation = 0;
	age = 0;
    screenSpace = ofVec3f(0,0,0);
    isPartOfCamPath = false;
}

_N::TreeNode(ifstream &fin){

    ident = all.size();
	all.push_back(this);
    
    int fileIdent;
    fin >> fileIdent;
    if(fileIdent != ident){
        ofLogError() << "Neuron file loader: " << "non-matching identities: " << fileIdent << " != " << ident;
    }else{
        //cout << "MATCHING identities: " << fileIdent << " == " << ident << endl;
    }
    
    fin >> x >> y >> z >>
        future.x >> future.y >> future.z >>
        r >> g >> b >> a >>
        rot.x >> rot.y >> rot.z >>
        generation >> age >>
        direction.x >> direction.y >> direction.z;
    
    
    // store both parent index and child indices into pointer slots temporarily
    // and i promise to resolve the indices to pointers in a later global pass.
    
    int parentIndex;
    fin >> parentIndex;
    parent = (_N*)(parentIndex);
    
    int childCount;
    fin >> childCount;
    
    int childIndex;
    
    for(int i=0;i<childCount;i++){
        fin >> childIndex;
        children.push_back((_N*)childIndex);
    }
    
    isPartOfCamPath = false;
    
    updateMaxDepth();
    
    
    screenSpace = ofVec3f(0,0,0);
}

_N::~TreeNode(){
}

void _N::serialize(ofstream &fout){
    
    //dump out all my attributes.
    
    fout << "N " << ' ' <<
        ident << ' ' <<
        x << ' ' << y << ' ' << z << ' ' <<
        future.x << ' ' << future.y << ' ' << future.z << ' ' <<
        r << ' ' << g << ' ' << b << ' ' << a << ' ' <<
        rot.x << ' ' << rot.y << ' ' << rot.z << ' ' <<
        generation << ' ' << age << ' ' <<
        direction.x << ' ' << direction.y << ' ' << direction.z << ' ' ;
    
    // convert linkage pointers into file indexes,
    // assuming nodes are written in same order as they appear in _N::all
    
    if(parent==NULL){
        fout << -1 << ' ';
    }else{
        fout << parent->ident << ' ';
    }
    
    fout << children.size() << ' ';
    
    for(int i=0;i<children.size();i++){
        fout << children[i]->ident << ' ';
    }
    fout << endl;
}


void _N::clearPathFlags(){
    vector<jtn::TreeNode*>::iterator nit = _N::all.begin();
    for(;nit!=_N::all.end();nit++){
        (*nit)->isPartOfCamPath = false;
    }
}

#pragma mark

jtn::PointD jtn::Box::upper(){
    return _upper;
}


jtn::PointD jtn::Box::lower(){
    return _lower;
}

jtn::Box::Box(){
    
}

void jtn::Box::stretch(ofVec3f that){
    
    if (that.x < _lower.x) _lower.x = that.x;
    if (that.x > _upper.x) _upper.x = that.x;
    
    if (that.y < _lower.y) _lower.y = that.y;
    if (that.y > _upper.y) _upper.y = that.y;
    
    if (that.z < _lower.z) _lower.z = that.z;
    if (that.z > _upper.z) _upper.z = that.z;
    
    
    
}


jtn::PointD jtn::Box::getNormalized(jtn::PointD that){
    PointD delta = _upper - _lower;
    return (that - _lower) / delta;
}

void jtn::Box::setOppositeExtremes(){
    _lower = jtn::PointD(  9999,  9999,  9999);
    _upper = jtn::PointD( -9999.0f, -9999.0f, -9999.0f);
}


#pragma mark

jtn::PointD::PointD(){
    x=y=z=0;
}

jtn::PointD::PointD(double xx,double yy,double zz){
    x=xx;
    y=yy;
    z=zz;
}

jtn::PointD::PointD(ofVec3f copyable){
    x=copyable.x;
    y=copyable.y;
    z=copyable.z;
    
}

jtn::PointD jtn::PointD::operator-(jtn::PointD that){
    return jtn::PointD(
                       x-that.x,
                       y-that.y,
                       z-that.z
                       );
}



jtn::PointD jtn::PointD::operator/(jtn::PointD that){
    return jtn::PointD(
                       x/that.x,
                       y/that.y,
                       z/that.z
                       );
}

jtn::PointD::operator string(){
    stringstream ss;
    ss << x << ',' << y << ',' << z;
    return ss.str();
}


