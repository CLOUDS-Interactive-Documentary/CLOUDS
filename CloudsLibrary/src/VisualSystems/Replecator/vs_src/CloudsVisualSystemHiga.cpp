
#include "CloudsVisualSystemHiga.h"

static float rows = 0, cols = 0;
static float local_time = 0;

struct Grid2D
{
	float scale;
	
	void operator()(Replecator *repl)
	{
		int X = rows;
		int Y = cols;
		int NUM = X * Y;
		
		repl->children.resize(NUM);
	}
};

struct Grid2DUpdater
{
	float width, height;
	float scale;

	void operator()(Replecator *repl)
	{
		int X = rows;
		int Y = cols;
		int NUM = X * Y;
		
		if (repl->children.size() != NUM) return;
		
		float s = scale;
		width *= s;
		height *= s;
		
		float inv_w = width / X;
		float inv_h = height / Y;
		
		int idx = 0;
		for (int y = 0; y < Y; y++)
		{
			for (int x = 0; x < X; x++)
			{
				repl->children[idx].m.makeIdentityMatrix();
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
	
	float phase_x;
	float phase_y;
	
	void operator()(Replecator *repl)
	{
		if (repl->parent == NULL) return;
		
		float t = local_time;
		
		phase_x += t * rot_x;
		phase_y += t * rot_y;
		
		float hue = fmodf(ofMap(repl->idx, 0, 24, 0, 255) + phase_x + phase_y, 255);
		ofColor c = ofColor::fromHsb(hue, 255, 255);
		ofSetColor(c);
		
		ofTranslate(0, 0, hue);
		ofRotateY(hue * phase_x);
		ofRotateX(hue * phase_y);
		ofRect(0, 0, width, height);
	}
};

static Drawer drawer = {100, 100, 1, 0.5, 0, 0};
static Grid2D grid2d = {0.5};
static Grid2DUpdater grid2d_updater;

CloudsVisualSystemHiga::CloudsVisualSystemHiga(){

}

string CloudsVisualSystemHiga::getSystemName(){
	return "Replecator";
}

void CloudsVisualSystemHiga::selfSetup(){
	repl.setup(grid2d, 2);
	
	local_time = 0;
}

void CloudsVisualSystemHiga::selfSetupGuis(){
	
	replecatorGui = new ofxUISuperCanvas("REPLECATOR", gui);
	replecatorGui->copyCanvasStyle(gui);
    replecatorGui->copyCanvasProperties(gui);
    replecatorGui->setName("Replecator");
    replecatorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

	replecatorGui->addSlider("LOCAL_TIME", 0, 120, &local_time);
	replecatorGui->addButton("REGENERATE", &regenerate);
	
	replecatorGui->addSlider("ROWS", 1, 20, &rows);
	replecatorGui->addSlider("COLS", 1, 20, &cols);
	replecatorGui->addSlider("WIDTH", 1, 2000, &grid2d_updater.width);
	replecatorGui->addSlider("HEIGHT", 1, 2000, &grid2d_updater.height);
	replecatorGui->addSlider("ITERATION_SCALE", 0.1, 1, &grid2d_updater.scale);
	
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
	
	repl.update(grid2d_updater);
}

void CloudsVisualSystemHiga::selfDrawBackground(){

}

void CloudsVisualSystemHiga::selfDrawDebug(){

}

void CloudsVisualSystemHiga::selfSceneTransformation(){

}

void CloudsVisualSystemHiga::selfDraw(){
	ofPushStyle();
	ofSetRectMode(OF_RECTMODE_CORNER);
	ofScale(0.5, 0.5, 0.5);
	repl.draw(drawer);
	ofPopStyle();
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