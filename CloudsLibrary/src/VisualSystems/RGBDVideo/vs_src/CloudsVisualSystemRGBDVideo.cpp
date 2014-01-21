

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
    
    blendModeAdd = false;
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
	pointsGui->addSlider("Point Size", 0.0, 3.0, &pointSize);
    
 	ofAddListener(pointsGui->newGUIEvent, this, &CloudsVisualSystemRGBDVideo::selfGuiEvent);
	guis.push_back(pointsGui);
	guimap[pointsGui->getName()] = pointsGui;
    ////////////////// POINTS
    
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
	
/*
	for(int i = 0; i < indeces.size(); i+=3){
		
		ofVec3f& a = vertices[ indeces[i+0] ];
		ofVec3f& b = vertices[ indeces[i+1] ];
		ofVec3f& c = vertices[ indeces[i+2] ];
		ofVec3f mid = (a+b+c)/3.;
		
		ofVec3f toA = a-mid;
		ofVec3f toB = b-mid;
		ofVec3f toC = c-mid;
		
		m.addNormal(toA);
		m.addColor(ofFloatColor(toB.x/640.,toB.y/480.,toC.x/640.,toC.y/480.));
		m.addVertex(mid);
        
		m.addNormal(toB);
		m.addColor(ofFloatColor(toA.x/640.,toA.y/480.,toC.x/640.,toC.y/480.));
		m.addVertex(mid);
		
		m.addNormal(toC);
		m.addColor(ofFloatColor(toA.x/640.,toA.y/480.,toB.x/640.,toB.y/480.));
		m.addVertex(mid);
	}
    */
    
    occlusionVertexCount = m.getNumVertices();
    occlusionIndexCount = m.getNumIndices();
    
    occlusionMesh.setMesh(m, GL_STATIC_DRAW);
    refreshOcclusion = false;
//    cout << "generated " << occlusionVertexCount << " vertices" << endl;
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
    
    if(bEnablePoints){
        
        if(bEnableOcclusion){
            drawOcclusionLayer();
        }
        
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

void CloudsVisualSystemRGBDVideo::selfExit(){
	
}

void CloudsVisualSystemRGBDVideo::selfPresetLoaded(string presetPath){
	loadMoviePath = videoPathField->getTextString();
    
    refreshLines = true;
    refreshOcclusion = true;
    refreshPoints = true;
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

