//
//  CloudsRGBDCombinedRenderer.cpp
//  CloudsRGBDCombinedRenderer
//
//  Created by Patricio Gonzalez Vivo on 4/14/13.
//
//

#include "CloudsRGBDCombinedRenderer.h"

//---------------------------------------------------------------
CloudsRGBDCombinedRenderer::CloudsRGBDCombinedRenderer(){
	ofVideoPlayer* vp = new ofVideoPlayer();	
#ifdef AVF_PLAYER
	vp->setPlayer( ofPtr<ofBaseVideoPlayer>( new ofxAVFVideoPlayer() ) );
#else
	player = ofPtr<ofVideoPlayer>( vp );
#endif
	
	setShaderPath("shaders/rgbdcombined");
    
    simplify.set(0,0);
    
	nearClip    = 1.0f;
	edgeClip    = 50.0f;
	farClip     = 6000.0f;
    
	minDepth = 400;
	maxDepth = 2000;

    bMirror     = false;
    bFlipTexture = false;
    bRendererBound = false;
}

CloudsRGBDCombinedRenderer::~CloudsRGBDCombinedRenderer(){
    
}

//--------------------------------------------------------------- SET
bool CloudsRGBDCombinedRenderer::setup(string videoPath, string calibrationXMLPath){
	ofxXmlSettings XML;

	if ( !XML.loadFile(calibrationXMLPath)){
		ofLogError() << "CloudsRGBDCombinedRenderer::setup -- XML Path " << calibrationXMLPath << " failed to load";
		return false;		
	}
	
	colorPrincipalPoint.x = XML.getValue("colorIntrinsics:ppx", 971.743835449);
	colorPrincipalPoint.y = XML.getValue("colorIntrinsics:ppy", 546.945983887);
	colorFOV.x = XML.getValue("colorIntrinsics:fovx", 923.500793457);
	colorFOV.y = XML.getValue("colorIntrinsics:fovy", 921.060791016);
	colorRect.x = 0.0f;
	colorRect.y = 0.0f;
	colorRect.width = XML.getValue("colorIntrinsics:width", 1920.000000000);
	colorRect.height = XML.getValue("colorIntrinsics:height", 1080.000000000);
	
	float depthToRGBRotation[9];
	float depthToRGBTranslation[3];
	for (int i = 0; i < 9; i++) {
		depthToRGBRotation[i] = XML.getValue("extrinsics:rotation:r"+ofToString(i), 1.0f);
	}
	
	for (int i = 0; i < 3; i++) {
		depthToRGBTranslation[i] = XML.getValue("extrinsics:translation:t"+ofToString(i), 1.0f);
	}
	
	for (int i = 0; i < 3; i++) {
		distortionK[i] = XML.getValue("colorIntrinsics:dK:k"+ofToString(i), 1.0f);
	}
	
	for (int i = 0; i < 2; i++) {
		distortionP[i] = XML.getValue("colorIntrinsics:dP:p"+ofToString(i), 1.0f);
	}
	
	headPosition = ofVec3f(-XML.getValue("face:x", 0.0),
						   -XML.getValue("face:y", 0.0),
						   XML.getValue("face:z", 0.0));
	
	cout << "head position " << headPosition << endl;
	
 	float mat4x4[16] = {
		depthToRGBRotation[0],depthToRGBRotation[1],depthToRGBRotation[2],0,
		depthToRGBRotation[3],depthToRGBRotation[4],depthToRGBRotation[5],0,
		depthToRGBRotation[6],depthToRGBRotation[7],depthToRGBRotation[8],0,
		depthToRGBTranslation[0],depthToRGBTranslation[1],depthToRGBTranslation[2],1
	};
	
	extrinsics = ofMatrix4x4(mat4x4);
	
//	cout << "extrinsic matrix: " << endl << extrinsics << endl;
	
	//adjustment
	adjustTranslate.x = XML.getValue("adjustment:translate:x", 0.0);
	adjustTranslate.y = XML.getValue("adjustment:translate:y", 0.0);
	adjustTranslate.z = XML.getValue("adjustment:translate:z", 0.0);

	adjustRotate.x = XML.getValue("adjustment:rotate:x", 0.0);
	adjustRotate.y = XML.getValue("adjustment:rotate:y", 0.0);
	adjustRotate.z = XML.getValue("adjustment:rotate:z", 0.0);

	adjustScale.x = XML.getValue("adjustment:scale:x", 1.0);
	adjustScale.y = XML.getValue("adjustment:scale:y", 1.0);
	
	depthPrincipalPoint.x = XML.getValue("depthIntrinsics:ppx", 320.0);
	depthPrincipalPoint.y = XML.getValue("depthIntrinsics:ppy", 240.0);
	depthFOV.x = XML.getValue("depthIntrinsics:fovx", 570.34);
	depthFOV.y = XML.getValue("depthIntrinsics:fovy", 570.34);
	
	depthRect.x = 0.0;      //  TODO: do this automatically
	depthRect.y = 720.0;    //
	depthRect.width = XML.getValue("depthIntrinsics:width", 640.0);
	depthRect.height = XML.getValue("depthIntrinsics:height", 480.0);
	
	normalRect.x = 640.0;       //  TODO: do this automatically
	normalRect.y = 720.0;       //
	normalRect.width = 640.0;
	normalRect.height = 480.0;
	
	nearClip = minDepth = XML.getValue("adjustment:depth:min", 1.0f);
	farClip = maxDepth = XML.getValue("adjustment:depth:max", 6000.0f);
		
	//TODO automatically
	//this describes the face features: eyes, mouth, and skin
	faceFeatureRect = ofRectangle(depthRect.x, depthRect.getMaxY(), 640, 360);
	//this describes the change each frame
	deltaChangeRect = ofRectangle(normalRect.x, normalRect.getMaxY(), 640, 360);

#ifdef AVF_PLAYER
    //TODO make asynchronous
	//if(!player->loadMovie(videoPath)){
	getPlayer().stop();
	if(!getPlayer().loadMovie(videoPath)){
		ofLogError() << "CloudsRGBDCombinedRenderer::setup -- Movie path " << videoPath << " failed to load";
//		return false;
	}
#else
	if(!player->loadMovie(videoPath)){
		ofLogError() << "CloudsRGBDCombinedRenderer::setup -- Movie path " << videoPath << " failed to load";	
	}
#endif
	
//	float colorWidth  = getPlayer().getWidth();
//	float colorHeight = getPlayer().getHeight();
	float colorWidth  = 1280;
	float colorHeight = 1560;
	
    colorScale.x = float(colorWidth) / float(colorRect.width);
	if(colorHeight > 1200){
		useFaces = true;
		colorScale.y = float(colorHeight - (depthRect.height + faceFeatureRect.height) ) / float(colorRect.height);
	}
	else{
		useFaces = false;
		colorScale.y = float(colorHeight - (depthRect.height) ) / float(colorRect.height);
	}

    return true;
}

