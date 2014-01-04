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
    displayMode = "NONE";
    lightBlue = ofColor::fromHex(0x97d7fb);
    darkBlue = ofColor::fromHex(0x439ced);
    color = false;
    stringCounter = 0;
    tx = .1;
}

void CloudsSecondaryDisplayController::setup(){
    
	parser.loadFromFiles();
	
	clusterMap.forceScreenResolution(1920, 1080);
	clusterMap.setNumSamples(4);
	clusterMap.setDrawToScreen(false);

	clusterMap.setup();
	clusterMap.playSystem();

	clusterMap.loadPresetGUISFromName("2DFollowCam");
	clusterMap.buildEntireCluster(parser);

    //setup OSC reciever
	receiver.setup(123456);
	loadSVGs();
    
    //FONT SIZES ARE IN POINTS
    //load a bunch of fonts at different sizes
    int minFontSize = 1;
    int maxFontSize = 80;
    
    for( int i=minFontSize; i<maxFontSize; i++){
        ofxFTGLFont *tmpThin = new ofxFTGLFont();
        tmpThin->loadFont( GetCloudsDataPath() + "font/Blender-THIN.ttf", i );
        tempFontListThin.push_back( tmpThin );
        
        ofxFTGLFont *tmpBook = new ofxFTGLFont();
        tmpBook->loadFont( GetCloudsDataPath() + "font/Blender-BOOK.ttf", i );
        tempFontListBook.push_back( tmpBook );
    }
    
    //setup references to all meshes that need references
    meshQuestion = questionLayout.getMeshByID("TEXTBOX_x5F_QUESTION_1_");
    ////for bio
    meshBioFirstName = bioLayout.getMeshByID("TEXTBOX_x5F_FIRSTNAME_2_");
    meshBioLastName = bioLayout.getMeshByID("TEXTBOX_x5F_LASTNAME_1_");
    meshBioTitle = bioLayout.getMeshByID("TEXTBOX_x5F_TITLE");
    meshBioTitle->bounds.width = 1000;
    meshBioTitleBG = bioLayout.getMeshByID("BOX_x5F_TITLE");
    meshBioLocation = bioLayout.getMeshByID("TEXTBOX_x5F_LOC");
    meshBioLocation->bounds.width = 1000;
    meshBioLocationBG = bioLayout.getMeshByID("BOX_x5F_LOC");
    meshBioDescription = bioLayout.getMeshByID("TEXTBOX_x5F_BIO");
    ////for project example
    meshProjectVideo = projectLayout.getMeshByID("BOX_x5F_VIDEO");
    meshProjectTitle = projectLayout.getMeshByID("TEXTBOX_x5F_NAME");
    meshProjectArtist = projectLayout.getMeshByID("TEXTBOX_x5F_ARTIST_1_");
    meshProjectArtist->bounds.width = 1000;
    meshProjectDescription = projectLayout.getMeshByID("TEXTBOX_x5F_DESC_1_");
    
    //setup all font layout objects
    layoutQuestion = getLayoutForLayer(meshQuestion, "Blender-BOOK", .04);
    
    fontBioLastName = getFontForLayer(meshBioLastName, "Blender-BOOK", 45);
    fontBioFirstName = getFontForLayer(meshBioFirstName, "Blender-THIN", 45);
    fontBioLocation = getFontForLayer(meshBioLocation, "Blender-BOOK", 65);
    fontBioTitle = getFontForLayer(meshBioTitle, "Blender-BOOK", 65);
    
    layoutBioDescription = getLayoutForLayer(meshBioDescription, "Blender-BOOK", .04);
    layoutProjectTitle = getLayoutForLayer(meshProjectTitle, "Blender-THIN", .045);
    layoutProjectArtist = getLayoutForLayer(meshProjectArtist, "Blender-BOOK", .065);
    layoutProjectDescription = getLayoutForLayer(meshProjectDescription, "Blender-BOOK", .04);
    
	displayTarget.allocate(1920, 1080, GL_RGB);
    // cleanup!
    for( int i=0; i<tempFontListThin.size(); i++ ){
        delete tempFontListThin[i];
        delete tempFontListBook[i];
    }
    tempFontListThin.clear();
    tempFontListBook.clear();
    
    hudLabelMap[meshProjectDescription->id]->caps = false;
    hudLabelMap[meshBioDescription->id]->caps = false;
	
	reloadShader();
}

/*LOADING SVG LAYOUT files from Sarah*/
void CloudsSecondaryDisplayController::loadSVGs(){
    //load the three different layouts
    bioLayout.load(GetCloudsDataPath() + "secondaryDisplay/SVG/BIO/BIO.svg");
    projectLayout.load(GetCloudsDataPath() + "secondaryDisplay/SVG/PROJECTEX/PROJECTEX.svg");
    questionLayout.load(GetCloudsDataPath() + "secondaryDisplay/SVG/QUESTION/QUESTION.svg");
	
	for(int i = 0; i < bioLayout.getMeshes().size(); i++){
		SVGMesh& mesh = bioLayout.getMeshes()[i];
		if(!mesh.fill){
			for(int v = 0; v < mesh.mesh.getNumVertices(); v++){
				mesh.mesh.addNormal( ofVec3f( (ofRandomuf() > .5 ? 0. : 1.0), 0, 0) ); //flag as lines, with random for now
			}
		}
		else{
			for(int v = 0; v < mesh.mesh.getNumVertices(); v++){
				mesh.mesh.addNormal( ofVec3f( 0.0, 1.0, 0.0) ); //flag as filled
			}
		}
		
	}
}

