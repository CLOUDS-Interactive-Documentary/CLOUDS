#include "testApp.h"

#if (OF_VERSION_MINOR <= 7 && OF_VERSION_PATCH <= 4)
#define ofParameter ofxParameter
#endif

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
    ofSetEscapeQuitsApp(false); //for Allison
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofBackground(22);
	ofxTimeline::removeCocoaMenusFromGlut("RGBDVisualize");
	
#ifdef TARGET_WIN32
	pathDelim = "\\";
#else
	pathDelim = "/";
#endif
    renderQueueIndexToRemove = -1;
    
    cam.setup();
	cameraSpeed = cam.speed = 20;
	cam.autosavePosition = true;
	cam.loadCameraPosition();
	cam.usemouse = true;
	cam.useArrowKeys = false;
	cam.setFarClip(30000);
	
    lockTo720p = false;
    lockTo1080p = true;
    customWidth = 1920;
    customHeight = 1080;
    
	cam.maximumY =  120;
	cam.minimumY = -120;
	cameraRollSpeed = cam.rollSpeed = 1;
	
    selectedScene = NULL;
    selectedComp = NULL;
    loadedScene = NULL;
	
    currentMirror = false;
    renderObjectFiles = false;
	startSequenceAt0 = false;
    currentRenderObjectFiles = false;
    selfOcclude = false;
    
	startRenderMode = false;
	currentlyRendering = false;
    firstRenderFrame = false;
    
	viewComps = false;
	temporalAlignmentMode = false;
	
	fillHoles = false;
	currentHoleKernelSize = 1;
	currentHoleFillIterations = 1;
    
	
	player.updateVideoPlayer = false;
    rendererDirty = true;
    isSceneLoaded = false;
    
	timeline.setup();
	timeline.setMinimalHeaders(true);
	timeline.setOffset(ofVec2f(0, ofGetHeight() - 200));
	timeline.setPageName("Main");
	timeline.setDurationInFrames(300); //base duration
    timeline.setMovePlayheadOnDrag(false);
    timeline.setSpacebarTogglePlay(false);
    
	newCompButton = new ofxMSAInteractiveObjectWithDelegate();
    newCompButton->fontReference = &timeline.getFont();
	newCompButton->setLabel("Create New Composition From This Scene");
	newCompButton->setDelegate(this);
	newCompButton->setPosAndSize(fboRectangle.x+fboRectangle.width+25, 0, 100, 25);
    setButtonColors(newCompButton);
	
	saveCompButton = new ofxMSAInteractiveObjectWithDelegate();
    saveCompButton->fontReference = &timeline.getFont();
	saveCompButton->setLabel("Save Comp");
	saveCompButton->setDelegate(this);
	saveCompButton->setPosAndSize(750, 0, 125, 25);
    setButtonColors(saveCompButton);
	
	saveCompAsNewButton = new ofxMSAInteractiveObjectWithDelegate();
    saveCompAsNewButton->fontReference = &timeline.getFont();
	saveCompAsNewButton->setLabel("Copy To New");
	saveCompAsNewButton->setDelegate(this);
	saveCompAsNewButton->setPosAndSize(750, 25, 125, 25);
    setButtonColors(saveCompAsNewButton);
	
    mediaBinButton = new ofxMSAInteractiveObjectWithDelegate();
	mediaBinButton->fontReference = &timeline.getFont();
	mediaBinButton->setLabel("Load MediaBin");
	mediaBinButton->setDelegate(this);
	mediaBinButton->setPosAndSize(250,0, 500, 25);
    setButtonColors(mediaBinButton);
	
    changeCompButton = new ofxMSAInteractiveObjectWithDelegate();
    changeCompButton->fontReference = &timeline.getFont();
	changeCompButton->setLabel("Change Comp");
	changeCompButton->setDelegate(this);
	changeCompButton->setPosAndSize(250,25, 500, 25);
    setButtonColors(changeCompButton);
	
    renderBatch = new ofxMSAInteractiveObjectWithDelegate();
    renderBatch->fontReference = &timeline.getFont();
    renderBatch->setLabel("Start Rendering Queue >>");
	renderBatch->setDelegate(this);
    setButtonColors(renderBatch);
		
	gui.setup("Settings");
	gui.add(cameraSpeed.setup("Camera Speed", ofParameter<float>(), 0, 40));
    gui.add(cameraRollSpeed.setup("Cam Roll Speed", ofParameter<float>(), .0, 4));
    gui.add(shouldResetCamera.setup("Reset Camera", ofParameter<bool>()));
	gui.add(currentLockCamera.setup("Lock to Track", ofParameter<bool>()));
    gui.add(shouldSaveCameraPoint.setup("Set Camera Point", ofParameter<bool>()));
	
	gui.add(drawPointcloud.setup("Draw Pointcloud",ofParameter<bool>()));
    gui.add(drawWireframe.setup("Draw Wireframe",ofParameter<bool>()));
    gui.add(drawMesh.setup("Draw Mesh",ofParameter<bool>()));
    gui.add(selfOcclude.setup("Self Occlude", ofParameter<bool>()));
    gui.add(drawDOF.setup("Draw DOF", ofParameter<bool>()));
 	
    gui.add( customWidth.setup("Frame Width", ofParameter<int>(), 320, 1920*2));
    gui.add( customHeight.setup("Frame Height", ofParameter<int>(), 240, 1080*2));
    gui.add( setCurrentSize.setup("Apply Custom Size", ofParameter<bool>()));
    gui.add( lockTo720p.setup("720p", ofParameter<bool>()));
    gui.add( lockTo1080p.setup("1080p",ofParameter<bool>()));
    
    gui.add(currentMirror.setup("Mirror", ofParameter<bool>()));
//	gui.add(flipTexture.setup("Flip Texture", ofParameter<bool>()));
	
    gui.add(fillHoles.setup("Fill Holes", ofParameter<bool>()));
    gui.add(currentHoleKernelSize.setup("Hole Kernel Size", ofParameter<int>(), 1, 10));
    gui.add(currentHoleFillIterations.setup("Hole Fill Iterations", ofParameter<int>(), 1, 20));
	
    gui.add(temporalAlignmentMode.setup("Temporal Alignment", ofParameter<bool>()));
	gui.add(captureFramePair.setup("Set Color-Depth Time", ofParameter<bool>()));
	
	gui.add(renderObjectFiles.setup("Export .obj Files", ofParameter<bool>()));
	gui.add(renderRainbowVideo.setup("Export Combined Rainbow", ofParameter<bool>()));
	
	gui.add(startSequenceAt0.setup("Start Sequence at 1", ofParameter<bool>()));
	
    gui.loadFromFile("defaultGuiSettings.xml");
    
	loadDefaults();

	//populateTimelineElements();
	timelineElementsAdded = false;
	allocateFrameBuffers();
    loadShaders();
	
	currentLockCamera = false;
    meshBuilder.cacheValidVertices = true;
	
    accumulatedPerlinOffset = 0;
	
    ofxXmlSettings defaultBin;
    if(defaultBin.loadFile("defaultBin.xml")){
		mediaBinFolder = defaultBin.getValue("bin", "");
        mediaBinButton->setLabel(mediaBinFolder);
		populateScenes();
    }
    else{
        ofLogError("No default bin found");
    }
    
    ofSetWindowShape(ofGetScreenWidth()-125, ofGetScreenHeight()-100);
}

