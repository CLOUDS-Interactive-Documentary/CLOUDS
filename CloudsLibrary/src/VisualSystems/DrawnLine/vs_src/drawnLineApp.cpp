#include "drawnLineApp.h"
#include "iostream.h"

#include "CloudsVisualSystem.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsGlobal.h"
#include "CloudsInput.h"


#define NROT 120


void threadedSeeker::threadedFunction(){
    
    bFoundMatch = false;
    
    //drawnLineApp * app = (drawnLineApp *) ofGetAppPtr() ;
    
    //ofPolyline line40 = app->nodeLine40;
    
    //cout << " here " << endl;
    
    for (int i = 0; i < polyPtrs.size(); i++){
        distancePP(line40, polyPtrs[i], 2);
    }
    sort(polyPtrs.begin(), polyPtrs.end(),sortFuncPP);
    
    for (int i = 0; i < polyPtrs.size()/4; i++){
        distancePP(line40, polyPtrs[i], 6);
    }
    sort(polyPtrs.begin(), polyPtrs.begin() + polyPtrs.size()/4,sortFuncPP);
    
    
    for (int i = 0; i < polyPtrs.size()/20; i++){
        distancePP(line40, polyPtrs[i], 20);
    }
    
    for (int i = 0; i < lastFound.size(); i++){
        for (int j = 0; j < polyPtrs.size()/20; j++){
            if (polyPtrs[j].whichLine == lastFound[i]){
                polyPtrs[j].distance = 100000000;
            }
        }
    }

    sort(polyPtrs.begin(), polyPtrs.begin() + polyPtrs.size()/20,sortFuncPP);
    
    if (sqrt(polyPtrs[0].distance) < thresh){
        
            bFoundMatch = true;
        
            match.matchA = nodeLineLast100.getVertices()[0];
            match.matchB = nodeLineLast100.getVertices()[99];
            match.idOfMatch = polyPtrs[0].whichLine;
            ofPolyline new40 = nodeLineLast100.getResampledByCount(40);;
            match.trans = normalizeLineSetGetTrans(new40);
            match.offset.set(0,0,0);
            match.startTime = ofGetElapsedTimef();
            match.matchLine = line40;
            match.bestAngle = polyPtrs[0].whichAngle;
        
            //matchStructs.push_back(match);
            //if (matchStructs.size() > 8) matchStructs.erase(matchStructs.begin());
            
            lastFound.push_back(polyPtrs[0].whichLine);
            if (lastFound.size() > 80) lastFound.erase(lastFound.begin());
            
            //lastMatchTime = ofGetElapsedTimef();
        //}
    }

}


//--------------------------------------------------------------
void drawnLineApp::setup(){

    
    seeker.bFoundMatch = false;
    seeker.thresh = 7;
    
    ofDirectory dir;
    
    dir.listDir( GetCloudsDataPath()  + "visualsystems/DrawnLine/other/" + "output");
    for (int i = 0; i < dir.size(); i++){
        string fileName = dir.getPath(i);
        
        lineSets.push_back( linesetFromFile(fileName) );
        lineSets[lineSets.size()-1].idMe = i;
        
        
        //int nRot = 100;
        ofMatrix4x4 mat;
        mat.makeRotationMatrix(360 / NROT, 0, 0, 1);
        
        ofPolyline temp = lineSets[lineSets.size()-1].normalized;
        
        
        for (int i = 0; i < NROT; i++){
            for (int j = 0; j < temp.size(); j++){
               temp[j] = temp[j] * mat;
            }
            ofPolyline * newLine = new ofPolyline();
            *newLine = temp;
            lineSets[lineSets.size()-1].normalizedDiffAngles.push_back(newLine);
        }
        
        distanceResult res;
        res.id = i;
        res.distance = 0;
        distanceResults.push_back(res);
    }
    
    
    for (int i = 0; i < lineSets.size(); i++){
        
        for (int j = 0; j < lineSets[i].normalizedDiffAngles.size(); j++){
            polyPtr pp;
            pp.distance = 0;
            pp.line = lineSets[i].normalizedDiffAngles[j];
            pp.whichAngle = j;
            pp.whichLine = i;
            
            seeker.polyPtrs.push_back(pp);
        }
    }
    
    
    BL.img = new ofImage();
    BL.img->loadImage(GetCloudsDataPath()  + "visualsystems/DrawnLine/other/" +"assets/spot.png");
    
    
    CL.setup();
    
    lastMatchTime = ofGetElapsedTimef();
    
    //ofSetVerticalSync(true);
    
//    TIME_SAMPLE_SET_FRAMERATE( 60.0f );
    
    
    shader.load(GetCloudsDataPath()  + "visualsystems/DrawnLine/other/" + "shader/shader");
    
    angleCatch = 0;
    matchEnergy = 0;
    scale = 1;
    
    ofFbo::Settings settings;
    //settings.useStencil = true;
    settings.height = 1080;
    settings.width = 1920;
    settings.internalformat = GL_RGBA;
    settings.numSamples = 2;
    fbo.allocate(settings);
    
//    
//    _defaultRenderer = ofGetCurrentRenderer();
//    _shivaVGRenderer = ofPtr<ofxShivaVGRenderer>(new ofxShivaVGRenderer);
//    ofSetCurrentRenderer(_shivaVGRenderer);
//    
//    _shivaVGRenderer->setLineJoinStyle(VG_JOIN_ROUND);
//    _shivaVGRenderer->setLineCapStyle(VG_CAP_ROUND);

}

