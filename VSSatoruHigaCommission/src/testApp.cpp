#include "testApp.h"

#include <tr1/functional>

namespace std
{
	using tr1::function;
}

class Replecator
{
public:

	typedef function<void(Replecator *repl)> SetupOp;
	typedef function<void(Replecator *repl)> UpdateOp;
	typedef function<void(Replecator *repl)> DrapOp;

	int idx;
	ofMatrix4x4 m;
	
	Replecator *parent;
	vector<Replecator> children;
	
	Replecator() : parent(NULL) {}
	Replecator(Replecator* parent) : parent(parent) {}
	
	void setup(SetupOp sOp, int iteration)
	{
		sOp(this);
		for (int i = 0; i < children.size(); i++)
		{
			children[i].idx = i;
			children[i].parent = this;
		}
		
		iteration--;
		
		if (iteration > 0)
		{
			for (int i = 0; i < children.size(); i++)
			{
				children[i].setup(sOp, iteration);
			}
		}
	}
	
	void update(UpdateOp gOp)
	{
		gOp(this);
		
		for (int i = 0; i < children.size(); i++)
		{
			children[i].update(gOp);
		}
	}
	
	void draw(DrapOp dOp)
	{
		ofPushMatrix();
		ofMultMatrix(m);
		dOp(this);
		
		for (int i = 0; i < children.size(); i++)
		{
			children[i].draw(dOp);
		}
		ofPopMatrix();
	}

};

Replecator repl;

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	
//	higaSystem.setup();
//	
//	higaSystem.playSystem();
//	
//	ofToggleFullscreen();
	
	struct Grid2D
	{
		int rows;
		int cols;
		float width;
		float height;
		
		void operator()(Replecator *repl)
		{
			repl->children.resize(rows * cols);
			
			float s = 0.5;
			width *= s;
			height *= s;
			
			float inv_w = width / (rows - 1);
			float inv_h = height / (cols - 1);
			
			int idx = 0;
			for (int y = 0; y < cols; y++)
			{
				for (int x = 0; x < rows; x++)
				{
					repl->children[idx].m.glScale(s, s, s);
					repl->children[idx].m.glTranslate(inv_w * x, inv_h * y, 0);
					
					idx++;
				}
			}
		}
	};
	
	Grid2D grid2d = {10, 10, 2000, 2000};

	repl.setup(grid2d, 2);
}


//--------------------------------------------------------------
void testApp::update(){

}

ofEasyCam cam;

//--------------------------------------------------------------
void testApp::draw(){
	struct Point
	{
		void operator()(Replecator *repl)
		{
			if (repl->parent == NULL) return;
			
			if (repl->idx % 2 == 0) ofNoFill();
			else ofFill();
			
			float t = ofGetElapsedTimef();
			
			float hue = fmodf(ofMap(repl->idx, 0, 24, 0, 255) + t, 255);
			ofColor c = ofColor::fromHsb(hue, 255, 255, 30);
			ofSetColor(c);
			
			ofTranslate(0, 0, hue);
			ofRotateY(hue * t);
			ofRect(0, 0, 200, 1000);
//			ofLine(0, 0, 100, 100);
		}
	};
	
	Point p;
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	cam.begin();
	repl.draw(p);
	cam.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::exit(){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}