void testApp::loadShaders(){
    
    dofRange.load("shaders/dofrange2");
	cout << "LOADING DOF BLUR" << endl;
    dofBlur.load("shaders/dofblur");
	cout << "LOADING DOF BLURANGE" << endl;
	
    dofQuad.clear();
    dofQuad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    dofQuad.addVertex(ofVec3f(0,0,0));
    dofQuad.addVertex(ofVec3f(0,fbo1.getHeight(),0));
    dofQuad.addVertex(ofVec3f(fbo1.getWidth(),0,0));
    dofQuad.addVertex(ofVec3f(fbo1.getWidth(),fbo1.getHeight(),0));
    
    dofQuad.addTexCoord(ofVec2f(0,0));
    dofQuad.addTexCoord(ofVec2f(0,fbo1.getHeight()));
    dofQuad.addTexCoord(ofVec2f(fbo1.getWidth(),0));
    dofQuad.addTexCoord(ofVec2f(fbo1.getWidth(),fbo1.getHeight()));
	
    renderer.reloadShader();
}

void testApp::populateTimelineElements(){
	
	timeline.setPageName("Camera");
	cameraTrack = new ofxTLCameraTrack();
	cameraTrack->setCamera(cam);
	cameraTrack->lockCameraToTrack = false;

	timeline.addTrack("Camera", cameraTrack);
	timeline.addCurves("Camera Dampen", ofRange(0,1.0), .3);
	videoTrack = new ofxTLVideoTrack();
	timeline.addTrack("Video", videoTrack);
	
    //rendering
    timeline.addPage("Geometry", true);
    timeline.addCurves("Simplify X", currentCompositionDirectory + "simplifyx.xml", ofRange(1, 8), 2);
    timeline.addCurves("Simplify Y", currentCompositionDirectory + "simplifyy.xml", ofRange(1, 8), 2);
    timeline.addCurves("Edge Clip", currentCompositionDirectory + "edgeClip.xml", ofRange(1.0, 2000), 2000 );
    timeline.addCurves("Z Threshold", currentCompositionDirectory + "zThreshold.xml", ofRange(1.0, sqrtf(6000)), sqrtf(6000) );
    timeline.addCurves("Z Near", currentCompositionDirectory + "zNear.xml", ofRange(1.0, sqrtf(6000)), sqrtf(6000) );
    
    timeline.addPage("Rotation", true);
	timeline.addCurves("X Rotate", currentCompositionDirectory + "meshXRot.xml", ofRange(-360,360), 0.);
    timeline.addCurves("Y Rotate", currentCompositionDirectory + "meshYRot.xml", ofRange(-360,360), 0.);
    timeline.addCurves("Z Rotate", currentCompositionDirectory + "meshZRot.xml", ofRange(-360,360), 0.);
	
    timeline.addPage("Rendering", true);
    timeline.addCurves("Point Alpha", currentCompositionDirectory + "pointAlpha.xml", ofRange(0,1.0) );
    timeline.addCurves("Point Size", currentCompositionDirectory + "pointSize.xml", ofRange(1.0, sqrtf(20.0) ) );
    timeline.addCurves("Wireframe Alpha", currentCompositionDirectory + "wireframeAlpha.xml", ofRange(0,1.0), 0.0 );
    timeline.addCurves("Wireframe Thickness", currentCompositionDirectory + "wireframeThickness.xml", ofRange(0.0,sqrtf(20.0)) );
    timeline.addCurves("Mesh Alpha", currentCompositionDirectory + "meshAlpha.xml", ofRange(0,1.0), 1.0 );
	
    timeline.addPage("Depth of Field", true);
    timeline.addCurves("DOF Distance", currentCompositionDirectory + "DOFDistance.xml", ofRange(0,sqrtf(1500.0)), 10 );
    timeline.addCurves("DOF Range", currentCompositionDirectory + "DOFRange.xml", ofRange(10,sqrtf(1500.0)) );
    timeline.addCurves("DOF Blur", currentCompositionDirectory + "DOFBlur.xml", ofRange(0,5.0) );
	
	timeline.addPage("Time Alignment", true);
	timeline.addTrack("Video", videoTrack);
	timeline.addTrack("Depth Sequence", &depthSequence);
	timeline.addTrack("Alignment", &alignmentScrubber);
	
	timeline.addPage("Texture Alignment");
	timeline.addCurves("X Texture Shift", currentCompositionDirectory + "XTextureShift.xml", ofRange(-.25, .25), 0.0 );
	timeline.addCurves("Y Texture Shift", currentCompositionDirectory + "YTextureShift.xml", ofRange(-.25, .25), 0.0 );
	timeline.addCurves("X Texture Scale", currentCompositionDirectory + "XTextureScale.xml", ofRange(.85, 1.15), 1.0 );
	timeline.addCurves("Y Texture Scale", currentCompositionDirectory + "YTextureScale.xml", ofRange(.85, 1.15), 1.0 );
	
	timeline.setCurrentPage("Rendering");
}

