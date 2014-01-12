
#include "CloudsVisualSystem.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsGlobal.h"
#include "CloudsInput.h"

#ifdef KINECT_INPUT
#include "CloudsInputKinectOSC.h"
#endif

#ifdef TARGET_OSX
#include "ofxSystemTextbox.h"
#endif

#ifdef AVF_PLAYER
#include "ofxAVFVideoPlayer.h"
#endif

static ofFbo staticRenderTarget;
static ofImage sharedCursor;
static CloudsRGBDVideoPlayer rgbdPlayer;
static bool backgroundShaderLoaded = false;
static ofShader backgroundShader;
static ofImage backgroundGradientCircle;
static ofImage backgroundGradientBar;
static bool screenResolutionForced = false;
static int forcedScreenWidth;
static int forcedScreenHeight;
static int numSamples = 0;
//default render target is a statically shared FBO
ofFbo& CloudsVisualSystem::getStaticRenderTarget(){
	return staticRenderTarget;
}

void CloudsVisualSystem::forceScreenResolution(int screenWidth, int screenHeight){
	screenResolutionForced = true;
	forcedScreenWidth = screenWidth;
	forcedScreenHeight = screenHeight;
}

void CloudsVisualSystem::setNumSamples(int samples){
	numSamples = samples;
}

ofImage& CloudsVisualSystem::getCursor(){
	if(!sharedCursor.bAllocated()){
		sharedCursor.loadImage( getVisualSystemDataPath() + "images/cursor.png");
	}
	return sharedCursor;
}

int CloudsVisualSystem::getCanvasWidth(){
	return getSharedRenderTarget().getWidth();
}

int CloudsVisualSystem::getCanvasHeight(){
	return getSharedRenderTarget().getHeight();
}

CloudsRGBDVideoPlayer& CloudsVisualSystem::getRGBDVideoPlayer(){
	return rgbdPlayer;
}

void CloudsVisualSystem::loadBackgroundShader(){
	backgroundGradientBar.loadImage(GetCloudsDataPath() + "backgrounds/bar.png");
	backgroundGradientCircle.loadImage(GetCloudsDataPath() + "backgrounds/circle.png");
	backgroundShader.load(GetCloudsDataPath() + "shaders/background");
	backgroundShaderLoaded = true;
}

void CloudsVisualSystem::getBackgroundMesh(ofMesh& mesh, ofImage& image, float width, float height){
	ofRectangle screenRect(0,0,width,height);
	ofRectangle imageRect(0,0,image.getWidth(), image.getHeight());
	
	imageRect.scaleTo(screenRect, OF_ASPECT_RATIO_KEEP_BY_EXPANDING);

	mesh.addVertex(ofVec3f(imageRect.getMinX(),imageRect.getMinY()));
	mesh.addTexCoord(ofVec2f(0,0));

	mesh.addVertex(ofVec3f(imageRect.getMinX(),imageRect.getMaxY()));
	mesh.addTexCoord(ofVec2f(0,image.getHeight()));

	mesh.addVertex(ofVec3f(imageRect.getMaxX(),imageRect.getMinY()));
	mesh.addTexCoord(ofVec2f(image.getWidth(),0));

	mesh.addVertex(ofVec3f(imageRect.getMaxX(),imageRect.getMaxY()));
	mesh.addTexCoord(ofVec2f(image.getWidth(),image.getHeight()));
	
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
}

void CloudsVisualSystem::get2dMesh(ofMesh& mesh, float width, float height){
	ofRectangle imageRect(0,0, width, height);
	    
	mesh.addVertex(ofVec3f(0,0));
	mesh.addTexCoord(ofVec2f(0,0));
    
	mesh.addVertex(ofVec3f(0,height));
	mesh.addTexCoord(ofVec2f(0,height));
    
	mesh.addVertex(ofVec3f(width,0));
	mesh.addTexCoord(ofVec2f(width,0));
    
	mesh.addVertex(ofVec3f(width,height));
	mesh.addTexCoord(ofVec2f(width,height));
	
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
}


#ifdef OCULUS_RIFT
static ofxOculusRift oculusRift;
#include "OVR.h"
ofxOculusRift& CloudsVisualSystem::getOculusRift(){
	if(!oculusRift.isSetup()){
        checkOpenGLError("PRE SETUP OCULUS");
		oculusRift.setup();
        checkOpenGLError("POST SETUP OCULUS");
	}

	return oculusRift;
}
#endif

CloudsVisualSystem::CloudsVisualSystem(){
	bIsPlaying = false;
	timeline = NULL;
	bIsSetup = false;
	bClearBackground = true;
	bDrawToScreen = true;
	bUseCameraTrack = false;
	cameraTrack = NULL;
	pointcloudScale = .25;
	confirmedDataPath = false;
	bBarGradient = false;
    bMatchBackgrounds = false;
	bIs2D = false;
	drawCursorMode = DRAW_CURSOR_NONE;
	updateCyclced = false;
#ifdef OCULUS_RIFT
	bUseOculusRift = true;
#else
	bUseOculusRift = false;
#endif 
    
    lastMouseMoveMillis = 0;
	
}

CloudsVisualSystem::~CloudsVisualSystem(){
	//can't save guis because the virtual subclass members return the wrong data
//    saveGUIS();
}

ofFbo& CloudsVisualSystem::getSharedRenderTarget(){
	
	//ofFbo& renderTarget = sharedRenderTarget != NULL ? *sharedRenderTarget : getStaticRenderTarget();
	ofFbo& renderTarget = getStaticRenderTarget();
	
	bool reallocateTarget = !renderTarget.isAllocated();
	reallocateTarget |= !screenResolutionForced &&
						(renderTarget.getWidth() != ofGetWidth() ||
						 renderTarget.getHeight() != ofGetHeight());
	reallocateTarget |= screenResolutionForced &&
						(renderTarget.getWidth() != forcedScreenWidth ||
						 renderTarget.getHeight() != forcedScreenHeight);

	if(reallocateTarget){
		if(screenResolutionForced){
//			renderTarget.allocate(forcedScreenWidth, forcedScreenHeight, GL_RGB, numSamples);
			renderTarget.allocate(forcedScreenWidth, forcedScreenHeight, GL_RGB, numSamples);
		}
		else{
//			renderTarget.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, numSamples);
			renderTarget.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, numSamples);
		}
		renderTarget.begin();
		ofClear(0,0,0,1.0);
		renderTarget.end();
    }
    return renderTarget;
}

string CloudsVisualSystem::getVisualSystemDataPath(bool ignoredFolder){

	if(!confirmedDataPath){
		cachedDataPath = GetCloudsVisualSystemDataPath(getSystemName());
		cachedDataPathIgnore = GetCloudsVisualSystemDataPath(getSystemName(), true);
		confirmedDataPath = true;
	}
	
	return ignoredFolder ? cachedDataPathIgnore : cachedDataPath;
}

ofxTimeline* CloudsVisualSystem::getTimeline(){
	return timeline;
}

void CloudsVisualSystem::setup(){
	
	if(bIsSetup){
		return;
	}
    
	cout << "SETTING UP SYSTEM " << getSystemName() << endl;
	
	//ofAddListener(ofEvents().exit, this, &CloudsVisualSystem::exit);
	if(!backgroundShaderLoaded){
		loadBackgroundShader();
	}

//	currentCamera = &cam;
	
    ofDirectory dir;
    string directoryName = getVisualSystemDataPath()+"Presets/";
    if(!dir.doesDirectoryExist(directoryName))
    {
        dir.createDirectory(directoryName);
    }
    
    string workingDirectoryName = directoryName+"Working/";
    if(!dir.doesDirectoryExist(workingDirectoryName))
    {
        dir.createDirectory(workingDirectoryName);
    }
    
    setupAppParams();
    setupDebugParams();
    setupCameraParams();
	setupLightingParams();
	setupMaterialParams();
    setupTimeLineParams();
	setupTimeline();
    
	selfSetDefaults();
    selfSetup();
    setupCoreGuis();
    selfSetupGuis();
    setupTimelineGui();
    
	loadGUIS();
	hideGUIS();

	bIsSetup = true;
	
	bUseInteractiveCamera = false;
	interactiveCameraDamping = 0;
	interactiveCameraMinX = interactiveCameraMaxX = interactiveCameraMinY = interactiveCameraMaxY = 0;
	interactiveCameraRot = ofVec2f(0,0);
	interactiveCameraDamping = 0;
	interactiveCameraRot.set(0,0);
}

bool CloudsVisualSystem::isSetup(){
	return bIsSetup;
}

bool CloudsVisualSystem::isPlaying(){
	return bIsPlaying;
}

void CloudsVisualSystem::playSystem(){

	if(!bIsPlaying){
		cout << "**** PLAYING " << getSystemName() << endl;
		//ofRegisterMouseEvents(this);
		CloudsRegisterInputEvents(this);
//		ofAddListener(GetCloudsInput()->getEvents().interactionMoved, this, &CloudsVisualSystem::interactionMoved);
		ofRegisterKeyEvents(this);
		ofAddListener(ofEvents().update, this, &CloudsVisualSystem::update);
		ofAddListener(ofEvents().draw, this, &CloudsVisualSystem::draw);
		
		bIsPlaying = true;
		
		cam.enableMouseInput();
		for(map<string, ofxLight *>::iterator it = lights.begin(); it != lights.end(); ++it) {
			it->second->light.setup();
		}
		
		getSharedRenderTarget().begin();
		ofClear(0.0,0.0,0.0,1.0);
		getSharedRenderTarget().end();
		selfBegin();

		cloudsCamera.setup();
		
		bDebug = false;
	}
}

void CloudsVisualSystem::stopSystem(){
	if(bIsPlaying){
		cout << "**** STOPPING " << getSystemName() << endl;

		selfEnd();
		
		cloudsCamera.remove();
		
		hideGUIS();
		saveGUIS();
		cam.disableMouseInput();
		for(map<string, ofxLight *>::iterator it = lights.begin(); it != lights.end(); ++it){
			it->second->light.destroy();
		}
		
		CloudsUnregisterInputEvents(this);
		ofUnregisterKeyEvents(this);
		ofRemoveListener(ofEvents().update, this, &CloudsVisualSystem::update);
		ofRemoveListener(ofEvents().draw, this, &CloudsVisualSystem::draw);
			
		timeline->stop();
		cameraTrack->lockCameraToTrack = false;
		bIsPlaying = false;
	}
}

void CloudsVisualSystem::setKeywords(string main, vector<string> allKeywords){
	main = mainKeyword;
	keywords = allKeywords;
}

void CloudsVisualSystem::setupSpeaker(string speakerFirstName,
									  string speakerLastName,
									  string quoteName)
{
	this->speakerFirstName = speakerFirstName;
	this->speakerLastName = speakerLastName;
	this->quoteName = quoteName;
	hasSpeaker = true;
	speakerChanged();
	
}

void CloudsVisualSystem::speakerEnded()
{
	hasSpeaker = false;
}

void CloudsVisualSystem::update(ofEventArgs & args)
{

#ifndef VISUAL_SYSTEM_EDITOR
    // show/hide the mouse cursor
    currMousePos.set(ofGetMouseX(), ofGetMouseY());
    if (currMousePos != lastMousePos) {
        lastMouseMoveMillis = ofGetElapsedTimeMillis();
        ofShowCursor();
    }
    else if ((ofGetElapsedTimeMillis() - lastMouseMoveMillis) > 1000) {
        ofHideCursor();
    }
    lastMousePos = currMousePos;
#endif
    
    if(bEnableTimeline && !bEnableTimelineTrackCreation && !bDeleteTimelineTrack)
    {
        updateTimelineUIParams();
    }
    
	cloudsCamera.setCanvasWidthHeight(getCanvasWidth(), getCanvasHeight());
	
	//JG Never skip the update loop this is causing lots of problems
//    if(bUpdateSystem)
    {
        for(vector<ofx1DExtruder *>::iterator it = extruders.begin(); it != extruders.end(); ++it)
        {
            (*it)->update();
        }
        
		
		//update camera
		translatedHeadPosition = getRGBDVideoPlayer().headPosition * pointcloudScale + ofVec3f(0,0,pointcloudOffsetZ);
		cloudsCamera.lookTarget = translatedHeadPosition;
		
        selfUpdate();
    }
	
	if(bMatchBackgrounds){
		bgHue2 = bgHue;
		bgSat2 = bgSat;
		bgBri2 = bgBri;
	}
    
    durationLabel->setLabel(ofxTimecode::timecodeForSeconds(timeline->getInOutRange().span() * timeline->getDurationInSeconds()));
    
	bgColor = ofColor::fromHsb(MIN(bgHue,254.), bgSat, bgBri, 255);
	bgColor2 = ofColor::fromHsb(MIN(bgHue2,254.), bgSat2, bgBri2, 255);
	
	//Make this happen only when the timeline is modified by the user or when a new track is added.
    
	if(!ofGetMousePressed())
    {
//		ofLogError("TIMELINE UPDATE FOR SYSTEM " + getSystemName());
		timeline->setOffset(ofVec2f(4, ofGetHeight() - timeline->getHeight() - 4 ));
		timeline->setWidth(ofGetWidth() - 8);
	}
	
	checkOpenGLError(getSystemName() + ":: UPDATE");
	
	updateCyclced = true;
}

