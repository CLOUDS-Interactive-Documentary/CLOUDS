#!/bin/bash

# Dependencies for RGBDToolkit

#make sure you are in the RGBDToolkit project when you run this script

cd ../../../addons
if [ $? -ne 0 ]; then
    echo "Couldn't cd into addons. Make sure you are checked out to of_v0.7.4_osx_release/apps/myApps"
	exit
fi

#passing no arguments means read only
if [ -z $1 ]; then
    PREFIX="git clone https://github.com/"
	echo "Cloning read-only"
else
    PREFIX="git clone git@github.com:"
	echo "Cloning writable"
fi

#private, must be read
git clone https://github.com/rezaali/ofxUI
cd ofxUI
git checkout --track develop
cd ..

${PREFIX}YCAMInterlab/ofxTimeline
cd ofxTimeline
git checkout --track develop
cd ..

git clone https://github.com/roymacdonald/ofxCameraSaveLoad
${PREFIX}rezaali/ofxGenerative
${PREFIX}obviousjim/ofxAVFVideoPlayer
${PREFIX}Flightphase/ofxGameCamera
${PREFIX}Flightphase/ofxRange
${PREFIX}Flightphase/ofxTween
${PREFIX}Flightphase/ofxTextInputField
${PREFIX}obviousjim/ofxMSATimer
${PREFIX}YCAMInterlab/ofxTimecode

