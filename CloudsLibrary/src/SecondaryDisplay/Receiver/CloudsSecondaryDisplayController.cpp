//
//  CloudsSecondaryDisplayController.cpp
//  CloudsSecondaryDisplay
//
//  Created by James George on 12/23/13.
//
//

#include "CloudsSecondaryDisplayController.h"
#include "CloudsGlobal.h"

CloudsSecondaryDisplayController::CloudsSecondaryDisplayController(){
	hasSpeaker = false;
	playingMovie = false;
    displayMode = "BIO";
    lightBlue = ofColor::fromHex(0x97d7fb);
    darkBlue = ofColor::fromHex(0x439ced);
    color = false;
}

void CloudsSecondaryDisplayController::setup(){

	parser.loadFromFiles();
	
	clusterMap.buildEntireCluster(parser);

	clusterMap.forceScreenResolution(1920, 1080);
	clusterMap.setDrawToScreen(false);
	
	clusterMap.setup();
	clusterMap.playSystem();

	receiver.setup(12346);
	
    
    
	loadSVGs();
    
    //FONT SIZES ARE IN POINTS
    //1 pixel = .75pts
   // float fScale = .75;
    
    //load a nunch of fonts at different sizes
    int minFontSize = 1;
    int maxFontSize = 80;
    
    for( int i=minFontSize; i<maxFontSize; i++){
        ofxFTGLFont *tmp = new ofxFTGLFont();
        tmp->loadFont( GetCloudsDataPath() + "font/Blender-THIN.ttf", i );
        tempFontList.push_back( tmp );
    }
    
    //setup references to all meshes that need references
    meshQuestion = questionLayout.getMeshByID("TEXTBOX_x5F_QUESTION_1_");
    ////for bio
    meshBioFirstName = bioLayout.getMeshByID("TEXTBOX_x5F_FIRSTNAME_2_");
    meshBioLastName = bioLayout.getMeshByID("TEXTBOX_x5F_LASTNAME_1_");
    meshBioTitle = bioLayout.getMeshByID("TEXTBOX_x5F_TITLE_1_");
    meshBioLocation = bioLayout.getMeshByID("TEXTBOX_x5F_LOC_1_");
    meshBioDescription = bioLayout.getMeshByID("TEXTBOX_x5F_BIO");
    ////for project example
    meshProjectVideo = projectLayout.getMeshByID("BOX_x5F_VIDEO");
    meshProjectTitle = projectLayout.getMeshByID("TEXTBOX_x5F_TITLE_1_");
    meshProjectArtist = projectLayout.getMeshByID("TEXTBOX_x5F_ARTIST_1_");
    meshProjectDescription = projectLayout.getMeshByID("TEXTBOX_x5F_DESC_1_");

    //load all fonts
    ////last name
    h1 = getLayoutForLayer(meshBioLastName);
    ////first name
    h2 = getLayoutForLayer(meshBioFirstName);
    ////question
    h3 = getLayoutForLayer(meshQuestion);
    ////locations
    h4 = getLayoutForLayer(meshBioLocation);
    ////byline / description
    p = getLayoutForLayer(meshBioDescription);

	displayTarget.allocate(1920, 1080, GL_RGB);
    // cleanup!
    for( int i=0; i<tempFontList.size(); i++ ){
        delete tempFontList[i];
    }
    tempFontList.clear();
}

/*LOADING SVG LAYOUT files from Sarah*/
void CloudsSecondaryDisplayController::loadSVGs(){
	/*ofDirectory svgs(GetCloudsDataPath() + "secondaryDisplay/SVG/BIO/");
	svgs.allowExt("svg");
	svgs.listDir();
    //loading all the SVG files in the BIO dir, but why?
	for(int i = 0; i < svgs.numFiles(); i++){
		testAllLayout.push_back(CloudsSVGMesh());
		testAllLayout.back().load(svgs.getPath(i));
	}*/
    
    //load the three different layouts
    bioLayout.load(GetCloudsDataPath() + "secondaryDisplay/SVG/BIO/BIO.svg");
    projectLayout.load(GetCloudsDataPath() + "secondaryDisplay/SVG/PROJECTEX/PROJECTEX.svg");
    questionLayout.load(GetCloudsDataPath() + "secondaryDisplay/SVG/QUESTION/QUESTION.svg");

}

