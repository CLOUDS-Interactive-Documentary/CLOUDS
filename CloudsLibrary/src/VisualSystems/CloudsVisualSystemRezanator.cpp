#include "CloudsVisualSystemRezanator.h"

#define REZANATOR_GUI_ALPHA_MULTIPLIER 4

void CloudsVisualSystemRezanator::setup()
{
    ofAddListener(ofEvents().exit, this, &CloudsVisualSystemRezanator::exit);
    
    ofDirectory dir;
    string directoryName = getVisualSystemDataPath();
    if(!dir.doesDirectoryExist(directoryName))
    {
        dir.createDirectory(directoryName);
    }
    
    string workingDirectoryName = directoryName+"Working/";
    if(!dir.doesDirectoryExist(workingDirectoryName))
    {
        dir.createDirectory(workingDirectoryName);
    }    
    
    setupAppParams();
    setupDebugParams();
    setupCameraParams();
    setupLightingParams();
    setupMaterialParams();
    selfSetup();
    setupCoreGuis();
    selfSetupGuis();
}

void CloudsVisualSystemRezanator::update(ofEventArgs & args)
{
    if(bUpdateSystem)
    {
        for(vector<ofx1DExtruder *>::iterator it = extruders.begin(); it != extruders.end(); ++it)
        {
            (*it)->update();
        }
        bgColor->setHsb(bgHue->getPos(), bgSat->getPos(), bgBri->getPos(), 255);
        bgColor2->setHsb(bgHue2->getPos(), bgSat2->getPos(), bgBri2->getPos(), 255);
        selfUpdate();
    }
}

void CloudsVisualSystemRezanator::draw(ofEventArgs & args)
{
    ofPushStyle();
    if(bRenderSystem)
    {
        cam.begin();
        
        drawBackground();
        
        ofRotateX(xRot->getPos());
        ofRotateY(yRot->getPos());
        ofRotateZ(zRot->getPos());
        
        selfSceneTransformation();
        
        glEnable(GL_DEPTH_TEST);
        
        drawDebug();
        
        lightsBegin();
        
        selfDraw();
        
        lightsEnd();
        
        cam.end();
    }
    
    ofPopStyle();
}

void CloudsVisualSystemRezanator::exit(ofEventArgs & args)
{
    delete colorPalletes; 
    delete bgColor;
    delete bgColor2;
    
    saveGUIS();
    
    for(vector<ofx1DExtruder *>::iterator it = extruders.begin(); it != extruders.end(); ++it)
    {
        ofx1DExtruder *e = (*it);
        delete e;
    }
    extruders.clear();
    
    for(map<string, ofxLight *>::iterator it = lights.begin(); it != lights.end(); ++it)
    {
        ofxLight *l = it->second;
        delete l;
    }
    lights.clear();
    
    for(map<string, ofMaterial *>::iterator it = materials.begin(); it != materials.end(); ++it)
    {
        ofMaterial *m = it->second;
        delete m;
    }
    materials.clear();
    
    materialGuis.clear();
    
    selfExit();
    
    deleteGUIS();
}

void CloudsVisualSystemRezanator::begin()
{
    loadGUIS();
    hideGUIS();
    showGUIS();
    cam.enableMouseInput();
    for(map<string, ofxLight *>::iterator it = lights.begin(); it != lights.end(); ++it)
    {
        it->second->light.setup();
    }
    selfBegin();
}

void CloudsVisualSystemRezanator::end()
{
    hideGUIS();
    saveGUIS();
    cam.disableMouseInput();
    for(map<string, ofxLight *>::iterator it = lights.begin(); it != lights.end(); ++it)
    {
        it->second->light.destroy();
    }
    selfEnd();
}