ofxFTGLSimpleLayout* CloudsSecondaryDisplayController::getLayoutForLayer( SVGMesh* textMesh, string font, float kerning) {
    
    if( textMesh != NULL ){
        
        int fontSize = getFontSizeForMesh( textMesh, font );
        // make a layout
        ofxFTGLSimpleLayout *newLayout = new ofxFTGLSimpleLayout();
        newLayout->loadFont( GetCloudsDataPath() + "font/"+font+".ttf", fontSize );
      //  newLayout->setTracking(kerning);
        newLayout->setLineLength( textMesh->bounds.width );
        //newLayout->set
        
        // make a label
        CloudsHUDLabel *newLabel = new CloudsHUDLabel();
        newLabel->setup( newLayout, textMesh->bounds );
        hudLabelMap[textMesh->id] = newLabel;
        
        return newLayout;
    }
    
    return NULL;
}

ofxFTGLFont* CloudsSecondaryDisplayController::getFontForLayer( SVGMesh* textMesh, string font, float kerning) {
    
    if( textMesh != NULL ){
        
        int fontSize = getFontSizeForMesh( textMesh, font );
        // make a layout
        ofxFTGLFont *newFont = new ofxFTGLFont();
        newFont->loadFont( GetCloudsDataPath() + "font/"+font+".ttf", fontSize );
        newFont->setTracking(kerning * .1);
        
        // make a label
        CloudsHUDLabel *newLabel = new CloudsHUDLabel();
        newLabel->setup( newFont, textMesh->bounds );
        hudLabelMap[textMesh->id] = newLabel;
        
        return newFont;
    }
    
    return NULL;
}

int CloudsSecondaryDisplayController::getFontSizeForMesh( SVGMesh* textMesh, string font ){
    if( !textMesh ){
        ofLogError("CloudsHUDController :: Text box not found");
        return 0;
    }
    
    int fontSize = 0;
    float textBoxHeight = textMesh->bounds.height;
    
    for( int k=0; k<tempFontListThin.size()-1; k++){
        float f1h, f2h;
        if(font == "Blender-THIN"){
            f1h = tempFontListThin[k]->getStringBoundingBox("M", 0, 0).height;
            f2h = tempFontListThin[k+1]->getStringBoundingBox("M", 0, 0).height;
        }
        else if(font == "Blender-BOOK"){
            f1h = tempFontListBook[k]->getStringBoundingBox("M", 0, 0).height;
            f2h = tempFontListBook[k+1]->getStringBoundingBox("M", 0, 0).height;
        }
        if( f1h <= textBoxHeight && f2h > textBoxHeight ){
            fontSize = 1 + k;
            break;
        }
    }
    
    return fontSize;
}

void CloudsSecondaryDisplayController::update(){
	
	//TODO: fix with perma preset
	//clusterMap.incrementalTraversalMode = true;
	
    stringCounter++;
    
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
            stringCounter = 0;
//			currentSpeaker = CloudsSpeaker::speakers[m.getArgAsString(0)];
            
			lastQuestion = m.getArgAsString(5);
			currentClip = parser.getClipWithID(m.getArgAsString(1));
		}
		else if(m.getAddress() == "/actBegan"){
			onActBegan();
		}
		else if(m.getAddress() == "/actEnded"){
			onActEnded();
		}
	}
	
	
	if(playingMovie){
		archivePlayer.update();
	}
}