ofxFTGLSimpleLayout* CloudsSecondaryDisplayController::getLayoutForLayer( SVGMesh* textMesh ) {
    
        if( textMesh != NULL ){
            int fontSize = getFontSizeForMesh( textMesh );
            // make a layout
            ofxFTGLSimpleLayout *newLayout = new ofxFTGLSimpleLayout();
            newLayout->loadFont( GetCloudsDataPath() + "font/Blender-THIN.ttf", fontSize );
            newLayout->setLineLength( textMesh->bounds.width );
            
            return newLayout;
        }
    
    return NULL;
}

int CloudsSecondaryDisplayController::getFontSizeForMesh( SVGMesh* textMesh ){
    if( !textMesh ){
        ofLogError("CloudsHUDController :: Text box not found");
        return 0;
    }
    
    int fontSize = 0;
    float textBoxHeight = textMesh->bounds.height;
    
    for( int k=0; k<tempFontList.size()-1; k++){
        float f1h = tempFontList[k]->getStringBoundingBox("M", 0, 0).height;
        float f2h = tempFontList[k+1]->getStringBoundingBox("M", 0, 0).height;
        if( f1h <= textBoxHeight && f2h > textBoxHeight ){
            fontSize = 1 + k;
            break;
        }
    }
    
    return fontSize;
}

void CloudsSecondaryDisplayController::update(){
	
	//TODO: fix with perma preset
	clusterMap.incrementalTraversalMode = true;
	
	while(receiver.hasWaitingMessages()){
		ofxOscMessage m;
		receiver.getNextMessage(&m);
		
		//all the args sent for clip for reference
//0		m.addStringArg(clip.person);//final cut person id
//1		m.addStringArg(clip.getID());//clip id
//2		m.addFloatArg(clip.getDuration());//duraiton
//3		m.addStringArg(currentTopic); //topic
//4		m.addStringArg(clip.projectExampleTitle); //example
//5		m.addStringArg(lastQuestionAsked); //question

		if(m.getAddress() == "/clip"){
			hasSpeaker = true;
			currentSpeaker = CloudsSpeaker::speakers[m.getArgAsString(0)];
			currentClip = parser.getClipWithID(m.getArgAsString(1));
			
			clusterMap.traverseToClip(currentClip);
			
			string exampleId = m.getArgAsString(4);
			if(exampleId != ""){
                displayMode = "PROJECT";
				//need to do something smarter here
				currentExample = parser.getProjectExampleWithTitle(exampleId);
				if(currentExample.exampleVideos.size() > 0){
					playingMovie = archivePlayer.loadMovie(currentExample.exampleVideos[0]);
					if(playingMovie){
						archivePlayer.setLoopState(OF_LOOP_NONE);
						archivePlayer.play();
					}
				}
			}
			else{
                displayMode = "BIO";
				playingMovie = false;
				archivePlayer.stop();
                
                //setup all bio data
                lastQuestion = m.getArgAsString(5);
                cout << "lastQuestion: '" << lastQuestion << "'";
			}
		}
	}
	
	
	if(playingMovie){
		archivePlayer.update();
	}
}