void testApp::drawGeometry(){
	
    float pointAlpha = timeline.getValue("Point Alpha");
    float wireAlpha = timeline.getValue("Wireframe Alpha");
    float meshAlpha = timeline.getValue("Mesh Alpha");
	
	if(!alignmentScrubber.ready()){
        pointAlpha = 0;
        wireAlpha = .0;
        meshAlpha = 1.0;
    }
	
	if(!drawPointcloud && !drawWireframe && !drawMesh){
		drawMesh = true;
	}
	
    //helps eliminate zfight by translating the mesh occluder slightly back from the camera
    ofVec3f camTranslateVec = cam.getLookAtDir();
    ofRectangle renderFboRect = ofRectangle(0, 0, fbo1.getWidth(), fbo1.getHeight());
    
    rendererDirty |= (renderedCameraPos.getPosition() != cam.getPosition() ||
                      renderedCameraPos.getOrientationQuat() != cam.getOrientationQuat() );
	
    if(rendererDirty){
		
        renderedCameraPos.setPosition(cam.getPosition());
        renderedCameraPos.setOrientation(cam.getOrientationQuat());
		
        ofBlendMode blendMode = OF_BLENDMODE_SCREEN;
		fbo1.begin();
		ofClear(0,0,0,0);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POINT_SMOOTH);
		
		cam.begin(renderFboRect);
		
		ofPushMatrix();
		ofPushStyle();
		ofEnableAlphaBlending();
		
		bool usedDepth = false;
		if(selfOcclude){
			ofTranslate(0, 0, 1);
			renderer.useTexture = false;
			renderer.drawMesh();
			renderer.useTexture = true;
			ofTranslate(0, 0, -1);
			usedDepth = true;
		}
		
		ofTranslate(0,0,-.5);
		if(drawMesh && meshAlpha > 0){
			ofSetColor(255*meshAlpha);
			renderer.drawMesh();
			usedDepth = true;
		}
		
		if(!usedDepth){
			glDisable(GL_DEPTH_TEST);
		}
		
		ofEnableBlendMode(blendMode);
		
		if(drawWireframe && wireAlpha > 0){
			ofTranslate(0,0,-.5);
			ofSetColor(255*wireAlpha);
            float thickness = timeline.getValue("Wireframe Thickness");
            thickness *= thickness;
			ofSetLineWidth(thickness);
			renderer.drawWireFrame();
		}
		
		if(drawPointcloud && pointAlpha > 0){
			
			ofTranslate(0,0,-.5);
			ofSetColor(255*pointAlpha);
            float pointSize = timeline.getValue("Point Size");
            glPointSize(pointSize*pointSize);
			renderer.drawPointCloud();
		}
		
		ofPopStyle();
		ofPopMatrix();
		
		glDisable(GL_DEPTH_TEST);
		
		cam.end();
		fbo1.end();
		//END NEW STYLE
		
        if(drawDOF){
            
            //render DOF
            dofBuffer.begin();
            ofClear(0,0,0,0);
            cam.begin(renderFboRect);
            glEnable(GL_DEPTH_TEST);
            renderer.drawMesh();
            glDisable(GL_DEPTH_TEST);
            cam.end();
            dofBuffer.end();
            
            float dofFocalDistance = timeline.getValue("DOF Distance");
            dofFocalDistance*=dofFocalDistance;
            float dofFocalRange = timeline.getValue("DOF Range");
            dofFocalRange*=dofFocalRange;
            float dofBlurAmount = timeline.getValue("DOF Blur");
            
            //composite
            swapFbo.begin();
            ofClear(0.0,0.0,0.0,0.0);
            
            ofPushStyle();
            ofEnableBlendMode(blendMode);
            
            ofSetColor(255);
            dofBlur.begin();
            
            dofBlur.setUniform2f("sampleOffset", dofBlurAmount, 0);
            dofBlur.setUniform1f("focalDistance", dofFocalDistance);
            dofBlur.setUniform1f("focalRange", dofFocalRange);
            
            dofBlur.setUniformTexture("tex", fbo1.getTextureReference(), 0);
            dofBlur.setUniformTexture("range", dofBuffer.getDepthTexture(), 1);
            
            dofQuad.draw();
			
            dofBlur.end();
            
            ofPopStyle();
            
            swapFbo.end();
            
            fbo1.begin();
            ofClear(0.0,0.0,0.0,0.0);
            
            ofPushStyle();
            
            ofSetColor(255, 255, 255, 255);
            dofBlur.begin();
            dofBlur.setUniform2f("sampleOffset", 0, dofBlurAmount);
            dofBlur.setUniformTexture("tex", swapFbo.getTextureReference(), 0);
            dofBlur.setUniformTexture("range", dofBuffer.getDepthTexture(), 1);
            
            dofQuad.draw();
            
            dofBlur.end();
            
            ofPopStyle();
            
            fbo1.end();
        }
		
        rendererDirty = false;
	}
    
    ofEnableAlphaBlending();
	ofPushStyle();
	ofSetColor(0,0,0);
	ofRect(fboRectangle);
	ofPopStyle();
    fbo1.getTextureReference().draw(ofRectangle(fboRectangle.x,fboRectangle.y+fboRectangle.height,fboRectangle.width,-fboRectangle.height));
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if(key == 'f'){
		ofToggleFullscreen();
	}
	
	if(loadedScene == NULL) return;
	
    
	
	if(currentlyRendering){
		if(key == ' '){
			finishRender();
		}
		return;
	}
	
	if(fboRectangle.inside(mouseX, mouseY) && !videoTrack->hasFocus()){
		if(key == OF_KEY_RIGHT){
			player.getVideoPlayer()->nextFrame();
		}
		if(key == OF_KEY_LEFT){
			player.getVideoPlayer()->previousFrame();
		}
	}
	
	if(key == ' '){
		timeline.togglePlay();
	}
	
    if(key == 'T'){
		cameraTrack->addKeyframe();
    }
    
    if(key == 'L'){
    	currentLockCamera = !currentLockCamera;
    }
	
	if(key == 'i'){
		timeline.setCurrentTimeToInPoint();
	}
	
	if(key == 'o'){
		timeline.setCurrentTimeToOutPoint();
	}
	
    if(key == 'S'){
        loadShaders();
    }
    
	if(key == '\t'){
		videoTrack->toggleThumbs();
		depthSequence.toggleThumbs();
	}
    
    if(key == '1'){
        timeline.setCurrentPage(0);
    }
    else if(key == '2'){
        timeline.setCurrentPage(1);
    }
    else if(key == '3'){
        timeline.setCurrentPage(2);
    }
    else if(key == '4'){
        timeline.setCurrentPage(3);
    }
    else if(key == '5'){
        timeline.setCurrentPage(4);
    }
    else if(key == '5'){
        timeline.setCurrentPage(5);
    }
    else if(key == '6'){
        timeline.setCurrentPage(6);
    }
    else if(key == '7'){
        timeline.setCurrentPage(7);
    }
    else if(key == '7'){
	    timeline.setCurrentPage(8);
    }
    
    if(key == 'J'){
        cameraTrack->jumpToTarget();
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	cam.usemouse = fboRectangle.inside(x, y);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	rendererDirty = true; //catch all for buttons...
}