void CloudsVisualSystem::draw(ofEventArgs & args)
{

	if(!updateCyclced)
		return;
	
	ofPushStyle();

    if(bRenderSystem)
    {
	  
		//bind our fbo, lights, debug
        if(bUseOculusRift){
			#ifdef OCULUS_RIFT
			checkOpenGLError(getSystemName() + ":: BEFORE DRAW BACKGROUND");
            getOculusRift().beginBackground();
			drawBackgroundGradient();
            getOculusRift().endBackground();
			checkOpenGLError(getSystemName() + ":: AFTER DRAW BACKGROUND");

			getOculusRift().beginOverlay(-230, 320,240);
			checkOpenGLError(getSystemName() + ":: BEFORE DRAW OVERLAY");
			selfDrawOverlay();
			checkOpenGLError(getSystemName() + ":: AFTER DRAW OVERLAY");
			getOculusRift().endOverlay();
			
            if(bIs2D){
                CloudsVisualSystem::getSharedRenderTarget().begin();
                if(bClearBackground){
                    ofClear(0, 0, 0, 1.0);
                }                
                selfDrawBackground();
                CloudsVisualSystem::getSharedRenderTarget().end();
                
                getOculusRift().baseCamera = &getCameraRef();
                getOculusRift().beginLeftEye();
                draw2dSystemPlane();
                getOculusRift().endLeftEye();
                
                getOculusRift().beginRightEye();
                draw2dSystemPlane();
                getOculusRift().endRightEye();
            }
            else{
                getOculusRift().baseCamera = &getCameraRef();
                getOculusRift().beginLeftEye();
                drawScene();
                getOculusRift().endLeftEye();
                
                getOculusRift().beginRightEye();
                drawScene();
                getOculusRift().endRightEye();
            }
			#endif
		}
		else {
			
			CloudsVisualSystem::getSharedRenderTarget().begin();
			if(bClearBackground){
				ofClear(0, 0, 0, 1.0);
			}
			drawBackground();
			
			getCameraRef().begin();
			
			drawScene();
			
			getCameraRef().end();
			
			ofPushStyle();
			ofPushMatrix();
			ofTranslate(0, getCanvasHeight() );
			ofScale(1,-1,1);
			
			selfDrawOverlay();

			ofPopMatrix();
			ofPopStyle();
	
			CloudsVisualSystem::getSharedRenderTarget().end();
            
		}
		
		//draw the fbo to the screen as a full screen quad
		if(bDrawToScreen){
			selfPostDraw();
            checkOpenGLError(getSystemName() + ":: POST DRAW");
		}
		
#ifndef OCULUS_RIFT
        drawCursor();
#endif
        drawKinectDebug();

	}
    
	if(timeline != NULL && timeline->getIsShowing())
    {
		ofEnableAlphaBlending();
        ofSetColor(gui->getColorBack());
        ofRect(timeline->getDrawRect()); 
		timeline->draw();
	}
	
    ofPopStyle();
}


void CloudsVisualSystem::drawKinectDebug(){
#ifdef KINECT_INPUT
    if (timeline->getIsShowing()) {
        ofPtr<CloudsInputKinectOSC> kinectInput = dynamic_pointer_cast<CloudsInputKinectOSC>(GetCloudsInput());
        if (kinectInput->bDoDebug) {
            static const int kDebugMargin = 0;
            static const int kDebugWidth  = 640;
            static const int kDebugHeight = 480;
            kinectInput->debug(CloudsVisualSystem::getSharedRenderTarget().getWidth()  - kDebugWidth  - kDebugMargin,
                               kDebugMargin,
                               kDebugWidth, kDebugHeight);
        }
    }
#endif
}

void CloudsVisualSystem::draw2dSystemPlane(){
    // create a plane and map our 2d systems to it
    ofPushMatrix();
    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2, 0);
    
    ofMesh mesh;
    get2dMesh(mesh, ofGetWidth(), ofGetHeight());
    getSharedRenderTarget().getTextureReference().bind();
    mesh.draw();
    getSharedRenderTarget().getTextureReference().unbind();
    
    ofPopMatrix();
}

void CloudsVisualSystem::drawScene(){
	
	ofPushMatrix();
    
	//start our 3d scene
	ofRotateX(xRot->getPos());
	ofRotateY(yRot->getPos());
	ofRotateZ(zRot->getPos());
    
	selfSceneTransformation();

    if(bUseInteractiveCamera){
        interactiveCameraRot *= 1. - interactiveCameraDamping;
        
        interactiveCameraRot.x += ofMap(GetCloudsInputX(), 0, getCanvasWidth(), interactiveCameraMinX, interactiveCameraMaxX)*interactiveCameraDamping;
        interactiveCameraRot.y += ofMap(GetCloudsInputY(), 0, getCanvasHeight(), interactiveCameraMinY, interactiveCameraMaxY)*interactiveCameraDamping;
  
        GLfloat model[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, model);
        ofMatrix4x4 curmv;
        curmv.set(model);
        ofMultMatrix(curmv.getInverse());
        ofRotate( interactiveCameraRot.x, 0, 1, 0);
        ofRotate( interactiveCameraRot.y, 1, 0, 0);
        ofMultMatrix(curmv);
    }
	
	glEnable(GL_DEPTH_TEST);
	
	ofPushStyle();
	drawDebug();
	checkOpenGLError(getSystemName() + ":: DRAW DEBUG");
	ofPopStyle();
	
	lightsBegin();
	
	//draw this visual system
	ofPushStyle();
	selfDraw();
	checkOpenGLError(getSystemName() + ":: DRAW");
	ofPopStyle();
	
	lightsEnd();
	
	glDisable(GL_DEPTH_TEST);
    
    ofPopMatrix();

#ifdef OCULUS_RIFT
    if(drawCursorMode > DRAW_CURSOR_NONE){
        ofPushStyle();
        ofPushMatrix();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        
        ofTranslate(getCameraRef().getPosition());
        ofMatrix4x4 baseRotation;
        baseRotation.makeRotationMatrix(getCameraRef().getOrientationQuat());
        if(getOculusRift().lockView){
            ofMultMatrix(baseRotation);
        }
        else {
            ofMultMatrix(getOculusRift().getOrientationMat() * baseRotation);
        }
        
        ofEnableAlphaBlending();
        
        ofVec3f cursorPt = ofVec3f(0, 0, -150);
        selfDrawCursor(cursorPt, 0);
        
        glPopAttrib();
        ofPopMatrix();
        ofPopStyle();
    }
#endif
	
}

void CloudsVisualSystem::setupRGBDTransforms(){
	ofTranslate(0,0,pointcloudOffsetZ);
	ofScale(pointcloudScale,pointcloudScale,pointcloudScale);
	ofScale(-1, -1, 1);
}

void CloudsVisualSystem::exit()
{
	if( !bIsSetup ){
		return;
	}
	
	
    saveGUIS();
	deleteGUIS();
	
    for(vector<ofx1DExtruder *>::iterator it = extruders.begin(); it != extruders.end(); ++it)
    {
        ofx1DExtruder *e = (*it);
        delete e;
    }
    extruders.clear();
    
    for(map<string, ofxLight *>::iterator it = lights.begin(); it != lights.end(); ++it)
    {
        ofxLight *l = it->second;
        delete l;
    }
    lights.clear();
    
    for(map<string, ofxMaterial *>::iterator it = materials.begin(); it != materials.end(); ++it)
    {
        ofxMaterial *m = it->second;
        delete m;
    }
    materials.clear();
	
	if(cameraTrack != NULL){
		cameraTrack->disable();
		delete cameraTrack;
		cameraTrack = NULL;
	}
	
	if(timeline != NULL){
		ofRemoveListener(timeline->events().bangFired, this, &CloudsVisualSystem::timelineBangEvent);
		delete timeline;
		timeline = NULL;
	}
	bIsSetup = false;
 
}

void CloudsVisualSystem::keyPressed(ofKeyEventArgs & args)
{
	
	if(timeline->isModal()){
		return;
	}
	
    for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
    {
        if((*it)->hasKeyboardFocus())
        {
            return;
        }
    }
	
    switch (args.key)
    {
        case '1':
            toggleGuiAndPosition(gui);
            break;
        case '2':
            toggleGuiAndPosition(sysGui);
            break;
        case '3':
            toggleGuiAndPosition(rdrGui);
            break;
        case '4':
            toggleGuiAndPosition(bgGui);
            break;
        case '5':
            toggleGuiAndPosition(lgtGui);
            break;
#ifdef KINECT_INPUT
        case '8':
            toggleGuiAndPosition(kinectGui);
            break;
#endif
#ifdef OCULUS_RIFT
        case '9':
            toggleGuiAndPosition(oculusGui);
            break;
#endif
//        case '0':
//            toggleGuiAndPosition(camGui);
//            break;
            
        case 'u':
        {
            bUpdateSystem = !bUpdateSystem;
        }
            break;
			
        case ' ':
        {
			timeline->togglePlay();

        }
            break;
			
        case '`':
        {
            ofImage img;
            img.grabScreen(0,0,ofGetWidth(), ofGetHeight());
			if( !ofDirectory(getVisualSystemDataPath()+"snapshots/").exists() ){
				ofDirectory(getVisualSystemDataPath()+"snapshots/").create();
			}
            img.saveImage(getVisualSystemDataPath()+"snapshots/" + getSystemName() + " " + ofGetTimestampString() + ".png");
        }
            break;
            
        case 'h':
        {
			toggleGUIS();
        }
            break;
            
        case 'f':
        {
            ofToggleFullscreen();
        }
            break;
            
        case 'p':
        {
            for(int i = 0; i < guis.size(); i++)
            {
                guis[i]->setDrawWidgetPadding(true);
            }
        }
            break;
            
        case 'e':
        {
			stackGuiWindows();
        }
            break;
            
        case 'r':
        {
            ofxUISuperCanvas *last = NULL;
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                if(last != NULL)
                {
                    (*it)->getRect()->setX(last->getRect()->getX()+last->getRect()->getWidth()+1);
                    (*it)->getRect()->setY(1);
                }
                else
                {
                    (*it)->getRect()->setX(1);
                    (*it)->getRect()->setY(1);
                }
                last = (*it);
                last->setMinified(false);
            }
        }
            break;
            
        case 't':
        {
            ofxUISuperCanvas *last = NULL;
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                (*it)->setMinified(true);
                if(last != NULL)
                {
                    (*it)->getRect()->setX(1);
                    (*it)->getRect()->setY(last->getRect()->getY()+last->getRect()->getHeight()+1);
                }
                else
                {
                    (*it)->getRect()->setX(1);
                    (*it)->getRect()->setY(1);
                }
                last = (*it);
            }
        }
            break;
            
        case 'y':
        {
            float x = ofGetWidth()*.5;
            float y = ofGetHeight()*.5;
            float tempRadius = gui->getGlobalCanvasWidth()*2.0;
            float stepSize = TWO_PI/(float)guis.size();
            float theta = 0;
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                (*it)->getRect()->setX(x+sin(theta)*tempRadius - (*it)->getRect()->getHalfWidth());
                (*it)->getRect()->setY(y+cos(theta)*tempRadius - (*it)->getRect()->getHalfHeight());
                theta +=stepSize;
            }
        }
            break;
            
        case '=':
        {
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                (*it)->toggleMinified();
            }
        }
		break;
            
        case 'T':
			cameraTrack->addKeyframe();
			break;
		case 'L':
			cameraTrack->lockCameraToTrack = !cameraTrack->lockCameraToTrack;
			break;
		case ',':
			timeline->setCurrentFrame(0);
			break;
		case 'I':
			timeline->setInPointAtPlayhead();
			break;
		case 'O':
			timeline->setOutPointAtPlayhead();
			break;
#ifdef OCULUS_RIFT
		case '0':
			oculusRift.reset();
			break;
#endif
        default:
            selfKeyPressed(args);
            break;
    }
}

void CloudsVisualSystem::stackGuiWindows(){
	ofxUISuperCanvas *last = NULL;
	for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
	{
		(*it)->setMinified(false);
		if(last != NULL)
		{
			(*it)->getRect()->setX(last->getRect()->getX());
			(*it)->getRect()->setY(last->getRect()->getY()+last->getRect()->getHeight()+1);
			if((*it)->getRect()->getY()+(*it)->getRect()->getHeight() > ofGetHeight()-timeline->getHeight())
			{
				(*it)->getRect()->setX(last->getRect()->getX()+last->getRect()->getWidth()+1);
				(*it)->getRect()->setY(1);
			}
		}
		else
		{
			(*it)->getRect()->setX(1);
			(*it)->getRect()->setY(1);
		}
		last = (*it);
	}
}

void CloudsVisualSystem::keyReleased(ofKeyEventArgs & args)
{
    switch (args.key)
    {
        case 'p':
        {
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                (*it)->setDrawWidgetPadding(false);
            }
        }
		break;
            
            
        default:
            selfKeyReleased(args);
            break;
    }
}

void CloudsVisualSystem::interactionMoved(CloudsInteractionEventArgs& args){
    if(args.primary){
        ofMouseEventArgs fakeArgs;
        fakeArgs.x = args.position.x;
        fakeArgs.y = args.position.y;
        fakeArgs.button = args.actionType;
        mouseMoved(fakeArgs);
    }
	selfInteractionMoved(args);
}

void CloudsVisualSystem::interactionStarted(CloudsInteractionEventArgs& args){
    if(args.primary){
        ofMouseEventArgs fakeArgs;
        fakeArgs.x = args.position.x;
        fakeArgs.y = args.position.y;
        fakeArgs.button = args.actionType;
        mousePressed(fakeArgs);
    }
	selfInteractionStarted(args);
}

void CloudsVisualSystem::interactionDragged(CloudsInteractionEventArgs& args){
    if(args.primary){
        ofMouseEventArgs fakeArgs;
        fakeArgs.x = args.position.x;
        fakeArgs.y = args.position.y;
        fakeArgs.button = args.actionType;
        mouseDragged(fakeArgs);
    }
	selfInteractionDragged(args);
}

void CloudsVisualSystem::interactionEnded(CloudsInteractionEventArgs& args){
    if(args.primary){ 
        ofMouseEventArgs fakeArgs;
        fakeArgs.x = args.position.x;
        fakeArgs.y = args.position.y;
        fakeArgs.button = args.actionType;
        mouseReleased(fakeArgs);
    }
	selfInteractionEnded(args);
}

void CloudsVisualSystem::mouseDragged(ofMouseEventArgs& data)
{
    selfMouseDragged(data);
}