void CloudsVisualSystemRezanator::keyPressed(ofKeyEventArgs & args)
{
    for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
    {
        if((*it)->hasKeyboardFocus())
        {
            return;
        }
    }
    
    switch (args.key)
    {
        case '1':
            toggleGuiAndPosition(gui);
            break;
        case '2':
            toggleGuiAndPosition(sysGui);
            break;
        case '3':
            toggleGuiAndPosition(rdrGui);
            break;
        case '4':
            toggleGuiAndPosition(bgGui);
            break;
        case '5':
            toggleGuiAndPosition(lgtGui);
            break;
        case '0':
            toggleGuiAndPosition(camGui);
            break;
            
        case 'u':
        {
            bUpdateSystem = !bUpdateSystem; 
        }
            break;
            
        case '`':
        {
            ofImage img;
            img.grabScreen(0,0,ofGetWidth(), ofGetHeight());
            img.saveImage(getDataPath()+"snapshots/" + getSystemName() + " " + ofGetTimestampString() + ".png");
        }
            break;
            
        case 'h':
        {
            toggleGUIS();
        }
            break;
            
        case 'f':
        {
            ofToggleFullscreen();
        }
            break;
            
        case 'p':
        {
            for(int i = 0; i < guis.size(); i++)
            {
                guis[i]->setDrawWidgetPadding(true);
            }
        }
            break;
            
        case 'e':
        {
            ofxUISuperCanvas *last = NULL;
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                (*it)->setMinified(false);
                if(last != NULL)
                {
                    (*it)->getRect()->setX(last->getRect()->getX());
                    (*it)->getRect()->setY(last->getRect()->getY()+last->getRect()->getHeight()+1);
                    if((*it)->getRect()->getY()+(*it)->getRect()->getHeight() > ofGetHeight())
                    {
                        (*it)->getRect()->setX(last->getRect()->getX()+last->getRect()->getWidth()+1);
                        (*it)->getRect()->setY(1);
                    }
                }
                else
                {
                    (*it)->getRect()->setX(1);
                    (*it)->getRect()->setY(1);
                }
                last = (*it);
            }
        }
            break;
            
        case 'r':
        {
            ofxUISuperCanvas *last = NULL;
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                if(last != NULL)
                {
                    (*it)->getRect()->setX(last->getRect()->getX()+last->getRect()->getWidth()+1);
                    (*it)->getRect()->setY(1);
                }
                else
                {
                    (*it)->getRect()->setX(1);
                    (*it)->getRect()->setY(1);
                }
                last = (*it);
                last->setMinified(false);
            }
        }
            break;
            
        case 't':
        {
            ofxUISuperCanvas *last = NULL;
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                (*it)->setMinified(true);
                if(last != NULL)
                {
                    (*it)->getRect()->setX(1);
                    (*it)->getRect()->setY(last->getRect()->getY()+last->getRect()->getHeight()+1);
                }
                else
                {
                    (*it)->getRect()->setX(1);
                    (*it)->getRect()->setY(1);
                }
                last = (*it);
            }
        }
            break;
            
        case 'y':
        {
            float x = ofGetWidth()*.5;
            float y = ofGetHeight()*.5;
            float tempRadius = gui->getGlobalCanvasWidth()*2.0;
            float stepSize = TWO_PI/(float)guis.size();
            float theta = 0;
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                (*it)->getRect()->setX(x+sin(theta)*tempRadius - (*it)->getRect()->getHalfWidth());
                (*it)->getRect()->setY(y+cos(theta)*tempRadius - (*it)->getRect()->getHalfHeight());
                theta +=stepSize;
            }
        }
            break;
            
        case '=':
        {
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                (*it)->toggleMinified();
            }
        }
            break;
            
        default:
            selfKeyPressed(args);
            break;
    }
}

void CloudsVisualSystemRezanator::keyReleased(ofKeyEventArgs & args)
{
    switch (args.key)
    {
        case 'p':
        {
            for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
            {
                (*it)->setDrawWidgetPadding(false);
            }
        }
            break;
            
            
        default:
            selfKeyReleased(args);
            break;
    }
}

void CloudsVisualSystemRezanator::mouseDragged(ofMouseEventArgs& data)
{
    selfMouseDragged(data);
}

void CloudsVisualSystemRezanator::mouseMoved(ofMouseEventArgs& data)
{
    selfMouseMoved(data);
}

void CloudsVisualSystemRezanator::mousePressed(ofMouseEventArgs & args)
{
    for(int i = 0; i < guis.size(); i++)
    {
        if(guis[i]->isHit(args.x, args.y))
        {
            cam.disableMouseInput();
            return;
        }
    }
    selfMousePressed(args);
}

void CloudsVisualSystemRezanator::mouseReleased(ofMouseEventArgs & args)
{
    cam.enableMouseInput();
    selfMouseReleased(args);	
}

void CloudsVisualSystemRezanator::setupAppParams()
{
	colorPalletes = new ofxColorPalettes(getDataPath()+"colors/");
    ofSetSphereResolution(30);
    bRenderSystem = true;
    bUpdateSystem = true;
}

void CloudsVisualSystemRezanator::setupDebugParams()
{
    //DEBUG
    bDebug = true;
    debugGridSize = 100;
}

void CloudsVisualSystemRezanator::setupCameraParams()
{
    //CAMERA
    camFOV = 60;
    camDistance = 200;
    cam.setDistance(camDistance);
    cam.setFov(camFOV);
    cam.setForceAspectRatio(true);
    bgAspectRatio = (float)ofGetWidth()/(float)ofGetHeight();
    cam.setAspectRatio(bgAspectRatio);
    xRot = new ofx1DExtruder(0);
    yRot = new ofx1DExtruder(0);
    zRot = new ofx1DExtruder(0);
    xRot->setPhysics(.9, 5.0, 25.0);
    yRot->setPhysics(.9, 5.0, 25.0);
    zRot->setPhysics(.9, 5.0, 25.0);
    extruders.push_back(xRot);
    extruders.push_back(yRot);
    extruders.push_back(zRot);
}

void CloudsVisualSystemRezanator::setupLightingParams()
{
    //LIGHTING
    bSmoothLighting = true;
    bEnableLights = true;
    globalAmbientColor = new float[4];
    globalAmbientColor[0] = 0.5;
    globalAmbientColor[1] = 0.5;
    globalAmbientColor[2] = 0.5;
    globalAmbientColor[3] = 1.0;
}

void CloudsVisualSystemRezanator::setupMaterialParams()
{
    mat = new ofMaterial();
}

void CloudsVisualSystemRezanator::setupCoreGuis()
{
    setupGui();
    setupSystemGui();
    setupRenderGui();
    setupBackgroundGui();
    setupLightingGui();
    setupCameraGui();
    setupMaterial("MATERIAL 1", mat);
    setupPointLight("POINT LIGHT 1");
	setupPresetGui();
}