#pragma mark application logic
//--------------------------------------------------------------
void testApp::update(){
	
    if(!isSceneLoaded){
        viewComps = true;
    }
    
	for(int i = 0; i < scenes.size(); i++){
        scenes[i].button->enabled  = viewComps;
	}
	
    for(int i = 0; i < comps.size(); i++){
		comps[i].load->enabled    = viewComps;
		comps[i].toggle->enabled  = viewComps;
    }
	
    for(int i = 0; i < renderQueue.size(); i++){
    	renderQueue[i].remove->enabled = viewComps;
    }
    
	
	renderBatch->enabled = viewComps && (renderQueue.size() > 0);
	
    changeCompButton->enabled = isSceneLoaded;
    newCompButton->enabled = (viewComps || !isSceneLoaded) && selectedScene != NULL;
    
	saveCompButton->enabled = !viewComps && isSceneLoaded;
    saveCompAsNewButton->enabled = !viewComps && isSceneLoaded;
    
    if(renderQueueIndexToRemove != -1){
        delete renderQueue[renderQueueIndexToRemove].remove;
        renderQueue.erase(renderQueue.begin() + renderQueueIndexToRemove);
        populateRenderQueue();
        renderQueueIndexToRemove = -1;
        populateCompositionsForScene();
    }
    
    cam.speed = cameraSpeed;
    cam.rollSpeed = cameraRollSpeed;
    
	if(startRenderMode){
        startRenderMode = false;
		firstRenderFrame = true;
        
        fbo1.begin();
        ofClear(0,0,0,0);
        fbo1.end();
        
		viewComps = false;
		saveComposition();
        
		bool foundCompToRender = false;
        for(int i = 0; i < renderQueue.size(); i++){
            if(!renderQueue[i].completed){
                selectedScene = renderQueue[i].sceneButton;
                currentCompShortName = renderQueue[i].compShortName;
				cout << "RENDER: loading comp " << renderQueue[i].compositionFolder << endl;
                loadComposition(renderQueue[i].compositionFolder + pathDelim);
                foundCompToRender = true;
                renderQueue[i].completed = true;
                break;
            }
        }
        
        if(foundCompToRender){
            currentlyRendering = true;
            
            //saveFolder = currentCompositionDirectory + "rendered" + pathDelim;
            char renderFilePath[1024] ;
            sprintf(renderFilePath, "%s/_RenderBin/%s_%s_%02d_%02d_%02d_%02d_%02d",mediaBinFolder.c_str(), selectedScene->scene.name.c_str(), currentCompShortName.c_str(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds());
            saveFolder = string(renderFilePath);
            ofDirectory saveDirectory(saveFolder);
            if(!saveDirectory.exists()){
                saveDirectory.create(true);
            }
            
			if(player.hasHighresVideo()){
				player.useHiresVideo();
				renderer.setRGBTexture(*player.getVideoPlayer());
				meshBuilder.setRGBTexture(*player.getVideoPlayer());
			}
			
            player.getVideoPlayer()->setVolume(0);
            
            cameraTrack->setTimelineInOutToTrack();
            timeline.setCurrentTimeToInPoint();
			player.getVideoPlayer()->setPosition(timeline.getPercentComplete());
			player.getVideoPlayer()->update();
            
            timeline.setPercentComplete(player.getVideoPlayer()->getPosition());
//            cout << "setting current time to " << timeline.getPercentComplete() << " seconds: " << timeline.getCurrentTime() << " video: " << player.getVideoPlayer()->getCurrentFrame() << " sec: " <<     (player.getVideoPlayer()->getPosition() * player.getVideoPlayer()->getDuration()) << endl;
            
            currentLockCamera = cameraTrack->lockCameraToTrack = true;
            cameraTrack->jumpToTarget();
        }
	}
    
	if(!isSceneLoaded) return;
	
    if(!currentlyRendering){
        checkReallocateFrameBuffers();
    }
    
	//if we don't have good pairings, force pages on timeline + gui
	if(!alignmentScrubber.ready()){
		videoTrack->setInFrame(0);
		videoTrack->setOutFrame(player.getVideoPlayer()->getTotalNumFrames());
        temporalAlignmentMode = true;
		drawMesh = true;
	}
	
	if(currentLockCamera != cameraTrack->lockCameraToTrack){
		if(!currentLockCamera){
			cam.setAnglesFromOrientation();
		}
		cameraTrack->lockCameraToTrack = currentLockCamera;
	}
	
	if(cameraTrack->lockCameraToTrack){
		cameraTrack->setTimelineInOutToTrack();
	}
	else{
		timeline.setInOutRange(ofRange(0,1));
	}
	
	cam.applyRotation = !cameraTrack->lockCameraToTrack;
	cam.applyTranslation = !cameraTrack->lockCameraToTrack;
    
	if(currentlyRendering){
		
		currentRenderFrame = player.getVideoPlayer()->getCurrentFrame();
		timeline.setPercentComplete(player.getVideoPlayer()->getPosition());
		
		//		cout << "would have set hi res frame to " << currentRenderFrame % hiResPlayer->getTotalNumFrames() << endl;
		//		cout << "instead set it to " << hiResPlayer->getCurrentFrame() << endl;
		
		////////
		//		char filename[512];
		//		sprintf(filename, "%s/TEST_FRAME_%05d_%05d_A.png", saveFolder.c_str(), currentRenderFrame, hiResPlayer->getCurrentFrame());
		//		savingImage.saveImage(filename);
		//		savingImage.setFromPixels(hiResPlayer->getPixelsRef());
		//		savingImage.saveImage(filename);
		//
		//		cout << "FRAME UPDATE" << endl;
		//		cout << "	setting frame to " << currentRenderFrame << " actual frame is " << hiResPlayer->getCurrentFrame() << endl;
		//		cout << "	set to percent " << 1.0*currentRenderFrame/hiResPlayer->getTotalNumFrames() << " actual percent " << hiResPlayer->getPosition() << endl;
		////////
	}
	else {
		
		if(shouldSaveCameraPoint){
			cameraTrack->addKeyframe();
            shouldSaveCameraPoint = false;
		}
		
		if(shouldResetCamera){
			resetCameraPosition();
            shouldResetCamera = false;
		}
		else{
			cam.applyRotation = cam.applyTranslation = true;
		}
		
		if(captureFramePair && temporalAlignmentMode){
            if(alignmentScrubber.getPairs().size() == 1){
                ofSystemAlertDialog("You have just set a second Color/Depth pair. Most of the time you just need one. If you are having alignment troubles, make sure to delete the existing pair first before setting a second one. You can delete the pairs in the Time Alignment tab by selecting them in the timeline track and hitting 'delete' key.");
            }
			alignmentScrubber.registerCurrentAlignment();
			alignmentScrubber.save();
            temporalAlignmentMode = false;
		}
		captureFramePair = false;
	}
	
	bool rendererNeedsUpdate = false;
	player.update();
	if(player.isFrameNew()){
		rendererNeedsUpdate = true;
	}
	
	renderer.worldRotation.x = timeline.getValue("X Rotate");
    renderer.worldRotation.y = timeline.getValue("Y Rotate");
    renderer.worldRotation.z = timeline.getValue("Z Rotate");
	renderer.flipTexture = flipTexture;
    ofVec2f simplification = ofVec2f( timeline.getValue("Simplify X"), timeline.getValue("Simplify Y") );
	
	if(renderer.getSimplification().x != simplification.x || renderer.getSimplification().y != simplification.y){
    	renderer.setSimplification(simplification);
        meshBuilder.setSimplification(simplification);
		rendererNeedsUpdate = true;
    }
	
    if(currentRenderObjectFiles != renderObjectFiles){
        rendererNeedsUpdate = true;
        currentRenderObjectFiles = renderObjectFiles;
    }
	
	float currentFarClip = powf(timeline.getValue("Z Threshold"), 2.0);
    float currentNearClip = powf(timeline.getValue("Z Near"), 2.0);
    
	if(timeline.getValue("X Texture Shift") != renderer.shift.x ||
	   timeline.getValue("Y Texture Shift") != renderer.shift.y ||
       timeline.getValue("X Texture Scale") != renderer.scale.x ||
       timeline.getValue("Y Texture Scale") != renderer.scale.y ||
	   timeline.getValue("Edge Clip") != renderer.edgeClip ||
	   currentMirror != renderer.mirror ||
	   fillHoles != holeFiller.enable ||
	   currentHoleKernelSize != holeFiller.getKernelSize() ||
       currentHoleFillIterations != holeFiller.getIterations()||
	   currentFarClip != renderer.farClip ||
       currentNearClip != renderer.nearClip )
	{
		renderer.shift.x = timeline.getValue("X Texture Shift");
		renderer.shift.y = timeline.getValue("Y Texture Shift");
        renderer.scale.x = timeline.getValue("X Texture Scale");
        renderer.scale.y = timeline.getValue("Y Texture Scale");
		renderer.mirror = currentMirror;
		renderer.farClip = currentFarClip;
        renderer.nearClip = currentNearClip;
        renderer.edgeClip = timeline.getValue("Edge Clip");
        
        meshBuilder.mirror = currentMirror;
		meshBuilder.shift.x = timeline.getValue("X Texture Shift");
		meshBuilder.shift.y = timeline.getValue("Y Texture Shift");
        meshBuilder.scale.x = timeline.getValue("X Texture Scale");
        meshBuilder.scale.y = timeline.getValue("Y Texture Scale");
		
		meshBuilder.farClip = currentFarClip;
        meshBuilder.edgeClip = renderer.edgeClip;
        
		holeFiller.enable = fillHoles;
		currentHoleKernelSize = holeFiller.setKernelSize(currentHoleKernelSize);
		currentHoleFillIterations = holeFiller.setIterations(currentHoleFillIterations);
		
		rendererNeedsUpdate = true;
	}
	
    if(timeline.getUserChangedValue()){
		rendererDirty = true;
    }
	
	if(temporalAlignmentMode &&
	   (currentDepthFrame != player.getDepthSequence()->getCurrentFrame() ||
		currentVideoFrame != videoTrack->getPlayer()->getCurrentFrame())){
		   rendererNeedsUpdate = true;
	   }
	
	if(rendererNeedsUpdate){
		updateRenderer();
	}
}

//--------------------------------------------------------------
void testApp::updateRenderer(){
	
	if(currentDepthFrame != player.getDepthSequence()->getCurrentFrame()){
		holeFiller.close(player.getDepthPixels());
    }
    
    renderer.update();
    if(currentlyRendering && renderObjectFiles){
        meshBuilder.update();
    }
	
	cameraTrack->setDampening(powf(timeline.getValue("Camera Dampen"),2.));
	//used for temporal aligmnet nudging
	currentDepthFrame = player.getDepthSequence()->getCurrentFrame();
	currentVideoFrame = player.getVideoPlayer()->getCurrentFrame();
	
    rendererDirty = true;
}

//--------------------------------------------------------------
void testApp::checkReallocateFrameBuffers(){
    
    if(lockTo720p && (fbo1.getWidth() != 1280 || fbo1.getHeight() != 720)){
        customWidth = 1280;
        customHeight = 720;
        allocateFrameBuffers();
    }
    else if(lockTo1080p && (fbo1.getWidth() != 1920 || fbo1.getHeight() != 1080) ){
        customWidth = 1920;
        customHeight = 1080;
        allocateFrameBuffers();
    }
    else if(setCurrentSize && (fbo1.getWidth() != customWidth || fbo1.getHeight() != customHeight)){
        allocateFrameBuffers();
    }
    setCurrentSize = false;
    
    lockTo720p  = fbo1.getWidth() == 1280 && fbo1.getHeight() == 720;
    lockTo1080p = fbo1.getWidth() == 1920 && fbo1.getHeight() == 1080;
}

void testApp::allocateFrameBuffers(){
	
    int fboWidth = customWidth;
    int fboHeight = customHeight;
	
    fboWidth  = ofClamp(fboWidth,  320, 1920*2);
    fboHeight = ofClamp(fboHeight, 240, 1080*2);
	
	savingImage.allocate(fboWidth,fboHeight,OF_IMAGE_COLOR);
	savingImage.setUseTexture(false);
	depthSequence.setAutoUpdate(false);
	
	fboRectangle = ofRectangle(250, 100, fboWidth, fboHeight);
    ofFbo::Settings dofBuffersSettings;
    dofBuffersSettings.width = fboWidth;
    dofBuffersSettings.height = fboHeight;
    dofBuffersSettings.internalformat = GL_RGB;
    dofBuffersSettings.numSamples = 0;
    dofBuffersSettings.useDepth = true;
    dofBuffersSettings.useStencil = true;
    dofBuffersSettings.depthStencilAsTexture = true;
    dofBuffersSettings.textureTarget = ofGetUsingArbTex() ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D;
    dofBuffer.allocate(dofBuffersSettings);
	
	
    swapFbo.allocate(fboWidth, fboHeight, GL_RGB);
    fbo1.allocate(fboWidth, fboHeight, GL_RGBA, 4);
	
    fbo1.begin();
    ofClear(0,0,0,0);
    fbo1.end();
    dofBuffer.begin();
    ofClear(0,0,0,0);
    dofBuffer.end();
	
	cout << "finished allocating frame buffers" << endl;
	
}


//--------------------------------------------------------------
void testApp::draw(){
	
	//REMOVE
	if(!isSceneLoaded) return;
	
	if(isSceneLoaded){
		if(!viewComps){
            
			if(!ofGetMousePressed(0)){
				timeline.setOffset(ofVec2f(0, ofGetHeight() - timeline.getDrawRect().height));
			}
            
			ofRectangle fboRenderArea = ofRectangle(0,0,ofGetWidth()-220-300, timeline.getDrawRect().y - 50);
			ofRectangle naturalVideoRect = ofRectangle(0,0,fbo1.getWidth(),fbo1.getHeight());
			fboRectangle = naturalVideoRect;
			fboRectangle.scaleTo(fboRenderArea);
			fboRectangle.x = 220;
			fboRectangle.y = 50;
			
			ofRectangle colorAssistRenderArea = ofRectangle(0,0,ofGetWidth() - fboRectangle.getMaxX(),timeline.getDrawRect().y - 50);
			colorAlignAssistRect = naturalVideoRect;
			colorAlignAssistRect.scaleTo(colorAssistRenderArea);
			colorAlignAssistRect.x = fboRectangle.getMaxX();
			colorAlignAssistRect.y = fboRectangle.getMinY();
			colorAssistRenderArea.height -= colorAlignAssistRect.height;
			
			depthAlignAssistRect =  ofRectangle(0,0,640,480);
            depthAlignAssistRect.scaleTo(colorAssistRenderArea);
			depthAlignAssistRect.y = colorAlignAssistRect.getMaxY();
			depthAlignAssistRect.x = colorAlignAssistRect.getX();
			
            drawGeometry();
            
			if(temporalAlignmentMode){
                player.getVideoPlayer()->draw(colorAlignAssistRect);
				depthSequence.getCurrentDepthImage().draw(depthAlignAssistRect);
			}
            
			if(drawDOF && !temporalAlignmentMode){
                dofRange.begin();
                float dofFocalDistance = timeline.getValue("DOF Distance");
                dofFocalDistance*=dofFocalDistance;
                float dofFocalRange = timeline.getValue("DOF Range");
                dofFocalRange*=dofFocalRange;
                dofRange.setUniform1f("focalDistance", dofFocalDistance);
                dofRange.setUniform1f("focalRange", dofFocalRange);
				dofBuffer.getDepthTexture().draw(ofRectangle(colorAlignAssistRect.x,
															 colorAlignAssistRect.getMaxY(),
															 colorAlignAssistRect.width,
															 -colorAlignAssistRect.height));
                dofRange.end();
            }
            
			if(currentlyRendering){
				char filename[512];
				int videoFrame = player.getVideoPlayer()->getCurrentFrame();
				if(startSequenceAt0){
					videoFrame -= timeline.getInFrame();
				}
                sprintf(filename, "%s/save.%05d.png", saveFolder.c_str(), videoFrame);
				
				if(renderRainbowVideo){
					rainbowExporter.setPlayer( &player );
					rainbowExporter.setRenderer( &meshBuilder );
					rainbowExporter.maxDepth = renderer.farClip;
                    cout << "Far: " << renderer.farClip << endl;
					rainbowExporter.minDepth = renderer.nearClip;
                    cout << "Near: " << renderer.nearClip << endl;
					rainbowExporter.inoutPoint.min = timeline.getInFrame();
					rainbowExporter.inoutPoint.max = timeline.getOutFrame();
					rainbowExporter.render(saveFolder, "save.");
					currentRenderFrame = timeline.getOutFrame()+1;
				}

                if(!firstRenderFrame){
                    if(renderObjectFiles){
                        char objFilename[512];
                        sprintf(objFilename, "%s/save.%05d.obj", saveFolder.c_str(), videoFrame);
                        ofMesh reducedMesh = meshBuilder.getReducedMesh(true, ofVec3f(.001, -.001, .001), false, true);
                        ofxObjLoader::save(string(objFilename), reducedMesh);
                        savingImage.setFromPixels(player.getVideoPlayer()->getPixelsRef());
                    }
                    else{
                        fbo1.getTextureReference().readToPixels(savingImage.getPixelsRef());
                        if(!renderObjectFiles){
                            savingImage.mirror(true, false);
                        }
                    }
                    
					if(!renderRainbowVideo){
						savingImage.saveImage(filename);
					}
                    
                    //cout << "   at save time its set to " << player.getVideoPlayer()->getCurrentFrame() << " time " << (player.getVideoPlayer()->getPosition() * player.getVideoPlayer()->getDuration() ) << endl;
                }
                firstRenderFrame = false;
                
				///////frame debugging
				//		numFramesRendered++;
				//		cout << "	Rendered (" << numFramesRendered << "/" << numFramesToRender << ") +++ current render frame is " << currentRenderFrame << " quick time reports frame " << hiResPlayer->getCurrentFrame() << endl;
				//		sprintf(filename, "%s/TEST_FRAME_%05d_%05d_B.png", saveFolder.c_str(), currentRenderFrame, hiResPlayer->getCurrentFrame());
				//		savingImage.saveImage(filename);
				//		savingImage.setFromPixels(hiResPlayer->getPixelsRef());
				//		savingImage.saveImage(filename);
				//////
				
				if(currentRenderFrame > timeline.getOutFrame()){
					finishRender();
				}
				else{
					//					cout << "advancing video frame from " << player.getVideoPlayer()->getCurrentFrame() << " with timeline time " << timeline.getCurrentFrame() << " current render frame: " << currentRenderFrame << endl;
					player.getVideoPlayer()->nextFrame();
					player.getVideoPlayer()->update();
					timeline.setPercentComplete(player.getVideoPlayer()->getPosition());
					//					cout << " to " << player.getVideoPlayer()->getCurrentFrame() << endl;
				}
			}
            //ofDrawBitmapString();
            timeline.getFont().drawString("fps: " + ofToString(ofGetFrameRate()), saveCompButton->x + saveCompButton->width + 10, saveCompButton->y + 10);
			if(!currentlyRendering){
				gui.draw();
            }
            timeline.draw();
		}
		
		ofSetColor(255);
	}
	
    if(viewComps){
        
        ofPushStyle();
        ofSetColor(timeline.getColors().highlightColor, 255);
        ofEnableAlphaBlending();
        for(int i = 0; i < renderQueue.size(); i++){
            if(renderQueue[i].completed){
                ofRect(*renderQueue[i].remove);
            }
        }
        for(int i = 0; i < comps.size(); i++){
            if(comps[i].inRenderQueue){
                ofRect(*comps[i].toggle);
            }
        }
        ofPopStyle();
    }
}

#pragma mark compositions
//--------------------------------------------------------------
bool testApp::createNewComposition(){
	cout << "createNewComposition -- selected take " << selectedScene->scene.name << endl;
    
    if(selectedScene == NULL){
        ofLogError("testApp::createNewComposition -- Cannot create new comp with no selected take!");
        return false;
    }
    
	ofDirectory compBin( ofToDataPath(selectedScene->scene.mediaFolder + "/compositions/") );
	if(!compBin.exists()){
		if(!compBin.create(true)){
            ofSystemAlertDialog("Unable to create composition directory: " + compBin.getOriginalDirectory());
            return false;
        }
	}
	compBin.listDir();
	
	string newName =  ofSystemTextBoxDialog("New Composition Name");
	ofStringReplace(newName, pathDelim, "_");
	if(newName == ""){
		return false;
	}
	
	currentCompShortName = newName + pathDelim;
	currentCompositionDirectory = ofToDataPath( selectedScene->scene.mediaFolder + "/compositions/" + currentCompShortName);
	ofDirectory compDirectory( currentCompositionDirectory );
    
    if(!compDirectory.exists()){
        if(!compDirectory.create()){
            ofSystemAlertDialog("Unable to create composition: " + compDirectory.getOriginalDirectory());
            return false;
        }
    }
    
	return true;
}

bool testApp::loadAssetsForScene(SceneButton* sceneButton){
	
	ofxRGBDScene& scene = sceneButton->scene;
	if(!player.setup(scene)){
		ofSystemAlertDialog("Load Failed -- Scene invalid");
		return false;
	}
	
	timeline.setWorkingFolder(currentCompositionDirectory);
	
	if(!timelineElementsAdded){
		timelineElementsAdded = true;
		populateTimelineElements();
	}
	
	alignmentScrubber.setXMLFileName(scene.pairingsFile);
	renderer.setup(player.getScene().calibrationFolder);
	meshBuilder.setup(player.getScene().calibrationFolder);
    
	cam.setFov(renderer.getRGBCalibration().getDistortedIntrinsics().getFov().y);
	
	renderer.setRGBTexture(*player.getVideoPlayer());
	renderer.setDepthImage(player.getDepthPixels());
	meshBuilder.setRGBTexture(*player.getVideoPlayer());
	meshBuilder.setDepthImage(player.getDepthPixels());
	
	depthSequence.setSequence(player.getDepthSequence());
	videoTrack->setPlayer(player.getVideoPlayer());
	alignmentScrubber.setPairSequence(player.getVideoDepthAligment());
	
	timeline.setTimecontrolTrack(videoTrack);
	timeline.setFrameRate(1.0*videoTrack->getPlayer()->getTotalNumFrames()/videoTrack->getPlayer()->getDuration());
	timeline.setDurationInSeconds(MAX(depthSequence.getDurationInMillis()/1000.0, videoTrack->getPlayer()->getDuration()) );
	
    //trick to help if there is no pairing file
    if(!alignmentScrubber.ready()){
        resetCameraPosition();
		timeline.setPercentComplete(.5);
        player.getVideoPlayer()->setFrame(timeline.getCurrentFrame());
        depthSequence.selectTimeInMillis(timeline.getCurrentTimeMillis());
    }
	
    return true;
}

//--------------------------------------------------------------
void testApp::loadNewMediaBin(){
	
	ofFileDialogResult r = ofSystemLoadDialog("Select Media Bin", true);
	if(r.bSuccess){
		
		isSceneLoaded = false;
		selectedScene = NULL;
		selectedComp = NULL;
		loadedScene = NULL;
		
		clearRenderQueue();
		clearCompositionButtons();
		
		viewComps = true;
		mediaBinFolder = r.getPath();
        mediaBinButton->setLabel(mediaBinFolder);
		populateScenes();
        //save it
        ofxXmlSettings defaultBin;
        defaultBin.setValue("bin", mediaBinFolder);
        defaultBin.saveFile("defaultBin.xml");
	}
}

//--------------------------------------------------------------
void testApp::populateScenes(){
    
    ofDirectory dir(mediaBinFolder);
	dir.listDir();
	int mediaFolders = dir.numFiles();
	int currentCompButton = 0;
    
    for(int i = scenes.size()-1; i >= 0; i--){
        delete scenes[i].button;
    }
    scenes.clear();
	
	for(int i = 0; i < mediaFolders; i++){
		
        SceneButton sceneButton;
        sceneButton.scene.loadFromFolder(dir.getPath(i), false);
        if(!sceneButton.scene.valid()){
            continue;
        }
        
        sceneButton.button = new ofxMSAInteractiveObjectWithDelegate();
        sceneButton.button->fontReference = &timeline.getFont();
        sceneButton.button->setup();
        sceneButton.button->setDelegate(this);
        sceneButton.button->setLabel(sceneButton.scene.name);
        setButtonColors(sceneButton.button);
        scenes.push_back( sceneButton );
	}
	
    if(scenes.size() == 0){
        ofSystemAlertDialog(mediaBinFolder + " has no valid scenes! Make sure to select the folder containing all of the scenes.");
        mediaBinButton->setLabel("Load MediaBin");
    }
    
    positionSceneButtons();
}

//--------------------------------------------------------------
void testApp::positionSceneButtons(){
    int compx = 0;
	int compy = 50;
	
    for(int i = 0; i < scenes.size(); i++){
        
        scenes[i].button->setPosAndSize(compx,compy,250,25);
		
        compy += 25;
        if(compy > ofGetHeight()-100){
            compy  = 150;
        	compx += 250;
        }
    }
    maxSceneX = compx+250;
}

//--------------------------------------------------------------
void testApp::populateCompositionsForScene(){
    
	if(selectedScene == NULL){
        ofLogError("populateCompositionsForScene -- Take is null");
        return;
    }
    
	clearCompositionButtons();
	
    string compositionsFolder = selectedScene->scene.mediaFolder + pathDelim + "compositions" + pathDelim;
    ofDirectory compositionsDirectory(compositionsFolder);
    if(!compositionsDirectory.exists()){
        compositionsDirectory.create();
    }
	
    int compx = maxSceneX;
	int compy = selectedScene->button->y;
    newCompButton->setPosAndSize(compx, compy, 325, 25);
    
	compy+=25;
    compositionsDirectory.listDir();
    int numComps = compositionsDirectory.numFiles();
    for(int c = 0; c < numComps; c++){
        if(!compositionsDirectory.getFile(c).isDirectory()){
            continue;
        }
        
		CompButton comp;
        comp.load  = new ofxMSAInteractiveObjectWithDelegate();
        comp.load->fontReference = &timeline.getFont();
        comp.load->setup();
        comp.load->setDelegate(this);
        comp.load->setPosAndSize(compx, compy, 300, 25);
        setButtonColors(comp.load);
        
        comp.toggle = new ofxMSAInteractiveObjectWithDelegate();
        comp.toggle->fontReference = &timeline.getFont();
        comp.toggle->setLabel("R");
        comp.toggle->setup();
        comp.toggle->setDelegate(this);
        comp.toggle->setPosAndSize(compx+300, compy,25,25);
        setButtonColors(comp.toggle);
        comp.compositionFolder = compositionsDirectory.getPath(c);
        comp.inRenderQueue = false;
        for(int i = 0; i < renderQueue.size(); i++){
            if(comp.compositionFolder == renderQueue[i].compositionFolder){
                comp.inRenderQueue = true;
                break;
            }
        }
        
        compy += 25;
        if(compy > ofGetHeight()-100){
            compy  = 150;
            compx += 325;
        }
        
        //string compLabel = selectedScene->scene.name + ":" + compositionsDirectory.getName(c);
        string compLabel = compositionsDirectory.getName(c);
        comp.load->setLabel(compLabel);
        
	    comps.push_back(comp);
	}
}

void testApp::clearCompositionButtons(){
	for(int i = comps.size()-1; i >= 0; i--){
        delete comps[i].toggle;
        delete comps[i].load;
    }
    comps.clear();
}

void testApp::clearRenderQueue(){
	for(int i = renderQueue.size()-1; i >= 0; i--){
		delete renderQueue[i].remove;
	}
	renderQueue.clear();
}

//--------------------------------------------------------------
void testApp::loadDefaults(){
    
	drawPointcloud = false;
	drawWireframe = false;
	drawMesh = true;
    
	selfOcclude = false;
	drawDOF = false;
	
	captureFramePair = false;
	temporalAlignmentMode = true;
	
    cam.speed = 20;
	cam.rollSpeed = 0;
	
    fillHoles = true;
    currentHoleKernelSize = 3;
    currentHoleFillIterations = 2;
	
    currentMirror = false;
    
    customWidth = 1920;
    customHeight = 1080;
    lockTo1080p = true;
	setCurrentSize = false;
	
    renderObjectFiles = false;
	renderRainbowVideo = false;
    startSequenceAt0 = false;
	
    resetCameraPosition();
	
	saveComposition();
}

//--------------------------------------------------------------
void testApp::resetCameraPosition(){
	cam.setPosition(0, 0, 0);
	cam.setOrientation(ofQuaternion());
	cam.rotate(180, ofVec3f(0,1,0));
	cam.setAnglesFromOrientation();
	
	/*
	 cam.setPosition(0, 0, 0);
	 ofMatrix4x4 yflip,xflip;
	 ofMatrix4x4 transform;
	 yflip.makeScaleMatrix(ofVec3f(1,-1,1));
	 xflip.makeScaleMatrix(ofVec3f(-1,1,1));
	 transform = yflip * renderer.getDepthToRGBTransform().getInverse() * yflip;
	 //			transform = yflip * renderer.getDepthToRGBTransform().getInverse() * yflip;
	 //			transform = xflip * yflip * renderer.getDepthToRGBTransform()  * yflip * xflip;
	 cam.setTransformMatrix(transform);
	 cam.setFov(renderer.getRGBCalibration().getDistortedIntrinsics().getFov().y);
	 cam.setAnglesFromOrientation();
	 cam.invertControls = true;
	 cam.applyRotation = cam.applyTranslation = false;
	 */
}

//--------------------------------------------------------------
void testApp::setButtonColors(ofxMSAInteractiveObjectWithDelegate* btn){
	ofColor downColor  = ofColor(255, 120, 0);
	ofColor idleColor  = ofColor(220, 200, 200);
	ofColor hoverColor = ofColor(255*.2, 255*.2, 30*.2);
	
    btn->setIdleColor(idleColor);
    btn->setDownColor(downColor);
    btn->setHoverColor(hoverColor);
	
}

//--------------------------------------------------------------
void testApp::saveComposition(){
	
	if(loadedScene == NULL){
		return;
	}
	
	cam.saveCameraPosition();
	cameraTrack->save();
    
    timeline.save();
    gui.saveToFile(currentCompositionFile);
	setCompositionButtonName();
}

//--------------------------------------------------------------
void testApp::objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y){
}