//--------------------------------------------------------------
void drawnLineApp::update(){
    
    //if (ofGetFrameNum() % 30 == 0)shader.load("shader/shader");
    
    //matchEnergy
    
   
    
    
    if (ofGetElapsedTimef()- lastMatchTime > 1.0){
        matchEnergy = 0.95f * matchEnergy + 0.05 * 0.0;
    } else {
        matchEnergy = 0.95f * matchEnergy + 0.05 * 1.0;
    }
    
    
    //cout << scale << endl;
    
    if (CL.nodeLine.size() > 0){
        if (ofGetElapsedTimef()- lastMatchTime > 3.0){
            
            scale = 0.99 * scale + 0.01 * 1.0;
            catchPt = 0.97f * catchPt + 0.03 * CL.nodeLine[CL.nodeLine.size()-1];;
        } else {
            if (matchStructs.size() > 0){
                
                float pct = ((ofGetElapsedTimef()- lastMatchTime) / 3.0);
                float val = sin (pct * PI);
                
                
                
                ofRectangle origRect = matchStructs[matchStructs.size()-1].bounds;
                
                if (origRect.width > 0){
                ofRectangle screenRect = ofRectangle(0,0, SYS->getCanvasWidth(), SYS->getCanvasHeight());
                ofRectangle tranRect = origRect;
                tranRect.scaleTo(screenRect);
                
                
                float scaleTarget =  tranRect.width / origRect.width;
                    
                    scaleTarget = 0.17 * scaleTarget + 0.83 * 1.0;
                
                scale = 0.99 * scale + 0.01 * scaleTarget;
                }
                
                ofPoint ptToCatch = val * (matchStructs[matchStructs.size()-1].matchA + matchStructs[matchStructs.size()-1].matchB)/2.0 + (1-val) * CL.nodeLine[CL.nodeLine.size()-1];
                catchPt = 0.97f * catchPt + 0.03 * ptToCatch;
            } else {
                //scale = 0.97 * scale + 0.03 * 1.0;
                catchPt = 0.97f * catchPt + 0.03 * CL.nodeLine[CL.nodeLine.size()-1];;
            }
        }
    }
    
    
   
    
    
    
    
    CL.timeStep = ofMap(matchEnergy, 0, 1, 0.5, 0.01, true);
    
    
    
    
  //  TIME_SAMPLE_START("update");
    
    CL.update();
    
    
    if (bShowDebug){
        
        
        ofPoint trans(ofPoint(-1, 0));
        CL.translateNodeLine(ofPoint(-1, 0));
        
        for (int i = 0; i < matchStructs.size(); i++){
            matchStructs[i].matchA += trans * 2.0*CL.timeStep;
            matchStructs[i].matchB += trans * 2.0*CL.timeStep;
            matchStructs[i].offset += trans * 2.0*CL.timeStep;
        }
    
    } else {
        
        ofPoint trans(ofPoint(-1, 0));
        CL.translateNodeLine(ofPoint(-1, 0));
        
        for (int i = 0; i < matchStructs.size(); i++){
            matchStructs[i].matchA += trans * 2.0*CL.timeStep;
            matchStructs[i].matchB += trans * 2.0*CL.timeStep;
            matchStructs[i].offset += trans * 2.0*CL.timeStep;
        }

        
    }
    
    
    
    if (seeker.isThreadRunning() == false){
        if (seeker.bFoundMatch == true){
            lastMatchTime = ofGetElapsedTimef();
            matchStructs.push_back(seeker.match);
            //matchStructs.push_back(match);
            if (matchStructs.size() > 8) matchStructs.erase(matchStructs.begin());
            
            seeker.bFoundMatch = false;
        }
    }
    
    if (ofGetElapsedTimef()- lastMatchTime > 3.0){
        if (CL.matchCount % 10 == 0 && CL.nodeLineForMatch.size() > 100){
            if (seeker.isThreadRunning() == false && seeker.bFoundMatch == false){
                lookForGoodMatchSetup();
                seeker.startThread(true, false);
            }
        }
    }
    
 //   TIME_SAMPLE_STOP("update");
}