void CloudsVisualSystemRezanator::setupGui()
{
    gui = new ofxUISuperCanvas(ofToUpper(getSystemName()));
    gui->setName("Settings");
    gui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIFPS *fps = gui->addFPS();
    gui->resetPlacer();
    gui->addWidgetDown(fps, OFX_UI_ALIGN_RIGHT, true);
    gui->addWidgetToHeader(fps);
    
    gui->addSpacer();
    gui->addButton("SAVE", false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ofxUIButton *loadbtn = gui->addButton("LOAD", false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    ofxUIButton *updatebtn = gui->addToggle("UPDATE", &bUpdateSystem);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ofxUIButton *renderbtn = gui->addToggle("RENDER", &bRenderSystem);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addWidgetNorthOf(loadbtn, "RENDER", true);
    gui->setPlacer(updatebtn);
    gui->addSpacer();    
    selfSetupGui();
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&CloudsVisualSystemRezanator::guiEvent);
    guis.push_back(gui);
}

vector<string> CloudsVisualSystemRezanator::getPresets(){
	vector<string> presets;
	ofDirectory presetsFolder = ofDirectory(getVisualSystemDataPath());
	if(presetsFolder.exists()){
		presetsFolder.listDir();
		for(int i = 0; i < presetsFolder.size(); i++){
			if(presetsFolder.getFile(i).isDirectory() && presetsFolder.getName(i) != "Working"){
				presets.push_back(presetsFolder.getName(i));
			}
		}
	}
	return presets;
}

void CloudsVisualSystemRezanator::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    cout << "name:" << name << endl;
    if(name == "SAVE")
    {
        ofxUIButton *b = (ofxUIButton *) e.widget;
        if(b->getValue())
        {
            string presetName = ofSystemTextBoxDialog("Save Preset As");
            if(presetName.length())
            {
                savePresetGUIS(presetName); 
            }
            else{
                saveGUIS();
            }
        }
    }
    else if(name == "LOAD")
    {
        ofxUIButton *b = (ofxUIButton *) e.widget;
        if(b->getValue())
        {
            ofFileDialogResult result = ofSystemLoadDialog("Load Visual System Preset Folder", true, getVisualSystemDataPath());
            if(result.bSuccess && result.fileName.length())
            {
                loadPresetGUIS(result.filePath);
            }
            else{
                loadGUIS();
            }
        }
    }

    selfGuiEvent(e);
}

void CloudsVisualSystemRezanator::setupSystemGui()
{
    sysGui = new ofxUISuperCanvas("SYSTEM", gui);
    sysGui->copyCanvasStyle(gui);
    sysGui->copyCanvasProperties(gui);
    sysGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    sysGui->setName("SystemSettings");
    sysGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = sysGui->addToggle("DEBUG", &bDebug);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    sysGui->resetPlacer();
    sysGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    sysGui->addWidgetToHeader(toggle);
    sysGui->addSpacer();
    
    selfSetupSystemGui();
    sysGui->autoSizeToFitWidgets();
    ofAddListener(sysGui->newGUIEvent,this,&CloudsVisualSystemRezanator::guiSystemEvent);
    guis.push_back(sysGui);
}

void CloudsVisualSystemRezanator::setupRenderGui()
{
    rdrGui = new ofxUISuperCanvas("RENDER", gui);
    rdrGui->copyCanvasStyle(gui);
    rdrGui->copyCanvasProperties(gui);
    rdrGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    rdrGui->setName("RenderSettings");
    rdrGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    rdrGui->addSpacer();
    selfSetupRenderGui();
    
    rdrGui->autoSizeToFitWidgets();
    ofAddListener(rdrGui->newGUIEvent,this,&CloudsVisualSystemRezanator::guiRenderEvent);
    guis.push_back(rdrGui);
}

void CloudsVisualSystemRezanator::setupBackgroundGui()
{
    bgHue = new ofx1DExtruder(0);
	bgSat = new ofx1DExtruder(0);
	bgBri = new ofx1DExtruder(0);
	
	bgHue->setPhysics(.95, 5.0, 25.0);
	bgSat->setPhysics(.95, 5.0, 25.0);
	bgBri->setPhysics(.95, 5.0, 25.0);
	
    bgHue2 = new ofx1DExtruder(0);
	bgSat2 = new ofx1DExtruder(0);
	bgBri2 = new ofx1DExtruder(0);
	
	bgHue2->setPhysics(.95, 5.0, 25.0);
	bgSat2->setPhysics(.95, 5.0, 25.0);
	bgBri2->setPhysics(.95, 5.0, 25.0);
    
	gradientMode = 0;
    bgHue->setHome((330.0/360.0)*255.0);
	bgSat->setHome(0);
	bgBri->setHome(0);
    bgColor = new ofColor(0,0,0);
    
    bgHue2->setHome((330.0/360.0)*255.0);
	bgSat2->setHome(0);
	bgBri2->setHome(0);
	bgColor2 = new ofColor(0,0,0);
    
    extruders.push_back(bgHue);
    extruders.push_back(bgSat);
    extruders.push_back(bgBri);
    
    extruders.push_back(bgHue2);
    extruders.push_back(bgSat2);
    extruders.push_back(bgBri2);
    
    bgGui = new ofxUISuperCanvas("BACKGROUND", gui);
    bgGui->copyCanvasStyle(gui);
    bgGui->copyCanvasProperties(gui);
    bgGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    bgGui->setName("BgSettings");
    bgGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = bgGui->addToggle("GRAD", false);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    bgGui->resetPlacer();
    bgGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    bgGui->addWidgetToHeader(toggle);
    bgGui->addSpacer();
    
    bgGui->addSlider("HUE", 0.0, 255.0, bgHue->getPosPtr());
    bgGui->addSlider("SAT", 0.0, 255.0, bgSat->getPosPtr());
    bgGui->addSlider("BRI", 0.0, 255.0, bgBri->getPosPtr());
    bgGui->addSpacer();
    hueSlider = bgGui->addSlider("HUE2", 0.0, 255.0, bgHue2->getPosPtr());
    satSlider = bgGui->addSlider("SAT2", 0.0, 255.0, bgSat2->getPosPtr());
    briSlider = bgGui->addSlider("BRI2", 0.0, 255.0, bgBri2->getPosPtr());
    bgGui->autoSizeToFitWidgets();
    ofAddListener(bgGui->newGUIEvent,this,&CloudsVisualSystemRezanator::guiBackgroundEvent);
    guis.push_back(bgGui);
}