void CloudsRGBDCombinedRenderer::setShaderPath(string _shaderPath){
    shaderPath = _shaderPath;
	reloadShader();
}

void CloudsRGBDCombinedRenderer::setSimplification(ofVec2f _simplification){
	
	if(simplify == _simplification){
		return;
	}
	
	if(_simplification.x <= 0  || _simplification.y <= 0){
		return;
	}
	
	simplify = _simplification;
	
/*
	mesh.clearIndices();
	int x = 0;
	int y = 0;
	
	int gw = ceil(depthRect.width / simplify.x);
	int w = gw*simplify.x;
	int h = depthRect.height;
	
	for (float ystep = 0; ystep < h-simplify.y; ystep += simplify.y){
		for (float xstep = 0; xstep < w-simplify.x; xstep += simplify.x){
			ofIndexType a,b,c;
			
			a = x+y*gw;
			b = (x+1)+y*gw;
			c = x+(y+1)*gw;
			mesh.addIndex(a);
			mesh.addIndex(b);
			mesh.addIndex(c);
            
			a = (x+1)+(y+1)*gw;
			b = x+(y+1)*gw;
			c = (x+1)+(y)*gw;
			mesh.addIndex(a);
			mesh.addIndex(b);
			mesh.addIndex(c);
			
			x++;
		}
		
		y++;
		x = 0;
	}
	
    mesh.clearColors();
    mesh.clearNormals();
    mesh.clearVertices();
    mesh.clearTexCoords();
	for (float y = 0; y < depthRect.height; y += simplify.y){
		for (float x = 0; x < depthRect.width; x += simplify.x){
            mesh.addColor(ofFloatColor(1.0,1.0,1.0,1.0));
            mesh.addVertex( ofVec3f(x, y,0));
            mesh.addTexCoord(ofVec2f(x,y));
		}
	}
    
	bMeshGenerated = true;
*/
	
	if(bRendererBound){
		shader.setUniform2f("simplify", simplify.x,simplify.y);
	}
}