//--------------------------------------------------------------
void drawnLineApp::lookForGoodMatchSetup(){
    nodeLineLast100.clear();
    for (int i = CL.nodeLineForMatch.size()-100; i < CL.nodeLineForMatch.size(); i++){
        nodeLineLast100.addVertex(CL.nodeLineForMatch[i]);
        //cout << CL.nodeLineForMatch[i] << endl;
        
    }
    nodeLine40 = nodeLineLast100.getResampledByCount(40);
    nodeLine40 = returnNormalizedLine(nodeLine40);
    seeker.nodeLineLast100 = nodeLineLast100;
    seeker.line40 = nodeLine40;
}

void drawnLineApp::lookForGoodMatchWork(){

    
}


//--------------------------------------------------------------
void drawnLineApp::draw(){

    //return;
    
    //ofSetCurrentRenderer(_shivaVGRenderer);
    
    //
    ofSetLineWidth(3.4f); //- ofMap(scale, 0,3, 0, 1, true));
    fbo.begin();
    
    

   // TIME_SAMPLE_START("draw");
    
    shader.begin();
    

    if (CL.nodeLine.size()> 0){
        
    ofPoint pt = catchPt;
    ofMatrix4x4 mat;
    ofMatrix4x4 t1;
    ofMatrix4x4 t2;
        
    ofMatrix4x4 scaleMatrix;
    scaleMatrix.makeScaleMatrix(ofPoint(scale, scale, scale));
    t1.makeTranslationMatrix( -pt);
    t2.makeTranslationMatrix( ofPoint(SYS->getCanvasWidth()/2, SYS->getCanvasHeight()/2));
    ofMatrix4x4 res;

        float angle = 0;
        if (matchStructs.size() > 0){
            angle = (360 / NROT) * matchStructs[matchStructs.size()-1].bestAngle;
            
            //ofMatrix4x4 rotate; rotate.makeRotationMatrix( (360 / NROT) * match.bestAngle, 0, 0, 1 );
        }
        
        
        angleCatch = ofLerpDegrees(angleCatch, angle, 0.003);
        
    mat.makeRotationMatrix(-angleCatch, 0, 0, 1);
    
    res = t1 * mat * scaleMatrix * t2;
    
    shader.setUniformMatrix4f("matrix", res);
    } else {
       // ofMatrix4x4 mat;
        
       //shader.setUniformMatrix
    }
    
    ofBackground(0);
    ofPushStyle();
    
    BL.size = 1.8;
    BL.drawPolyline(&CL.nodeLine);

    
 ofSetLineWidth( 2.0);
  ///  ofSetLineWidth( 1.7);// - ofMap(scale, 0,3, 0, 0.4, true));
    for (int i = 0; i < matchStructs.size(); i++){
        
        //cout << matchStructs[i].offset << endl;
        //if (matchStructs[i].offset.x < -400) continue;
        
        drawLineSet( lineSets[matchStructs[i].idOfMatch],matchStructs[i], matchStructs[i].matchA, matchStructs[i].matchB);

        ofNoFill();
        //ofRect(matchStructs[i].bounds);
        
        
        ofSetColor(ofColor::pink);
        ofPushMatrix();
        
        //ofTranslate(mouseX, mouseY);
        
        if (i == matchStructs.size()-1){
            //matchStructs[i].matchLine.draw();
        }
        ofPopMatrix();
        //ofLine(matchStructs[i].matchA, matchStructs[i].matchB);
    }
    
    
    ofPopStyle();
    
    shader.end();
    
   // TIME_SAMPLE_STOP("draw"); /////////////////////////////////////////////////  STOP MEASURING  ///
	
    fbo.end();
    
    
    
    ofSetColor(255,255,255);
    fbo.draw(0,0);
    
    
	//TIME_SAMPLE_DRAW( 10, 10); 	//finally draw our time measurements

    
    //ofTranslate(mouseX, mouseY);
    
    
    
    
    //nodeLine40.draw();
    //nodeLineLast100.draw();
    
}

