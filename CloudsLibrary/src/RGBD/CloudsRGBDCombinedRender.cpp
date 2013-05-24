//
//  CloudsRGBDCombinedRender.cpp
//  CloudsRGBDCombinedRender
//
//  Created by Patricio Gonzalez Vivo on 4/14/13.
//
//

#include "CloudsRGBDCombinedRender.h"


//---------------------------------------------------------------
CloudsRGBDCombinedRender::CloudsRGBDCombinedRender(){
    
    setShaderPath("shaders/rgbdcombined");
    
    simplify.set(1.0f,1.0f);
    
	nearClip    = 1.0f;
	edgeClip    = 50.0f;
	farClip     = 6000.0f;
    
    bMirror     = false;
    bFlipTexture = false;
    bRendererBound = false;
}

CloudsRGBDCombinedRender::~CloudsRGBDCombinedRender(){
    
}

//--------------------------------------------------------------- SET
bool CloudsRGBDCombinedRender::setup(string videoPath, string calibrationXMLPath){
	ofxXmlSettings XML;
    

	if ( !XML.loadFile(calibrationXMLPath)){
		ofLogError() << "CloudsRGBDCombinedRender::setup -- XML Path " << calibrationXMLPath << " failed to load";
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
	
	float mat4x4[16] = {
		depthToRGBRotation[0],depthToRGBRotation[1],depthToRGBRotation[2],0,
		depthToRGBRotation[3],depthToRGBRotation[4],depthToRGBRotation[5],0,
		depthToRGBRotation[6],depthToRGBRotation[7],depthToRGBRotation[8],0,
		depthToRGBTranslation[0],depthToRGBTranslation[1],depthToRGBTranslation[2],1
	};
	
	extrinsics = ofMatrix4x4(mat4x4);
	
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
	
	nearClip    = XML.getValue("adjustment:minDepth", 1.0f);
	farClip     = XML.getValue("adjustment:maxDepth", 6000.0f);

    //TODO make asynchronous
	if(!player.loadMovie(videoPath)){
		ofLogError() << "CloudsRGBDCombinedRender::setup -- Movie path " << videoPath << " failed to load";
		return false;
	}
	
    colorScale.x = float(player.getWidth()) / float(colorRect.width);
    colorScale.y = float(player.getHeight() - depthRect.height) / float(colorRect.height);
	
    return true;
}

void CloudsRGBDCombinedRender::setShaderPath(string _shaderPath){
    shaderPath = _shaderPath;
	reloadShader();
}

void CloudsRGBDCombinedRender::setSimplification(ofVec2f _simplification){
	
	if(simplify == _simplification){
		return;
	}
	
	if(_simplification.x <= 0  || _simplification.y <= 0){
		return;
	}
	
	simplify = _simplification;
	
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
            //            mesh.addNormal( ofVec3f(1.0,1.0,1.0) );
            mesh.addVertex( ofVec3f(x, y,0));
            mesh.addTexCoord(ofVec2f(x,y));
		}
	}
    
	bMeshGenerated = true;
}

//void CloudsRGBDCombinedRender::setTexture(ofBaseHasTexture& _tex){
//    tex = &_tex;
//    
//    colorScale.x = float(_tex.getTextureReference().getWidth()) / float(colorRect.width);
//    colorScale.y = float(_tex.getTextureReference().getHeight()-depthRect.height) / float(colorRect.height);
//}

void CloudsRGBDCombinedRender::reloadShader(){
	shader.load( shaderPath );
}

//--------------------------------------------------------------- BINDERS