void CloudsRGBDCombinedRenderer::reloadShader(){
	shader.load( shaderPath );
}

//--------------------------------------------------------------- BINDERS
bool CloudsRGBDCombinedRenderer::bindRenderer(){
    ofPushMatrix();
	
	ofScale(1, -1, 1);
	if(!bMirror){
		ofScale(-1, 1, 1);
	}
	
	ofRotate(worldRotation.x,1,0,0);
	ofRotate(worldRotation.y,0,1,0);
	ofRotate(worldRotation.z,0,0,1);
    
	shader.begin();
    
	setupProjectionUniforms();
	
	bRendererBound = true;
	return true;
}

void CloudsRGBDCombinedRenderer::unbindRenderer(){
    if(!bRendererBound){
		ofLogError("ofxRGBDGPURenderer::unbindRenderer -- called without renderer bound");
	 	return;
	}
	
	shader.end();
	bRendererBound = false;
    
	ofPopMatrix();
}

void CloudsRGBDCombinedRenderer::setupProjectionUniforms(){
    
	if(!getPlayer().isLoaded()){
		ofLogWarning() << " CloudsRGBDCombinedRenderer::setupProjectionUniforms -- player is not ready";
		return;
	}
	
	getPlayer().setLoopState(OF_LOOP_NONE);
	
    shader.setUniformTexture("texture", getPlayer().getTextureReference(), 0);
    shader.setUniform2f("textureSize",  getPlayer().getWidth(), getPlayer().getHeight());
    
    shader.setUniform4f("colorRect", colorRect.x, colorRect.y, colorRect.width, colorRect.height);
    shader.setUniform2f("colorScale", colorScale.x, colorScale.y);
    
    shader.setUniform2f("colorFOV", colorFOV.x, colorFOV.y );
    shader.setUniform2f("colorPP", colorPrincipalPoint.x, colorPrincipalPoint.y);
    shader.setUniform3f("dK", distortionK.x, distortionK.y, distortionK.z);
    shader.setUniform2f("dP", distortionP.x, distortionP.y);
    
	ofMatrix4x4 adjustmentMatrix;
	adjustmentMatrix.rotate(adjustRotate.x, 0, 1, 0);
	adjustmentMatrix.rotate(adjustRotate.y, 1, 0, 0);
	adjustmentMatrix.translate(adjustTranslate.x, adjustTranslate.y, adjustTranslate.z);

	shader.setUniformMatrix4f("extrinsics", extrinsics * adjustmentMatrix );
    
    shader.setUniform4f("depthRect", depthRect.x, depthRect.y, depthRect.width, depthRect.height);
	shader.setUniform2f("depthPP", depthPrincipalPoint.x, depthPrincipalPoint.y);
	shader.setUniform2f("depthFOV", depthFOV.x, depthFOV.y);
    
    shader.setUniform4f("normalRect", normalRect.x, normalRect.y, normalRect.width, normalRect.height);
    shader.setUniform4f("faceFeatureRect", faceFeatureRect.x, faceFeatureRect.y, faceFeatureRect.width, faceFeatureRect.height);
    shader.setUniform4f("deltaChangeRect", deltaChangeRect.x, deltaChangeRect.y, deltaChangeRect.width, deltaChangeRect.height);

	shader.setUniform1i("useFaces", useFaces ? 1 : 0);
	shader.setUniform1f("flowPosition", flowPosition);
    shader.setUniform2f("simplify", simplify.x,simplify.y);
	shader.setUniform1f("farClip", farClip);
    shader.setUniform1f("nearClip", nearClip);
	shader.setUniform1f("edgeClip", edgeClip);

	shader.setUniform1f("minDepth", minDepth);
    shader.setUniform1f("maxDepth", maxDepth);

    //    cout << ( farClip - nearClip ) + nearClip << endl;
    //    cout << "FarClip: " << farClip << endl;
    //    cout << "nearClip: " << nearClip << endl;
    //    cout << "edgeClip: " << edgeClip << endl;
    
//    shader.setUniform2f("shift", shift.x, shift.y);
//    shader.setUniform2f("scale", scale.x, scale.y);
}

