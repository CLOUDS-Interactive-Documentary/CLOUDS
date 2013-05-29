#include "CloudsVisualSystemCollaboration1.h"
#include <GLUT/GLUT.h>

#define DEFAULT_BLUE 0xC4 / 255.0
#define DEFAULT_GREEN 0x9A / 255.0
#define DEFAULT_RED 0x6C / 255.0

#define _N jtn::TreeNode

float _C::dotSize = 0.1;
float _C::axonThickness = 0;
float _C::alpha = 0;
float _C::sway = 0;

string _C::getSystemName(){
  return "Collab1";
}

void _C::selfSetup(){
	rotation = 0;
    
    int stepsTheta = 4;
    int stepsPhi = 4;
    float incTheta = (PI*2)/(float)stepsTheta;
    float incPhi = (PI)/(float)stepsPhi;
    for(int j=0;j<stepsPhi+1;j++){
        for(int i=0;i<stepsTheta;i++){
            
            float x = cos(i*incTheta) * sin(j*incPhi);
            float y = sin(i*incTheta) * sin(j*incPhi);
            float z = cos(j*incPhi);
            
            _N *n = new _N();
            n->generation = 0;
            n->future.x = n->x = x * 50;
            n->future.y = n->y = y * 50;
            n->future.z = n->z = z * 50;
            n->direction = ofPoint(-x,-y,-z);
            rootNodes.push_back(n);
        }
    }

}

void _C::selfSetupGuis(){
	spinSlider = gui->addSlider("Spin Speed",-6.0, 6.0,1);
	dotSizeSlider = gui->addSlider("Dot Size",1, 64, 2);
    nucleusSize = gui->addSlider("Nucleus Size",0, 5, 1);
    axonThicknessSlider = gui->addSlider("Axon Thickness",0, 10, 5);
    alphaSlider = gui->addSlider("Alpha",0, 1, 0.5);
    swaySlider = gui->addSlider("Sway",0, 10, 5);
}

void _C::selfUpdate(){
	
	rotation += spinSlider->getScaledValue();
	_N::terminals.clear();
    
    vector<_N*>::iterator it;
    for(it=rootNodes.begin();it!=rootNodes.end();it++){
        (*it)->update();
    }
	axonThickness = axonThicknessSlider->getScaledValue();
	dotSize = dotSizeSlider->getScaledValue();
    alpha = alphaSlider->getScaledValue();
    sway = swaySlider->getScaledValue();
}

void _C::selfDrawBackground(){
    ofBackgroundGradient(ofColor(0), ofColor(255));
}

void _C::selfDraw(){
	ofPushMatrix();
	ofRotate(rotation,0,0,1);
    ofTranslate(
                ofNoise( ofGetFrameNum() * 0.01 , 1000) * _C::sway,
                ofNoise( ofGetFrameNum() * 0.01 , 2000) * _C::sway,
                ofNoise( ofGetFrameNum() * 0.01 , 3000) * _C::sway
    );
	_N::drawMode = GL_LINES;

    vector<_N*>::iterator it;
    int tCount=0;
    for(it=rootNodes.begin();it!=rootNodes.end();it++){
        
        glPushMatrix();
        glTranslatef((*it)->x,(*it)->y,(*it)->z);
        glColor4f((*it)->r,(*it)->g,(*it)->b,_C::alpha);
        glutSolidSphere(nucleusSize->getScaledValue(),8,8);
        glPopMatrix();
        (*it)->draw();
        tCount++;
    }
	
	glPointSize(dotSize);
	glBegin(GL_POINTS);
	glColor3f(1,1,1);
	for(it=_N::terminals.begin();it!=_N::terminals.end();it++){
		glVertex3f( (*it)->x,
								(*it)->y,
								(*it)->z );
	}
	
	glEnd();
	


	ofPopMatrix();

    cout << _N::terminals.size() << endl;
	
}

//############################################################


vector<_N*> _N::all;
vector<_N*> _N::terminals;
GLuint _N::drawMode = 0;
int _N::maxDepth = 0;
 

void _N::update(){

    int n = ofGetFrameNum();
    
    ofPoint temp(future);
    temp += ofPoint(
                    ofNoise( n * 0.1 , 4000 , generation * 0.1 ) * 10,
                    ofNoise( n * 0.1 , 5000 , generation * 0.1) * 10,
                    ofNoise( n * 0.1 , 6000 , generation * 0.1) * 10
    );
	x += (temp.x - x)*0.1;
	y += (temp.y - y)*0.1;
	z += (temp.z - z)*0.1;
    
	
	vector<_N*>::iterator it;
	
	for(it=children.begin();it!=children.end();it++){
		(*it)->update();
	}
	
	if( _N::all.size() < 10000
			&& (
		 (ofRandomuf() < 0.1 && children.size()==0)
		 ||
		 (ofRandomuf() < 0.001 && children.size()==1)
		 ||
		 (ofRandomuf() < 0.0001 && children.size()>1)
         ||
         (generation == 40)
                    )
		 ){

		_N *n = new _N();
		n->parent = this;
		
		if(parent==NULL){
			n->future.x = n->x = x + direction.x;
			n->future.y = n->y = y + direction.y;
			n->future.z = n->z = z + direction.z;
		}else{
			
			ofPoint v = (this->future) + ((this->future) - (*parent)).normalize()*1;
			
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
            if(generation==41)s=10;
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
            if(maxDepth < n->generation)maxDepth = n->generation;
		}
		children.push_back( n );
	}

	if( isTerminal() ){
		terminals.push_back(this);
	}

	age++;
}

bool _N::isTerminal(){
	return children.size()==0 || (children.size()==1 && children[0]->age<1);
}

void _N::draw(){
	
	vector<_N*>::iterator that;
	
	for(that=children.begin(); that!=children.end();that++){
		glColor4f(r, g, b, _C::alpha);
		
        
        if(drawMode==GL_LINES){
            glLineWidth( ( 1 - (generation+1) / (float)maxDepth) * _C::axonThickness );
            glBegin(GL_LINES);
        }
        
		if( !(drawMode==GL_POINTS && isTerminal()) )
			glVertex3f(x,y,z);
		
		_N *t = *that;
		glColor4f(t->r, t->g, t->b, _C::alpha);
		
		if( !(drawMode==GL_POINTS && t->isTerminal()) )
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
}

_N::~TreeNode(){
}


