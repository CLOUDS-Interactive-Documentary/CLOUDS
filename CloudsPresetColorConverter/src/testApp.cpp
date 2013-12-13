#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
	ofDirectory dir(dragInfo.files[0]);
	dir.listDir();
	
	for(int i = 0; i < dir.numFiles(); i++){
		ofDirectory subdir(dir.getPath(i));
		if(subdir.isDirectory()){
			ofDirectory presetDir(subdir.getOriginalDirectory() + "Presets/");
			if(presetDir.exists()){
				presetDir.listDir();
				for(int preset = 0; preset < presetDir.numFiles(); preset++){
					
					///POINT LIGHT -----
					{
						string pointLightXMLFile = presetDir.getPath(preset) + "/POINT LIGHT 1Settings.xml";
						ofxXmlSettings pointLightXML;
						if(pointLightXML.load(pointLightXMLFile) ){
							
							cout << "+ Preset " << presetDir.getPath(preset) << " has point lights!" << endl;
							
							ofFloatColor ambientRGB,specularRGB,diffuseRGB;
							int numWidgets = pointLightXML.getNumTags("Widget");
							for(int w = 0; w < numWidgets; w++){
								pointLightXML.pushTag("Widget", w);
								
								if((pointLightXML.getValue("Name", "") == "AR")) ambientRGB.r  = pointLightXML.getValue("Value", 0.);
								if((pointLightXML.getValue("Name", "") == "AG")) ambientRGB.g  = pointLightXML.getValue("Value", 0.);
								if((pointLightXML.getValue("Name", "") == "AB")) ambientRGB.b  = pointLightXML.getValue("Value", 0.);

								if((pointLightXML.getValue("Name", "") == "DR")) diffuseRGB.r  = pointLightXML.getValue("Value", 0.);
								if((pointLightXML.getValue("Name", "") == "DG")) diffuseRGB.g  = pointLightXML.getValue("Value", 0.);
								if((pointLightXML.getValue("Name", "") == "DB")) diffuseRGB.b  = pointLightXML.getValue("Value", 0.);
								
								if((pointLightXML.getValue("Name", "") == "SR")) specularRGB.r = pointLightXML.getValue("Value", 0.);
								if((pointLightXML.getValue("Name", "") == "SG")) specularRGB.g = pointLightXML.getValue("Value", 0.);
								if((pointLightXML.getValue("Name", "") == "SB")) specularRGB.b = pointLightXML.getValue("Value", 0.);

								pointLightXML.popTag();
							}
								
							//AMBIENT
							int w;
							{
								//H
								w = pointLightXML.addTag("Widget");
								pointLightXML.pushTag("Widget", w);
								pointLightXML.addValue("Kind", 25);
								pointLightXML.addValue("Name", "AH");
								pointLightXML.addValue("Value", ambientRGB.getHue());
								pointLightXML.popTag();//Widget
								
								//S
								w = pointLightXML.addTag("Widget");
								pointLightXML.pushTag("Widget", w);
								pointLightXML.addValue("Kind", 25);
								pointLightXML.addValue("Name", "AS");
								pointLightXML.addValue("Value", ambientRGB.getSaturation());
								pointLightXML.popTag();//Widget
								
								//V
								w = pointLightXML.addTag("Widget");
								pointLightXML.pushTag("Widget", w);
								pointLightXML.addValue("Kind", 25);
								pointLightXML.addValue("Name", "AV");
								pointLightXML.addValue("Value", ambientRGB.getBrightness());
								pointLightXML.popTag();//Widget
								
							}
							
							//DIFFUSE
							{
								//H
								w = pointLightXML.addTag("Widget");
								pointLightXML.pushTag("Widget", w);
								pointLightXML.addValue("Kind", 25);
								pointLightXML.addValue("Name", "DH");
								pointLightXML.addValue("Value", diffuseRGB.getHue());
								pointLightXML.popTag();//Widget
								
								//S
								w = pointLightXML.addTag("Widget");
								pointLightXML.pushTag("Widget", w);
								pointLightXML.addValue("Kind", 25);
								pointLightXML.addValue("Name", "DS");
								pointLightXML.addValue("Value", diffuseRGB.getSaturation());
								pointLightXML.popTag();//Widget
								
								//V
								w = pointLightXML.addTag("Widget");
								pointLightXML.pushTag("Widget", w);
								pointLightXML.addValue("Kind", 25);
								pointLightXML.addValue("Name", "DV");
								pointLightXML.addValue("Value", diffuseRGB.getBrightness());
								pointLightXML.popTag();//Widget
							}
							
							//SPECULAR
							{
								//H
								w = pointLightXML.addTag("Widget");
								pointLightXML.pushTag("Widget", w);
								pointLightXML.addValue("Kind", 25);
								pointLightXML.addValue("Name", "SH");
								pointLightXML.addValue("Value", specularRGB.getHue());
								pointLightXML.popTag();//Widget
								
								//S
								w = pointLightXML.addTag("Widget");
								pointLightXML.pushTag("Widget", w);
								pointLightXML.addValue("Kind", 25);
								pointLightXML.addValue("Name", "SS");
								pointLightXML.addValue("Value", specularRGB.getSaturation());
								pointLightXML.popTag();//Widget
								
								//V
								w = pointLightXML.addTag("Widget");
								pointLightXML.pushTag("Widget", w);
								pointLightXML.addValue("Kind", 25);
								pointLightXML.addValue("Name", "SV");
								pointLightXML.addValue("Value", specularRGB.getBrightness());
								pointLightXML.popTag();//Widget
								
							}
							
							cout << "POINT LIGHT WAS: "  << endl;
							cout << "	" << ambientRGB  << endl;
							cout << "	" << specularRGB << endl;
							cout << "	" << diffuseRGB  << endl;
							
							pointLightXML.save(pointLightXMLFile);
						}
						else{
							cout << "- Preset " << presetDir.getPath(preset) << " does not have point light settings" << endl;
						}
					}
					
					///MATERIAL -----
					{
						ofxXmlSettings materialXML;
						string materialXMLFile = presetDir.getPath(preset) + "/MATERIAL 1Settings.xml";
						if(materialXML.load(materialXMLFile)){
							ofFloatColor ambientRGB,emissiveRGB,specularRGB,diffuseRGB;
							cout << "+ Preset " << presetDir.getPath(preset) << " has material settings!" << endl;
							int numWidgets = materialXML.getNumTags("Widget");
							for(int w = 0; w < numWidgets; w++){
								materialXML.pushTag("Widget", w);
								
								if( materialXML.getValue("Name", "") == "AR" ){
								   if(w < 3){
									   ambientRGB.r = materialXML.getValue("Value", 0.);
									   cout << "ambient R value" << ambientRGB.r << endl;
								   }
								   else{
									   diffuseRGB.r = materialXML.getValue("Value", 0.);
								   }
								}
								
								if( (materialXML.getValue("Name", "") == "AG") ){
									if(w < 3){
										ambientRGB.g = materialXML.getValue("Value", 0.);
									}
									else{
										diffuseRGB.g = materialXML.getValue("Value", 0.);
									}
								}
								if( (materialXML.getValue("Name", "") == "AB") ){
									if(w < 3){
										ambientRGB.b = materialXML.getValue("Value", 0.);
									}
									else{
										diffuseRGB.b = materialXML.getValue("Value", 0.);
									}
								}
								
								if((materialXML.getValue("Name", "") == "ER")) emissiveRGB.r = materialXML.getValue("Value", 0.);
								if((materialXML.getValue("Name", "") == "EG")) emissiveRGB.g = materialXML.getValue("Value", 0.);
								if((materialXML.getValue("Name", "") == "EB")) emissiveRGB.b = materialXML.getValue("Value", 0.);

								if((materialXML.getValue("Name", "") == "SR")) specularRGB.r = materialXML.getValue("Value", 0.);
								if((materialXML.getValue("Name", "") == "SG")) specularRGB.g = materialXML.getValue("Value", 0.);
								if((materialXML.getValue("Name", "") == "SB")) specularRGB.b = materialXML.getValue("Value", 0.);

								materialXML.popTag();
							}
							
							//AMBIENT
							int w;
							{
								
								//H
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "AH");
								materialXML.addValue("Value", ambientRGB.getHue());
								materialXML.popTag();//Widget
								
								//S
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "AS");
								materialXML.addValue("Value", ambientRGB.getSaturation());
								materialXML.popTag();//Widget

								//V
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "AV");
								materialXML.addValue("Value", ambientRGB.getBrightness());
								materialXML.popTag();//Widget
								
							}
							
							//DIFFUSE
							{
								//H
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "DH");
								materialXML.addValue("Value", diffuseRGB.getHue());
								materialXML.popTag();//Widget

								//S
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "DS");
								materialXML.addValue("Value", diffuseRGB.getSaturation());
								materialXML.popTag();//Widget

								//V
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "DV");
								materialXML.addValue("Value", diffuseRGB.getBrightness());
								materialXML.popTag();//Widget
							}
							
							//EMISSIVE
							{
								//H
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "EV");
								materialXML.addValue("Value", emissiveRGB.getHue());
								materialXML.popTag();//Widget
								
								//S
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "ES");
								materialXML.addValue("Value", emissiveRGB.getSaturation());
								materialXML.popTag();//Widget
								
								//V
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "EV");
								materialXML.addValue("Value", emissiveRGB.getBrightness());							
								materialXML.popTag();//Widget
							}
							
							//SPECULAR
							{
								//H
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "SH");
								materialXML.addValue("Value", specularRGB.getHue());
								materialXML.popTag();//Widget
								
								//S
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "SS");
								materialXML.addValue("Value", specularRGB.getSaturation());
								materialXML.popTag();//Widget

								//V
								w = materialXML.addTag("Widget");
								materialXML.pushTag("Widget", w);
								materialXML.addValue("Kind", 25);
								materialXML.addValue("Name", "SV");
								materialXML.addValue("Value", specularRGB.getBrightness());
								materialXML.popTag();//Widget
								
							}
							
							materialXML.save(materialXMLFile);
							
							cout << "MATERIAL WAS: "  << endl;
							cout << "	" << ambientRGB  << endl;
							cout << "	" << specularRGB << endl;
							cout << "	" << diffuseRGB  << endl;
							cout << "	" << emissiveRGB  << endl;
							

						}
						else{
							cout << "- Preset " << presetDir.getPath(preset) << " Does not have material settings" << endl;
						}
					}
					
					//LIGHT SETTINGS
					{
						ofxXmlSettings lightXML;
						string lightXMLFile = presetDir.getPath(preset) + "/LightSettings.xml";
						if(lightXML.load(lightXMLFile)){
							
							cout << "+ Preset " << presetDir.getPath(preset) << " has global light!" << endl;
							
							ofFloatColor globalAmbientRGB;
							int numWidgets = lightXML.getNumTags("Widget");
							for(int w = 0; w < numWidgets; w++){
								
								lightXML.pushTag("Widget", w);

								if((lightXML.getValue("Name", "") == "R")) globalAmbientRGB.r = lightXML.getValue("Value", 0.);
								if((lightXML.getValue("Name", "") == "G")) globalAmbientRGB.g = lightXML.getValue("Value", 0.);
								if((lightXML.getValue("Name", "") == "B")) globalAmbientRGB.b = lightXML.getValue("Value", 0.);
								
								lightXML.popTag();
							}
							
							int w;
							{
								
								//H
								w = lightXML.addTag("Widget");
								lightXML.pushTag("Widget", w);
								lightXML.addValue("Kind", 25);
								lightXML.addValue("Name", "H");
								lightXML.addValue("Value", globalAmbientRGB.getHue());
								lightXML.popTag();//Widget
								
								//S
								w = lightXML.addTag("Widget");
								lightXML.pushTag("Widget", w);
								lightXML.addValue("Kind", 25);
								lightXML.addValue("Name", "S");
								lightXML.addValue("Value", globalAmbientRGB.getSaturation());
								lightXML.popTag();//Widget
								
								//V
								w = lightXML.addTag("Widget");
								lightXML.pushTag("Widget", w);
								lightXML.addValue("Kind", 25);
								lightXML.addValue("Name", "V");
								lightXML.addValue("Value", globalAmbientRGB.getBrightness());
								lightXML.popTag();//Widget
								
							}
							
							cout << "LIGHT SETTING WAS: "  << endl;
							cout << "	" << globalAmbientRGB  << endl;
							lightXML.save(lightXMLFile);
						}
						else{
							cout << "- Preset " << presetDir.getPath(preset) << " Does not have global light settings" << endl;
						}
					}
				}
			}
		}
	}
}

	