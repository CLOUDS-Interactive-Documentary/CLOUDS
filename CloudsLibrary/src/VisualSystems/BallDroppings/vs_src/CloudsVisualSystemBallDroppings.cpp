//
//  CloudsVisualSystemEmpty.cpp
//

#include "CloudsVisualSystemBallDroppings.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
//statics
float CloudsVisualSystemBallDroppings::friction;
float CloudsVisualSystemBallDroppings::frequencyRange;

CloudsVisualSystemBallDroppings::CloudsVisualSystemBallDroppings()
{
}

void CloudsVisualSystemBallDroppings::selfSetupGui()
{
    ParamManager* pm = &ParamManager::getInstance();

    customGui = new ofxUISuperCanvas("MAZE BUILDER", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	customGui->setName("MAZE BUILDER");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

    /*

    customGui->addSpacer();
    customGui->addSlider("CAM SPEED", -100, 300, &pm->cameraSpeed);
    customGui->addSlider("CAM HEIGHT", 0, 5000, &pm->cameraHeight);
    customGui->addSlider("CAM ANGLE", 0, 360, &pm->cameraAngle);
    customGui->addToggle("GROUND CAM", &pm->groundCam);
    customGui->addSlider("GCAM SPEED", 0, 1, &pm->groundCamSpeed);
    customGui->addSlider("GCAM LOOKAT", 0, 1, &pm->groundCamLookAt);
    
    customGui->addSpacer();
    customGui->addSlider("SHOW AHEAD", 10, 150, &pm->showAhead);
    customGui->addSlider("FOG DENSITY", 0, .1, &pm->fogDensity);
    customGui->addSlider("FOG START", 0, 10000, &pm->fogStart);
    customGui->addSlider("FOG END", 0, 10000, &pm->fogEnd);
    
    customGui->addSpacer();
    customGui->addSlider("NUM BALLS", 0, 150, &pm->numberOfBalls);
    customGui->addSlider("BALLS RADIUS", 0, 40, &pm->ballRadius);
    customGui->addSlider("BALLS SPEED", 0, 0.01, &pm->ballMaxSpeed);
    
    customGui->addSlider("WALL HEIGHT VARIANCE", 0, 1, &pm->heightRandom);
    

    float length = (customGui->getGlobalCanvasWidth()-customGui->getWidgetSpacing()*5)/3.;
    float dim = customGui->getGlobalSliderHeight();

    customGui->addSpacer();
    customGui->addToggle("LIGHT", &bLights);
    customGui->addSlider("LX", 0, 10000, &lightPos.x);
    customGui->addSlider("LY", 0, 1000, &lightPos.y);
    customGui->addSlider("LZ", 0, 100000, &lightPos.z);
    
    customGui->addSpacer();
    customGui->addLabel("COLORS");

    customGui->addLabel("FOG COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("FH", 0.0, 255, &(pm->fogColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("FS", 0.0, 255, &(pm->fogColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("FB", 0.0, 255, &(pm->fogColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    customGui->addLabel("GROUND COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("GH", 0.0, 255, &(pm->groundColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("GS", 0.0, 255, &(pm->groundColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("GB", 0.0, 255, &(pm->groundColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    customGui->addLabel("WALL COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("WH", 0.0, 255, &(pm->wallColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("WS", 0.0, 255, &(pm->wallColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("WB", 0.0, 255, &(pm->wallColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    customGui->addLabel("BALL COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("SH", 0.0, 255, &(pm->ballColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("SS", 0.0, 255, &(pm->ballColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("SB", 0.0, 255, &(pm->ballColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
 */
    
    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemBallDroppings::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemBallDroppings::selfGuiEvent(ofxUIEventArgs &e)
{

}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemBallDroppings::selfSetupSystemGui(){
	
}

void CloudsVisualSystemBallDroppings::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemBallDroppings::selfSetupRenderGui(){
    
}

void CloudsVisualSystemBallDroppings::guiRenderEvent(ofxUIEventArgs &e)
{
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemBallDroppings::selfSetup()
{
    //ofEnableSmoothing();
	ofToggleFullscreen();
	
	//defaults
	friction = 0.99997f;
	frequencyRange = 50000;
	fullScreenMode=true;
	oldMillis=0;
	clickCount=0;
	ball_drop_rate = 3000;
	emptyBalls=0;
	closestLine=0;
	closestLineVertex=0;
	closestLine_maxPickingDistance = 20;
	closestLineDistance = 0;
	mousestate_draggingvert=0;
	undoables=0;
	closestLine_beginMove.copyFrom(0,0,0);
	
	hole.copyFrom(400,100);
	holeLag.copyFrom(400,100);
    
	undoBusy=false;
	paused = false;
	gravity = 0.01f;
	ballcolor=1;
	oldMouse.copyFrom(-1,-1,-1);
    
	undoables = new LinkedList();
	balls = new LinkedList();
	lines = new LinkedList();
	emptyBalls = new LinkedList(); //make a new queue for recyclable ball spots.
	
	//load a new ball.
	Ball *b = new Ball(hole, balls->size(), getVisualSystemDataPath() + "sine.wav");
	balls->push((long)b);
	ball_drop_rate = 3000;
	
	font.loadFont("Verdana.ttf",8);
	displayAlpha = 0;

}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemBallDroppings::selfPresetLoaded(string presetPath)
{

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemBallDroppings::selfBegin()
{
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemBallDroppings::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemBallDroppings::selfUpdate()
{
	if(!paused) {
        step(); //animation and physics are seperated out into a seperate routine
    }    
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemBallDroppings::selfDraw()
{
}

// draw any debug stuff here
void CloudsVisualSystemBallDroppings::selfDrawDebug()
{
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemBallDroppings::selfDrawBackground()
{
    
	ofBackground(!ballcolor*255,!ballcolor*255,!ballcolor*255);
    
	//draw balldropper
	ofSetColor(51,51,51);
	ofCircle( holeLag.x,holeLag.y ,4);
    
    
	//draw the mouse line in-progress
	if(clickCount%2==1 && mousestate_draggingvert==0){
		lineGradient(lastMouseDown.x,lastMouseDown.y,ofGetMouseX(),ofGetMouseY(),76.5,0);
	}
    
	//draw the regular lines
	ofSetColor(ballcolor*255,ballcolor*255,ballcolor*255);
	for(int i=0;i<lines->size();i++){
		EditLine *thisLine = (EditLine*)(lines->get(i));
		ofLine(thisLine->x1, thisLine->y1, thisLine->x2,thisLine->y2 );
	}
    
	if ( mousestate_draggingvert==0){//am i free roaming, or am i dragging a vertex?
        
		//draw closest vertex line.
		if(closestLine!=0 && closestLineDistance < closestLine_maxPickingDistance){
			glPushMatrix();
			
			//ok, which point to be drawn?
			if(closestLineVertex==0){
				glTranslatef(closestLine->x1,closestLine->y1,0);
			} else {
				glTranslatef(closestLine->x2,closestLine->y2,0);
			}
			
			//draw a little square
			ofNoFill();
			ofRect(-3,-3,6,6);
			glPopMatrix();
		}
	} else {
        
		//then draw the vertex as you pull it.
		glPushMatrix();
		if(closestLineVertex==0){
			glTranslatef(closestLine->x1,closestLine->y1,0);
		} else {
			glTranslatef(closestLine->x2,closestLine->y2,0);
		}
		ofRect(-3,-3,6,6);
		glPopMatrix();
	}
    
	ofFill();
    
	//draw all the balls
	for(int i=0;i<balls->size();i++){
		if(balls->get(i)!=0){
			Ball *b = (Ball*)(balls->get(i));
			ofCircle(b->x,b->y, b->jitter*5.0f+2);
		}
	}
	
	//draw the text info display
	if(displayAlpha>0){
		ofSetColor(ballcolor*255,ballcolor*255,ballcolor*255,  displayAlpha );
		char displayString[512];
		sprintf(displayString,"ball drop rate: %i [- +] \nfriction: %f [f F]\ngravity: %f [g G]\npitch range: %f [r R]\n\n\
                shift-space clears balls\nctrl-space clears lines\nspace alone clears all\nctrl-z to undo line editing\nctrl-f toggles fullscreen\n\
                w inverts video\nb moves balldropper to mouse\ns saves world state to disk\no loads world state from disk\ne,del deletes point under mouse\np pause",
                ball_drop_rate, friction, gravity, frequencyRange);
		
		font.drawString(displayString,20,20);
	}
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemBallDroppings::selfEnd()
{
	
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemBallDroppings::selfExit()
{
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemBallDroppings::selfKeyPressed(ofKeyEventArgs & args)
{
    int key = args.key;
    
	if(key==26){//undo
		if(undoables->size()>0){
			undo();
		}
	}else if(key==6){ //toggle full screen
		ofToggleFullscreen();
	}else if(key==0){ //ctrl+space on the mac
		for(int i=0;i<lines->size();i++){
			EditLine*thisL = (EditLine*)(lines->get(i));
			delete thisL;
		}
		delete lines;
		lines = new LinkedList();
	} else if (key=='w') { // toggle callcolor
		ballcolor = !ballcolor;
	} else if (key=='t'){
        
		// this used to be the network server enable
		// when network was supported
		/*
         FSOUND_Close();
         if(!server_valid()){
         server_init(6668);
         }
         */
	} else if (key=='B'||key=='b'){
		hole.copyFrom(ofGetMouseX(), ofGetMouseY(),0);
	} else if (key=='T'){
		//server_close();
		//initfmod();
	} else if(key=='r'){
		frequencyRange /= 1.09;
		showPanel();
	} else if(key=='R'){
		frequencyRange *= 1.09;
		showPanel();
	} else if(key=='f'){
		friction+=0.0001f;
		showPanel();
	} else if(key=='F'){
		friction-=0.0001f;
		showPanel();
	} else if(key=='g'){
		gravity-=0.001;
		showPanel();
	} else if(key=='G'){
		gravity+=0.001;
		showPanel();
	} else if(key=='e'||key=='E'){
		deletePickedVertex();
	} else if(key=='p'||key=='P'){
		paused = !paused;
	}else if(key=='0'){
		resetVars();
	}else if(key == 45 || key == 95){//-
		ball_drop_rate -=100;
		showPanel();
	}else if(key == 61 || key == 43){//+
		ball_drop_rate += 100;
		showPanel();
	}else if(key==' '){
//		if( glutGetModifiers()==GLUT_ACTIVE_CTRL){
//            resetLines();
//		} else if( glutGetModifiers()==GLUT_ACTIVE_SHIFT ){
//            resetBalls();
//		}else { // else kill both the lines and the balls.
//            resetBalls();
//            resetLines();
//        }
	}else if (key==127 || key==8){
        deletePickedVertex(); // delete last vertex
    }
}

void CloudsVisualSystemBallDroppings::selfKeyReleased(ofKeyEventArgs & args)
{
	
}

void CloudsVisualSystemBallDroppings::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemBallDroppings::selfMouseMoved(ofMouseEventArgs& data)
{
}

void CloudsVisualSystemBallDroppings::selfMousePressed(ofMouseEventArgs& data)
{
	if(closestLineDistance < closestLine_maxPickingDistance){
		mousestate_draggingvert=1;
        
		//taking some notes for the undoable later on.
		if(closestLineVertex==0) closestLine_beginMove.copyFrom( closestLine->x1 , closestLine->y1 , 0);
		else closestLine_beginMove.copyFrom( closestLine->x2 , closestLine->y2 , 0);
		
	} else {
		clickCount++;
		if(clickCount%2==0){ //only draw something every 2 clicks.
            
			//draw with mouse
			if(oldMouse.x!=-1 && oldMouse.y!=-1){ //but only if the old mouse value is useful
				//load a new line
				EditLine *el = new EditLine(oldMouse.x,oldMouse.y,ofGetMouseX(), ofGetMouseY());
                
				//construct and register an undoable
				LinkedList *v=new LinkedList();
				v->push((long)el);
				v->push((long)"create_line");
				undoables->push((long)v);
				lines->push((long)el);
			}
		}
	}
    
	oldMouse.copyFrom(ofGetMouseX(), ofGetMouseY(),0);
	lastMouseDown.copyFrom(ofGetMouseX(), ofGetMouseY(),0);
	
	
}

void CloudsVisualSystemBallDroppings::selfMouseReleased(ofMouseEventArgs& data)
{
	float xd = data.x - lastMouseDown.x;
	float yd = data.y - lastMouseDown.y;
    
	if (  mousestate_draggingvert==1){//then we had been dragging something else.
		
		mousestate_draggingvert=0;
		clickCount = 0;
		//register undoable
		LinkedList *v=new LinkedList();
		v->push(closestLineVertex);
		v->push((int)(closestLine_beginMove.x));
		v->push((int)(closestLine_beginMove.y));
		v->push((long)closestLine);
		v->push((long)"move_line");
		undoables->push((long)v);
        
	} else {
		if ( sqrt(xd*xd+yd*yd) > 10 ){//10 is the mouse drag movement margin for nondraggers
            ofMouseEventArgs args;
            args.x = ofGetMouseX();
            args.y = ofGetMouseY();
            args.button = 0;
			mousePressed(args);//nudge the new line drawing.
		}
	}
}

void CloudsVisualSystemBallDroppings::step(){
    
	int i;//for loop variable to be reused
    
	holeLag.lerpSelfTo(hole,0.1);
    
    
	//release a ball on a timer
	if(ofGetElapsedTimeMillis()-oldMillis > ball_drop_rate ){
		createBall();
		oldMillis = ofGetElapsedTimeMillis();
	}
    
	updateClosestPickingLine();
	//for all the balls . . .
	for(i=0;i<balls->size();i++){
		if(balls->get(i)!=0){
            
			Ball *b = (Ball*)(balls->get(i));
			if(b->y > 1280 || b->force.getLength()==0){
				balls->set(i,0);
				emptyBalls->push(i);
				delete b;
			} else {
				b->applyForce(0,gravity);//gravity
				
				//check collisions with lines
				//and bounce accordingly
				
				for(int j=0;j< lines->size() ;j++){
					//am i on one side when i was just on another side a second ago?
					EditLine *thisLine = (EditLine*)(lines->get(j));
					if(thisLine->checkBallCollide(b)){
						//then also reset my memory to give me 1 frame's worth of amnesia.
						b->amnesia();
						b->bounce( thisLine->x1,thisLine->y1, thisLine->x2,thisLine->y2);
						break;//skip the rest of the lines
					}
				}
                
				b->stepPhysics();
			}
		}
	}
	
	if(displayAlpha>0)displayAlpha--;
	
}


//--------------------------------------------------------------

void CloudsVisualSystemBallDroppings::createBall(){
    
	//load a new ball.
	Ball *b = new Ball(hole, balls->size(), getVisualSystemDataPath() + "sine.wav");
	b->applyForce(0.0001,0);//give it an initial push
    
	//search for an empty spot in the list
	if(emptyBalls->size()>0){
		balls->set( emptyBalls->unshift(),(long)b);
	} else {//else, you have to make a new one.
		balls->push((long)b);
	}
	
}

//--------------------------------------------------------------

void CloudsVisualSystemBallDroppings::deletePickedVertex(){
	if( closestLineDistance < closestLine_maxPickingDistance){
		//register undoable
		LinkedList *v=new LinkedList();
		v->push((int)(closestLine->x1));
		v->push((int)(closestLine->y1));
		v->push((int)(closestLine->x2));
		v->push((int)(closestLine->y2));
		v->push((long)"delete_line");
		undoables->push((long)v);
        
		//then one of them is highlighted.
		lines->deleteMatch((long)closestLine);
		delete closestLine;
		closestLine = 0;
		closestLineDistance=100000;//turn off picking!
	}
}

//--------------------------------------------------------------

void CloudsVisualSystemBallDroppings::undo(){
	if(undoBusy){
		return;
	}else{
		undoBusy=true;
		if(undoables->size()>0){
            
			//get the most recent undoable action.
			LinkedList *thisUndoable = (LinkedList*)undoables->pop();
			char *action = (char*)thisUndoable->pop();
			//get its variables and do the action.
            
			if(0==strcmp(action,"create_line")){
				//kill the line
				EditLine *l = (EditLine*)thisUndoable->pop();
				if(validLine(l)){
					lines->deleteMatch((long)l);
					delete l;
				}
                
			} else if(0==strcmp(action,"move_line")){
				//move the line back.
				EditLine *l = (EditLine*)thisUndoable->pop();
				if(validLine(l)){
					int y = (int)thisUndoable->pop();
					int x = (int)thisUndoable->pop();
					int which = thisUndoable->pop();
					if(which==0){
						l->set1(x,y);
					} else {
						l->set2(x,y);
					}
				}
                
			} else if(0==strcmp(action,"delete_line")){
                
				int y2 = (int)thisUndoable->pop();
				int x2 = (int)thisUndoable->pop();
				int y1 = (int)thisUndoable->pop();
				int x1 = (int)thisUndoable->pop();
                
				EditLine *l = new EditLine(x1,y1,x2,y2);
				lines->push((long)l);
                
			} else {
				printf("Undoable action unknown: %s\n",action);
			}
            
			delete thisUndoable;
		}
		undoBusy = false;
	}
    
}

//--------------------------------------------------------------

int  CloudsVisualSystemBallDroppings::validLine(EditLine*l){
	int foundOne = 0;
	for(int i=0;i<lines->size();i++){
		if(lines->get(i)==((long)l)){
			foundOne=1;
			break;
		}
	}
	return foundOne;
}


//--------------------------------------------------------------

//--------------------------------------------------------------

void CloudsVisualSystemBallDroppings::resetWorld(){
    resetLines();
    resetBalls();
    resetVars();
}

//--------------------------------------------------------------

void CloudsVisualSystemBallDroppings::resetVars(){
	ball_drop_rate = 3000;
	friction = 0.99997f;
	gravity = 0.01f;
	frequencyRange = 50000;
	hole.copyFrom(400,100,0);
	holeLag.copyFrom(400,100,0);
}

//--------------------------------------------------------------

void CloudsVisualSystemBallDroppings::resetBalls(){
	for(int i=0;i<balls->size();i++){
		Ball*thisB = (Ball*)(balls->get(i));
		delete thisB;
	}
	delete balls;
	balls = new LinkedList();
}

//--------------------------------------------------------------

void CloudsVisualSystemBallDroppings::resetLines(){
	int i;
	for(i=0;i<lines->size();i++){
		EditLine*thisL = (EditLine*)(lines->get(i));
		delete thisL;
	}
	delete lines;
	lines = new LinkedList();
}

//--------------------------------------------------------------
/**
 by setting a color channel value to 512, and having it count down
 slowly over time, we get a period of solid opacity
 */
void CloudsVisualSystemBallDroppings::showPanel(){
    displayAlpha = 512;
}
//--------------------------------------------------------------
/**
 a line drawing routine that uses different colors for the vertices
 */
void CloudsVisualSystemBallDroppings::lineGradient(float x1,float y1,float x2,float y2,float color1,float color2){
	glBegin(GL_LINES);
	ofSetColor(color1,color1,color1);
	glVertex3f(x1,y1,0);
	ofSetColor(color2,color2,color2);
	glVertex3f(x2,y2,0);
	glEnd();
}
//--------------------------------------------------------------

/**
 takes care of mouse interaction with the lines
 */

void CloudsVisualSystemBallDroppings::updateClosestPickingLine(){
	
	/*
     find closest picking vertex
     by sorting through them and measuring
     distance from mouse
     */
	EditLine *closeL=0;
	int closeLV=0;
	float closeDist=100000;//very far to start.
	for(int i=0;i<lines->size();i++){
        
		EditLine *thisLine = (EditLine*)(lines->get(i));
        
		//recalculating the closest line for both vertices only if we are not dragging one.
		if ( mousestate_draggingvert==0){
			//v1
			//measuring distance from mouse
			float xd = thisLine->x1 - ofGetMouseX();
			float yd = thisLine->y1 - ofGetMouseY();
			float dist = sqrt(xd*xd+yd*yd);
			//have we got a winner?
			if ( dist  < closeDist){
				closeDist = dist;
				closeL = thisLine;
				closeLV = 0;
			}
            
			//v2
			//measuring distance from mouse
			xd = thisLine->x2 - ofGetMouseX();
			yd = thisLine->y2 - ofGetMouseY();
			dist = sqrt(xd*xd+yd*yd);
			//have we got a winner?
			if ( dist  < closeDist){
				closeDist = dist;
				closeL = thisLine;
				closeLV = 1;
                
			}
		}
	}
    
	/*
     then update interface variables accordingly
     */
    
	if ( mousestate_draggingvert==0){//am i free roaming, or am i dragging a vertex?
		//commit local calculations  globally.
		closestLine = closeL;
		closestLineVertex = closeLV;
		closestLineDistance = closeDist;
	} else {
		//set vertex to mouse position.
		if(closestLineVertex==0){//which side of the line?
			closestLine->set1(ofGetMouseX(),ofGetMouseY());
		} else {
			closestLine->set2(ofGetMouseX(),ofGetMouseY());
		}
		//fix just in case
		if(closestLine->fixDirection()){
			//also adjust the line-siding if it got swapped.
			closestLineVertex=!closestLineVertex;
		}
	}
    
    
}



