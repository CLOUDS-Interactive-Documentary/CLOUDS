#include "CloudsRGBDCombinedExporter.h"

CloudsRGBDCombinedExporter::CloudsRGBDCombinedExporter(){
	minDepth = 0;
	maxDepth = 0;
    
	renderer = NULL;
	player = NULL;
    
	videoRectangle = ofRectangle(0,0,1280,720);
}

CloudsRGBDCombinedExporter::~CloudsRGBDCombinedExporter(){
	
}

void CloudsRGBDCombinedExporter::setRenderer(ofxRGBDCPURenderer* renderer){
	this->renderer = renderer;
    this->renderer->setSimplification(ofVec2f(1,1));
}

void CloudsRGBDCombinedExporter::setPlayer(ofxRGBDPlayer* player){
	this->player = player;
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
    
	writeMetaFile(outputPath);
	
	outputImage.allocate(videoRectangle.getWidth(), videoRectangle.getHeight() + 480, OF_IMAGE_COLOR);
	
    int counter = 1;
	for(int i = inoutPoint.min; i < inoutPoint.max; i++){
		
		//COPY video pixels into buffer
		ofPixels temp = player->getVideoPlayer()->getPixelsRef();
		temp.resize(videoRectangle.width, videoRectangle.height);
		temp.pasteInto(outputImage, 0, 0);
        
		ofShortPixels& p = player->getDepthPixels();
        ofRectangle depthBox;
        depthBox.x = 0;
        depthBox.y = videoRectangle.getHeight();
        depthBox.width = p.getWidth();
        depthBox.height = p.getHeight();
        
		for(int y = 0; y < depthBox.height; y++){
			for(int x = 0; x < depthBox.width; x++){
				outputImage.setColor(x + depthBox.x,
                                     y + depthBox.y,
                                     getColorForZDepth(p.getPixels()[ p.getPixelIndex(x, y)] ));
			}
		}
		
        //  Update the mesh
        //
        renderer->update();
        renderer->farClip;
        //  Process Normals on PCL
        //
        ofxPCL::PointCloud pc;
        ofxPCL::convert( renderer->getReducedMesh(false), pc);
        
        ofxPCL::PointNormalPointCloud pc_n;
        ofxPCL::normalEstimation(pc, pc_n );
        
        
        //  Make a new mesh with that information
        //
        ofMesh mesh = ofxPCL::toOF(pc_n);
        //memset(outputImage.getPixels(), 0, outputImage.getWidth()*outputImage.getHeight()*3);
        cout << "normals generated, building image for " << renderer->validVertIndices.size() << " verts " << endl;
        
        //  Define the are where the normals will be;
        //
        ofRectangle normalsBox;
        normalsBox.x = p.getWidth();
        normalsBox.y = videoRectangle.getHeight();
        normalsBox.width = p.getWidth();
        normalsBox.height = p.getHeight();
        
        //  Clean this area
        //
        for(int y = 0; y < normalsBox.height; y++){
			for(int x = 0; x < normalsBox.width; x++){
				outputImage.setColor(x + normalsBox.x,
                                     y + normalsBox.y,
                                     ofColor(0) );
			}
		}
        
        //  Use the new mesh and the valid verteces ( from the original ) to make an image
        //
        for(int i = 0; i < renderer->validVertIndices.size(); i++){
            ofVec3f norm = ( mesh.getNormals()[ i ] + ofVec3f(1.0, 1.0, 1.0) ) / 2.0;
            pair<int,int> pixelCoord = renderer->getPixelLocationForIndex( renderer->validVertIndices[i]  );
            outputImage.setColor(640 + pixelCoord.first,
                                 videoRectangle.getHeight() + pixelCoord.second, ofColor(norm.x*255,norm.y*255,norm.z*255) );
        }
        
		char filename[1024];
		sprintf(filename, "%s/%s%05d.png", outputPath.c_str(), clipName.c_str(), player->getVideoPlayer()->getCurrentFrame());
		ofSaveImage(outputImage, filename);
		
		player->getVideoPlayer()->nextFrame();
		player->getVideoPlayer()->update();
		player->update();
        
        counter++;
	}
    
    
}

void CloudsRGBDCombinedExporter::writeMetaFile(string outputDirectory){
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
	
	calibration.addValue("minDepth", minDepth);
	calibration.addValue("maxDepth", maxDepth);
	
	//calibration.save(outputDirectory + "/_calibration.xml");
	calibration.saveFile(outputDirectory + "/_calibration.xml");
	
}

ofColor CloudsRGBDCombinedExporter::getColorForZDepth(unsigned short z){
	if(z > maxDepth || z < minDepth){
		return ofColor(0,0,0);
	}
	
	float colorPoint = ofMap(z, minDepth, maxDepth, 0, 255, true);
	return ofColor::fromHsb(colorPoint, 255, 255);
}