void CloudsSecondaryDisplayController::respondToClip(CloudsClip& clip){
	
	currentClip = clip;
	currentSpeaker = CloudsSpeaker::speakers[currentClip.person];
	//if the speaker has no name, there is no speaker
	cout << "currentSpeaker.lastName: "<< currentSpeaker.lastName << endl;
	if(currentSpeaker.lastName == "")
		hasSpeaker = false;
	else
		hasSpeaker = true;
	

	//TEMP HACK TO REVEAL ALL QUESTIONS!!
	if(currentClip.hasQuestion()){
		lastQuestion = currentClip.getQuestionForTopic(currentClip.getTopicsWithQuestions()[0]);
	}
	else{
		lastQuestion = ofRandomuf() > .5 ? "IS THIS A SHORT TEST QUESTION?" : "OR A LONGER ONE TO TEST WHAT IT LOOKS LIKE WHEN TEXT WRAPS?";
	}
	///JG END TEMP HACK

	//	string exampleId = m.getArgAsString(4);
	if(currentClip.hasProjectExample){
		displayMode = "PROJECT";
		currentExample = currentClip.projectExample;
		if(currentExample.exampleVideos.size() > 0){
			playingMovie = archivePlayer.loadMovie(currentExample.exampleVideos[0]);
			if(playingMovie){
				archivePlayer.setLoopState(OF_LOOP_NONE);
				archivePlayer.play();
			}
		}
		
		//setup project text
		hudLabelMap[meshProjectArtist->id]->setText( currentExample.creatorName );
		hudLabelMap[meshProjectTitle->id]->setText( currentExample.title );
		hudLabelMap[meshProjectDescription->id]->setText( currentExample.description );
	}
	else{
		displayMode = "BIO";
		playingMovie = false;
		archivePlayer.stop();
		
		//setup all bio data
		hudLabelMap[meshQuestion->id]->setText( lastQuestion );
		hudLabelMap[meshBioFirstName->id]->setText( currentSpeaker.firstName );
		hudLabelMap[meshBioLastName->id]->setText( currentSpeaker.lastName );
		hudLabelMap[meshBioTitle->id]->setText( currentSpeaker.title );
		hudLabelMap[meshBioLocation->id]->setText( currentSpeaker.location2 );
		hudLabelMap[meshBioDescription->id]->setText( currentSpeaker.byline1 );
	}

	//bust a move
	clusterMap.traverseToClip(currentClip);

}

void CloudsSecondaryDisplayController::onActBegan(){
    
}

void CloudsSecondaryDisplayController::onActEnded(){
    //hide the secondary display hud
    displayMode = "NONE";
}

void CloudsSecondaryDisplayController::draw(){
	
	displayTarget.begin();
    ofEnableAlphaBlending();
	
	clusterMap.selfPostDraw();
    
    SVGMesh* t;
    
    if(displayMode == "BIO"){
        ////question
        ////// don't display if not avilable
        if(lastQuestion != ""){
            //show the question box
            questionLayout.draw();
            //find the text box
            lastQuestion = ofToUpper(lastQuestion);
            hudLabelMap[meshQuestion->id]->draw();
        }
        
        //only draw speaker info if there is a speaker, duh
        if(hasSpeaker){
            //DRAW BIO LAYOUT, need to draw this first, text goes over it
            //use shader to animate the alpha
			
			shader.begin();
			shader.setUniform1f("alphaAmt", tx);
			bioLayout.draw();
			shader.end();

            
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
            ////last name
            lastName = ofToUpper(lastName);
            
            float firstNameWidth = fontBioFirstName->getStringBoundingBox(firstName, 0, 0).width;
            float lastNameWidth = fontBioLastName->getStringBoundingBox(lastName, 0, 0).width;
            float longestNameWidth;
            if(firstNameWidth > lastNameWidth)
                longestNameWidth = firstNameWidth;
            else
                longestNameWidth = lastNameWidth;
            
            float margin = 60;
            float titleX = meshBioFirstName->bounds.x + longestNameWidth + margin;
            
            ////title
            string title = ofToUpper(currentSpeaker.title);
            //reposition title to float left
            hudLabelMap[meshBioTitle->id]->bounds.x = titleX;
            
            hudLabelMap[meshBioFirstName->id]->draw();
            hudLabelMap[meshBioLastName->id]->draw();
            hudLabelMap[meshBioTitle->id]->draw();
            
            if(color)
                ofSetColor(lightBlue);
            
                ////location
                if(color)
                    ofSetColor(darkBlue);
            
            string loc = ofToUpper(currentSpeaker.location2);
            //float left
            hudLabelMap[meshBioLocation->id]->bounds.x = titleX;
            hudLabelMap[meshBioLocation->id]->bounds.width = 9999;
            hudLabelMap[meshBioLocation->id]->draw();
            
            if(color)
                ofSetColor(255);
            
            ////byline / bio / description
            hudLabelMap[meshBioDescription->id]->draw();
        }
        
        
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
        hudLabelMap[meshProjectTitle->id]->draw();
        
        ////artist name
        string name = currentExample.creatorName;
        hudLabelMap[meshProjectArtist->id]->draw();
        
        ////project description
        hudLabelMap[meshProjectDescription->id]->draw();
        
    }
	
	displayTarget.end();
	
	ofRectangle screenRect(0,0,ofGetWidth(), ofGetHeight());
	ofRectangle targetRect(0,0,displayTarget.getWidth(),displayTarget.getHeight());
	targetRect.scaleTo(screenRect);
	displayTarget.getTextureReference().draw(targetRect);
	
	
}

void CloudsSecondaryDisplayController::reloadShader(){
    GLuint err = glGetError();
    shader.load( GetCloudsDataPath() + "shaders/secondaryDisplay");
}

/*void CloudsSecondaryDisplayController::drawBioLayout(){
 
 }*/

/*void CloudsSecondaryDisplayController::drawTextToMesh(ofxFTGLSimpleLayout* font, string text, SVGMesh* mesh){
    //update line length
    font->setLineLength(mesh->bounds.width);
    //draw the text
    font->drawString(text.substr(0), mesh->bounds.x, mesh->bounds.y + font->getStringBoundingBox("M", 0, 0).height);
}*/

