#CLOUDS Visual Systems

Welcome to the CLOUDS Interactive Documentary CodeStorm! We'll be creating Visual Systems, modular visual sketches used that are used along with the pointcloud interviews to illustrate the concepts presented in the film.

In order to make the process of making a VisualSystem efficient and easy we have put together a wrapper that is a lot like a testApp.cpp but with ofxTimeline and ofxUI built in for variable and animation controls. The system can also store all the settings in presets letting you configure each sketch in different ways and invoke the visuals within the film.

## Create your own visual system

Download a fresh openFrameworks 0.7.4 from http://www.openframeworks.cc/download/. Unzip it and do the following commands in the terminal
	
* Clone the empty template

		$ cd of_v0.7.4_osx_release/apps/myApps
		$ git clone https://github.com/CLOUDS-Interactive-Documentary/EmptyVisualSystem
	
* Get the shared code library for CLOUDS
 
		$ cd EmptyVisualSystem/
		$ git submodule init
		$ git submodule update
		
* Clone all the addons

		$ chmod 733 cloneaddons.sh
		$ ./cloneaddons.sh
		
Next you need to rename the project to the name of your sketch,
	
* Rename the project file from EmptyVisualSystem to something else
* In `CloudsVisualSystemEmpty.h` modify the system name to match your sketch
		
		string getSystemName(){
			return "MyVisualSystem"
		}

* Change CloudsVisualSystem class name to another class name using find-replace
* Ask us to create a new repo on [CLOUDS-Interactive-Documentary](https://github.com/CLOUDS-Interactive-Documentary) that matches your sketch name 'MyVisualSystem' 
* Its URL would be git@github.com:CLOUDS-Interactive-Documentary/MyVisualSystem
* Rename emptyVisualSystem folder in myApps/ to MyVisualSystem
* In the terminal, cd into MyVisualSystem/ and redirect from the empty repo to your new repo

		$ git remote rm origin
		$ git remote add origin git@github.com:CLOUDS-Interactive-Documentary/MyVisualSystem.git 
		$ git push -u origin master

### Modify ofMaterial

To Run you'll need to modify two things in the oF Core

In libs/openFrameworks/gl/ofMaterial.h add reference `&` to the material colors

		ofFloatColor& getDiffuseColor();
		ofFloatColor& getAmbientColor();
		ofFloatColor& getSpecularColor();
		ofFloatColor& getEmissiveColor();
		float& getShininess();

Match the changes in libs/openFrameworks/gl/ofMaterial.cpp 

		float& ofMaterial::getShininess(){
			return shininess;
		}
		
		ofFloatColor& ofMaterial::getDiffuseColor() {
			return diffuse;
		}
		
		ofFloatColor& ofMaterial::getAmbientColor() {
			return ambient;
		}
		
		ofFloatColor& ofMaterial::getSpecularColor() {
			return specular;
		}
		
		ofFloatColor& ofMaterial::getEmissiveColor() {
			return emissive;
		}


### Included Addons

CLOUDS uses many addons from the community 

* [ofxRange](https://github.com/Flightphase/ofxRange) by James George

* [ofxMSATimer](https://github.com/obviousjim/ofxMSATimer) by Memo Atken, James George, Philip Whitfield, Juan Pablo Manson
		
* [ofxTween](https://github.com/Flightphase/ofxTween) by Arturo Castro, James George

* [ofxTextInputField](https://github.com/Flightphase/ofxTextInputField) by James George & Elliot Woods
		
* [ofxTimecode](https://github.com/YCAMInterlab/ofxTimecode) by James George
		
* [ofxTimeline](https://github.com/YCAMInterlab/ofxTimeline) by James George, supported by YCAMInterlab
	
* [ofxCameraSaveLoad](https://github.com/roymacdonald/ofxCameraSaveLoad) by Roy MacDonald
		
* [ofxGenerative](https://github.com/rezaali/ofxGenerative) by Reza Ali
		
* [ofxUI](https://github.com/rezaali/ofxUI) by Reza Ali

### Loading Data

Anywhere you want to access things in your bin/data folder you'll need to do so by first calling 

    getVisualSystemDataPath() + "path/to/my/stuff"
    
This will ensure that your data access works when running the module inside of CLOUDS.

### Saving Presets

CLOUDS VisualSystems are driven by presets. To create a preset, configure the variables and timing just how you like and then click Save on the main GUI. Enter a name on the popup window and confirm. This will save all your settings into bin/data/Presets/

The Presets GUI now has a button with the name you just entered, and you can click it to bring these settings back. Feel free to change anything after you've created a preset, since you'll always be modifying the Working/ preset.

Make sure to commit your presets so everyone can check them out!

### Hot-Keys 

Some useful hotkeys for the interface
* F : Toogle Fullscreen
* H : Hide/Show GUI
* E : Arrange GUI in Cascade  
* R : Arrange GUI at Top
* T : Minimize GUI
* Y : Arrange GUI in circle
* ` : Take a snapshoot
* SPACE : Toggly play on the timeline
* SHIFT+T : Set a camera point on the camera track
* SHIFT+L : Toggle following your camera track