void CloudsVisualSystem::mouseMoved(ofMouseEventArgs& data)
{
#ifdef OCULUS_RIFT
    // Remap the mouse coords.
    ofRectangle viewport = getOculusRift().getOculusViewport();
//    cout << "MOUSE IN: " << data.x << ", " << data.y << " // VIEWPORT: " << viewport.x << ", " << viewport.y << ", " << viewport.width << ", " << viewport.height;
    data.x = ofMap(data.x, 0, ofGetWidth(), viewport.x, viewport.width);
    data.y = ofMap(data.y, 0, ofGetHeight(), viewport.y, viewport.height);
//    cout << "// MOUSE OUT: " << data.x << ", " << data.y << endl;
#endif
    selfMouseMoved(data);
}

void CloudsVisualSystem::mousePressed(ofMouseEventArgs & args)
{
	if(cursorIsOverGUI()){
		cam.disableMouseInput();
	}
    selfMousePressed(args);
}

void CloudsVisualSystem::mouseReleased(ofMouseEventArgs & args)
{
    cam.enableMouseInput();
    selfMouseReleased(args);
}


bool CloudsVisualSystem::cursorIsOverGUI(){
	if( timeline->getIsShowing() && timeline->getDrawRect().inside(ofGetMouseX(),ofGetMouseY())){
		return true;
	}
	
    for(int i = 0; i < guis.size(); i++)
    {
		
		if(guis[i]->isHit(ofGetMouseX(), ofGetMouseY()))
		{
			return true;
		}
	}
	return false;
}

void CloudsVisualSystem::setupAppParams()
{
//	colorPalletes = new ofxColorPalettes(GetCloudsDataPath()+"colors/");
    ofSetSphereResolution(30);
    bRenderSystem = true;
    bUpdateSystem = true;
}

void CloudsVisualSystem::setupDebugParams()
{
    //DEBUG
    bDebug = false;
    debugGridSize = 100;
}

void CloudsVisualSystem::setupCameraParams()
{
    //CAMERA
    camFOV = 60;
    camDistance = 200;
    cam.setDistance(camDistance);
    cam.setFov(camFOV);

    xRot = new ofx1DExtruder(0);
    yRot = new ofx1DExtruder(0);
    zRot = new ofx1DExtruder(0);
    xRot->setPhysics(.9, 5.0, 25.0);
    yRot->setPhysics(.9, 5.0, 25.0);
    zRot->setPhysics(.9, 5.0, 25.0);
    extruders.push_back(xRot);
    extruders.push_back(yRot);
    extruders.push_back(zRot);
}

void CloudsVisualSystem::setupLightingParams()
{
    //LIGHTING
    bSmoothLighting = true;
    bEnableLights = true;
	globalAmbientColorHSV.r = 1.0; //hue
	globalAmbientColorHSV.g = 0.0; //sat
	globalAmbientColorHSV.b = 0.5; //bri
	globalAmbientColorHSV.a = 1.0;
	
	light = new ofxLight();

}

void CloudsVisualSystem::setupMaterialParams()
{
	mat = new ofxMaterial();
}

void CloudsVisualSystem::setupTimeLineParams()
{
	timeline = NULL;
    bShowTimeline = false;
	bTimelineIsIndefinite = false;
    bDeleteTimelineTrack = false;
    timelineDuration = 60;
    bEnableTimeline = true;
    bEnableTimelineTrackCreation = false;
}

void CloudsVisualSystem::setupCoreGuis()
{
    setupGui();
    setupSystemGui();
    setupRenderGui();
    setupBackgroundGui();
    setupLightingGui();
    setupCameraGui();
    setupMaterial("MATERIAL 1", mat);
    setupPointLight("POINT LIGHT 1", light);
    setupPresetGui();
#ifdef KINECT_INPUT
    setupKinectGui();
#endif
#ifdef OCULUS_RIFT
    setupOculusGui();
#endif
}

void CloudsVisualSystem::setupGui()
{
    gui = new ofxUISuperCanvas(ofToUpper(getSystemName()));
    gui->setName("Settings");
    gui->setWidgetFontSize(OFX_UI_FONT_SMALL);

    
    ofxUIFPS *fps = gui->addFPS();
    gui->resetPlacer();
    gui->addWidgetDown(fps, OFX_UI_ALIGN_RIGHT, true);
    gui->addWidgetToHeader(fps);
    
    gui->addSpacer();
    gui->addButton("SAVE", false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ofxUIButton *loadbtn = gui->addButton("LOAD", false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    ofxUIButton *updatebtn = gui->addToggle("UPDATE", &bUpdateSystem);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ofxUIButton *renderbtn = gui->addToggle("RENDER", &bRenderSystem);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addWidgetNorthOf(loadbtn, "RENDER", true);
    gui->setPlacer(updatebtn);
    gui->addSpacer();
    vector<string> cursorModeNames;
    cursorModeNames.push_back("NO CURSOR");
    cursorModeNames.push_back("PRIMARY CURSOR");
    cursorModeNames.push_back("ALL CURSORS");
    gui->addRadio("CURSOR MODES", cursorModeNames)->activateToggle(cursorModeNames[DRAW_CURSOR_NONE]);
    gui->addSpacer();
    selfSetupGui();
    gui->autoSizeToFitWidgets();

    
    ofAddListener(gui->newGUIEvent,this,&CloudsVisualSystem::guiEvent);
    guis.push_back(gui);
    guimap[gui->getName()] = gui;
}

vector<string> CloudsVisualSystem::getPresets()
{
	vector<string> presets;
	string presetPath = getVisualSystemDataPath() + "Presets/";
	ofDirectory presetsFolder = ofDirectory(presetPath);
	
	if(presetsFolder.exists()){
		presetsFolder.listDir();
		for(int i = 0; i < presetsFolder.size(); i++){
			if(presetsFolder.getFile(i).isDirectory() &&
               ofFilePath::removeTrailingSlash(presetsFolder.getName(i)) != "Working" &&
			   presetsFolder.getName(i).at(0) != '_') //use leading _ to hide folders
            {
				presets.push_back(presetsFolder.getName(i));
			}
		}
	}
	return presets;
}


float CloudsVisualSystem::getIntroDuration(){
	vector<ofxTLKeyframe*>& flags = introOutroTrack->getKeyframes();
	for(int i = 0; i < flags.size(); i++){
		ofxTLFlag* flag = (ofxTLFlag*)flags[i];
		string text = ofToLower(flag->textField.text);
		if( (!bTimelineIsIndefinite && text == "intro") ||
		    ( bTimelineIsIndefinite && text == "middle"))
		{
			return flag->time/1000.0;
		}
	}
	return 0;
}

float CloudsVisualSystem::getOutroDuration(){
	vector<ofxTLKeyframe*>& flags = introOutroTrack->getKeyframes();
	for(int i = 0; i < flags.size(); i++){
		ofxTLFlag* flag = (ofxTLFlag*)flags[i];
		string text = ofToLower(flag->textField.text);
		if( (!bTimelineIsIndefinite && text == "outro") ||
			( bTimelineIsIndefinite && text == "middle"))
		{
			return timeline->getDurationInSeconds() - flag->time/1000.0;
		}
	}
	
	return 0;
}

void CloudsVisualSystem::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if(name == "SAVE")
    {
        ofxUIButton *b = (ofxUIButton *) e.widget;
        if(b->getValue())
        {
			#ifdef TARGET_OSX
            string presetName = ofxSystemTextBoxDialog("Save Preset As", currentPresetName);
			#else
			string presetName = ofSystemTextBoxDialog("Save Preset As", currentPresetName);
			#endif
            if(presetName != ""){
                savePresetGUIS(presetName);
				currentPresetName = presetName;
            }
        }
    }
    else if(name == "LOAD")
    {
        ofxUIButton *b = (ofxUIButton *) e.widget;
        if(b->getValue())
        {
            ofFileDialogResult result = ofSystemLoadDialog("Load Visual System Preset Folder", true, getVisualSystemDataPath()+"Presets/");
            if(result.bSuccess && result.fileName.length())
            {
                loadPresetGUISFromPath(result.filePath);
            }
            else{
                loadGUIS();
            }
            
        }
    }

    else if(name == "NO CURSOR")
    {
        ofxUIButton *b = (ofxUIButton *) e.widget;
        if(b->getValue())
        {
            drawCursorMode = DRAW_CURSOR_NONE;
        }
    }
    else if(name == "PRIMARY CURSOR")
    {
        ofxUIButton *b = (ofxUIButton *) e.widget;
        if(b->getValue())
        {
            drawCursorMode = DRAW_CURSOR_PRIMARY;
        }
    }
    else if(name == "ALL CURSORS")
    {
        ofxUIButton *b = (ofxUIButton *) e.widget;
        if(b->getValue())
        {
            drawCursorMode = DRAW_CURSOR_ALL;
        }
    }
	
    selfGuiEvent(e);
}

void CloudsVisualSystem::setupSystemGui()
{
    sysGui = new ofxUISuperCanvas("SYSTEM", gui);
    sysGui->copyCanvasStyle(gui);
    sysGui->copyCanvasProperties(gui);
    sysGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    sysGui->setName("SystemSettings");
    sysGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = sysGui->addToggle("DEBUG", &bDebug);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    sysGui->resetPlacer();
    sysGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    sysGui->addWidgetToHeader(toggle);
    sysGui->addSpacer();
    
    selfSetupSystemGui();
    sysGui->autoSizeToFitWidgets();
    ofAddListener(sysGui->newGUIEvent,this,&CloudsVisualSystem::guiSystemEvent);
    guis.push_back(sysGui);
    guimap[sysGui->getName()] = sysGui;
}

void CloudsVisualSystem::setupRenderGui()
{
    rdrGui = new ofxUISuperCanvas("RENDER", gui);
    rdrGui->copyCanvasStyle(gui);
    rdrGui->copyCanvasProperties(gui);
    rdrGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    rdrGui->setName("RenderSettings");
    rdrGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    rdrGui->addSpacer();
    selfSetupRenderGui();
    
    rdrGui->autoSizeToFitWidgets();
    ofAddListener(rdrGui->newGUIEvent,this,&CloudsVisualSystem::guiRenderEvent);
    guis.push_back(rdrGui);
    guimap[rdrGui->getName()] = rdrGui;
}

void CloudsVisualSystem::setupBackgroundGui()
{
    
	gradientMode = 0;
    bgColor = ofColor(0,0,0);
    bgColor2 = ofColor(0,0,0);
    
    bgGui = new ofxUISuperCanvas("BACKGROUND", gui);
    bgGui->copyCanvasStyle(gui);
    bgGui->copyCanvasProperties(gui);
    bgGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    bgGui->setName("BgSettings");
    bgGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = bgGui->addToggle("GRAD", false);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    bgGui->resetPlacer();
    bgGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    bgGui->addWidgetToHeader(toggle);
	bgGui->addToggle("BAR GRAD", &bBarGradient);
    bgGui->addSpacer();

    bgGui->addSlider("HUE", 0.0, 255.0, &bgHue);
    bgGui->addSlider("SAT", 0.0, 255.0, &bgSat);
    bgGui->addSlider("BRI", 0.0, 255.0, &bgBri);
    bgGui->addSpacer();
	bgGui->addButton("MATCH", &bMatchBackgrounds);
    hueSlider = bgGui->addSlider("HUE2", 0.0, 255.0, &bgHue2);
    satSlider = bgGui->addSlider("SAT2", 0.0, 255.0, &bgSat2);
    briSlider = bgGui->addSlider("BRI2", 0.0, 255.0, &bgBri2);
    bgGui->autoSizeToFitWidgets();
    ofAddListener(bgGui->newGUIEvent, this, &CloudsVisualSystem::guiBackgroundEvent);
    guis.push_back(bgGui);
    guimap[bgGui->getName()] = bgGui;
	
	
}

void CloudsVisualSystem::guiBackgroundEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    
    if(name == "BRI")
    {
       // bgBri->setPosAndHome(bgBri->getPos());
        for(int i = 0; i < guis.size(); i++)
        {
            guis[i]->setColorBack(ofColor(255*.2, 255*.9));
			
        }
    }

    else if(name == "GRAD")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            gradientMode = OF_GRADIENT_CIRCULAR;
            hueSlider->setVisible(true);
            satSlider->setVisible(true);
            briSlider->setVisible(true);
            bgGui->autoSizeToFitWidgets();
            if(bgGui->isMinified())
            {
                bgGui->setMinified(true);
            }
        }
        else
        {
            gradientMode = -1;
            hueSlider->setVisible(false);
            satSlider->setVisible(false);
            briSlider->setVisible(false);
            bgGui->autoSizeToFitWidgets();
        }
    }
	
	//change the gui color on slider change
	if(name == "HUE" || name == "SAT" || name == "BRI")
	{
		ofColor backgrounfFillColor;
		backgrounfFillColor.setHsb(bgHue, bgSat, bgBri);
		
		bgGui->getWidget("HUE")->setColorFill(backgrounfFillColor);
		bgGui->getWidget("SAT")->setColorFill(backgrounfFillColor);
		bgGui->getWidget("BRI")->setColorFill(backgrounfFillColor);
	}
	else if(name == "HUE2" || name == "SAT2" || name == "BRI2")
	{
		ofColor backgrounfFillColor;
		backgrounfFillColor.setHsb(bgHue2, bgSat2, bgBri2);
		
		bgGui->getWidget("HUE2")->setColorFill(backgrounfFillColor);
		bgGui->getWidget("SAT2")->setColorFill(backgrounfFillColor);
		bgGui->getWidget("BRI2")->setColorFill(backgrounfFillColor);
	}
}