void CloudsVisualSystemRezanator::guiBackgroundEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    
    if(name == "BRI")
    {
        bgBri->setPosAndHome(bgBri->getPos());
        for(int i = 0; i < guis.size(); i++)
        {
            guis[i]->setWidgetColor(OFX_UI_WIDGET_COLOR_BACK, ofColor(bgBri->getPos(),OFX_UI_COLOR_BACK_ALPHA*REZANATOR_GUI_ALPHA_MULTIPLIER));
            guis[i]->setColorBack(ofColor(255-bgBri->getPos(), OFX_UI_COLOR_BACK_ALPHA*REZANATOR_GUI_ALPHA_MULTIPLIER));
        }
    }
    else if(name == "SAT")
    {
        bgSat->setPosAndHome(bgSat->getPos());
    }
    else if(name == "HUE")
    {
        bgHue->setPosAndHome(bgHue->getPos());
    }
    else if(name == "BRI2")
    {
        bgBri2->setPosAndHome(bgBri2->getPos());
    }
    else if(name == "SAT2")
    {
        bgSat2->setPosAndHome(bgSat2->getPos());
    }
    else if(name == "HUE2")
    {
        bgHue2->setPosAndHome(bgHue2->getPos());
    }
    else if(name == "GRAD")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            gradientMode = OF_GRADIENT_CIRCULAR;
            hueSlider->setVisible(true);
            satSlider->setVisible(true);
            briSlider->setVisible(true);
            bgGui->autoSizeToFitWidgets();
            if(bgGui->isMinified())
            {
                bgGui->setMinified(true);
            }
        }
        else
        {
            gradientMode = -1;
            hueSlider->setVisible(false);
            satSlider->setVisible(false);
            briSlider->setVisible(false);
            bgGui->autoSizeToFitWidgets();
        }
    }
}

void CloudsVisualSystemRezanator::setupLightingGui()
{
    lgtGui = new ofxUISuperCanvas("LIGHT", gui);
    lgtGui->copyCanvasStyle(gui);
    lgtGui->copyCanvasProperties(gui);
    lgtGui->setName("LightSettings");
    lgtGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    lgtGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = lgtGui->addToggle("ENABLE", &bEnableLights);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    lgtGui->resetPlacer();
    lgtGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    lgtGui->addWidgetToHeader(toggle);
    
    lgtGui->addSpacer();
    lgtGui->addToggle("SMOOTH", &bSmoothLighting);
    lgtGui->addSpacer();
    float length = (lgtGui->getGlobalCanvasWidth()-lgtGui->getWidgetSpacing()*5)/3.;
    float dim = lgtGui->getGlobalSliderHeight();
    lgtGui->addLabel("GLOBAL AMBIENT COLOR", OFX_UI_FONT_SMALL);
    lgtGui->addMinimalSlider("R", 0.0, 1.0, &globalAmbientColor[0], length, dim)->setShowValue(false);
    lgtGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    lgtGui->addMinimalSlider("G", 0.0, 1.0, &globalAmbientColor[1], length, dim)->setShowValue(false);
    lgtGui->addMinimalSlider("B", 0.0, 1.0, &globalAmbientColor[2], length, dim)->setShowValue(false);
    lgtGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    lgtGui->autoSizeToFitWidgets();
    ofAddListener(lgtGui->newGUIEvent,this,&CloudsVisualSystemRezanator::guiLightingEvent);
    guis.push_back(lgtGui);
}

void CloudsVisualSystemRezanator::guiLightingEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if(name == "R")
    {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientColor);
    }
    else if(name == "G")
    {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientColor);
    }
    else if(name == "B")
    {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientColor);
    }
}

