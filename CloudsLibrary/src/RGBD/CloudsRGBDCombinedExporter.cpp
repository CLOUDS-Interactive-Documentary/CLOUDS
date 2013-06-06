#include "CloudsRGBDCombinedExporter.h"

#undef Nil
#include "ofxPCL.h"
#define Nil NULL


CloudsRGBDCombinedExporter::CloudsRGBDCombinedExporter(){
    
	renderer = NULL;
	player = NULL;
	
	videoRectangle = ofRectangle(0,0,1280,720);
}

CloudsRGBDCombinedExporter::~CloudsRGBDCombinedExporter(){
	
}

void CloudsRGBDCombinedExporter::prepare(){
	foundFirstFace = false;
	lastFaceFrameFound = 0;
	inFace = false;
	tracker.setup();

}

void CloudsRGBDCombinedExporter::setRenderer(ofxRGBDCPURenderer* renderer){
	this->renderer = renderer;
    this->renderer->setSimplification(ofVec2f(1,1));
}

void CloudsRGBDCombinedExporter::setPlayer(ofxRGBDPlayer* player){
	this->player = player;
}

void CloudsRGBDCombinedExporter::writeMetaFile(string outputDirectory, ofxRGBDCPURenderer* renderer){
	//write calibration into an xml file
    
	ofxXmlSettings calibration;
	calibration.addTag("depthIntrinsics");
	calibration.pushTag("depthIntrinsics");
	
	calibration.addValue("ppx", renderer->depthPrincipalPoint.x);
	calibration.addValue("ppy", renderer->depthPrincipalPoint.y);
	calibration.addValue("fovx", renderer->depthFOV.x);
	calibration.addValue("fovy", renderer->depthFOV.y);
	calibration.addValue("width", renderer->depthImageSize.width);
	calibration.addValue("height", renderer->depthImageSize.height);
	calibration.popTag();//depthIntrinsics
	
	calibration.addTag("colorIntrinsics");
	calibration.pushTag("colorIntrinsics");
	calibration.addValue("ppx", renderer->colorPrincipalPoint.x);
	calibration.addValue("ppy", renderer->colorPrincipalPoint.y);
	calibration.addValue("fovx", renderer->colorFOV.x);
	calibration.addValue("fovy", renderer->colorFOV.y);
	calibration.addValue("width", renderer->colorImageSize.width);
	calibration.addValue("height", renderer->colorImageSize.height);
	
	calibration.addTag("dK");
	calibration.pushTag("dK");
	for(int i = 0; i < 3; i++) calibration.addValue("k" + ofToString(i), renderer->distortionK[i] );
	calibration.popTag();//dK
    
	calibration.addTag("dP");
	calibration.pushTag("dP");
	for(int i = 0; i < 2; i++) calibration.addValue("p" + ofToString(i), renderer->distortionP[i] );
	calibration.popTag();//dP
    
	calibration.popTag();//colorIntrinsics
    
	calibration.addTag("extrinsics");
	calibration.pushTag("extrinsics");
	calibration.addTag("rotation");
	calibration.pushTag("rotation");
	for(int i = 0; i < 9; i++) calibration.addValue("r" + ofToString(i), renderer->depthToRGBRotation[i] );
	calibration.popTag();
	
	calibration.addTag("translation");
	calibration.pushTag("translation");
	for(int i = 0; i < 3; i++) calibration.addValue("t" + ofToString(i), renderer->depthToRGBTranslation[i]);
	calibration.popTag();//translation
	
	calibration.popTag();//extrinsics
	
	calibration.addTag("adjustment");
	calibration.pushTag("adjustment");
	
	
	calibration.addTag("translate");
	calibration.pushTag("translate");
	calibration.addValue("x", renderer->colorMatrixTranslate.x);
	calibration.addValue("y", renderer->colorMatrixTranslate.y);
	calibration.addValue("z", renderer->colorMatrixTranslate.z);
	calibration.popTag();
	
	calibration.addTag("rotate");
	calibration.pushTag("rotate");
	calibration.addValue("x", renderer->colorMatrixRotate.x);
	calibration.addValue("y", renderer->colorMatrixRotate.y);
	calibration.addValue("z", renderer->colorMatrixRotate.z);
	calibration.popTag();
	
	calibration.addTag("scale");
	calibration.pushTag("scale");
	calibration.addValue("x", renderer->scale.x);
	calibration.addValue("y", renderer->scale.y);
	calibration.popTag();
	
	calibration.addTag("depth");
	calibration.pushTag("depth");
	calibration.addValue("min", renderer->nearClip);
	calibration.addValue("max", renderer->farClip);
	calibration.popTag();
	
	calibration.popTag();
		
	
	calibration.popTag();//adjustment
	
	calibration.saveFile(outputDirectory + "/_calibration.xml");
}