//--------------------------------------------------------------
void drawnLineApp::keyPressed(int key){

}

//--------------------------------------------------------------
void drawnLineApp::keyReleased(int key){

}

//--------------------------------------------------------------
void drawnLineApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void drawnLineApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void drawnLineApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void drawnLineApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void drawnLineApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void drawnLineApp::gotMessage(ofMessage msg){

}


void drawnLineApp::drawLineSet(lineSet & set, matchStruct & match, ofPoint ptA, ofPoint ptB){
    
    
    //int nRot = 100;
    
    
    ofMatrix4x4 inverse = match.trans.mat.getInverse();
    ofMatrix4x4 inverse2 = match.trans.mat2.getInverse();
    
    //cout << match.bestAngle << endl;
    ofMatrix4x4 rotate; rotate.makeRotationMatrix( (360 / NROT) * match.bestAngle, 0, 0, 1 );
    
    float pct = ofMap( ofGetElapsedTimef()- match.startTime, 0, 1.5, 0, 1, true);
    
    int nTotal = 0;
    for (int i = 0; i < set.normalizeLines.size(); i++){
        nTotal +=set.normalizeLines[i].size();
    }
    
    int nCountTo = nTotal * pct;
    //cout << nCountTo << endl;
    int count = 0;
    
    //match.bounds.set( match.offset.x, match.offset.y, 1,1);
    
    
    for (int i = 0; i < set.normalizeLines.size(); i++){
        
        ofEnableAlphaBlending() ;
        
        if (i == 0)  bShowDebug == false ? ofSetColor(255,255,255,0) : ofSetColor(ofColor::cyan);
        else ofSetColor(ofColor::white);
        
        ofPolyline line;
        //ofMesh mesh;
        //mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
        for (int j = 0; j < set.normalizeLines[i].size(); j++){
            
            ofPoint newPt = set.normalizeLines[i][j] * rotate * inverse2 * inverse;
            
            if (count < nCountTo){
                line.addVertex(newPt + match.offset);
                
                
            }
            
            
            if (i > 0){
                if (i == 1 && j == 0) {
                    match.bounds.set((newPt + match.offset).x, (newPt + match.offset).y, 1,1);
                } else {
                    match.bounds.growToInclude(newPt + match.offset);
                    
                }
            }
            
            count++;
        }
        
        if (line.getVertices().size() > 0){
            
            ofRectangle rect(0,0,SYS->getCanvasWidth(), SYS->getCanvasHeight());
            if (rect.intersects( match.bounds)){
                //cout << match.bounds << endl;
                line.draw();
            }
        }
    }
}
//--------------------------------------------------------------
lineSet & drawnLineApp::linesetFromFile( string fileName ){
    
    set.lines.clear();
    
    ifstream myFile (ofToDataPath(fileName).c_str(), ios::out | ios::binary);
    
    int howMany;
    myFile.read((char *)&howMany, sizeof(int));
    
    int * lengths = new int[howMany];
    for(int i = 0; i < howMany; i++){
        myFile.read((char *)&lengths[i], sizeof(int));
    }
    
    for(int i = 0; i < howMany; i++){
        ofPolyline line;
        float pt[2];
        for(int j = 0; j < lengths[i]; j++){
            
            myFile.read((char *)&pt[0], sizeof(float));
            myFile.read((char *)&pt[1], sizeof(float));
            
            line.addVertex(ofPoint(pt[0], pt[1]));
        }
        
        set.lines.push_back(line);
    }
    
    myFile.close();
   
    ofPolyline lineAt40 = set.lines[0].getResampledByCount(40);
    set.normalized = returnNormalizedLine(lineAt40);
    normalizeLineSet(set, lineAt40);
    
    return set;
}

//--------------------------------------------------------------
void drawnLineApp::dragEvent(ofDragInfo dragInfo){
    
    
    
    
    

}