void CloudsSecondaryDisplayController::draw(){
    ofEnableAlphaBlending();
	
	displayTarget.begin();
	
	clusterMap.selfPostDraw();

    SVGMesh* t;
    
    if(displayMode == "BIO"){
        //DRAW BIO LAYOUT, need to draw this first, text goes over it
        bioLayout.draw();
        
        ////question
        ////// don't display if not avilable
        //find the question box
        if(lastQuestion != ""){
            //show the question box
            questionLayout.draw();
            //find the text box
            lastQuestion = ofToUpper(lastQuestion);
            drawTextToMesh(h3, lastQuestion, meshQuestion);
         //   if(debug)
           //     ofRect(t->bounds);
        }
        
        ////speaker name
        string firstName, lastName;
        if(hasSpeaker){
            firstName = currentSpeaker.firstName;
            lastName = currentSpeaker.lastName;
        }
        else{
            firstName = "NO";
            lastName = "SPEAKER";
        }
        
        //DRAW SPEAKER NAME
        ////first name
        firstName = ofToUpper(firstName);
        drawTextToMesh(h2, firstName, meshBioFirstName);
        
        ////last name
        lastName = ofToUpper(lastName);
        drawTextToMesh(h1, lastName, meshBioLastName);
        
        float firstNameWidth = h2->getStringBoundingBox(firstName, 0, 0).width;
        float lastNameWidth = h1->getStringBoundingBox(lastName, 0, 0).width;
        float longestNameWidth;
        if(firstNameWidth > lastNameWidth)
            longestNameWidth = firstNameWidth;
        else
            longestNameWidth = lastNameWidth;
        
        float margin = 60;
        float titleX = meshBioFirstName->bounds.x + longestNameWidth + margin;
        
        ////title
        string title = ofToUpper(currentSpeaker.title);
        meshBioTitle->bounds.width = 9999;
        //reposition title to float left
        meshBioTitle->bounds.x = titleX;
        
        if(color)
            ofSetColor(lightBlue);
        drawTextToMesh(h4, title, meshBioTitle);
        if(debug)
            ofRect(t->bounds);
        
        ////location
        if(color)
            ofSetColor(darkBlue);

        string loc = ofToUpper(currentSpeaker.location2);
        meshBioLocation->bounds.x = titleX;
        meshBioLocation->bounds.width = 9999;
        drawTextToMesh(h4, loc, meshBioLocation);
        
        if(color)
            ofSetColor(255);
        
        ////byline / bio / description
        drawTextToMesh(p, currentSpeaker.byline1, meshBioDescription);

        
        
        
    }else if(displayMode == "PROJECT"){
        //DISPLAY PROJECT LAYOUT
        projectLayout.draw();
        
        //video
        if(playingMovie){
            //scale and preserve the aspect ratio
            ofRectangle playerRect(0,0,archivePlayer.getWidth(), archivePlayer.getHeight());
            playerRect.scaleTo(meshProjectVideo->bounds);
            archivePlayer.draw(playerRect);
            playingMovie = archivePlayer.isPlaying();
        }
        
        ////project title
            string title = ofToUpper(currentExample.title);
            drawTextToMesh(h2, title, meshProjectTitle);
        
        ////artist name
            string name = currentExample.creatorName;
            drawTextToMesh(h4, name, meshProjectArtist);
        
        ////project description
            drawTextToMesh(p, currentExample.description, meshProjectDescription);
        
    }
	
	displayTarget.end();
	
	ofRectangle screenRect(0,0,ofGetWidth(), ofGetHeight());
	ofRectangle targetRect(0,0,displayTarget.getWidth(),displayTarget.getHeight());
	targetRect.scaleTo(screenRect);
	displayTarget.getTextureReference().draw(targetRect);
	
	
}

/*void CloudsSecondaryDisplayController::drawBioLayout(){
    
}*/

void CloudsSecondaryDisplayController::drawTextToMesh(ofxFTGLSimpleLayout* font, string text, SVGMesh* mesh){
    //update line length
    font->setLineLength(mesh->bounds.width);
    //draw the text
    font->drawString(text, mesh->bounds.x, mesh->bounds.y + font->getStringBoundingBox("M", 0, 0).height);
}