void CloudsVisualSystemRezanator::setupCameraGui()
{
    camGui = new ofxUISuperCanvas("CAMERA", gui);
    camGui->copyCanvasStyle(gui);
    camGui->copyCanvasProperties(gui);
    camGui->setName("CamSettings");
    camGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    camGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIButton *button = camGui->addButton("RESET", false);
    button->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    camGui->resetPlacer();
    camGui->addWidgetDown(button, OFX_UI_ALIGN_RIGHT, true);
    camGui->addWidgetToHeader(button);
    
    camGui->addSpacer();
    camGui->addSlider("DIST", 0, 1000, &camDistance);
    camGui->addSlider("FOV", 0, 180, &camFOV);
    camGui->addSlider("ROT-X", 0, 360.0, xRot->getPosPtr())->setIncrement(1.0);
    camGui->addSlider("ROT-Y", 0, 360.0, yRot->getPosPtr())->setIncrement(1.0);
    camGui->addSlider("ROT-Z", 0, 360.0, zRot->getPosPtr())->setIncrement(1.0);    
    
    camGui->addSpacer();
    vector<string> views;
    views.push_back("TOP");
    views.push_back("BOTTOM");
    views.push_back("FRONT");
    views.push_back("BACK");
    views.push_back("RIGHT");
    views.push_back("LEFT");
    views.push_back("3D");
    ofxUIDropDownList *ddl = camGui->addDropDownList("VIEW", views);
    ddl->setAutoClose(false);
    ddl->setShowCurrentSelected(true);
    
    camGui->autoSizeToFitWidgets();
    ofAddListener(camGui->newGUIEvent,this,&CloudsVisualSystemRezanator::guiCameraEvent);
    guis.push_back(camGui);
}

void CloudsVisualSystemRezanator::guiCameraEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if(name == "DIST")
    {
        cam.setDistance(camDistance);
    }
    else if(name == "FOV")
    {
        cam.setFov(camFOV);
    }
    else if(name == "ROT-X")
    {
        xRot->setPosAndHome(xRot->getPos());
    }
    else if(name == "ROT-Y")
    {
        yRot->setPosAndHome(yRot->getPos());
    }
    else if(name == "ROT-Z")
    {
        zRot->setPosAndHome(zRot->getPos());
    }
    else if(name == "RESET")
    {
        ofxUIButton *b = (ofxUIButton *) e.widget;
        if(b->getValue())
        {
            cam.reset();
        }
    }
    else if(name == "TOP")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(0);
            yRot->setHome(0);
            zRot->setHome(0);
        }
    }
    else if(name == "BOTTOM")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-180);
            yRot->setHome(0);
            zRot->setHome(0);
        }
    }
    else if(name == "BACK")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-90);
            yRot->setHome(0);
            zRot->setHome(180);
        }
    }
    else if(name == "FRONT")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-90);
            yRot->setHome(0);
            zRot->setHome(0);
        }
    }
    else if(name == "RIGHT")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-90);
            yRot->setHome(0);
            zRot->setHome(90);
        }
    }
    else if(name == "LEFT")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-90);
            yRot->setHome(0);
            zRot->setHome(-90);
            
        }
    }
    else if(name == "3D")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        if(t->getValue())
        {
            xRot->setHome(-70);
            yRot->setHome(0);
            zRot->setHome(45);
        }
    }
}

void CloudsVisualSystemRezanator::setupPresetGui()
{
	presetGui = new ofxUISuperCanvas("PRESETS");
	presetGui->setName("Presets");
	presetGui->copyCanvasStyle(gui);
    presetGui->copyCanvasProperties(gui);
    presetGui->addSpacer();
    
    vector<string> empty; empty.clear();
	presetRadio = presetGui->addRadio("PRESETS", empty);

	presetGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    vector<string> presets = getPresets();
    for(vector<string>::iterator it = presets.begin(); it != presets.end(); ++it)
    {
        ofxUIToggle *t = presetGui->addToggle((*it), false);
        presetRadio->addToggle(t);
    }
	
	presetGui->autoSizeToFitWidgets();
    ofAddListener(presetGui->newGUIEvent,this,&CloudsVisualSystemRezanator::guiPresetEvent);
    guis.push_back(presetGui);
}

void CloudsVisualSystemRezanator::guiPresetEvent(ofxUIEventArgs &e)
{
    ofxUIToggle *t = (ofxUIToggle *) e.widget;
    if(t->getValue())
    {
        loadPresetGUIS(getVisualSystemDataPath() + e.widget->getName());
    }
}

