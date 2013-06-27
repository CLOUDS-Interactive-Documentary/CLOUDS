
#include "CloudsVisualSystemHiga.h"

struct Grid2D
{
	float rows;
	float cols;
	float width;
	float height;
	float scale;
	
	void operator()(Replecator *repl)
	{
		int X = rows;
		int Y = cols;
		
		repl->children.resize(X * Y);
		
		float s = scale;
		width *= s;
		height *= s;
		
		float inv_w = width / (X - 1);
		float inv_h = height / (Y - 1);
		
		int idx = 0;
		for (int y = 0; y < Y; y++)
		{
			for (int x = 0; x < X; x++)
			{
				repl->children[idx].m.glScale(s, s, s);
				repl->children[idx].m.glTranslate(inv_w * x, inv_h * y, 0);
				
				idx++;
			}
		}
	}
};

struct Drawer
{
	float width;
	float height;
	float rot_x;
	float rot_y;
	
	void operator()(Replecator *repl)
	{
		if (repl->parent == NULL) return;
		
		if (repl->idx % 2 == 0) ofNoFill();
		else ofFill();
		
		float t = ofGetElapsedTimef();
		
		float hue = fmodf(ofMap(repl->idx, 0, 24, 0, 255) + t, 255);
		ofColor c = ofColor::fromHsb(hue, 255, 255, 255);
		ofSetColor(c);
		
		ofTranslate(0, 0, hue);
		ofRotateY(hue * t * rot_x);
		ofRotateX(hue * t * rot_y);
		ofRect(0, 0, width, height);
	}
};

static Drawer drawer = {100, 100, 1, 0.5};
static Grid2D grid2d = {10, 10, 2000, 2000, 0.5};

CloudsVisualSystemHiga::CloudsVisualSystemHiga(){

}

string CloudsVisualSystemHiga::getSystemName(){
	return "HigaSystem";
}

void CloudsVisualSystemHiga::selfSetup(){
	repl.setup(grid2d, 2);
}

void CloudsVisualSystemHiga::selfSetupGuis(){
	
	replecatorGui = new ofxUISuperCanvas("REPLECATOR", gui);
	replecatorGui->copyCanvasStyle(gui);
    replecatorGui->copyCanvasProperties(gui);
    replecatorGui->setName("Replecator");
    replecatorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

	replecatorGui->addButton("REGENERATE", &regenerate);
	
	replecatorGui->addSlider("ROWS", 2, 20, &grid2d.rows);
	replecatorGui->addSlider("COLS", 2, 20, &grid2d.cols);
	replecatorGui->addSlider("WIDTH", 1, 2000, &grid2d.width);
	replecatorGui->addSlider("HEIGHT", 1, 2000, &grid2d.height);
	replecatorGui->addSlider("ITERATION_SCALE", 0.1, 1, &grid2d.scale);
	
	guis.push_back(replecatorGui);
	guimap[replecatorGui->getName()] = replecatorGui;
	
	//
	
    drawerGui = new ofxUISuperCanvas("DRAWER", gui);
    drawerGui->copyCanvasStyle(gui);
    drawerGui->copyCanvasProperties(gui);
    drawerGui->setName("Drawer");
    drawerGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	drawerGui->addSlider("RECT_W", 1, 1000, &drawer.width);
	drawerGui->addSlider("RECT_H", 1, 1000, &drawer.height);

	drawerGui->addSlider("ROT_X", -1, 1, &drawer.rot_x);
	drawerGui->addSlider("ROT_Y", -1, 1, &drawer.rot_y);
	
    guis.push_back(drawerGui);
    guimap[drawerGui->getName()] = drawerGui;
}


void CloudsVisualSystemHiga::selfUpdate(){
	if (regenerate)
	{
		repl.setup(grid2d, 2);
	}
}

void CloudsVisualSystemHiga::selfDrawBackground(){

}

void CloudsVisualSystemHiga::selfDrawDebug(){

}

void CloudsVisualSystemHiga::selfSceneTransformation(){

}

void CloudsVisualSystemHiga::selfDraw(){
	repl.draw(drawer);
}

void CloudsVisualSystemHiga::selfExit(){

}

void CloudsVisualSystemHiga::selfBegin(){

}

void CloudsVisualSystemHiga::selfEnd(){

}


void CloudsVisualSystemHiga::selfKeyPressed(ofKeyEventArgs & args){

}

void CloudsVisualSystemHiga::selfKeyReleased(ofKeyEventArgs & args){

}


void CloudsVisualSystemHiga::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemHiga::selfMouseMoved(ofMouseEventArgs& data){

}

void CloudsVisualSystemHiga::selfMousePressed(ofMouseEventArgs& data){

}

void CloudsVisualSystemHiga::selfMouseReleased(ofMouseEventArgs& data){

}


void CloudsVisualSystemHiga::selfSetupGui(){

}

void CloudsVisualSystemHiga::selfGuiEvent(ofxUIEventArgs &e){

}


void CloudsVisualSystemHiga::selfSetupSystemGui(){

}

void CloudsVisualSystemHiga::guiSystemEvent(ofxUIEventArgs &e){

}


void CloudsVisualSystemHiga::selfSetupRenderGui(){

}

void CloudsVisualSystemHiga::guiRenderEvent(ofxUIEventArgs &e){
	
}