void CloudsRGBDCombinedExporter::render(string outputPath, string clipName){
	
	if(player == NULL){
		ofLogError("CloudsRGBDCombinedExporter::render -- player is null");
		return;
	}
    
	if(renderer == NULL){
		ofLogError("CloudsRGBDCombinedExporter::render -- renderer is null");
		return;
	}
    
	writeMetaFile(outputPath, renderer);
	
    int counter = 1;
	for(int i = inoutPoint.min; i < inoutPoint.max; i++){
		//  Update the mesh
		//
		renderer->update();
		renderer->farClip;

		renderFrame(outputPath, clipName, renderer, player->getVideoPlayer()->getPixelsRef(), player->getVideoPlayer()->getCurrentFrame());
		
		player->getVideoPlayer()->nextFrame();
		player->getVideoPlayer()->update();
		player->update();
        
        counter++;
	}
}

void CloudsRGBDCombinedExporter::renderFrame(string outputPath, string clipName, ofxRGBDCPURenderer* rgbdRenderer, ofPixelsRef videoPixels, int frameNum){


	if(!outputImage.isAllocated() ||
	   outputImage.getWidth() != videoRectangle.getWidth() ||
	   outputImage.getHeight() != videoRectangle.getHeight() + 480 + 360)
	{
		outputImage.allocate(videoRectangle.getWidth(), videoRectangle.getHeight() + 480 + 360, OF_IMAGE_COLOR);
	}

	//COPY video pixels into buffer
	ofPixels temp = videoPixels;
	temp.resize(videoRectangle.width, videoRectangle.height);
	temp.pasteInto(outputImage, 0, 0);
	
	ofShortPixels& p = rgbdRenderer->getDepthImage();
	ofRectangle depthBox;
	depthBox.x = 0;
	depthBox.y = videoRectangle.getHeight();
	depthBox.width = p.getWidth();
	depthBox.height = p.getHeight();
	
	for(int y = 0; y < depthBox.height; y++){
		for(int x = 0; x < depthBox.width; x++){
			outputImage.setColor(x + depthBox.x,
								 y + depthBox.y,
								 getColorForZDepth(p.getPixels()[ p.getPixelIndex(x, y)], rgbdRenderer->nearClip, rgbdRenderer->farClip));
		}
	}
	
	//  Process Normals on PCL
	//
	ofxPCL::PointCloud pc;
	ofMesh mesh;
	rgbdRenderer->getReducedMesh(mesh, false);
	ofxPCL::convert(mesh, pc);
	
	ofxPCL::PointNormalPointCloud pc_n;
	ofxPCL::normalEstimation(pc, pc_n );
	
	//  Make a new mesh with that information
	//
	mesh = ofxPCL::toOF(pc_n);
//	memset(outputImage.getPixels(), 0, outputImage.getWidth()*outputImage.getHeight()*3);
//	cout << "normals generated, building image for " << rgbdRenderer->validVertIndices.size() << " verts " << endl;
	
	//  Define the are where the normals will be;
	//
	ofRectangle normalsBox;
	normalsBox.x = p.getWidth();
	normalsBox.y = videoRectangle.getHeight();
	normalsBox.width = p.getWidth();
	normalsBox.height = p.getHeight();
	
	//  Clean this area
	//
	//TODO: paste in blank pixels?
	for(int y = 0; y < normalsBox.height; y++){
		for(int x = 0; x < normalsBox.width; x++){
			outputImage.setColor(x + normalsBox.x,
								 y + normalsBox.y,
								 ofColor(0) );
		}
	}
	
	
	ofRectangle faceTargetRectangle(0,normalsBox.getMaxY(),640,360);
	
	//  Use the new mesh and the valid verteces ( from the original ) to make an image
	//
	
	for(int i = 0; i < rgbdRenderer->validVertIndices.size(); i++){
		ofVec3f norm = ( mesh.getNormals()[ i ] + ofVec3f(1.0, 1.0, 1.0) ) / 2.0;
		pair<int,int> pixelCoord = rgbdRenderer->getPixelLocationForIndex( rgbdRenderer->validVertIndices[i]  );
		outputImage.setColor(640 + pixelCoord.first,
							 videoRectangle.getHeight() + pixelCoord.second, ofColor(norm.x*255,norm.y*255,norm.z*255) );
	}
	
	if(!faceFrame.isAllocated() || videoPixels.getWidth() != faceFrame.getWidth() || videoPixels.getHeight() != faceFrame.getHeight()){
		faceFrame.allocate(videoPixels.getWidth(), videoPixels.getHeight(), OF_IMAGE_COLOR);
	}
	
	if(!cairoRenderer.getImageSurfacePixels().isAllocated() ||
	   cairoRenderer.getImageSurfacePixels().getWidth() !=  videoPixels.getWidth() ||
	   cairoRenderer.getImageSurfacePixels().getHeight() !=  videoPixels.getHeight())
	{
	   cairoRenderer.setupMemoryOnly(ofCairoRenderer::IMAGE, false, false, ofRectangle(0,0,videoPixels.getWidth(), videoPixels.getHeight()) );
	}
	
	//face extract
	bool foundFace = tracker.update(ofxCv::toCv(videoPixels));
	//bool foundFace = false;
	if(foundFace){
		ofPolyline leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
		ofPolyline rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);
		ofPolyline faceOutline = tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
		
		if(!inFace && foundFirstFace){
			
			ofPolyline interpLeftEye;
			ofPolyline interpRightEye;
			ofPolyline interpFace;
			
			for(int i = lastFaceFrameFound + 1; i < frameNum; i++){
				float delta = ofMap(i, lastFaceFrameFound, frameNum, 0, 1.0);
				
				//LOAD IMAGE
				char filename[1024];
				sprintf(filename, "%s/%s_%05d.png", outputPath.c_str(), clipName.c_str(), i);
				ofPixels pix;
				if(!ofLoadImage(pix, filename)){
					ofLogError() << "Retro face fail -- Couldn't load image " << filename << endl;
					continue;
				}
				
				interpolatePolyLine(lastLeftEye, leftEye, interpLeftEye, delta);
				interpolatePolyLine(lastRightEye, rightEye, interpRightEye, delta);
				interpolatePolyLine(lastFace, faceOutline, interpFace, delta);
				
				//ADD FACE
				addFaceToPixels(outputImage, faceFrame, faceTargetRectangle,
								interpLeftEye, interpRightEye, interpFace);
				
				cout << "RE-SAVING filename with face " << filename << endl;
				//SAVE IMAGE AGAIN
				ofSaveImage(pix, filename);
			}
		}
		
		foundFirstFace = true;
		inFace = true;
		
		lastLeftEye = leftEye;
		lastRightEye = rightEye;
		lastFace = faceOutline;

		addFaceToPixels(outputImage, faceFrame, faceTargetRectangle,
						leftEye, rightEye, faceOutline);

		lastFaceFrameFound = frameNum;
	}
	else{
		ofLogError() << " NO FACE FOUND FOR CLIP " << clipName << " FRAME " << ofToString(frameNum);
		inFace = false;
	}
	
	char filename[1024];
	sprintf(filename, "%s/%s_%05d.png", outputPath.c_str(), clipName.c_str(), frameNum);
	cout << "Saving filename " << filename << endl;
	ofSaveImage(outputImage, filename);
}