bool CloudsRGBDCombinedRender::bindRenderer(){
    ofPushMatrix();
	
	ofScale(1, -1, 1);
	if(!bMirror){
		ofScale(-1, 1, 1);
	}
	
	ofRotate(worldRotation.x,1,0,0);
	ofRotate(worldRotation.y,0,1,0);
	ofRotate(worldRotation.z,0,0,1);
    
    
	shader.begin();
	glActiveTexture(GL_TEXTURE1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glActiveTexture(GL_TEXTURE0);
    
	setupProjectionUniforms();
	
	bRendererBound = true;
	return true;
}

void CloudsRGBDCombinedRender::unbindRenderer(){
    if(!bRendererBound){
		ofLogError("ofxRGBDGPURenderer::unbindRenderer -- called without renderer bound");
	 	return;
	}
	
	shader.end();
	bRendererBound = false;
    
	ofPopMatrix();
}

ofVideoPlayer& CloudsRGBDCombinedRender::getPlayer(){
	return player;
}

void CloudsRGBDCombinedRender::setupProjectionUniforms(){
    
    //  Texture
    //
	if(!player.isLoaded() || !player.isPlaying()){
		ofLogWarning() << " CloudsRGBDCombinedRender::setupProjectionUniforms -- player is not ready";
		return;
	}
	
    shader.setUniformTexture("texture", player, 0);
    shader.setUniform2f("textureSize",  player.getWidth(), player.getHeight());
    
    shader.setUniform4f("colorRect", colorRect.x, colorRect.y, colorRect.width, colorRect.height);
    shader.setUniform2f("colorScale", colorScale.x, colorScale.y);
    
    shader.setUniform2f("colorFOV", colorFOV.x, colorFOV.y );
    shader.setUniform2f("colorPP", colorPrincipalPoint.x, colorPrincipalPoint.y);
    shader.setUniform3f("dK", distortionK.x, distortionK.y, distortionK.z);
    shader.setUniform2f("dP", distortionP.x, distortionP.y);
    
//    glUniformMatrix3fv( glGetUniformLocation(shader.getProgram(), "colorRotate"), 1, GL_FALSE,depthToRGBRotation);
//    shader.setUniform3f("colorTranslate", depthToRGBTranslation.x,depthToRGBTranslation.y,depthToRGBTranslation.z);
	ofMatrix4x4 modMat;
	modMat.rotate(adjustRotate.x, 0, 1, 0);
	modMat.rotate(adjustRotate.y, 1, 0, 0);
	modMat.translate(adjustTranslate.x, adjustTranslate.y, adjustTranslate.z);

	shader.setUniformMatrix4f( "extrinsics", (extrinsics * modMat) );
    
    shader.setUniform4f("depthRect", depthRect.x, depthRect.y, depthRect.width, depthRect.height);
	shader.setUniform2f("depthPP", depthPrincipalPoint.x, depthPrincipalPoint.y);
	shader.setUniform2f("depthFOV", depthFOV.x, depthFOV.y);
    
    shader.setUniform4f("normalRect", normalRect.x, normalRect.y, normalRect.width, normalRect.height);
    
    shader.setUniform2f("simplify", simplify.x,simplify.y);
	shader.setUniform1f("farClip", farClip);
    shader.setUniform1f("nearClip", nearClip);
	shader.setUniform1f("edgeClip", edgeClip);
    
    //    cout << ( farClip - nearClip ) + nearClip << endl;
    //    cout << "FarClip: " << farClip << endl;
    //    cout << "nearClip: " << nearClip << endl;
    //    cout << "edgeClip: " << edgeClip << endl;
    
//    shader.setUniform2f("shift", shift.x, shift.y);
//    shader.setUniform2f("scale", scale.x, scale.y);
}

//--------------------------------------------------------------- ACTIONS
void CloudsRGBDCombinedRender::update(){
    if(simplify == ofVec2f(0,0)){
		setSimplification(ofVec2f(1.0, 1.0));
	}
    
	if(player.isLoaded()){
		player.update();
	}
}

bool CloudsRGBDCombinedRender::isPlaying(){
	return player.isLoaded() && player.isPlaying();
}

bool CloudsRGBDCombinedRender::isDone(){
	return player.isLoaded() && !player.isPlaying();
}

void CloudsRGBDCombinedRender::drawMesh(){
	draw(OF_MESH_FILL);
}
void CloudsRGBDCombinedRender::drawPointCloud(){
	draw(OF_MESH_POINTS);
}
void CloudsRGBDCombinedRender::drawWireFrame(){
	draw(OF_MESH_WIREFRAME);
}

void CloudsRGBDCombinedRender::draw(ofPolyRenderMode drawMode){
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