void CloudsVisualSystemRezanator::setupMaterial(string name, ofMaterial *m)
{
    materials[name] = m;
    ofxUISuperCanvas* g = new ofxUISuperCanvas(name, gui);
    materialGuis[name] = g;
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setName(name+"Settings");
    g->addSpacer();
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    float length = (g->getGlobalCanvasWidth()-g->getWidgetSpacing()*5)/3.;
    float dim = g->getGlobalSliderHeight();
    
    g->addLabel("AMBIENT", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("AR", 0.0, 1.0, &m->getAmbientColor().r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("AG", 0.0, 1.0, &m->getAmbientColor().g, length, dim)->setShowValue(false);
    g->addMinimalSlider("AB", 0.0, 1.0, &m->getAmbientColor().b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    g->addSpacer();
    
    g->addLabel("DIFFUSE", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("AR", 0.0, 1.0, &m->getDiffuseColor().r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("AG", 0.0, 1.0, &m->getDiffuseColor().g, length, dim)->setShowValue(false);
    g->addMinimalSlider("AB", 0.0, 1.0, &m->getDiffuseColor().b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    g->addSpacer();
    
    g->addLabel("EMISSIVE", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("ER", 0.0, 1.0, &m->getEmissiveColor().r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("EG", 0.0, 1.0, &m->getEmissiveColor().g, length, dim)->setShowValue(false);
    g->addMinimalSlider("EB", 0.0, 1.0, &m->getEmissiveColor().b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    g->addSpacer();
    
    g->addLabel("SPECULAR", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("SR", 0.0, 1.0, &m->getSpecularColor().r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("SG", 0.0, 1.0, &m->getSpecularColor().g, length, dim)->setShowValue(false);
    g->addMinimalSlider("SB", 0.0, 1.0, &m->getSpecularColor().b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    g->addSpacer();
    
    g->addMinimalSlider("SHINY", 0.0, 128.0, &(m->getShininess()))->setShowValue(false);
    
    g->autoSizeToFitWidgets();
    g->setPosition(ofGetWidth()*.5-g->getRect()->getHalfWidth(), ofGetHeight()*.5 - g->getRect()->getHalfHeight());
    
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemRezanator::guiMaterialEvent);
    guis.push_back(g);
}

void CloudsVisualSystemRezanator::guiMaterialEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemRezanator::setupPointLight(string name)
{
    ofxLight *l = new ofxLight();
    l->light.setPointLight();
    lights[name] = l;
    
    ofxUISuperCanvas* g = new ofxUISuperCanvas(name, gui);
    lightGuis[name] = g;
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ENABLE", &l->bEnabled);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    setupGenericLightProperties(g, l);
    
    g->autoSizeToFitWidgets();
    g->setPosition(ofGetWidth()*.5-g->getRect()->getHalfWidth(), ofGetHeight()*.5 - g->getRect()->getHalfHeight());
    
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemRezanator::guiLightEvent);
    guis.push_back(g);
}

void CloudsVisualSystemRezanator::setupSpotLight(string name)
{
    ofxLight *l = new ofxLight();
    l->light.setSpotlight();
    lights[name] = l;
    
    ofxUISuperCanvas* g = new ofxUISuperCanvas(name, gui);
    lightGuis[name] = g;
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ENABLE", &l->bEnabled);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    g->addSlider("CUT OFF", 0, 90, &l->lightSpotCutOff);
    g->addSlider("EXPONENT", 0.0, 128.0, &l->lightExponent);
    g->addSpacer();
    
    setupGenericLightProperties(g, l);
    
    g->autoSizeToFitWidgets();
    g->setPosition(ofGetWidth()*.5-g->getRect()->getHalfWidth(), ofGetHeight()*.5 - g->getRect()->getHalfHeight());
    
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemRezanator::guiLightEvent);
    guis.push_back(g);
}

void CloudsVisualSystemRezanator::setupBeamLight(string name)
{
    ofxLight *l = new ofxLight();
    l->light.setDirectional();
    lights[name] = l;
    
    ofxUISuperCanvas* g = new ofxUISuperCanvas(name, gui);
    lightGuis[name] = g;
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ENABLE", &l->bEnabled);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    setupGenericLightProperties(g, l);
    
    g->autoSizeToFitWidgets();
    g->setPosition(ofGetWidth()*.5-g->getRect()->getHalfWidth(), ofGetHeight()*.5 - g->getRect()->getHalfHeight());
    
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemRezanator::guiLightEvent);
    guis.push_back(g);
}

void CloudsVisualSystemRezanator::setupGenericLightProperties(ofxUISuperCanvas *g, ofxLight *l)
{
    float length = (g->getGlobalCanvasWidth()-g->getWidgetSpacing()*5)/3.;
    float dim = g->getGlobalSliderHeight();
    
    switch(l->light.getType())
    {
        case OF_LIGHT_POINT:
        {
            g->addLabel("POSITION", OFX_UI_FONT_SMALL);
            g->addMinimalSlider("X", -1000.0, 1000.0, &l->lightPos.x, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
            g->addMinimalSlider("Y", -1000.0, 1000.0, &l->lightPos.y, length, dim)->setShowValue(false);
            g->addMinimalSlider("Z", -1000.0, 1000.0, &l->lightPos.z, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
            g->addSpacer();
        }
            break;
            
        case OF_LIGHT_SPOT:
        {
            g->addLabel("POSITION", OFX_UI_FONT_SMALL);
            g->addMinimalSlider("X", -1000.0, 1000.0, &l->lightPos.x, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
            g->addMinimalSlider("Y", -1000.0, 1000.0, &l->lightPos.y, length, dim)->setShowValue(false);
            g->addMinimalSlider("Z", -1000.0, 1000.0, &l->lightPos.z, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
            g->addSpacer();
            
            g->addLabel("ORIENTATION", OFX_UI_FONT_SMALL);
            g->addMinimalSlider("OX", -90.0, 90.0, &l->lightOrientation.x, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
            g->addMinimalSlider("OY", -90.0, 90.0, &l->lightOrientation.y, length, dim)->setShowValue(false);
            g->addMinimalSlider("OZ", -90.0, 90.0, &l->lightOrientation.z, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
            g->addSpacer();
        }
            break;
            
        case OF_LIGHT_DIRECTIONAL:
        {
            g->addLabel("ORIENTATION", OFX_UI_FONT_SMALL);
            g->addMinimalSlider("OX", -90.0, 90.0, &l->lightOrientation.x, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
            g->addMinimalSlider("OY", -90.0, 90.0, &l->lightOrientation.y, length, dim)->setShowValue(false);
            g->addMinimalSlider("OZ", -90.0, 90.0, &l->lightOrientation.z, length, dim)->setShowValue(false);
            g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
            g->addSpacer();
        }
            break;
    }
    
    g->addLabel("AMBIENT", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("AR", 0.0, 1.0, &l->lightAmbient.r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("AG", 0.0, 1.0, &l->lightAmbient.g, length, dim)->setShowValue(false);
    g->addMinimalSlider("AB", 0.0, 1.0, &l->lightAmbient.b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    g->addSpacer();
    g->addLabel("DIFFUSE", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("DR", 0.0, 1.0, &l->lightDiffuse.r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("DG", 0.0, 1.0, &l->lightDiffuse.g, length, dim)->setShowValue(false);
    g->addMinimalSlider("DB", 0.0, 1.0, &l->lightDiffuse.b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    g->addSpacer();
    g->addLabel("SPECULAR", OFX_UI_FONT_SMALL);
    g->addMinimalSlider("SR", 0.0, 1.0, &l->lightSpecular.r, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    g->addMinimalSlider("SG", 0.0, 1.0, &l->lightSpecular.g, length, dim)->setShowValue(false);
    g->addMinimalSlider("SB", 0.0, 1.0, &l->lightSpecular.b, length, dim)->setShowValue(false);
    g->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    g->addSpacer();
}

void CloudsVisualSystemRezanator::guiLightEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemRezanator::lightsBegin()
{
    ofSetSmoothLighting(bSmoothLighting);
    if(bEnableLights)
    {
        for(map<string, ofxLight *>::iterator it = lights.begin(); it != lights.end(); ++it)
        {
            ofEnableLighting();
            it->second->enable();
        }
    }
}

void CloudsVisualSystemRezanator::lightsEnd()
{
    if(!bEnableLights)
    {
        ofDisableLighting();
        for(map<string, ofxLight *>::iterator it = lights.begin(); it != lights.end(); ++it)
        {
            it->second->disable();
        }
    }
}

void CloudsVisualSystemRezanator::loadGUIS()
{
    for(int i = 0; i < guis.size(); i++)
    {
        guis[i]->loadSettings(getVisualSystemDataPath()+"Working/"+getSystemName()+guis[i]->getName()+".xml");
    }
    ofxLoadCamera(cam, getVisualSystemDataPath()+"Working/"+"ofEasyCamSettings");
}

void CloudsVisualSystemRezanator::saveGUIS()
{
    for(int i = 0; i < guis.size(); i++)
    {
        guis[i]->saveSettings(getVisualSystemDataPath()+"Working/"+getSystemName()+guis[i]->getName()+".xml");
    }
    ofxSaveCamera(cam, getVisualSystemDataPath()+"Working/"+"ofEasyCamSettings");
}

void CloudsVisualSystemRezanator::loadPresetGUIS(string presetPath)
{
    for(int i = 0; i < guis.size(); i++)
    {
        if(guis[i]->getName() != "Presets")
        {
            guis[i]->loadSettings(presetPath+"/"+getSystemName()+guis[i]->getName()+".xml");
        }
    }
    ofxLoadCamera(cam, presetPath+"/"+"ofEasyCamSettings");
}

void CloudsVisualSystemRezanator::savePresetGUIS(string presetName)
{
    ofDirectory dir;
    string presetDirectory = getVisualSystemDataPath()+presetName+"/";
    if(!dir.doesDirectoryExist(presetDirectory))
    {
        dir.createDirectory(presetDirectory);
        presetRadio->addToggle(presetGui->addToggle(presetName, true));
        presetGui->autoSizeToFitWidgets();
    }
    
    for(int i = 0; i < guis.size(); i++)
    {
        guis[i]->saveSettings(presetDirectory+getSystemName()+guis[i]->getName()+".xml");
    }
    ofxSaveCamera(cam, getVisualSystemDataPath()+presetName+"/"+"ofEasyCamSettings");
}

void CloudsVisualSystemRezanator::deleteGUIS()
{
    for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
    {
        ofxUICanvas *g = (*it);
        delete g;
    }
    guis.clear();
}

void CloudsVisualSystemRezanator::showGUIS()
{
    for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
    {
        (*it)->enable();
    }
}

void CloudsVisualSystemRezanator::hideGUIS()
{
    for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
    {
        (*it)->disable();
    }
}

void CloudsVisualSystemRezanator::toggleGUIS()
{
    for(vector<ofxUISuperCanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
    {
        (*it)->toggleVisible();
    }
}

void CloudsVisualSystemRezanator::toggleGuiAndPosition(ofxUISuperCanvas *g)
{
    if(g->isMinified())
    {
        g->setMinified(false);
        g->setPosition(ofGetMouseX(), ofGetMouseY());
    }
    else
    {
        g->setMinified(true);
    }
}

void CloudsVisualSystemRezanator::drawDebug()
{
    if(bDebug)
    {
        float color = 255-bgBri->getPos();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        drawGrid(-debugGridSize,-debugGridSize,debugGridSize*2,debugGridSize*2, color);
        drawAxis(debugGridSize, color);
        selfDrawDebug();
    }
}

void CloudsVisualSystemRezanator::drawAxis(float size, float color)
{
    ofSetColor(color, 100);
    ofLine(-size, 0, 0, size, 0, 0);
    ofLine(0, -size, 0, 0, size, 0);
    ofLine(0, 0, -size, 0, 0, size);
}

void CloudsVisualSystemRezanator::drawGrid(float x, float y, float w, float h, float color)
{
    ofNoFill();
    ofSetColor(color, 25);
    ofRect(x, y, w, h);
    
    float md = MAX(w,h);
    ofSetColor(color, 50);
    for(float i = 0; i <= w; i+=md*.025)
    {
        ofLine(x+i, y, x+i, y+h);
    }
    for(float j = 0; j <= h; j+=md*.025)
    {
        ofLine(x, y+j, x+w, y+j);
    }
}

void CloudsVisualSystemRezanator::billBoard(ofVec3f globalCamPosition, ofVec3f globelObjectPosition)
{
    ofVec3f objectLookAt = ofVec3f(0,0,1);
    ofVec3f objToCam = globalCamPosition - globelObjectPosition;
    objToCam.normalize();
    float theta = objectLookAt.angle(objToCam);
    ofVec3f axisOfRotation = objToCam.crossed(objectLookAt);
    axisOfRotation.normalize();
    glRotatef(-theta, axisOfRotation.x, axisOfRotation.y, axisOfRotation.z);
}

void CloudsVisualSystemRezanator::drawTexturedQuad()
{
    glBegin (GL_QUADS);
    
    glTexCoord2f (0.0, 0.0);
    glVertex3f (0.0, 0.0, 0.0);
    
    glTexCoord2f (ofGetWidth(), 0.0);
    glVertex3f (ofGetWidth(), 0.0, 0.0);
    
    
    glTexCoord2f (ofGetWidth(), ofGetHeight());
    glVertex3f (ofGetWidth(), ofGetHeight(), 0.0);
    
    glTexCoord2f (0.0, ofGetHeight());
    glVertex3f (0.0, ofGetHeight(), 0.0);
    
    glEnd ();
}

void CloudsVisualSystemRezanator::drawNormalizedTexturedQuad()
{
    glBegin (GL_QUADS);
    
    glTexCoord2f (0.0, 0.0);
    glVertex3f (0.0, 0.0, 0.0);
    
    glTexCoord2f (1.0, 0.0);
    glVertex3f (ofGetWidth(), 0.0, 0.0);
    
    
    glTexCoord2f (1.0, 1.0);
    glVertex3f (ofGetWidth(), ofGetHeight(), 0.0);
    
    glTexCoord2f (0.0, 1.0);
    glVertex3f (0.0, ofGetHeight(), 0.0);
    
    glEnd ();
}

void CloudsVisualSystemRezanator::drawBackground()
{
    if(gradientMode == OF_GRADIENT_CIRCULAR)
    {
        ofPushMatrix();
        if(camFOV > 60)
        {
            ofBackground(*bgColor2);
        }        
        billBoard(cam.getGlobalPosition(), ofVec3f(0,0,0));
        ofDisableLighting();
        ofSetSmoothLighting(true);
        glNormal3f(0,0,1);
        ofLayerGradient(*bgColor, *bgColor2);
        ofPopMatrix();
        selfDrawBackground();
    }
    else
    {
        ofSetSmoothLighting(false);
        ofBackground(*bgColor);
    }
}

void CloudsVisualSystemRezanator::ofLayerGradient(const ofColor& start, const ofColor& end)
{
    float w = cam.getDistance()*bgAspectRatio;
    float h = cam.getDistance()*bgAspectRatio;
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    // this could be optimized by building a single mesh once, then copying
    // it and just adding the colors whenever the function is called.
    ofVec2f center(0.0,0.0);
    mesh.addVertex(center);
    mesh.addColor(start);
    int n = 32; // circular gradient resolution
    float angleBisector = TWO_PI / (n * 2);
    float smallRadius = ofDist(0, 0, w / 2, h / 2);
    float bigRadius = smallRadius / cos(angleBisector);
    for(int i = 0; i <= n; i++) {
        float theta = i * TWO_PI / n;
        mesh.addVertex(center + ofVec2f(sin(theta), cos(theta)) * bigRadius);
        mesh.addColor(end);
    }
    glDepthMask(false);
    mesh.draw();
    glDepthMask(true);
}

//Grab These Methods
string CloudsVisualSystemRezanator::getSystemName()
{
	return "Rezanator";
}

void CloudsVisualSystemRezanator::selfSetup()
{
    
}

void CloudsVisualSystemRezanator::selfSetupGuis()
{
    
}

void CloudsVisualSystemRezanator::selfUpdate()
{
    
}

void CloudsVisualSystemRezanator::selfDrawBackground()
{
    
}

void CloudsVisualSystemRezanator::selfDrawDebug()
{
    
}

void CloudsVisualSystemRezanator::selfSceneTransformation()
{
    
}

void CloudsVisualSystemRezanator::selfDraw()
{
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    mat->begin();
    ofSetColor(ofColor(255));
    ofFill();
    ofDrawSphere(100); 
    mat->end();
}

void CloudsVisualSystemRezanator::selfExit()
{
    
}

void CloudsVisualSystemRezanator::selfBegin()
{
    
}

void CloudsVisualSystemRezanator::selfEnd()
{
    
}

void CloudsVisualSystemRezanator::selfKeyPressed(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemRezanator::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemRezanator::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemRezanator::selfMouseMoved(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemRezanator::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemRezanator::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemRezanator::selfSetupGui()
{

}

void CloudsVisualSystemRezanator::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemRezanator::selfSetupSystemGui()
{
    
}

void CloudsVisualSystemRezanator::guiSystemEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemRezanator::selfSetupRenderGui()
{
    
}

void CloudsVisualSystemRezanator::guiRenderEvent(ofxUIEventArgs &e)
{
    
}