void CloudsRGBDCombinedExporter::interpolatePolyLine(ofPolyline& a, ofPolyline& b, ofPolyline& out, float delta){
    if(a.getVertices().size() != b.getVertices().size()){
        ofLogError("Polylines did not match in size");
        return;
    }
    
    out.clear();
    
    for(int i = 0; i < a.getVertices().size(); i++){
        out.addVertex( a.getVertices()[i].getInterpolated(b.getVertices()[i], delta) );
    }
}


void CloudsRGBDCombinedExporter::addFaceToPixels(ofPixelsRef& targetPixels, ofPixelsRef& tempPixels, ofRectangle target,
												 ofPolyline& leftEye, ofPolyline& rightEye, ofPolyline& faceOutline){

	ofPath faceShape;
	for(int i = 0; i < faceOutline.getVertices().size(); i++){
		faceShape.lineTo(faceOutline.getVertices()[i]);
	}
	faceShape.close();
	faceShape.setFillColor(ofColor(0,0,255));
	cairoRenderer.setupGraphicDefaults();
	cairoRenderer.background(0);
	cairoRenderer.setColor(0, 0, 255);
	cairoRenderer.draw(faceShape);
	cairoRenderer.flush();
	
	ofPixels resized = cairoRenderer.getImageSurfacePixels();
	resized.setImageType(OF_IMAGE_COLOR);
	resized.resize(target.getWidth(), target.getHeight());
//	cout << "pasting image of size " << resized.getWidth() << " " << resized.getHeight() << " into " << target.x << " " << target.y <<  endl;
//	cout << "Target image size is " << targetPixels.getWidth() << " " << targetPixels.getHeight() << endl;
	resized.pasteInto(targetPixels, target.x, target.y);
}

ofColor CloudsRGBDCombinedExporter::getColorForZDepth(unsigned short z, float minDepth, float maxDepth){
	if(z > maxDepth || z < minDepth){
		return ofColor(0,0,0);
	}
	
	float colorPoint = ofMap(z, minDepth, maxDepth, 0, 255, true);
	return ofColor::fromHsb(colorPoint, 255, 255);
}