void CloudsVisualSystem::setupLightingGui()
{
    lgtGui = new ofxUISuperCanvas("LIGHT", gui);
    lgtGui->copyCanvasStyle(gui);
    lgtGui->copyCanvasProperties(gui);
    lgtGui->setName("LightSettings");
    lgtGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    lgtGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = lgtGui->addToggle("ENABLE", &bEnableLights);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    lgtGui->resetPlacer();
    lgtGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    lgtGui->addWidgetToHeader(toggle);
    
    lgtGui->addSpacer();
    lgtGui->addToggle("SMOOTH", &bSmoothLighting);
    lgtGui->addSpacer();
    float length = (lgtGui->getGlobalCanvasWidth()-lgtGui->getWidgetSpacing()*5)/3.;
    float dim = lgtGui->getGlobalSliderHeight();
    lgtGui->addLabel("GLOBAL AMBIENT COLOR", OFX_UI_FONT_SMALL);
    lgtGui->addMinimalSlider("H", 0.0, 1.0, &globalAmbientColorHSV.r, length, dim)->setShowValue(false);
    lgtGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    lgtGui->addMinimalSlider("S", 0.0, 1.0, &globalAmbientColorHSV.g, length, dim)->setShowValue(false);
    lgtGui->addMinimalSlider("V", 0.0, 1.0, &globalAmbientColorHSV.b, length, dim)->setShowValue(false);
    lgtGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    lgtGui->autoSizeToFitWidgets();
    ofAddListener(lgtGui->newGUIEvent,this,&CloudsVisualSystem::guiLightingEvent);
    guis.push_back(lgtGui);
    guimap[lgtGui->getName()] = lgtGui;
}

void CloudsVisualSystem::guiLightingEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if(name == "H" ||
	   name == "S" ||
	   name == "V")
    {
		ofFloatColor globalAmbientColorRGB = ofFloatColor::fromHsb(globalAmbientColorHSV.r,
																   globalAmbientColorHSV.g,
																   globalAmbientColorHSV.b);
		float globalAmbientColor[4] = {
			globalAmbientColorRGB.r,
			globalAmbientColorRGB.g,
			globalAmbientColorRGB.b,
			1.0
		};
		
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientColor);
		lgtGui->getWidget("H")->setColorFill(globalAmbientColorRGB);
		lgtGui->getWidget("S")->setColorFill(globalAmbientColorRGB);
		lgtGui->getWidget("V")->setColorFill(globalAmbientColorRGB);
    }
}


void CloudsVisualSystem::setupCameraGui()
{
    camGui = new ofxUISuperCanvas("CAMERA", gui);
    camGui->copyCanvasStyle(gui);
    camGui->copyCanvasProperties(gui);
    camGui->setName("CamSettings");
    camGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    camGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIButton *button = camGui->addButton("RESET", false);
    button->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    camGui->resetPlacer();
    camGui->addWidgetDown(button, OFX_UI_ALIGN_RIGHT, true);
    camGui->addWidgetToHeader(button);
    camGui->addSpacer();
    camGui->addSlider("DIST", 0, 1000, &camDistance);
    camGui->addSlider("FOV", 0, 180, &camFOV);
    camGui->addSlider("ROT-X", 0, 360.0, xRot->getPosPtr())->setIncrement(1.0);
    camGui->addSlider("ROT-Y", 0, 360.0, yRot->getPosPtr())->setIncrement(1.0);
    camGui->addSlider("ROT-Z", 0, 360.0, zRot->getPosPtr())->setIncrement(1.0);
    camGui->addLabel("TRACK");
    camGui->addButton("ADD KEYFRAME", false);
	
	camGui->addSpacer();
	camGui->addToggle("InteractiveCamera", &bUseInteractiveCamera);
	camGui->addSlider("damping", 0, 1, &interactiveCameraDamping);
	
	camGui->addMinimalSlider("minX", -90	, 90, &interactiveCameraMinX);
	camGui->addMinimalSlider("maxX", -90	, 90, &interactiveCameraMaxX);
	camGui->addMinimalSlider("minY", -90	, 90, &interactiveCameraMinY);
	camGui->addMinimalSlider("maxY", -90	, 90, &interactiveCameraMaxY);
	camGui->addSpacer();
	
	vector<string> transitions;
	transitions.push_back("2D");
	transitions.push_back("3D FLY THROUGH");
	transitions.push_back("3D WHIP PAN");
	transitions.push_back("3D RGBD");
	transitionRadio = camGui->addRadio("TRANSITION", transitions, OFX_UI_ORIENTATION_VERTICAL);

    camGui->addSpacer();
    vector<string> views;
    views.push_back("TOP");
    views.push_back("BOTTOM");
    views.push_back("FRONT");
    views.push_back("BACK");
    views.push_back("RIGHT");
    views.push_back("LEFT");
    views.push_back("3D");
    views.push_back("DISABLE");
    
    ofxUIDropDownList *ddl = camGui->addDropDownList("VIEW", views);
    ddl->setAutoClose(false);
    ddl->setShowCurrentSelected(true);
    ddl->activateToggle("DISABLE");
	
	selfSetupCameraGui();
    
    camGui->autoSizeToFitWidgets();
    ofAddListener(camGui->newGUIEvent,this,&CloudsVisualSystem::guiCameraEvent);
    guis.push_back(camGui);
    guimap[camGui->getName()] = camGui;
	

}

////load our Transitions.xml into a map of vectors used for saving transition option name
//void CloudsVisualSystem::loadTransitionOptions()
//{
//	ofxXmlSettings *XML = new ofxXmlSettings();
//	XML->loadFile( GetCloudsDataPath() + "transitions/Transitions.xml" );
//	
//	for(int i=0; i<XML->getNumTags("TRANSITION_TYPE"); i++)
//	{
//		XML->pushTag("TRANSITION_TYPE", i);
//		
//		string typeName = XML->getValue("NAME", "NULL", 0);
//		transitionOptionMap[typeName];
//		transitionOptionMap[typeName].clear();
//		
//		int numOptions = XML->getNumTags("OPTION");
//		
//		for(int j=0; j<numOptions; j++)
//		{
//			XML->pushTag("OPTION", j);
//			
//			string optionName = XML->getValue("NAME", "NULL", 0);
//			
//			transitionOptionMap[typeName].push_back(optionName);
//			
//			XML->popTag();
//		}
//		XML->popTag();
//	}
//	delete XML;
//}

CloudsVisualSystem::RGBDTransitionType CloudsVisualSystem::getTransitionType()
{
	if(transitionRadio->getActive() == NULL)
		return WHIP_PAN;
	
	string activeTransitionType = transitionRadio->getActive()->getName();
	if(activeTransitionType == "2D"){
		cout << "TWO_DIMENSIONAL" << endl;
		return TWO_DIMENSIONAL;
	}
	else if(activeTransitionType == "3D FLY THROUGH"){
		cout << "3D FLY THROUGH" << endl;
		return FLY_THROUGH;
	}
	else if(activeTransitionType == "3D WHIP PAN"){
		cout << "3D WHIP PAN" << endl;
		return WHIP_PAN;
	}
	else if(activeTransitionType == "3D RGBD"){
		cout << "3D RGBD" << endl;
		return RGBD;
	}
	return WHIP_PAN;
}

void CloudsVisualSystem::guiCameraEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if(name == "DIST")
    {
        cam.setDistance(camDistance);
		//		currentCamera->setDistance(camDistance);
    }
    else if(name == "FOV")
    {
		//		currentCamera->setFov(camFOV);
        cam.setFov(camFOV);
    }
    else if(name == "ROT-X")
    {
        xRot->setPosAndHome(xRot->getPos());
    }
    else if(name == "ROT-Y")
    {
        yRot->setPosAndHome(yRot->getPos());
    }
    else if(name == "ROT-Z")
    {
        zRot->setPosAndHome(zRot->getPos());
    }
    else if(name == "RESET")
    {
        ofxUIButton *b = (ofxUIButton *) e.widget;
        if(b->getValue())
        {
            cam.reset();
        }
    }
    else if(name == "TOP")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(0);
            yRot->setHome(0);
            zRot->setHome(0);
        }
    }
    else if(name == "BOTTOM")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-180);
            yRot->setHome(0);
            zRot->setHome(0);
        }
    }
    else if(name == "BACK")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-90);
            yRot->setHome(0);
            zRot->setHome(180);
        }
    }
    else if(name == "FRONT")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-90);
            yRot->setHome(0);
            zRot->setHome(0);
        }
    }
    else if(name == "RIGHT")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-90);
            yRot->setHome(0);
            zRot->setHome(90);
        }
    }
    else if(name == "LEFT")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-90);
            yRot->setHome(0);
            zRot->setHome(-90);
            
        }
    }
    else if(name == "3D")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-70);
            yRot->setHome(0);
            zRot->setHome(45);
        }
    }
	else if(name == "ADD KEYFRAME"){
		cameraTrack->addKeyframe();
	}
	
	else if(name == "bUseInteractiveCamera")
	{
        interactiveCameraRot.x += ofMap(GetCloudsInputX(), 0, getCanvasWidth(), interactiveCameraMinX, interactiveCameraMaxX);
        interactiveCameraRot.y += ofMap(GetCloudsInputY(), 0, getCanvasHeight(), interactiveCameraMinY, interactiveCameraMaxY);
	}
	
//	//TRANSITION OPTIONS
//	if(name == "2D")
//	{
//		setTransitionOptionGui("TWO_DIMENSIONAL", "2D", e);
//	}
//	else if(name == "3D FLY THROUGH")
//	{
//		setTransitionOptionGui( "FLY_THROUGH", "3D FLY THROUGH", e);
//	}
//	else if(name == "3D WHIP PAN")
//	{
//		setTransitionOptionGui( "WHIP_PAN", "3D WHIP PAN", e);
//	}
//	else if(e.widget->getParent() == transitionOptionGui->getWidget("optionsRadio"))
//	{
//		cout << "getTransitionOption() = "<< getTransitionOption() << endl;
//	}
//	else{
//		transitionOptionGui->setVisible(false);
//	}
}

//void CloudsVisualSystem::setTransitionOptionGui(string transitionType, string screenName, ofxUIEventArgs &e)
//{
//	if(transitionOptionMap[transitionType].size()>0)
//	{
//		transitionOptionGui->setVisible(true);
//		if(transitionOptionGui->getWidget(screenName) == NULL)
//		{
//			//clear then fill the gui with stuff
//			transitionOptionGui->removeWidgets();
//			transitionOptionGui->addLabel(screenName);
//			transitionOptionGui->addSpacer();
//			transitionOptionGui->addRadio("optionsRadio", transitionOptionMap[transitionType]);
//			transitionOptionGui->autoSizeToFitWidgets();
//		}
//		
//		transitionOptionGui->setPosition( e.widget->getRect()->getX() + transitionOptionGui->getRect()->getWidth(), e.widget->getRect()->getY());
//	}
//}

//string CloudsVisualSystem::getTransitionOption()
//{
//	if(transitionOptionGui != NULL)
//	{
//		ofxUIRadio* r = (ofxUIRadio*)transitionOptionGui->getWidget("optionsRadio");
//		if( r != NULL)
//		{
//			for (auto &t: r->getToggles())
//			{
//				if(t->getValue())
//				{
//					return t->getName();
//				}
//			}
//		}
//	}
//	
//	return "default";
//}

void CloudsVisualSystem::setupPresetGui()
{
	presetGui = new ofxUISuperCanvas("PRESETS");
    presetGui->setTriggerWidgetsUponLoad(false);
	presetGui->setName("Presets");
	presetGui->copyCanvasStyle(gui);
    presetGui->copyCanvasProperties(gui);
    presetGui->addSpacer();
    
    vector<string> empty;
	empty.clear();
	presetRadio = presetGui->addRadio("PRESETS", empty);
	
	presetGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    vector<string> presets = getPresets();
    for(vector<string>::iterator it = presets.begin(); it != presets.end(); ++it) {
        ofxUIToggle *t = presetGui->addToggle((*it), false);
        presetRadio->addToggle(t);
    }
	
	presetGui->autoSizeToFitWidgets();
    ofAddListener(presetGui->newGUIEvent,this,&CloudsVisualSystem::guiPresetEvent);
    guis.push_back(presetGui);
    guimap[presetGui->getName()] = presetGui;
}

void CloudsVisualSystem::guiPresetEvent(ofxUIEventArgs &e)
{
    ofxUIToggle *t = (ofxUIToggle *) e.widget;
    if(t->getValue()){
		
		if(isSetup()){
			selfEnd();
		}
        
		loadPresetGUISFromName(e.widget->getName());
		
		if(isSetup()){
			selfBegin();
		}
    }
}

