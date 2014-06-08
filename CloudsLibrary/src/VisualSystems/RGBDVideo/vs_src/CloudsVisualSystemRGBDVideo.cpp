

#include "CloudsVisualSystemRGBDVideo.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
CloudsVisualSystemRGBDVideo::CloudsVisualSystemRGBDVideo(){
	videoPathField = NULL;
	movieLoaded = false;
}

//--------------------------------------------------------------
string CloudsVisualSystemRGBDVideo::getSystemName(){
	return "RGBDVideo";
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetDefaults(){
	
    bEnableOcclusion = false;
    bEnableOcclusionDebug = false;
	occlusionSimplifyX = 2.0;
	occlusionSimplifyY = 2.0;

    occlusionVertexCount = 0;
    occlusionIndexCount = 0;
	loadMoviePath = "";
	
	movieLoaded = false;
    pointscale = .25;
    pointShift = ofVec3f(0,0,0);
    pointSize = 1.0;
    pointAlpha = 1.0;

    bEnablePoints = true;
    
    pointsSimplifyX = 2.0;
	pointsSimplifyY = 2.0;
	pointSize = 1.0;

    bEnableLines = false;
    lineGranularity = 2.0;
	lineSpacing = 2.0;
	lineThickness = 1.0;
    
    yLift = 0;
    
    bDrawVideoDebug = false;
    
    refreshLines = false;
    refreshOcclusion = false;
    refreshPoints = false;
    
    bDrawDust = false;
    dustParticleCount = 1000;
    dustAlpha = 1.0;
    dustRadius = 1000.0;
    dustSizeWeight = .2;
    dustPointSizeSmall = 1.0;
    dustPointSizeBig = 3.0;


	bEnableMesh = false;
	meshAlpha = 0.0;
	meshSimplifyX = 2.0;
	meshSimplifyY = 2.0;
    refreshMesh = false;
    meshVertexCount = 0;
    meshIndexCount = 0;

    bDrawDust = false;
    dustParticleCount = 0;
    dustRadius = 0;


    blendModeAdd = false;
    primaryCursorMode = CURSOR_MODE_CAMERA;
    secondaryCursorMode = CURSOR_MODE_INACTIVE;
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetup(){
    
	reloadShader();
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::reloadShader(){
    
    cout << "loading points shader" << endl;
	pointsShader.load(getVisualSystemDataPath() + "shaders/rgbd_simple_points");
    cout << "loading lines shader" << endl;
	linesShader.load(getVisualSystemDataPath() + "shaders/rgbd_simple_lines");
    cout << "loading mesh shader" << endl;
	meshShader.load(getVisualSystemDataPath() + "shaders/rgbd_simple_mesh");
    cout << "loading occlusion shader" << endl;
	occlusionShader.load(getVisualSystemDataPath() + "shaders/rgbd_simple_occlude");

}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfBegin(){

}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetupGuis(){
	
	ofxUISuperCanvas *g = new ofxUISuperCanvas("VIDEO", gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
	
    g->setName("VideoSettings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    g->addToggle("DEBUG", &bDrawVideoDebug);
    
    videoPathField = g->addTextInput("VideoPath", "");
	g->addButton("Load Video", false);
    g->addToggle("Add Blend", &blendModeAdd);
    
   	g->addSlider("Point Offset X", -400, 400, &pointShift.x);
   	g->addSlider("Point Offset Y", -400, 400, &pointShift.y);
	g->addSlider("Point Offset", -4000, 0, &pointShift.z);

    
    g->autoSizeToFitWidgets();

	ofAddListener(g->newGUIEvent, this, &CloudsVisualSystemRGBDVideo::selfGuiEvent);
    guis.push_back(g);
    guimap[g->getName()] = g;
    
    ofxUIToggle* toggle;
    
    ////////////////// OCCLUSION
    occlusionGui = new ofxUISuperCanvas("OCCLUSION", gui);
	occlusionGui->copyCanvasStyle(gui);
	occlusionGui->copyCanvasProperties(gui);
	occlusionGui->setName("Occlusion");
	occlusionGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	toggle = occlusionGui->addToggle("ENABLE", &bEnableOcclusion);
	toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
	occlusionGui->resetPlacer();
	occlusionGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
	occlusionGui->addWidgetToHeader(toggle);
    
    occlusionGui->addToggle("OcclusionDebug", &bEnableOcclusionDebug);
	occlusionGui->addSlider("Occl X Simplify", 1., 8., &occlusionSimplifyX);
	occlusionGui->addSlider("Occl Y Simplify", 1., 8., &occlusionSimplifyY);

 	ofAddListener(occlusionGui->newGUIEvent, this, &CloudsVisualSystemRGBDVideo::selfGuiEvent);
	guis.push_back(occlusionGui);
	guimap[occlusionGui->getName()] = occlusionGui;
    ////////////////// OCCLUSION

	////////////////// MESH
    occlusionGui = new ofxUISuperCanvas("MESH", gui);
	occlusionGui->copyCanvasStyle(gui);
	occlusionGui->copyCanvasProperties(gui);
	occlusionGui->setName("Msh");
	occlusionGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	toggle = occlusionGui->addToggle("ENABLE", &bEnableMesh);
	toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
	occlusionGui->resetPlacer();
	occlusionGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
	occlusionGui->addWidgetToHeader(toggle);
    
	occlusionGui->addSlider("Mesh Alpha", 0.0, 1.0, &meshAlpha);
	occlusionGui->addSlider("Mesh X Simplify", 1.0, 8.0, &meshSimplifyX);
	occlusionGui->addSlider("Mesh Y Simplify", 1.0, 8.0, &meshSimplifyY);
	
 	ofAddListener(occlusionGui->newGUIEvent, this, &CloudsVisualSystemRGBDVideo::selfGuiEvent);
	guis.push_back(occlusionGui);
	guimap[occlusionGui->getName()] = occlusionGui;
	////////////////// MESH
	
    ////////////////// POINTS
    pointsGui = new ofxUISuperCanvas("POINTS", gui);
	pointsGui->copyCanvasStyle(gui);
	pointsGui->copyCanvasProperties(gui);
	pointsGui->setName("Points");
	pointsGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	toggle = pointsGui->addToggle("ENABLE", &bEnablePoints);
	toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
	pointsGui->resetPlacer();
	pointsGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
	pointsGui->addWidgetToHeader(toggle);

    pointsGui->addSlider("Points Alpha", 0.0, 1.0, &pointAlpha);
    pointsGui->addSlider("Point X Simplify", 1.0, 8, &pointsSimplifyX);
    pointsGui->addSlider("Point Y Simplify", 1.0, 8, &pointsSimplifyY);
	pointsGui->addSlider("Point Size", 0.0, 5.0, &pointSize);
    
 	ofAddListener(pointsGui->newGUIEvent, this, &CloudsVisualSystemRGBDVideo::selfGuiEvent);
	guis.push_back(pointsGui);
	guimap[pointsGui->getName()] = pointsGui;
    ////////////////// POINTS
    
    ////////////////// DUST
    dustGui = new ofxUISuperCanvas("DUST", gui);
	dustGui->copyCanvasStyle(gui);
	dustGui->copyCanvasProperties(gui);
	dustGui->setName("Dust");
	dustGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	toggle = dustGui->addToggle("ENABLE", &bDrawDust);
	toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
	dustGui->resetPlacer();
	dustGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
	dustGui->addWidgetToHeader(toggle);

    dustGui->addSlider("Dust Alpha", 0.0, 1.0, &dustAlpha);
    dustGui->addIntSlider("Dust Count", 0, 5000, &dustParticleCount);
    dustGui->addSlider("Dust Spread", 100, 2000, &dustRadius);
    dustGui->addRangeSlider("Dust Point Size", 0.0, 5.0, &dustPointSizeSmall, &dustPointSizeBig);
    dustGui->addSlider("Dust Size Weight", 0.0, 1.0, &dustSizeWeight);
    
 	ofAddListener(dustGui->newGUIEvent, this, &CloudsVisualSystemRGBDVideo::selfGuiEvent);
	guis.push_back(dustGui);
	guimap[dustGui->getName()] = dustGui;
    ////////////////// DUST
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfUpdate(){
	
	if(loadMoviePath != ""){
		cout << "CloudsVisualSystemRGBDVideo::selfUpdate LOADING MOVIE PATH " << loadMoviePath << endl;
		if( playMovie(loadMoviePath) ){
			videoPathField->setTextString(loadMoviePath);
		}
		loadMoviePath = "";
	}
	
    if(dustSizeWeight != currentDustSizeWeight || dustMeshSmall.getNumVertices() + dustMeshBig.getNumVertices() != dustParticleCount){
        generateDust();
    }
    
    cloudsCamera.lookTarget = ofVec3f(0,0,0);
    
	if(movieLoaded){
		player.update();
	}
    
    if(refreshOcclusion){
        generateOcclusion();
    }
    
    if(refreshLines){
        generateLines();
    }
    
    if(refreshPoints) {
        generatePoints();
    }
	
	if(refreshMesh){
		generateMesh();
	}
}

void CloudsVisualSystemRGBDVideo::generatePoints(){
    
    points.clear();
    
    for(int y = 0; y < 480; y += pointsSimplifyY){
		for(int x = 0; x < 640; x += pointsSimplifyX){
			points.addVertex( ofVec3f( x, y, 0) );
		}
	}
	
    refreshPoints = false;
}

void CloudsVisualSystemRGBDVideo::generateLines(){
    
}

void CloudsVisualSystemRGBDVideo::generateOcclusion(){
	
    occlusionMesh.clear();
    
    if(occlusionSimplifyX <= 0) occlusionSimplifyX = 1.0;
	if(occlusionSimplifyY <= 0) occlusionSimplifyY = 1.0;
    
	int x = 0;
	int y = 0;
    
	int gw = ceil(640. / occlusionSimplifyX);
	int w = gw*occlusionSimplifyX;
	int h = 480.;
	ofMesh m;
    
	for (float y = 0; y < 480; y += occlusionSimplifyY){
		for (float x = 0; x < 640; x += occlusionSimplifyX){
			m.addVertex( ofVec3f(x,y,0) );
		}
	}
	
	for (float ystep = 0; ystep < h-occlusionSimplifyY; ystep += occlusionSimplifyY){
		for (float xstep = 0; xstep < w-occlusionSimplifyX; xstep += occlusionSimplifyX){
			ofIndexType a,b,c;
			
			a = x+y*gw;
			b = (x+1)+y*gw;
			c = x+(y+1)*gw;
			m.addIndex(a);
			m.addIndex(b);
			m.addIndex(c);
			
			a = (x+1)+(y+1)*gw;
			b = x+(y+1)*gw;
			c = (x+1)+(y)*gw;
			m.addIndex(a);
			m.addIndex(b);
			m.addIndex(c);
			
			x++;
		}
		
		y++;
		x = 0;
	}
    
    occlusionVertexCount = m.getNumVertices();
    occlusionIndexCount = m.getNumIndices();
    
    occlusionMesh.setMesh(m, GL_STATIC_DRAW);
    refreshOcclusion = false;
//    cout << "generated " << occlusionVertexCount << " vertices" << endl;
}

void CloudsVisualSystemRGBDVideo::generateMesh(){
	
	meshVbo.clear();

    if(meshSimplifyX <= 0) meshSimplifyX = 1.0;
	if(meshSimplifyY <= 0) meshSimplifyY = 1.0;
    
	int x = 0;
	int y = 0;
    
	int gw = ceil(640. / meshSimplifyX);
	int w = gw*meshSimplifyX;
	int h = 480.;
	ofMesh m;
    
	for (float y = 0; y < 480; y += meshSimplifyY){
		for (float x = 0; x < 640; x += meshSimplifyX){
			m.addVertex( ofVec3f(x,y,0) );
		}
	}
	
	for (float ystep = 0; ystep < h-meshSimplifyY; ystep += meshSimplifyY){
		for (float xstep = 0; xstep < w-meshSimplifyX; xstep += meshSimplifyX){
			ofIndexType a,b,c;
			
			a = x+y*gw;
			b = (x+1)+y*gw;
			c = x+(y+1)*gw;
			m.addIndex(a);
			m.addIndex(b);
			m.addIndex(c);
			
			a = (x+1)+(y+1)*gw;
			b = x+(y+1)*gw;
			c = (x+1)+(y)*gw;
			m.addIndex(a);
			m.addIndex(b);
			m.addIndex(c);
			
			x++;
		}
		
		y++;
		x = 0;
	}
    
    meshVertexCount = m.getNumVertices();
    meshIndexCount = m.getNumIndices();
    
    meshVbo.setMesh(m, GL_STATIC_DRAW);
    refreshMesh = false;
}

void CloudsVisualSystemRGBDVideo::generateDust(){
    dustMeshBig.clear();
    dustMeshSmall.clear();
    
    for(int i = 0; i < dustParticleCount; i++){
        if( ofRandomuf() > dustSizeWeight ){
            dustMeshBig.addVertex(ofVec3f(ofRandomf(),
                                          ofRandomf(),
                                          ofRandomf()));
        }
        else{
            dustMeshSmall.addVertex(ofVec3f(ofRandomf(),
                                            ofRandomf(),
                                            ofRandomf()));
        }
    }
    
    currentDustSizeWeight = dustSizeWeight;
    //currentDustRadius = dustRadius;
}


void CloudsVisualSystemRGBDVideo::selfDrawBackground(){
    if(bDrawVideoDebug){
        player.draw(0, 0);
    }
}

void CloudsVisualSystemRGBDVideo::selfDrawDebug(){

}

void CloudsVisualSystemRGBDVideo::selfSceneTransformation(){
	
}

void CloudsVisualSystemRGBDVideo::selfDraw(){
    
	if(!movieLoaded || !player.isLoaded()){
        return;
    }

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
	if(bEnableMesh){
        if(bEnableOcclusion){
            drawOcclusionLayer();
        }
        
		drawMeshLayer();
	}
    
    if(!bEnableOcclusion){
        glDisable(GL_DEPTH_TEST);
    }
    
    if(bEnablePoints){
        if(bEnableOcclusion){
            glClear(GL_DEPTH_BUFFER_BIT);
            drawOcclusionLayer();
        }

		drawPointLayer();
    }
    
    if(bDrawDust){
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_POINT_SMOOTH);

        ofPushStyle();
        ofPushMatrix();

        ofScale(dustRadius, dustRadius, dustRadius);
        
        ofSetColor(255*dustAlpha);
        glPointSize(dustPointSizeSmall);
        dustMeshSmall.drawVertices();
        
        glPointSize(dustPointSizeBig);
        dustMeshBig.drawVertices();
        
        ofPopStyle();
        ofPopMatrix();
    }
    
    glPopAttrib();
}

void CloudsVisualSystemRGBDVideo::setupGeneralUniforms(ofShader& shader){
    shader.setUniformTexture("texture", player.getTextureReference(), 1);
    shader.setUniform2f("depthPP", videoIntrinsics.depthPP.x,videoIntrinsics.depthPP.y );
    shader.setUniform2f("depthFOV", videoIntrinsics.depthFOV.x,videoIntrinsics.depthFOV.y );
    shader.setUniform1f("minDepth", videoIntrinsics.depthRange.min);
    shader.setUniform1f("maxDepth", videoIntrinsics.depthRange.max);
    shader.setUniform3f("pointoffset",
                        pointShift.x,
                        pointShift.y,
                        pointShift.z);
    shader.setUniform1f("scale", 1.0);
    shader.setUniform1f("offset", 0.0);
}

void CloudsVisualSystemRGBDVideo::drawOcclusionLayer(){
    
    glPushMatrix();
	
    if(!bEnableOcclusionDebug){
        ofTranslate(0, 0, 5.44);
        glEnable(GL_DEPTH_TEST);  // We want depth test !
        glDepthFunc(GL_LESS);     // We want to get the nearest pixels
        glColorMask(0,0,0,0);     // Disable color, it's useless, we only want depth.
        glDepthMask(GL_TRUE);     // Ask z writing
    }
    
    setupRGBDTransforms();
    ofTranslate(0, yLift, 0);
	
    occlusionShader.begin();
    occlusionShader.setUniform1f("edgeClip", 200);
    occlusionShader.setUniform2f("simplify", occlusionSimplifyX,occlusionSimplifyY);
    setupGeneralUniforms(occlusionShader);
    occlusionMesh.drawElements(GL_TRIANGLES, occlusionIndexCount);
    occlusionShader.end();
    
    if(!bEnableOcclusionDebug){
        glEnable(GL_DEPTH_TEST);  // We still want depth test
        glDepthFunc(GL_LEQUAL);   // EQUAL should work, too. (Only draw pixels if they are the closest ones)
        glColorMask(1,1,1,1);     // We want color this time
        glDepthMask(GL_FALSE);
    }
    
    glPopMatrix();
}

void CloudsVisualSystemRGBDVideo::drawMeshLayer(){
    
	glPushMatrix();
	
    setupRGBDTransforms();
    ofSetColor(255, meshAlpha*255);
    meshShader.begin();
    meshShader.setUniform1f("edgeClip", 200);
    meshShader.setUniform2f("simplify", meshSimplifyX,meshSimplifyY);
    setupGeneralUniforms(meshShader);
    meshVbo.drawElements(GL_TRIANGLES, meshIndexCount);
    meshShader.end();
    
    glPopMatrix();
}

void CloudsVisualSystemRGBDVideo::drawPointLayer(){
	
	ofPushMatrix();
	glPointSize(pointSize);
	
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	
	ofEnableBlendMode(blendModeAdd ? OF_BLENDMODE_ADD : OF_BLENDMODE_SCREEN);
	
	setupRGBDTransforms();
	ofTranslate(0, yLift, 0);
	
	pointsShader.begin();
	pointsShader.setUniform1f("alpha", pointAlpha);
	setupGeneralUniforms(pointsShader);
	points.drawVertices();
	pointsShader.end();
	
	ofPopMatrix();
}

void CloudsVisualSystemRGBDVideo::selfExit(){
	
}

void CloudsVisualSystemRGBDVideo::selfPresetLoaded(string presetPath){
	loadMoviePath = videoPathField->getTextString();
    
    refreshLines = true;
    refreshOcclusion = true;
    refreshPoints = true;
	refreshMesh = true;
}

bool CloudsVisualSystemRGBDVideo::playMovie(string filePath){
	movieLoaded = false;
	
	if( filePath == ""){
		ofLogError("CloudsVisualSystemRGBDVideo::playMovie") << "Path is blank" << endl;
		return false;
	}
	
	filePath = getVisualSystemDataPath(true) + "videos/" + filePath;
	
	if(!ofFile(filePath).exists() ) {
		ofLogError("CloudsVisualSystemRGBDVideo::playMovie") << "File does not exist" << endl;
		return false;
	}
	
	if(!player.loadMovie(filePath)){
		ofLogError("CloudsVisualSystemRGBDVideo::selfPresetLoaded") << "Video File " << filePath << " Failed to load";
		return false;
	}
	
	ofxXmlSettings intrinsicsXml;
	string xmlFilePath = ofFilePath::removeExt(filePath) + ".xml";
	if(!intrinsicsXml.loadFile(xmlFilePath)){
		ofLogError("CloudsVisualSystemRGBDVideo::selfPresetLoaded") << "XML File " << xmlFilePath << " Failed to load";
		return false;
	}
	
	videoIntrinsics.depthFOV.x = intrinsicsXml.getValue("depth:fovx", 0.);
	videoIntrinsics.depthFOV.y = intrinsicsXml.getValue("depth:fovy", 0.);
	videoIntrinsics.depthRange.min = intrinsicsXml.getValue("depth:minDepth", 0.);
	videoIntrinsics.depthRange.max = intrinsicsXml.getValue("depth:maxDepth", 0.);
    //TODO: read from xml
	videoIntrinsics.depthPP.x = 320;
	videoIntrinsics.depthPP.y = 240;
	
    player.setLoopState(OF_LOOP_NORMAL);
    player.play();
    
	movieLoaded = true;
	return true;
}

void CloudsVisualSystemRGBDVideo::selfEnd(){
    player.stop();
}

void CloudsVisualSystemRGBDVideo::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key == 'R'){
        reloadShader();
    }
    
    if(args.key == 'G'){
        timeline->stop();
        timeline->setCurrentFrame(0);

        player.stop();
        player.setTime(0);
        
        player.play();
        timeline->play();
    }
    
    if(args.key == 'D'){
        timeline->setOutPointAtSeconds( player.getDuration() );
    }
}

void CloudsVisualSystemRGBDVideo::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemRGBDVideo::selfMouseDragged(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfMouseReleased(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetupGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfGuiEvent(ofxUIEventArgs &e){
	
	if(e.widget->getName() == "Load Video"){
		
		ofxUIButton* button = (ofxUIButton*)e.widget;
		if(button->getValue()){
			ofFileDialogResult f = ofSystemLoadDialog();
			if(f.bSuccess){
				loadMoviePath = f.getName();
			}
		}
	}
    
	if(e.widget->getName() == "Line Spacing" ||
       e.widget->getName() == "Line Granularity" ||
       e.widget->getName() == "Line Random Offset")
    {
		refreshLines = true;
	}
    else if(e.widget->getName() == "Occl X Simplify" ||
            e.widget->getName() == "Occl Y Simplify")
    {
        refreshOcclusion = true;
    }
    else if(e.widget->getName() == "Point X Simplify" ||
            e.widget->getName() == "Point Y Simplify" )
    {
        refreshPoints = true;
    }
    else if(e.widget->getName() == "Mesh X Simplify" ||
            e.widget->getName() == "Mesh Y Simplify" )
    {
		refreshMesh = true;
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetupSystemGui(){
}
//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::guiSystemEvent(ofxUIEventArgs &e){
}
//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetupRenderGui(){
}
//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::guiRenderEvent(ofxUIEventArgs &e){
}