#ifdef AVF_PLAYER
//--------------------------------------------------------------- ACTIONS
ofxAVFVideoPlayer& CloudsRGBDCombinedRenderer::getPlayer(){
	return avPlayer;
}

ofPtr<ofVideoPlayer> CloudsRGBDCombinedRenderer::getSharedPlayerPtr(){
	return ofPtr<ofVideoPlayer>( new ofVideoPlayer());
}
#else
//--------------------------------------------------------------- ACTIONS
ofVideoPlayer& CloudsRGBDCombinedRenderer::getPlayer(){
	return *player;
}
//--------------------------------------------------------------- ACTIONS
ofPtr<ofVideoPlayer> CloudsRGBDCombinedRenderer::getSharedPlayerPtr(){
	return player;
}
#endif


//--------------------------------------------------------------- ACTIONS
ofShader& CloudsRGBDCombinedRenderer::getShader(){
	return shader;
}

//--------------------------------------------------------------- ACTIONS
void CloudsRGBDCombinedRenderer::update(){

	getPlayer().update();
	
	float audioVolume = 1.0;
#ifdef AVF_PLAYER
	float position = getPlayer().getPositionInSeconds();
	float duration = getPlayer().getDuration();
#else
	float position = getPlayer().getPosition()*getPlayer().getDuration();
	float duration = getPlayer().getDuration();
#endif
	
	//cout << "position is " << position << " " << duration << " duration " << endl;
	
	if(position < 1){
		audioVolume = powf(position,2.0);
//		cout << "VOLUME " << audioVolume << endl;
	}
	else if(position > duration - 1){
		audioVolume = powf(duration - position, 2.0);
//		cout << "VOLUME " << audioVolume << endl;
	}
	
	getPlayer().setVolume(audioVolume);
}

bool CloudsRGBDCombinedRenderer::isPlaying(){
	return getPlayer().isLoaded() && getPlayer().isPlaying();
}

bool CloudsRGBDCombinedRenderer::isDone(){
	return getPlayer().isLoaded() && !getPlayer().isPlaying();
}

void CloudsRGBDCombinedRenderer::drawMesh(){
	draw(OF_MESH_FILL);
}
void CloudsRGBDCombinedRenderer::drawPointCloud(){
	draw(OF_MESH_POINTS);
}
void CloudsRGBDCombinedRenderer::drawWireFrame(){
	draw(OF_MESH_WIREFRAME);
}

void CloudsRGBDCombinedRenderer::draw(ofPolyRenderMode drawMode){
    if(bindRenderer()){
		
		switch(drawMode){
			case OF_MESH_POINTS:
				mesh.drawVertices(); break;
			case OF_MESH_WIREFRAME:
				mesh.drawWireframe(); break;
			case OF_MESH_FILL:
				mesh.drawFaces(); break;
		}
		
		unbindRenderer();
	}
}