void CloudsVisualSystem::setupMaterial(string name, ofxMaterial *m)
{
    materials[name] = m;
    ofxUISuperCanvas* g = new ofxUISuperCanvas(name, gui);
    materialGuis[name] = g;
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setName(name+"Settings");
    g->addSpacer();
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    float length = (g->getGlobalCanvasWidth()-g->getWidgetSpacing()*5)/3.;
    float dim = g->getGlobalSliderHeight();

   // NO EFFECT
//    g->addLabel("AMBIENT", OFX_UI_FONT_SMALL);
//    g->addMinimalSlider("AH", 0.0, 1.0, &m->matAmbientHSV.r, length, dim)->setShowValue(false);
//    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
//    g->addMinimalSlider("AS", 0.0, 1.0, &m->matAmbientHSV.g, length, dim)->setShowValue(false);
//    g->addMinimalSlider("AV", 0.0, 1.0, &m->matAmbientHSV.b, length, dim)->setShowValue(false);
//    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
//    g->addSpacer();
//    
//    g->addLabel("DIFFUSE", OFX_UI_FONT_SMALL);
//    g->addMinimalSlider("DH", 0.0, 1.0, &m->matDiffuseHSV.r, length, dim)->setShowValue(false);
//    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
//    g->addMinimalSlider("DS", 0.0, 1.0, &m->matDiffuseHSV.g, length, dim)->setShowValue(false);
//    g->addMinimalSlider("DV", 0.0, 1.0, &m->matDiffuseHSV.b, length, dim)->setShowValue(false);
//    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
//    g->addSpacer();
	
    g->addLabel("EMISSIVE", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("EH", 0.0, 1.0, &m->matEmissiveHSV.r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("ES", 0.0, 1.0, &m->matEmissiveHSV.g, length, dim)->setShowValue(false);
    g->addMinimalSlider("EV", 0.0, 1.0, &m->matEmissiveHSV.b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    g->addSpacer();
    
    g->addLabel("SPECULAR", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("SH", 0.0, 1.0, &m->matSpecularHSV.r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("SS", 0.0, 1.0, &m->matSpecularHSV.g, length, dim)->setShowValue(false);
    g->addMinimalSlider("SV", 0.0, 1.0, &m->matSpecularHSV.b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    g->addSpacer();
    
    g->addMinimalSlider("SHINY", 0.0, 128.0, &m->matShininess)->setShowValue(false);
    
    g->autoSizeToFitWidgets();
    g->setPosition(ofGetWidth()*.5-g->getRect()->getHalfWidth(), ofGetHeight()*.5 - g->getRect()->getHalfHeight());
    
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystem::guiMaterialEvent);
    guis.push_back(g);
    guimap[g->getName()] = g;
}

void CloudsVisualSystem::guiMaterialEvent(ofxUIEventArgs &e)
{
	if(e.widget->getName() == "EH" ||
	   e.widget->getName() == "ES" ||
	   e.widget->getName() == "EV")
	{
		mat->updateColors();
		guimap[e.widget->getParent()->getName()]->getWidget("EH")->setColorFill(mat->matEmissive);
		guimap[e.widget->getParent()->getName()]->getWidget("ES")->setColorFill(mat->matEmissive);
		guimap[e.widget->getParent()->getName()]->getWidget("EV")->setColorFill(mat->matEmissive);
	}
	if(e.widget->getName() == "SH" ||
	   e.widget->getName() == "SS" ||
	   e.widget->getName() == "SV")
	{
		mat->updateColors();
		guimap[e.widget->getParent()->getName()]->getWidget("SH")->setColorFill(mat->matSpecular);
		guimap[e.widget->getParent()->getName()]->getWidget("SS")->setColorFill(mat->matSpecular);
		guimap[e.widget->getParent()->getName()]->getWidget("SV")->setColorFill(mat->matSpecular);
	}
}

void CloudsVisualSystem::setupPointLight(string name, ofxLight* l)
{
	if(l == NULL){
		l = new ofxLight();
	}
	
    l->light.setPointLight();
	//removes light until we are active
	l->light.destroy();
	
    lights[name] = l;
    
    ofxUISuperCanvas* g = new ofxUISuperCanvas(name, gui);
    lightGuis[name] = g;
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ENABLE", &l->bEnabled);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    setupGenericLightProperties(g, l);
    
    g->autoSizeToFitWidgets();
    g->setPosition(ofGetWidth()*.5-g->getRect()->getHalfWidth(), ofGetHeight()*.5 - g->getRect()->getHalfHeight());
    
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystem::guiLightEvent);
    guis.push_back(g);
    guimap[g->getName()] = g;
	
}

void CloudsVisualSystem::setupSpotLight(string name)
{
    ofxLight *l = new ofxLight();
    l->light.setSpotlight();
	l->light.destroy();
	
    lights[name] = l;
    
    ofxUISuperCanvas* g = new ofxUISuperCanvas(name, gui);
    lightGuis[name] = g;
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ENABLE", &l->bEnabled);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    g->addSlider("CUT OFF", 0, 90, &l->lightSpotCutOff);
    g->addSlider("EXPONENT", 0.0, 128.0, &l->lightExponent);
    g->addSpacer();
    
    setupGenericLightProperties(g, l);
    
    g->autoSizeToFitWidgets();
    g->setPosition(ofGetWidth()*.5-g->getRect()->getHalfWidth(), ofGetHeight()*.5 - g->getRect()->getHalfHeight());
    
    ofAddListener(g->newGUIEvent, this, &CloudsVisualSystem::guiLightEvent);
    guis.push_back(g);
    guimap[g->getName()] = g;
}

void CloudsVisualSystem::setupBeamLight(string name)
{
    ofxLight *l = new ofxLight();
    l->light.setDirectional();
	l->light.destroy();
	
    lights[name] = l;
    
    ofxUISuperCanvas* g = new ofxUISuperCanvas(name, gui);
    lightGuis[name] = g;
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ENABLE", &l->bEnabled);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    setupGenericLightProperties(g, l);
    
    g->autoSizeToFitWidgets();
    g->setPosition(ofGetWidth()*.5 - g->getRect()->getHalfWidth(), ofGetHeight()*.5 - g->getRect()->getHalfHeight());
	
    ofAddListener(g->newGUIEvent,this, &CloudsVisualSystem::guiLightEvent);
    guis.push_back(g);
    guimap[g->getName()] = g;
}

void CloudsVisualSystem::setupGenericLightProperties(ofxUISuperCanvas *g, ofxLight *l)
{
    float length = (g->getGlobalCanvasWidth()-g->getWidgetSpacing()*5)/3.;
    float dim = g->getGlobalSliderHeight();
    
    switch(l->light.getType())
    {
        case OF_LIGHT_POINT:
        {
            g->addLabel("POSITION", OFX_UI_FONT_SMALL);
            g->addMinimalSlider("X", -1000.0, 1000.0, &l->lightPos.x, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
            g->addMinimalSlider("Y", -1000.0, 1000.0, &l->lightPos.y, length, dim)->setShowValue(false);
            g->addMinimalSlider("Z", -1000.0, 1000.0, &l->lightPos.z, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
            g->addSpacer();
        }
            break;
            
        case OF_LIGHT_SPOT:
        {
            g->addLabel("POSITION", OFX_UI_FONT_SMALL);
            g->addMinimalSlider("X", -1000.0, 1000.0, &l->lightPos.x, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
            g->addMinimalSlider("Y", -1000.0, 1000.0, &l->lightPos.y, length, dim)->setShowValue(false);
            g->addMinimalSlider("Z", -1000.0, 1000.0, &l->lightPos.z, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
            g->addSpacer();
            
            g->addLabel("ORIENTATION", OFX_UI_FONT_SMALL);
            g->addMinimalSlider("OX", -90.0, 90.0, &l->lightOrientation.x, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
            g->addMinimalSlider("OY", -90.0, 90.0, &l->lightOrientation.y, length, dim)->setShowValue(false);
            g->addMinimalSlider("OZ", -90.0, 90.0, &l->lightOrientation.z, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
            g->addSpacer();
        }
            break;
            
        case OF_LIGHT_DIRECTIONAL:
        {
            g->addLabel("ORIENTATION", OFX_UI_FONT_SMALL);
            g->addMinimalSlider("OX", -90.0, 90.0, &l->lightOrientation.x, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
            g->addMinimalSlider("OY", -90.0, 90.0, &l->lightOrientation.y, length, dim)->setShowValue(false);
            g->addMinimalSlider("OZ", -90.0, 90.0, &l->lightOrientation.z, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
            g->addSpacer();
        }
            break;
    }
    
    g->addLabel("AMBIENT", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("AH", 0.0, 1.0, &l->lightAmbientHSV.r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("AS", 0.0, 1.0, &l->lightAmbientHSV.g, length, dim)->setShowValue(false);
    g->addMinimalSlider("AV", 0.0, 1.0, &l->lightAmbientHSV.b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    g->addSpacer();
    g->addLabel("DIFFUSE", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("DH", 0.0, 1.0, &l->lightDiffuseHSV.r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("DS", 0.0, 1.0, &l->lightDiffuseHSV.g, length, dim)->setShowValue(false);
    g->addMinimalSlider("DV", 0.0, 1.0, &l->lightDiffuseHSV.b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    g->addSpacer();
    g->addLabel("SPECULAR", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("SH", 0.0, 1.0, &l->lightSpecularHSV.r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("SS", 0.0, 1.0, &l->lightSpecularHSV.g, length, dim)->setShowValue(false);
    g->addMinimalSlider("SV", 0.0, 1.0, &l->lightSpecularHSV.b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    g->addSpacer();
}

void CloudsVisualSystem::guiLightEvent(ofxUIEventArgs &e)
{
	if(e.widget->getName() == "AH" ||
	   e.widget->getName() == "AS" ||
	   e.widget->getName() == "AV")
	{
		light->updateColors();
		guimap[e.widget->getParent()->getName()]->getWidget("AH")->setColorFill(light->lightAmbient);
		guimap[e.widget->getParent()->getName()]->getWidget("AS")->setColorFill(light->lightAmbient);
		guimap[e.widget->getParent()->getName()]->getWidget("AV")->setColorFill(light->lightAmbient);
	}
	if(e.widget->getName() == "DH" ||
	   e.widget->getName() == "DS" ||
	   e.widget->getName() == "DV")
	{
		light->updateColors();
		guimap[e.widget->getParent()->getName()]->getWidget("DH")->setColorFill(light->lightDiffuse);
		guimap[e.widget->getParent()->getName()]->getWidget("DS")->setColorFill(light->lightDiffuse);
		guimap[e.widget->getParent()->getName()]->getWidget("DV")->setColorFill(light->lightDiffuse);
	}
	if(e.widget->getName() == "SH" ||
	   e.widget->getName() == "SS" ||
	   e.widget->getName() == "SV")
	{
		light->updateColors();
		guimap[e.widget->getParent()->getName()]->getWidget("SH")->setColorFill(light->lightSpecular);
		guimap[e.widget->getParent()->getName()]->getWidget("SS")->setColorFill(light->lightSpecular);
		guimap[e.widget->getParent()->getName()]->getWidget("SV")->setColorFill(light->lightSpecular);
	}
}

void CloudsVisualSystem::setupTimeline()
{
    if(timeline != NULL){
        delete timeline;
    }
	
	timeline = new ofxTimeline();
	timeline->setName("Working");
	timeline->setWorkingFolder(getVisualSystemDataPath()+"Presets/Working/Timeline/");
	
	timeline->setup();
	timeline->setShowInoutControl(true);
    timeline->setMinimalHeaders(true);
	timeline->setFrameBased(false);
	timeline->setSpacebarTogglePlay(false);
	timeline->setDurationInSeconds(60);
	timeline->setLoopType(OF_LOOP_NONE);
    timeline->setPageName(ofToUpper(getSystemName()));
	
	if(cameraTrack != NULL){
		cameraTrack->disable();
		delete cameraTrack;
	}
	cameraTrack = new ofxTLCameraTrack();
	cameraTrack->setCamera(getCameraRef());
	cameraTrack->setXMLFileName(getVisualSystemDataPath()+"Presets/Working/Timeline/cameraTrack.xml");
    timeline->addTrack("Camera", cameraTrack);
	introOutroTrack = timeline->addFlags("Intro-Outro", getVisualSystemDataPath()+"Presets/Working/Timeline/IntroOutro.xml");
	
    ofDirectory dir;
    string workingDirectoryName = getVisualSystemDataPath()+"Presets/Working/Timeline/";
    if(!dir.doesDirectoryExist(workingDirectoryName))
    {
        dir.createDirectory(workingDirectoryName);
    }
    
    timeline->setWorkingFolder(getVisualSystemDataPath()+"Presets/Working/Timeline/");
    ofAddListener(timeline->events().bangFired, this, &CloudsVisualSystem::timelineBangEvent);
	if(!bShowTimeline){
		timeline->hide();
	}
	
    selfSetupTimeline();
}

void CloudsVisualSystem::resetTimeline()
{
	if(timeline != NULL){
		ofRemoveListener(timeline->events().bangFired, this, &CloudsVisualSystem::timelineBangEvent);
		timeline->reset();
	}
	if(cameraTrack != NULL){
		cameraTrack->disable();
		cameraTrack->lockCameraToTrack = false;
		delete cameraTrack;
		cameraTrack = NULL;
	}
    setupTimeline();
}

void CloudsVisualSystem::timelineBangEvent(ofxTLBangEventArgs& args)
{
	
    if(bEnableTimeline)
    {
		if(args.track == introOutroTrack)
		{
			if(bTimelineIsIndefinite && args.flag == "middle"){
				timeline->stop();
			}
		}
		else{
			map<ofxTLBangs*, ofxUIButton*>::iterator it = tlButtonMap.find((ofxTLBangs *)args.track);
			if(it != tlButtonMap.end())
			{
				ofxUIButton *b = it->second;
				b->setValue(!b->getValue());
				b->triggerSelf();
				b->setValue(!b->getValue());
			}
		}
    }
}

void CloudsVisualSystem::setupTimelineGui()
{
    tlGui = new ofxUISuperCanvas("TIMELINE", gui);
    tlGui->copyCanvasStyle(gui);
    tlGui->copyCanvasProperties(gui);
    tlGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    tlGui->setName("TimelineSettings");
    tlGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
    ofxUIToggle *toggle = tlGui->addToggle("ENABLE", &bEnableTimeline);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    tlGui->resetPlacer();
    tlGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    tlGui->addWidgetToHeader(toggle);
    tlGui->addSpacer();
    
    durationLabel = tlGui->addLabel("");
    
    tlGui->addNumberDialer("DURATION", 0.0, 60*5, &timelineDuration, 0.0)->setDisplayLabel(true);
    tlGui->addToggle("INDEFINITE", &bTimelineIsIndefinite);
    
    tlGui->addToggle("ANIMATE", &bEnableTimelineTrackCreation);
    tlGui->addToggle("DELETE", &bDeleteTimelineTrack);
    
    //tlGui->addToggle("SHOW/HIDE", &bShowTimeline);
    
    selfSetupTimelineGui();
    tlGui->autoSizeToFitWidgets();
    ofAddListener(tlGui->newGUIEvent,this,&CloudsVisualSystem::guiTimelineEvent);
    guis.push_back(tlGui);
    guimap[tlGui->getName()] = tlGui;
}

void CloudsVisualSystem::guiTimelineEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if(name == "DURATION")
    {
//		cout << "****** TL duration changed " << timelineDuration << endl;
        timeline->setDurationInSeconds(timelineDuration);
		timelineDuration = timeline->getDurationInSeconds();
    }
    else if(name == "ANIMATE")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            setTimelineTrackDeletion(false);
        }
        setTimelineTrackCreation(t->getValue());
    }
    else if(name == "DELETE")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            setTimelineTrackCreation(false);
        }
        setTimelineTrackDeletion(t->getValue());
        
    }
    else if(name == "ENABLE")
    {
        if(bEnableTimeline)
        {
            if(bEnableTimelineTrackCreation)
            {
                setTimelineTrackCreation(false);
            }
            if(bDeleteTimelineTrack)
            {
                setTimelineTrackDeletion(false);
            }
        }
    }
//    else if(name == "SHOW/HIDE")
//    {
//        if(bShowTimeline)
//        {
//            timeline->show();
//        }
//        else
//        {
//            timeline->hide();
//        }
//    }
}

void CloudsVisualSystem::setTimelineTrackDeletion(bool state)
{
    bDeleteTimelineTrack = state;
    if(bDeleteTimelineTrack)
    {
        bEnableTimeline = false;
        for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
        {
            if((*it)->getName() != "TimelineSettings")
            {
                ofAddListener((*it)->newGUIEvent,this,&CloudsVisualSystem::guiAllEvents);
            }
        }
        bEnableTimeline = true;
    }
    else
    {
        for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
        {
            if((*it)->getName() != "TimelineSettings")
            {
                ofRemoveListener((*it)->newGUIEvent,this,&CloudsVisualSystem::guiAllEvents);
            }
        }
    }
}

void CloudsVisualSystem::setTimelineTrackCreation(bool state)
{
    bEnableTimelineTrackCreation = state;
    if(bEnableTimelineTrackCreation)
    {
        bEnableTimeline = false;
        for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
        {
            if((*it)->getName() != "TimelineSettings")
            {
                ofAddListener((*it)->newGUIEvent,this,&CloudsVisualSystem::guiAllEvents);
            }
        }
        bEnableTimeline = true;
    }
    else
    {
        for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
        {
            if((*it)->getName() != "TimelineSettings")
            {
                ofRemoveListener((*it)->newGUIEvent,this,&CloudsVisualSystem::guiAllEvents);
            }
        }
    }
}

void CloudsVisualSystem::guiAllEvents(ofxUIEventArgs &e)
{
    //All GUIS except for the Timeline UI will send events to this function
    if(bEnableTimelineTrackCreation)
    {
        bindWidgetToTimeline(e.widget);
        setTimelineTrackCreation(false);
    }
    
    if(bDeleteTimelineTrack)
    {
        unBindWidgetFromTimeline(e.widget);
        setTimelineTrackDeletion(false);
    }
}

void CloudsVisualSystem::bindWidgetToTimeline(ofxUIWidget* widget)
{
    string parentName = ((ofxUISuperCanvas *) widget->getCanvasParent())->getCanvasTitle()->getLabel();
    timeline->addPage(parentName, true);
    
    timeline->setCurrentPage(parentName);
    
    vector<ofxTLPage *> pages = timeline->getPages();
    
    ofxTLPage * currentPage = NULL;
    for(vector<ofxTLPage *>::iterator it = pages.begin(); it != pages.end(); ++it)
    {
        if((*it)->getName() == parentName)
        {
            currentPage = (*it);
        }
    }
	
    if(currentPage != NULL)
    {
        if(currentPage->getTrack(widget->getName()) != NULL)
        {
            return;
        }
    }
    
    switch (widget->getKind())
    {
        case OFX_UI_WIDGET_BUTTON:
        case OFX_UI_WIDGET_LABELBUTTON:
        case OFX_UI_WIDGET_IMAGEBUTTON:
        case OFX_UI_WIDGET_MULTIIMAGEBUTTON:
        {
            ofxUIButton *b = (ofxUIButton *) widget;
            tlButtonMap[timeline->addBangs(widget->getName())] = b;
        }
            break;
            
        case OFX_UI_WIDGET_TOGGLE:
        case OFX_UI_WIDGET_LABELTOGGLE:
        case OFX_UI_WIDGET_IMAGETOGGLE:
        case OFX_UI_WIDGET_MULTIIMAGETOGGLE:
        {
            ofxUIToggle *t = (ofxUIToggle *) widget;
            tlToggleMap[t] = timeline->addSwitches(widget->getName(),
												   widget->getCanvasParent()->getName() + "_" + widget->getName() + ".xml");
        }
            break;
            
        case OFX_UI_WIDGET_NUMBERDIALER:
        {
            ofxUINumberDialer *nd = (ofxUINumberDialer *) widget;
            tlDialerMap[nd] = timeline->addCurves(widget->getName(),
												  widget->getCanvasParent()->getName() + "_" + widget->getName() + ".xml",
												  ofRange(nd->getMin(), nd->getMax()), nd->getValue());
        }
            break;
            
        case OFX_UI_WIDGET_BILABELSLIDER:
        case OFX_UI_WIDGET_CIRCLESLIDER:
        case OFX_UI_WIDGET_MULTIIMAGESLIDER_H:
        case OFX_UI_WIDGET_MULTIIMAGESLIDER_V:
        case OFX_UI_WIDGET_IMAGESLIDER_H:
        case OFX_UI_WIDGET_IMAGESLIDER_V:
        case OFX_UI_WIDGET_ROTARYSLIDER:
        case OFX_UI_WIDGET_MINIMALSLIDER:
        case OFX_UI_WIDGET_SLIDER_H:
        case OFX_UI_WIDGET_SLIDER_V:
        {
            ofxUISlider *s = (ofxUISlider *) widget;
            tlSliderMap[s] = timeline->addCurves(widget->getName(),
												 widget->getCanvasParent()->getName() + "_" + widget->getName() + ".xml",
												 ofRange(s->getMin(), s->getMax()), s->getValue());
        }
            break;
        default:
            break;
    }

}

void CloudsVisualSystem::unBindWidgetFromTimeline(ofxUIWidget* widget)
{
    string parentName = ((ofxUISuperCanvas *) widget->getCanvasParent())->getCanvasTitle()->getLabel();
    timeline->setCurrentPage(parentName);
    
    if(!timeline->hasTrack(widget->getName()))
    {
        return;
    }
    
    
    
    switch (widget->getKind())
    {
        case OFX_UI_WIDGET_BUTTON:
        case OFX_UI_WIDGET_LABELBUTTON:
        case OFX_UI_WIDGET_IMAGEBUTTON:
        case OFX_UI_WIDGET_MULTIIMAGEBUTTON:
        {
            ofxTLBangs *track = (ofxTLBangs *) timeline->getTrack(widget->getName());
            map<ofxTLBangs *, ofxUIButton *>::iterator it = tlButtonMap.find(track);
            
            if(it != tlButtonMap.end())
            {
                timeline->removeTrack(it->first);
                tlButtonMap.erase(it);
            }
        }
            break;
            
        case OFX_UI_WIDGET_TOGGLE:
        case OFX_UI_WIDGET_LABELTOGGLE:
        case OFX_UI_WIDGET_IMAGETOGGLE:
        case OFX_UI_WIDGET_MULTIIMAGETOGGLE:
        {
            ofxUIToggle *t = (ofxUIToggle *) widget;
            map<ofxUIToggle *, ofxTLSwitches *>::iterator it = tlToggleMap.find(t);
            
            if(it != tlToggleMap.end())
            {
                timeline->removeTrack(it->second);
                tlToggleMap.erase(it);
            }
        }
            break;
            
        case OFX_UI_WIDGET_NUMBERDIALER:
        {
            ofxUINumberDialer *nd = (ofxUINumberDialer *) widget;
            map<ofxUINumberDialer *, ofxTLCurves *>::iterator it = tlDialerMap.find(nd);
            if(it != tlDialerMap.end())
            {
                timeline->removeTrack(it->second);
                tlDialerMap.erase(it);
            }
        }
            break;
            
        case OFX_UI_WIDGET_BILABELSLIDER:
        case OFX_UI_WIDGET_CIRCLESLIDER:
        case OFX_UI_WIDGET_MULTIIMAGESLIDER_H:
        case OFX_UI_WIDGET_MULTIIMAGESLIDER_V:
        case OFX_UI_WIDGET_IMAGESLIDER_H:
        case OFX_UI_WIDGET_IMAGESLIDER_V:
        case OFX_UI_WIDGET_ROTARYSLIDER:
        case OFX_UI_WIDGET_MINIMALSLIDER:
        case OFX_UI_WIDGET_SLIDER_H:
        case OFX_UI_WIDGET_SLIDER_V:
        {
            ofxUISlider *s = (ofxUISlider *) widget;
            map<ofxUISlider *, ofxTLCurves *>::iterator it = tlSliderMap.find(s);
            if(it != tlSliderMap.end())
            {
                timeline->removeTrack(it->second);
                tlSliderMap.erase(it);
            }
        }
            break;
        default:
            break;
    }
}

void CloudsVisualSystem::updateTimelineUIParams()
{
    for(map<ofxUIToggle*, ofxTLSwitches*>::iterator it = tlToggleMap.begin(); it != tlToggleMap.end(); ++it)
    {
        ofxUIToggle *t = it->first;
        ofxTLSwitches *tls = it->second;
        if(tls->isOn() != t->getValue())
        {
            t->setValue(tls->isOn());
            t->triggerSelf();
        }
    }
    
    for(map<ofxUISlider*, ofxTLCurves*>::iterator it = tlSliderMap.begin(); it != tlSliderMap.end(); ++it)
    {
        ofxUISlider *s = it->first;
        ofxTLCurves *tlc = it->second;
        s->setValue(tlc->getValue());
        s->triggerSelf();
    }
    
    for(map<ofxUINumberDialer*, ofxTLCurves*>::iterator it = tlDialerMap.begin(); it != tlDialerMap.end(); ++it)
    {
        ofxUINumberDialer *nd = it->first;
        ofxTLCurves *tlc = it->second;
        nd->setValue(tlc->getValue());
        nd->triggerSelf();
    }
}

void CloudsVisualSystem::saveTimelineUIMappings(string path)
{
    if(ofFile::doesFileExist(path))
    {
		//        cout << "DELETING OLD MAPPING FILE" << endl;
        ofFile::removeFile(path);
    }
	//    cout << "TIMELINE UI MAPPER SAVING" << endl;
    ofxXmlSettings *XML = new ofxXmlSettings(path);
    XML->clear();
    
    int mapIndex = XML->addTag("Map");
    XML->pushTag("Map", mapIndex);
    
    int bangsIndex = XML->addTag("Bangs");
    if(XML->pushTag("Bangs", bangsIndex))
    {
        for(map<ofxTLBangs*, ofxUIButton*>::iterator it = tlButtonMap.begin(); it != tlButtonMap.end(); ++it)
        {
            int index = XML->addTag("Mapping");
            if(XML->pushTag("Mapping", index))
            {
                int wIndex = XML->addTag("Widget");
                if(XML->pushTag("Widget", wIndex))
                {
                    XML->setValue("WidgetName", it->second->getName(), 0);
                    XML->setValue("WidgetID", it->second->getID(), 0);
                    XML->setValue("WidgetCanvasParent", it->second->getCanvasParent()->getName(), 0);
                    XML->popTag();
                }
                int tlIndex = XML->addTag("Track");
                if(XML->pushTag("Track", tlIndex))
                {
                    XML->popTag();
                }
                XML->popTag();
            }
        }
        XML->popTag();
    }
    
    int switchesIndex = XML->addTag("Switches");
    if(XML->pushTag("Switches", switchesIndex))
    {
        for(map<ofxUIToggle*, ofxTLSwitches*>::iterator it = tlToggleMap.begin(); it != tlToggleMap.end(); ++it)
        {
            int index = XML->addTag("Mapping");
            if(XML->pushTag("Mapping", index))
            {
                int wIndex = XML->addTag("Widget");
                if(XML->pushTag("Widget", wIndex))
                {
                    XML->setValue("WidgetName", it->first->getName(), 0);
                    XML->setValue("WidgetID", it->first->getID(), 0);
                    XML->setValue("WidgetCanvasParent", it->first->getCanvasParent()->getName(), 0);
                    XML->popTag();
                }
                int tlIndex = XML->addTag("Track");
                if(XML->pushTag("Track", tlIndex))
                {
                    XML->popTag();
                }
                XML->popTag();
            }
        }
        XML->popTag();
    }
    
    int sliderCurvesIndex = XML->addTag("SliderCurves");
    if(XML->pushTag("SliderCurves", sliderCurvesIndex))
    {
        for(map<ofxUISlider*, ofxTLCurves*>::iterator it = tlSliderMap.begin(); it != tlSliderMap.end(); ++it)
        {
            int index = XML->addTag("Mapping");
            if(XML->pushTag("Mapping", index))
            {
                int wIndex = XML->addTag("Widget");
                if(XML->pushTag("Widget", wIndex))
                {
                    XML->setValue("WidgetName", it->first->getName(), 0);
                    XML->setValue("WidgetID", it->first->getID(), 0);
                    XML->setValue("WidgetCanvasParent", it->first->getCanvasParent()->getName(), 0);
                    XML->popTag();
                }
                int tlIndex = XML->addTag("Track");
                if(XML->pushTag("Track", tlIndex))
                {
                    XML->popTag();
                }
                XML->popTag();
            }
        }
        XML->popTag();
    }
    
    int numberDialerCurvesIndex = XML->addTag("NumberDialerCurves");
    if(XML->pushTag("NumberDialerCurves", numberDialerCurvesIndex))
    {
        for(map<ofxUINumberDialer*, ofxTLCurves*>::iterator it = tlDialerMap.begin(); it != tlDialerMap.end(); ++it)
        {
            int index = XML->addTag("Mapping");
            if(XML->pushTag("Mapping", index))
            {
                int wIndex = XML->addTag("Widget");
                if(XML->pushTag("Widget", wIndex))
                {
                    XML->setValue("WidgetName", it->first->getName(), 0);
                    XML->setValue("WidgetID", it->first->getID(), 0);
                    XML->setValue("WidgetCanvasParent", it->first->getCanvasParent()->getName(), 0);
                    XML->popTag();
                }
                int tlIndex = XML->addTag("Track");
                if(XML->pushTag("Track", tlIndex))
                {
                    XML->popTag();
                }
                XML->popTag();
            }
        }
        XML->popTag();
    }
    
    XML->popTag();
    XML->saveFile(path);
    delete XML;
}

void CloudsVisualSystem::loadTimelineUIMappings(string path)
{
    tlButtonMap.clear();
    tlToggleMap.clear();
    tlSliderMap.clear();
    tlDialerMap.clear();
    
	//    cout << "LOADING TIMELINE UI MAPPINGS" << endl;
    ofxXmlSettings *XML = new ofxXmlSettings();
    XML->loadFile(path);
    if(XML->tagExists("Map",0) && XML->pushTag("Map", 0))
    {
        if(XML->pushTag("Bangs", 0))
        {
            int widgetTags = XML->getNumTags("Mapping");
            for(int i = 0; i < widgetTags; i++)
            {
                if(XML->pushTag("Mapping", i))
                {
                    if(XML->pushTag("Widget", 0))
                    {
                        string widgetname = XML->getValue("WidgetName", "NULL", 0);
                        int widgetID = XML->getValue("WidgetID", -1, 0);
                        string widgetCanvasParent = XML->getValue("WidgetCanvasParent", "NULL", 0);
                        map<string, ofxUICanvas *>::iterator it = guimap.find(widgetCanvasParent);
                        if(it != guimap.end())
                        {
                            ofxUIWidget *w = it->second->getWidget(widgetname, widgetID);
                            if(w != NULL)
                            {
                                bindWidgetToTimeline(w);
                            }
                        }
                        XML->popTag();
                    }
                    XML->popTag();
                }
            }
            XML->popTag();
        }
        
        if(XML->pushTag("Switches", 0))
        {
            int widgetTags = XML->getNumTags("Mapping");
            for(int i = 0; i < widgetTags; i++)
            {
                if(XML->pushTag("Mapping", i))
                {
                    if(XML->pushTag("Widget", 0))
                    {
                        string widgetname = XML->getValue("WidgetName", "NULL", 0);
                        int widgetID = XML->getValue("WidgetID", -1, 0);
                        string widgetCanvasParent = XML->getValue("WidgetCanvasParent", "NULL", 0);
                        map<string, ofxUICanvas *>::iterator it = guimap.find(widgetCanvasParent);
                        if(it != guimap.end())
                        {
                            ofxUIWidget *w = it->second->getWidget(widgetname, widgetID);
                            if(w != NULL)
                            {
                                bindWidgetToTimeline(w);
                            }
                        }
                        XML->popTag();
                    }
                    XML->popTag();
                }
            }
            XML->popTag();
        }
        
        if(XML->pushTag("SliderCurves", 0))
        {
            int widgetTags = XML->getNumTags("Mapping");
            for(int i = 0; i < widgetTags; i++)
            {
                if(XML->pushTag("Mapping", i))
                {
                    if(XML->pushTag("Widget", 0))
                    {
                        string widgetname = XML->getValue("WidgetName", "NULL", 0);
                        int widgetID = XML->getValue("WidgetID", -1, 0);
                        string widgetCanvasParent = XML->getValue("WidgetCanvasParent", "NULL", 0);
                        map<string, ofxUICanvas *>::iterator it = guimap.find(widgetCanvasParent);
                        if(it != guimap.end())
                        {
                            ofxUIWidget *w = it->second->getWidget(widgetname, widgetID);
                            if(w != NULL)
                            {
                                bindWidgetToTimeline(w);
                            }
                        }
                        XML->popTag();
                    }
                    XML->popTag();
                }
            }
            XML->popTag();
        }
        
        if(XML->pushTag("NumberDialerCurves", 0))
        {
            int widgetTags = XML->getNumTags("Mapping");
            for(int i = 0; i < widgetTags; i++)
            {
                if(XML->pushTag("Mapping", i))
                {
                    if(XML->pushTag("Widget", 0))
                    {
                        string widgetname = XML->getValue("WidgetName", "NULL", 0);
                        int widgetID = XML->getValue("WidgetID", -1, 0);
                        string widgetCanvasParent = XML->getValue("WidgetCanvasParent", "NULL", 0);
                        map<string, ofxUICanvas *>::iterator it = guimap.find(widgetCanvasParent);
                        if(it != guimap.end())
                        {
                            ofxUIWidget *w = it->second->getWidget(widgetname, widgetID);
                            if(w != NULL)
                            {
                                bindWidgetToTimeline(w);
                            }
                        }
                        XML->popTag();
                    }
                    XML->popTag();
                }
            }
            XML->popTag();
        }
        XML->popTag();
    }
	
	timeline->setCurrentPage(0);
}

#ifdef KINECT_INPUT
void CloudsVisualSystem::setupKinectGui()
{
    kinectGui = new ofxUISuperCanvas("KINECT", gui);
    kinectGui->copyCanvasStyle(gui);
    kinectGui->copyCanvasProperties(gui);
    kinectGui->setName("Kinect");
    kinectGui->setPosition(guis[guis.size() - 1]->getRect()->x + guis[guis.size() - 1]->getRect()->getWidth() + 1, 0);
    kinectGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofPtr<CloudsInputKinectOSC> kinectInput = dynamic_pointer_cast<CloudsInputKinectOSC>(GetCloudsInput());
    
    kinectGui->addSpacer();
    kinectGui->addToggle("DEBUG", &kinectInput->bDoDebug);
    
    kinectGui->addSpacer();
    kinectGui->addRangeSlider("BODY RANGE X", -1.0f, 1.0f, &kinectInput->boundsMin.x, &kinectInput->boundsMax.x);
    kinectGui->addRangeSlider("BODY RANGE Y", -1.0f, 1.0f, &kinectInput->boundsMin.y, &kinectInput->boundsMax.y);
    kinectGui->addRangeSlider("BODY RANGE Z",  0.5f, 4.5f, &kinectInput->boundsMin.z, &kinectInput->boundsMax.z);
    
    kinectGui->addSpacer();
    kinectGui->addSlider("ACTIVE THRESHOLD Y", 0.0f, 1.0f, &kinectInput->activeThresholdY);
    kinectGui->addSlider("ACTIVE THRESHOLD Z", 0.0f, 1.0f, &kinectInput->activeThresholdZ);
    
    kinectGui->addSpacer();
    kinectGui->addSlider("RESET LERP", 0, 1, &kinectInput->posResetLerpPct);
    kinectGui->addSlider("MOVE LERP", 0, 1, &kinectInput->posSetLerpPct);
    kinectGui->addSlider("MOVE THRESHOLD", 0, 100, &kinectInput->posSetInstantThreshold);
    kinectGui->addIntSlider("OUT OF BOUNDS DELAY", 0, 5000, &kinectInput->posOutOfBoundsDelay);
    
    kinectGui->autoSizeToFitWidgets();
    ofAddListener(kinectGui->newGUIEvent, this, &CloudsVisualSystem::guiKinectEvent);
    guis.push_back(kinectGui);
    guimap[kinectGui->getName()] = kinectGui;
}

void CloudsVisualSystem::guiKinectEvent(ofxUIEventArgs &e)
{

}
#endif

#ifdef OCULUS_RIFT
void CloudsVisualSystem::setupOculusGui()
{
    oculusGui = new ofxUISuperCanvas("OCULUS RIFT", gui);
    oculusGui->copyCanvasStyle(gui);
    oculusGui->copyCanvasProperties(gui);
    oculusGui->setName("OculusRift");
    oculusGui->setPosition(guis[guis.size() - 1]->getRect()->x + guis[guis.size() - 1]->getRect()->getWidth() + 1, 0);
    oculusGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    oculusGui->autoSizeToFitWidgets();
    ofAddListener(oculusGui->newGUIEvent, this, &CloudsVisualSystem::guiOculusEvent);
    guis.push_back(oculusGui);
    guimap[oculusGui->getName()] = oculusGui;
}

void CloudsVisualSystem::guiOculusEvent(ofxUIEventArgs &e)
{
    
}
#endif

void CloudsVisualSystem::lightsBegin()
{
    ofSetSmoothLighting(bSmoothLighting);
    if(bEnableLights)
    {
        for(map<string, ofxLight *>::iterator it = lights.begin(); it != lights.end(); ++it)
        {
            ofEnableLighting();
            it->second->enable();
        }
    }
}

void CloudsVisualSystem::lightsEnd()
{
    if(!bEnableLights)
    {
        ofDisableLighting();
        for(map<string, ofxLight *>::iterator it = lights.begin(); it != lights.end(); ++it)
        {
            it->second->disable();
        }
    }
}

void CloudsVisualSystem::loadGUIS()
{

    for(int i = 0; i < guis.size(); i++)
    {
#ifdef KINECT_INPUT
        if (guis[i] == kinectGui) continue;
#endif
#ifdef OCULUS_RIFT
        if (guis[i] == oculusGui) continue;
#endif
        guis[i]->loadSettings(getVisualSystemDataPath()+"Presets/Working/"+guis[i]->getName()+".xml");
		guis[i]->setColorBack(ofColor(255*.2, 255*.9));
//        setColors();
//        guis[i]->setTheme(OFX_UI_THEME_ZOOLANDER);
    }
    cam.reset();
    ofxLoadCamera(cam, getVisualSystemDataPath()+"Presets/Working/"+"ofEasyCamSettings");
    resetTimeline();
    loadTimelineUIMappings(getVisualSystemDataPath()+"Presets/Working/UITimelineMappings.xml");
    timeline->loadTracksFromFolder(getVisualSystemDataPath()+"Presets/Working/Timeline/");

#ifdef KINECT_INPUT
    kinectGui->loadSettings(GetCloudsDataPath()+kinectGui->getName()+".xml");
#endif
#ifdef OCULUS_RIFT
    oculusGui->loadSettings(GetCloudsDataPath()+oculusGui->getName()+".xml");
#endif
}

void CloudsVisualSystem::saveGUIS()
{
    for(int i = 0; i < guis.size(); i++)
    {
#ifdef KINECT_INPUT
        if (guis[i] == kinectGui) continue;
#endif
#ifdef OCULUS_RIFT
        if (guis[i] == oculusGui) continue;
#endif
        guis[i]->saveSettings(getVisualSystemDataPath()+"Presets/Working/"+guis[i]->getName()+".xml");
    }
    ofxSaveCamera(cam, getVisualSystemDataPath()+"Presets/Working/"+"ofEasyCamSettings");
    
    saveTimelineUIMappings(getVisualSystemDataPath()+"Presets/Working/UITimelineMappings.xml");
    if(timeline != NULL){
		timeline->saveTracksToFolder(getVisualSystemDataPath()+"Presets/Working/Timeline/");
	}
    
#ifdef KINECT_INPUT
    kinectGui->saveSettings(GetCloudsDataPath()+kinectGui->getName()+".xml");
#endif
#ifdef OCULUS_RIFT
    oculusGui->saveSettings(GetCloudsDataPath()+oculusGui->getName()+".xml");
#endif
}

void CloudsVisualSystem::loadPresetGUISFromName(string presetName)
{
    
	loadPresetGUISFromPath(getVisualSystemDataPath()+"Presets/"+ presetName);
}

void CloudsVisualSystem::loadPresetGUISFromPath(string presetPath)
{
    
	resetTimeline();
	
	selfSetDefaults();
	
    for(int i = 0; i < guis.size(); i++) {
#ifdef KINECT_INPUT
        if (guis[i] == kinectGui) continue;
#endif
#ifdef OCULUS_RIFT
        if (guis[i] == oculusGui) continue;
#endif
		string presetPathName = presetPath+"/"+guis[i]->getName()+".xml";
        guis[i]->loadSettings(presetPathName);
    }
	
    cam.reset();
	string easyCamPath = presetPath+"/ofEasyCamSettings";
	if(ofFile(easyCamPath).exists()){
		ofxLoadCamera(cam, easyCamPath);
	}
	
    loadTimelineUIMappings(presetPath+"/UITimelineMappings.xml");
	timeline->setName( ofFilePath::getBaseName( presetPath ) );
    timeline->loadTracksFromFolder(presetPath+"/Timeline/");
	timeline->setName("Working");
    timeline->saveTracksToFolder(getVisualSystemDataPath()+"Presets/Working/Timeline/");
	if(timelineDuration <= 0){
		timelineDuration = 60;
	}
	timeline->setDurationInSeconds(timelineDuration);
	timelineDuration = timeline->getDurationInSeconds();
		
	if(bTimelineIsIndefinite){
		timeline->setLoopType(OF_LOOP_NORMAL);
	}
	else{
		timeline->setLoopType(OF_LOOP_NONE);
	}

	stackGuiWindows();

	selfPresetLoaded(presetPath);
	currentPresetName = ofFilePath::getBaseName(presetPath);

		
	//auto play this preset
	cameraTrack->lockCameraToTrack = cameraTrack->getKeyframes().size() > 0;
	timeline->setCurrentTimeMillis(0);
	timeline->play();
	
	mat->updateColors();
	light->updateColors();
	
	bEnableTimeline = true;
}

void CloudsVisualSystem::savePresetGUIS(string presetName)
{
    ofDirectory dir;
	presetName = trim(presetName);
    string presetDirectory = getVisualSystemDataPath()+"Presets/"+presetName+"/";
    if(!dir.doesDirectoryExist(presetDirectory))
    {
        dir.createDirectory(presetDirectory);
        presetRadio->addToggle(presetGui->addToggle(presetName, true));
        presetGui->autoSizeToFitWidgets();
    }
    
    for(int i = 0; i < guis.size(); i++)
    {
#ifdef KINECT_INPUT
        if (guis[i] == kinectGui) continue;
#endif
#ifdef OCULUS_RIFT
        if (guis[i] == oculusGui) continue;
#endif
        guis[i]->saveSettings(presetDirectory+guis[i]->getName()+".xml");
    }
    ofxSaveCamera(cam, getVisualSystemDataPath()+"Presets/"+presetName+"/ofEasyCamSettings");
	
//	cout << "before save range " << timeline->getInOutRange() << endl;
	
    saveTimelineUIMappings(getVisualSystemDataPath()+"Presets/"+presetName+"/UITimelineMappings.xml");
	timeline->setName(presetName);
    timeline->saveTracksToFolder(getVisualSystemDataPath()+"Presets/"+presetName+"/Timeline/");
	
	
//	cout << "after save range " << timeline->getInOutRange() << endl;
	timeline->setName("Working");
    timeline->saveTracksToFolder(getVisualSystemDataPath()+"Presets/Working/Timeline/");

	ofxXmlSettings timeInfo;
	timeInfo.addTag("timeinfo");
	timeInfo.pushTag("timeinfo");
	timeInfo.addValue("indefinite", bTimelineIsIndefinite);
	timeInfo.addValue("duration", timeline->getInOutRange().span() * timeline->getDurationInSeconds());
	timeInfo.addValue("introDuration", getIntroDuration());
	timeInfo.addValue("outroDuration", getOutroDuration());
	timeInfo.popTag();//timeinfo
	timeInfo.saveFile(getVisualSystemDataPath()+"Presets/"+presetName+"/TimeInfo.xml");
    
    //Add auto refresh file
    ofBuffer refreshFlag;
    refreshFlag.append("refreshme");
    ofBufferToFile(getVisualSystemDataPath()+"Presets/Working/_refreshme.txt", refreshFlag);
}

void CloudsVisualSystem::deleteGUIS()
{
	
    ofRemoveListener(gui->newGUIEvent,this,&CloudsVisualSystem::guiEvent);
    ofRemoveListener(sysGui->newGUIEvent,this,&CloudsVisualSystem::guiSystemEvent);
    ofRemoveListener(bgGui->newGUIEvent, this, &CloudsVisualSystem::guiBackgroundEvent);
    ofRemoveListener(lgtGui->newGUIEvent,this,&CloudsVisualSystem::guiLightingEvent);
    ofRemoveListener(camGui->newGUIEvent,this,&CloudsVisualSystem::guiCameraEvent);
    ofRemoveListener(presetGui->newGUIEvent,this,&CloudsVisualSystem::guiPresetEvent);
	for(map<string, ofxUISuperCanvas *>::iterator it = lightGuis.begin(); it != lightGuis.end(); ++it)
	{
		ofRemoveListener(it->second->newGUIEvent,this,&CloudsVisualSystem::guiLightEvent);
	}
#ifdef KINECT_INPUT
    ofRemoveListener(kinectGui->newGUIEvent, this, &CloudsVisualSystem::guiKinectEvent);
#endif
#ifdef OCULUS_RIFT
    ofRemoveListener(oculusGui->newGUIEvent, this, &CloudsVisualSystem::guiOculusEvent);
#endif
	
    for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
    {
        ofxUICanvas *g = (*it);
        delete g;
    }
	
    guis.clear();
	guimap.clear();
	lightGuis.clear();
	materialGuis.clear();

}

void CloudsVisualSystem::showGUIS()
{
    for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
    {
        (*it)->enable();
    }
	timeline->show();
}

void CloudsVisualSystem::hideGUIS()
{
    for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
    {
        (*it)->disable();
    }
	
	timeline->hide();	
}

void CloudsVisualSystem::toggleGUIS()
{
    for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
    {
        (*it)->toggleVisible();
    }
	timeline->toggleShow();
	bShowTimeline = timeline->getIsShowing();
}

void CloudsVisualSystem::toggleGuiAndPosition(ofxUISuperCanvas *g)
{
    if(g->isMinified())
    {
        g->setMinified(false);
        g->setPosition(ofGetMouseX(), ofGetMouseY());
    }
    else
    {
        g->setMinified(true);
    }
}

ofCamera& CloudsVisualSystem::getCameraRef(){
	return cam;
}

void CloudsVisualSystem::setDrawToScreen( bool state )
{
	bDrawToScreen = state;
}


float CloudsVisualSystem::getCurrentAudioAmplitude(){
#if (AVF_PLAYER && __MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_7)
	return getRGBDVideoPlayer().getPlayer().getAmplitude();
#else
	return 0;
#endif
	
}

bool CloudsVisualSystem::getDrawToScreen()
{
	return bDrawToScreen;
}

void CloudsVisualSystem::drawDebug()
{
    if(bDebug)
    {
		ofPushStyle();
        float color = 255-bgBri;
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        drawGrid(-debugGridSize,-debugGridSize,debugGridSize*2,debugGridSize*2, color);
        drawAxis(debugGridSize, color);
        selfDrawDebug();
		ofPopStyle();
    }
}

void CloudsVisualSystem::drawAxis(float size, float color)
{
    ofSetColor(color, 100);
    ofLine(-size, 0, 0, size, 0, 0);
    ofLine(0, -size, 0, 0, size, 0);
    ofLine(0, 0, -size, 0, 0, size);
}

void CloudsVisualSystem::drawGrid(float x, float y, float w, float h, float color)
{
    ofNoFill();
    ofSetColor(color, 25);
    ofRect(x, y, w, h);
    
    float md = MAX(w,h);
    ofSetColor(color, 50);
    for(float i = 0; i <= w; i+=md*.025)
    {
        ofLine(x+i, y, x+i, y+h);
    }
    for(float j = 0; j <= h; j+=md*.025)
    {
        ofLine(x, y+j, x+w, y+j);
    }
}

void CloudsVisualSystem::billBoard(ofVec3f globalCamPosition, ofVec3f globelObjectPosition)
{
    ofVec3f objectLookAt = ofVec3f(0,0,1);
    ofVec3f objToCam = globalCamPosition - globelObjectPosition;
    objToCam.normalize();
    float theta = objectLookAt.angle(objToCam);
    ofVec3f axisOfRotation = objToCam.crossed(objectLookAt);
    axisOfRotation.normalize();
    glRotatef(-theta, axisOfRotation.x, axisOfRotation.y, axisOfRotation.z);
}

void CloudsVisualSystem::drawBackground()
{
	
	drawBackgroundGradient();
	
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(0, ofGetHeight());
	ofScale(1,-1,1);
	selfDrawBackground();
	checkOpenGLError(getSystemName() + ":: DRAW BACKGROUND");		
	ofPopMatrix();
	ofPopStyle();
}

void CloudsVisualSystem::drawBackgroundGradient(){

	ofPushStyle();

	ofEnableAlphaBlending();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	ofSetGlobalAmbientColor(ofColor(0,0,0));

	if(bClearBackground)
	{
		
		if(gradientMode != -1){
			if(bBarGradient){
				if(backgroundGradientBar.isAllocated()){
					backgroundShader.begin();
					backgroundShader.setUniformTexture("image", backgroundGradientBar, 1);
					backgroundShader.setUniform3f("colorOne", bgColor.r/255., bgColor.g/255., bgColor.b/255.);
					backgroundShader.setUniform3f("colorTwo", bgColor2.r/255., bgColor2.g/255., bgColor2.b/255.);
					ofMesh mesh;
                    getBackgroundMesh(mesh, backgroundGradientCircle, ofGetViewportWidth(), ofGetViewportHeight());
					//getBackgroundMesh(mesh, backgroundGradientCircle, ofGetWidth(), ofGetHeight());
					mesh.draw();
					backgroundShader.end();
				}
				else{
					ofSetSmoothLighting(true);
					ofBackgroundGradient(bgColor, bgColor2, OF_GRADIENT_BAR);
				}
			}
			else{
				if(backgroundGradientCircle.isAllocated()){
					backgroundShader.begin();
					backgroundShader.setUniformTexture("image", backgroundGradientCircle, 1);
					backgroundShader.setUniform3f("colorOne", bgColor.r/255., bgColor.g/255., bgColor.b/255.);
					backgroundShader.setUniform3f("colorTwo", bgColor2.r/255., bgColor2.g/255., bgColor2.b/255.);
					ofMesh mesh;
                    getBackgroundMesh(mesh, backgroundGradientCircle, ofGetViewportWidth(), ofGetViewportHeight());
					//getBackgroundMesh(mesh, backgroundGradientCircle, ofGetWidth(), ofGetHeight());
					mesh.draw();
					backgroundShader.end();
				}
				else{
					ofSetSmoothLighting(true);
					ofBackgroundGradient(bgColor, bgColor2, OF_GRADIENT_CIRCULAR);
				}
			}
		}
		else{
			ofSetSmoothLighting(false);
			ofBackground(bgColor);
		}
	}
	ofPopStyle();
}

void CloudsVisualSystem::ofLayerGradient(const ofColor& start, const ofColor& end)
{
    float w = cam.getDistance()*bgAspectRatio;
    float h = cam.getDistance()*bgAspectRatio;
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    // this could be optimized by building a single mesh once, then copying
    // it and just adding the colors whenever the function is called.
    ofVec2f center(0.0,0.0);
    mesh.addVertex(center);
    mesh.addColor(start);
    int n = 32; // circular gradient resolution
    float angleBisector = TWO_PI / (n * 2);
    float smallRadius = ofDist(0, 0, w / 2, h / 2);
    float bigRadius = smallRadius / cos(angleBisector);
    for(int i = 0; i <= n; i++) {
        float theta = i * TWO_PI / n;
        mesh.addVertex(center + ofVec2f(sin(theta), cos(theta)) * bigRadius);
        mesh.addColor(end);
    }
    glDepthMask(false);
    mesh.draw();
    glDepthMask(true);
}

void CloudsVisualSystem::selfSetDefaults(){
	
}

void CloudsVisualSystem::selfSetup()
{
    
}

void CloudsVisualSystem::selfPresetLoaded(string presetPath)
{
	
}

void CloudsVisualSystem::selfSetupGuis()
{
    
}

void CloudsVisualSystem::selfUpdate()
{
    
}

void CloudsVisualSystem::selfDrawBackground()
{
    
}

void CloudsVisualSystem::selfDrawDebug()
{
    
}

void CloudsVisualSystem::selfSceneTransformation()
{
    
}


ofVec3f CloudsVisualSystem::getCameraPosition()
{
	return getCameraRef().getPosition();
    
}

void CloudsVisualSystem::selfDraw()
{
	
}

void CloudsVisualSystem::selfDrawOverlay(){
	
}

void CloudsVisualSystem::selfPostDraw(){
	
	glDisable(GL_LIGHTING);
	
#ifdef OCULUS_RIFT
    oculusRift.draw();
#else
    //draws to viewport
    //use blabalh
    CloudsVisualSystem::getSharedRenderTarget().draw(0,CloudsVisualSystem::getSharedRenderTarget().getHeight(),
                                                       CloudsVisualSystem::getSharedRenderTarget().getWidth(),
                                                      -CloudsVisualSystem::getSharedRenderTarget().getHeight());
    //end
#endif
}

void CloudsVisualSystem::drawCursor()
{
    if (drawCursorMode > DRAW_CURSOR_NONE) {
        map<int, CloudsInteractionEventArgs>& inputPoints = GetCloudsInputPoints();
        for (map<int, CloudsInteractionEventArgs>::iterator it = inputPoints.begin(); it != inputPoints.end(); ++it) {
            if (drawCursorMode == DRAW_CURSOR_PRIMARY && !it->second.primary) {
                continue;
            }
            
#ifdef KINECT_INPUT
            selfDrawCursor(it->second.position, it->second.actionType > k4w::ActionState_Idle);
#else
            // EZ: This ofGetMousePressed() call is ghetto but will do for now
            selfDrawCursor(it->second.position, ofGetMousePressed());
#endif
        }
    }
}

void CloudsVisualSystem::selfDrawCursor(ofVec3f& pos, bool bDragged)
{
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(2);
    if (bDragged) {
        ofSetColor(213, 69, 62, 255);
#ifdef OCULUS_RIFT
        ofCircle(pos, 6);
#else
        ofCircle(pos.x, pos.y,
                 ofMap(pos.z, 2, -2, 3, 6, true));
#endif
    }
    else {
        ofSetColor(255, 255, 255, 175);
#ifdef OCULUS_RIFT
        ofCircle(pos, 10);
#else
        ofCircle(pos.x, pos.y,
                 ofMap(pos.z, 2, -2, 3, 10, true));
#endif
    }
    ofPopStyle();
}

void CloudsVisualSystem::selfExit()
{
    
}

void CloudsVisualSystem::selfBegin()
{
    
}

void CloudsVisualSystem::selfEnd()
{
    
}

void CloudsVisualSystem::selfKeyPressed(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystem::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystem::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystem::selfMouseMoved(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystem::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystem::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystem::selfInteractionMoved(CloudsInteractionEventArgs& args){
	
}

void CloudsVisualSystem::selfInteractionStarted(CloudsInteractionEventArgs& args){
	
}

void CloudsVisualSystem::selfInteractionDragged(CloudsInteractionEventArgs& args){
	
}

void CloudsVisualSystem::selfInteractionEnded(CloudsInteractionEventArgs& args){
	
}


void CloudsVisualSystem::selfSetupGui(){

}

void CloudsVisualSystem::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystem::selfSetupSystemGui()
{
    
}

void CloudsVisualSystem::guiSystemEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystem::selfSetupRenderGui()
{
    
}

void CloudsVisualSystem::guiRenderEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystem::selfSetupCameraGui()
{
	
}

void CloudsVisualSystem::selfSetupTimeline()
{
    
}

void CloudsVisualSystem::selfSetupTimelineGui()
{
    
}

void CloudsVisualSystem::selfTimelineGuiEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystem::checkOpenGLError(string function){
	
    GLuint err = glGetError();
    if (err != GL_NO_ERROR){
        ofLogError( "CloudsVisualSystem::checkOpenGLErrors") << "OpenGL generated error " << ofToString(err) << " : " << gluErrorString(err) << " in " << function;
    }
}