//--------------------------------------------------------------
void testApp::objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y){
}

//--------------------------------------------------------------
void testApp::objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button){
    
	if(object == mediaBinButton){
    	loadNewMediaBin();
    }
    else if(object == changeCompButton){
        if(!viewComps){
            viewComps = true;
            populateCompositionsForScene();
            changeCompButton->setLabel("<< back");
        }
        else {
            viewComps = false;
			setCompositionButtonName();
        }
    }
    else if(object == newCompButton){
        if(createNewComposition()){
			loadComposition(currentCompositionDirectory);
		}
    }
	else if(object == saveCompButton){
		saveComposition();
	}
	else if(object == saveCompAsNewButton){
        string oldCompFolder = currentCompositionDirectory;
        if(createNewComposition()){
			
            ofDirectory oldCompDirectory(oldCompFolder);
            oldCompDirectory.allowExt("xml");
            oldCompDirectory.listDir();
            for(int i = 0; i < oldCompDirectory.numFiles(); i++){
                oldCompDirectory.getFile(i).copyTo( currentCompositionDirectory + oldCompDirectory.getName(i));
            }
            loadComposition(currentCompositionDirectory);
        }
	}
    else if(object == renderBatch){
        startRenderMode = true;
    }
    else {
		
        for(int i = 0; i < scenes.size(); i++){
            if(scenes[i].button == object){
                selectedScene = &scenes[i];
                populateCompositionsForScene();
                return;
            }
        }
        
        for(int i = 0; i < comps.size(); i++){
            if(comps[i].load == object){
				
                currentCompShortName = comps[i].load->getLabel();
				loadComposition(comps[i].compositionFolder+pathDelim);
                return;
            }
            
            if(object == comps[i].toggle){
                if(comps[i].inRenderQueue){
                    //remove it
                    for(int j = 0; j < renderQueue.size(); j++){
                        if(renderQueue[j].compositionFolder == comps[i].compositionFolder){
                            renderQueueIndexToRemove = j;
                            break;
                        }
                    }
                }
                else {
                    addCompToRenderQueue(&comps[i]);
	                comps[i].inRenderQueue = true;
                }
                return;
            }
        }
        
        for(int i = renderQueue.size()-1; i >= 0; i--){
			if(object == renderQueue[i].remove){
                renderQueueIndexToRemove = i;
                return;
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){
    
}

//--------------------------------------------------------------
bool testApp::loadComposition(string compositionDirectory){
	
    if(selectedScene == NULL){
		ofLogError("loadComposition -- Loading with a NULL secene");
		return false;
	}
	
	string oldCompDir = currentCompositionDirectory;
	string oldCompFile = currentCompositionFile;
	currentCompositionDirectory = compositionDirectory;
    currentCompositionFile = currentCompositionDirectory+"compositionsettings.xml";
	
    if(loadedScene != selectedScene){
        isSceneLoaded = loadAssetsForScene(selectedScene);
		if(!isSceneLoaded){
			currentCompositionDirectory = oldCompDir;
			currentCompositionFile = oldCompFile;
			return false;
		}
		
        loadedScene = selectedScene;
    }
	
    //camera stuff
    cam.cameraPositionFile = currentCompositionDirectory + "camera_position.xml";
	cam.loadCameraPosition();
    string cameraSaveFile = currentCompositionDirectory + "camera.xml";
	cameraTrack->setXMLFileName(cameraSaveFile);
	
    timeline.setCurrentPage(0);
    accumulatedPerlinOffset = 0;
	if(ofFile::doesFileExist(currentCompositionFile)){
		gui.loadFromFile(currentCompositionFile);
	}
	else{
        loadDefaults();
    }

    alignmentScrubber.setup();
	alignmentScrubber.videoSequence = videoTrack;
	alignmentScrubber.depthSequence = &depthSequence;

	timeline.loadTracksFromFolder( currentCompositionDirectory );
    //fix up pairings file back into the main dir
    alignmentScrubber.setXMLFileName(selectedScene->scene.pairingsFile);
    alignmentScrubber.load();
    if(alignmentScrubber.ready()){ //failsafe to not leave the temopral alignment mode on
        temporalAlignmentMode = false;
    }
	else{
		timeline.setCurrentPage("Time Alignment");
	}
	
	cameraTrack->setup();
	cameraTrack->load();
	timeline.setCurrentTimeMillis(cameraTrack->getEarliestTime());
		
	setCompositionButtonName();
    //turn off view comps
	viewComps = false;
	return true;
}

void testApp::setCompositionButtonName(){
    lastSavedDate = "Last Saved on " + ofToString(ofGetMonth() ) + "/" + ofToString( ofGetDay()) + " at " + ofToString(ofGetHours()) + ":" + ofToString( ofGetMinutes() )  + ":" + ofToString( ofGetSeconds() );
    
    //TODO: change widths to prevent font overflow
    changeCompButton->setLabel(selectedScene->scene.name + "/" + currentCompShortName + " -- " + lastSavedDate);
}

void testApp::addCompToRenderQueue(CompButton* comp){
    for(int i = 0; i < renderQueue.size(); i++){
        if(comp->compositionFolder == renderQueue[i].compositionFolder){
            return;
        }
    }
    
    RenderButton b;
    b.sceneButton = selectedScene;
    b.compositionFolder = comp->compositionFolder;
    b.remove = NULL;
	b.compShortName = comp->load->getLabel();
    
	renderQueue.push_back(b);
    
    populateRenderQueue();
}

//--------------------------------------------------------------
void testApp::populateRenderQueue(){
    
    int posx = ofGetWidth()-300;
    int posy = 50;
    renderBatch->setPosAndSize(posx, posy-25, 225, 25);
    
    for(int i = 0; i < renderQueue.size(); i++){
        
        if(renderQueue[i].remove != NULL){
	        delete renderQueue[i].remove;
        }
        
        renderQueue[i].remove = new ofxMSAInteractiveObjectWithDelegate();
        renderQueue[i].remove->fontReference = &timeline.getFont();
        renderQueue[i].remove->setup();
        renderQueue[i].remove->setDelegate(this);
        renderQueue[i].remove->setPosAndSize(posx, posy, 225, 25);
		vector<string> pathElements = ofSplitString(renderQueue[i].compositionFolder,pathDelim);
        renderQueue[i].remove->setLabel("[x] " + pathElements[pathElements.size()-3] + " : " + renderQueue[i].compShortName );
        setButtonColors(renderQueue[i].remove);
        renderQueue[i].completed = false;
        
		posy+=25;
    }
}

//--------------------------------------------------------------
void testApp::finishRender(){
	
	currentlyRendering = false;
    for(int i = 0; i < renderQueue.size(); i++){
    	if(!renderQueue[i].completed){
            startRenderMode = true;
            return;
        }
    }
	
	timeline.enable();
	
	player.useLowResVideo();
    //render is done
	renderer.setRGBTexture(*player.getVideoPlayer());
	meshBuilder.setRGBTexture(*player.getVideoPlayer());
	player.getVideoPlayer()->setVolume(1.0);
    
	videoTrack->setPlayer(player.getVideoPlayer());
	player.getVideoPlayer()->setFrame(timeline.getInFrame());
	timeline.setCurrentTimeToInPoint();
    
    currentLockCamera = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    positionSceneButtons();
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
	
}