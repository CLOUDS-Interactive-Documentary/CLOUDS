#!/bin/bash

# Dependencies for RGBDToolkit

#make sure you are in the RGBDToolkit project when you run this script

cd ../../addons

#passing no arguments means read only
if [ -z $1 ]; then
    PREFIX="git clone https://github.com/"
	echo "Cloning read-only"
else
    PREFIX="git clone git@github.com:"
	echo "Cloning writable"
fi

#private, must be read
git clone git@github.com:rezaali/ofxExtras

git clone satoruhiga/ofxCocoaGLView
git clone satoruhiga/ofxPCL
git clone memo/ofxMSAObjCPointer
git clone memo/ofxMSACore
git clone https://github.com/roymacdonald/ofxCameraSaveLoad

${PREFIX}rezaali/ofxGenerative
git clone rezaali/ofxUI
${PREFIX}Flightphase/ofxGameCamera
${PREFIX}Flightphase/ofxRange
${PREFIX}Flightphase/ofxTween
${PREFIX}Flightphase/ofxMSAInteractiveObject
${PREFIX}Flightphase/ofxTextInputField
${PREFIX}obviousjim/ofxRGBDepth
${PREFIX}obviousjim/ofxCv
${PREFIX}obviousjim/ofxKinect
${PREFIX}obviousjim/ofxOpenNI
${PREFIX}obviousjim/ofxMSATimer
${PREFIX}YCAMInterlab/ofxTimecode
${PREFIX}YCAMInterlab/ofxTimeline
${PREFIX}obviousjim/ofxObjLoader		
