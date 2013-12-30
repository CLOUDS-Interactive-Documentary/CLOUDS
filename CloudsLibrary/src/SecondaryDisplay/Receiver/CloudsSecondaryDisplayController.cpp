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
}

void CloudsSecondaryDisplayController::setup(){

	parser.loadFromFiles();
//	visualSystems.loadPresets();
	
	//storyEngine.parser = &parser;
	////storyEngine.visualSystems = &visualSystems;
	//storyEngine.printDecisions = false;
	//storyEngine.combinedClipsOnly = false;
	//storyEngine.setup();
	
//	vector<CloudsClip> startingNodes = parser.getClipsWithKeyword("#start");
//	CloudsAct* act = storyEngine.buildAct(run, startingNodes[ ofRandom(startingNodes.size()) ]);
//	run.topicHistory = act->getAllTopics();
//	run.clipHistory = act->getAllClips();
	
	clusterMap.buildEntireCluster(parser);
//	clusterMap.setRun(run);

//	clusterMap.traverse();
	clusterMap.forceScreenResolution(1920, 1080);
	clusterMap.setDrawToScreen(false);
	
	clusterMap.setup();
	clusterMap.playSystem();

	receiver.setup(12346);
	
    
    
	loadSVGs();
    
    //FONT SIZES ARE IN POINTS
    //1 pixel = .75pts
    float fScale = .75;
    //load all fonts
    ////last name
    h1.loadFont(GetCloudsDataPath() + "font/Blender-THIN.ttf", bioLayout.getMeshByID("TEXTBOX_x5F_LASTNAME")->bounds.height * fScale);
    h1.setLineLength(bioLayout.getMeshByID("TEXTBOX_x5F_FIRSTNAME")->bounds.width);
    ////first name
    h2.loadFont(GetCloudsDataPath() + "font/Blender-THIN.ttf", bioLayout.getMeshByID("TEXTBOX_x5F_FIRSTNAME")->bounds.height * fScale);
    h2.setLineLength(bioLayout.getMeshByID("TEXTBOX_x5F_FIRSTNAME")->bounds.width);
    ////question
    h3FontSize = 38 * fScale;
    h3.loadFont(GetCloudsDataPath() + "font/Blender-THIN.ttf", h3FontSize);
    h3.setLineLength(bioLayout.getMeshByID("TEXTBOX_x5F_QUESTION")->bounds.width);
    ////location / creator name
    h4.loadFont(GetCloudsDataPath() + "font/Blender-THIN.ttf", bioLayout.getMeshByID("TEXTBOX_x5F_LOC")->bounds.height * fScale);
    h4.setLineLength(bioLayout.getMeshByID("BOX_x5F_LOC")->bounds.width);
    ////byline / description
    pFontSize = 21 * fScale;
    p.loadFont(GetCloudsDataPath() + "font/Blender-THIN.ttf", pFontSize);
    p.setLineLength(bioLayout.getMeshByID("TEXTBOX_x5F_BIO")->bounds.width);
 
	displayTarget.allocate(1920, 1080, GL_RGB);
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
    systemLayout.load(GetCloudsDataPath() + "secondaryDisplay/SVG/VISUALSYSTEM/VISUAL.svg");

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

/*void CloudsSecondaryDisplayController::drawNextLayout(){
    layoutID++;
    if(layoutID>testAllLayout.size()-1)
        layoutID = 0;
    cout << "Draw Display: "<< testAllLayout[layoutID].sourceFileName <<" #" << layoutID << " of " << testAllLayout.size()-1 << endl;
    
}

void CloudsSecondaryDisplayController::drawPrevLayout(){
    layoutID--;
    if(layoutID<0)
        layoutID =testAllLayout.size()-1;
    cout << "Draw Display: "<< testAllLayout[layoutID].sourceFileName <<" #" << layoutID << " of " << testAllLayout.size()-1 << endl;
}*/

void CloudsSecondaryDisplayController::draw(){
    ofEnableAlphaBlending();
	
	displayTarget.begin();
	
	clusterMap.selfPostDraw();

    SVGMesh* t;
    
    if(displayMode == "BIO"){
        //DRAW BIO LAYOUT
        
        
        ////question
        ////// don't display if not avilable
        //find the question box
        t = bioLayout.getMeshByID("BOX_x5F_QUESTION_x5F_DETAILS");
        if(lastQuestion != ""){
            //show the question box
            t->visible = true;
            
            //find the text box
            t = bioLayout.getMeshByID("TEXTBOX_x5F_QUESTION");
            if(t){
                h3.drawString(lastQuestion, t->bounds.x, t->bounds.y + h3FontSize);
            }
        }
        else{
            t->visible = false;
            //this does not work because the group I am looking for is composed not of one mesh, but of 8 different mesh objects, and the method getMeshByID does not return the entire group of meshes, but only one of them.
            //At this point, it is better to draw whetever elements need to show or hide as separate SVG files
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
        t = bioLayout.getMeshByID("TEXTBOX_x5F_FIRSTNAME");
        if(t){
            h2.drawString(firstName, t->bounds.x, t->bounds.y + t->bounds.height);
        }
        ////last name
        t = bioLayout.getMeshByID("TEXTBOX_x5F_LASTNAME");
        if(t){
            h1.drawString(lastName, t->bounds.x, t->bounds.y + t->bounds.height);
        }
        
        
        ////location
        t = bioLayout.getMeshByID("TEXTBOX_x5F_LOC");
        if(t){
            h4.drawString(currentSpeaker.location1, t->bounds.x, t->bounds.y + t->bounds.height);
        }
        
        ////title
        t = bioLayout.getMeshByID("BOX_x5F_TITLE");
        if(t){
            h4.drawString(currentSpeaker.title, t->bounds.x, t->bounds.y + t->bounds.height);
        }
        
        ////byline / bio / description
        t = bioLayout.getMeshByID("TEXTBOX_x5F_BIO");
        if(t)
            p.drawString(currentSpeaker.byline1, t->bounds.x, t->bounds.y + pFontSize);
        
        //DRAW BIO LAYOUT
        bioLayout.draw();
        
    }else if(displayMode == "PROJECT"){
        //DISPLAY PROJECT LAYOUT
        projectLayout.draw();
        
        //video
        if(playingMovie){
            t = projectLayout.getMeshByID("BOX_x5F_VIDEO");
            if(t){
                //scale and preserve the aspect ratio
                ofRectangle playerRect(0,0,archivePlayer.getWidth(), archivePlayer.getHeight());
                playerRect.scaleTo(t->bounds);
//                archivePlayer.draw(t->bounds.x, t->bounds.y, t->bounds.width, t->bounds.height);
                archivePlayer.draw(playerRect);
            }
            playingMovie = archivePlayer.isPlaying();
        }
        
        ////title
        t = projectLayout.getMeshByID("TEXTBOX_x5F_TITLE");
        if(t)
            h2.drawString(currentExample.title, t->bounds.x, t->bounds.y + t->bounds.height);
        
        ////name
        t = projectLayout.getMeshByID("TEXTBOX_x5F_ARTIST");
        if(t)
            h4.drawString(currentExample.creatorName, t->bounds.x, t->bounds.y + t->bounds.height);
        
        ////description
        t = projectLayout.getMeshByID("TEXTBOX_x5F_DESC");
        if(t)
            p.drawString(currentExample.description, t->bounds.x, t->bounds.y + pFontSize);
        
        
    }else if(displayMode == "SYSTEM"){
        systemLayout.draw();
    }
	
	displayTarget.end();
	
	ofRectangle screenRect(0,0,ofGetWidth(), ofGetHeight());
	ofRectangle targetRect(0,0,displayTarget.getWidth(),displayTarget.getHeight());
	targetRect.scaleTo(screenRect);
	displayTarget.getTextureReference().draw(targetRect);
	
    // ---------------- added
//	//get info for a speaker
//	CloudsSpeaker::speakers["Kyl_CH"].twitterHandle;
	//TODO: draw speaker layout
	//TODO: overlay with project example when relevant
	
}

void CloudsSecondaryDisplayController::hideQuestionBox(){
    bioLayout.getMeshByID("BOX_x5F_QUESTION_x5F_DETAILS")->visible = false;;
}

void CloudsSecondaryDisplayController::showQuestionBox(){
    
}
