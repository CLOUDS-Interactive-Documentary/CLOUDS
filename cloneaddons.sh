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

git clone https://github.com/rezaali/ofxUI
cd ofxUI
git checkout -b develop --track origin/develop
git pull
cd ..

git clone https://github.com/satoruhiga/ofxCocoaGLView
git clone https://github.com/satoruhiga/ofxPCL
git clone https://github.com/memo/ofxMSAObjCPointer
git clone https://github.com/obviousjim/ofxMSACore #temp obviousjim for windows
git clone https://github.com/roymacdonald/ofxCameraSaveLoad
git clone https://github.com/memo/ofxMSAPhysics
git clone https://github.com/memo/ofxMSAObjCPointer
git clone https://github.com/Larsberg/ofxMarchingCubes
git clone https://github.com/andreasmuller/ofxAutoReloadedShader
git clone https://github.com/obviousjim/ofxDelaunay
git clone https://github.com/prisonerjohn/ofxSQLite
git clone https://github.com/prisonerjohn/ofxFTGL
git clone https://github.com/prisonerjohn/ofxTextWriter
git clone https://github.com/prisonerjohn/ofxBillboard
git clone https://github.com/quinkennedy/ofxRegex
git clone https://github.com/satoruhiga/ofxAnimationPrimitives
git clone https://github.com/vanderlin/ofxBox2d.git
git clone https://github.com/underdoeg/ofxOpenSteer.git
git clone https://github.com/Larsberg/ofxSimpleSpline
git clone https://github.com/Larsberg/ofxSimpleSurface

${PREFIX}obviousjim/ofxGenerative #temp obviousjim for windows fix
${PREFIX}obviousjim/ofxAVFVideoPlayer
${PREFIX}obviousjim/ofxOculusRift
${PREFIX}rezaali/ofxColorPalettes
${PREFIX}obviousjim/ofxOcean
${PREFIX}Flightphase/ofxGameCamera
${PREFIX}Flightphase/ofxRange
${PREFIX}Flightphase/ofxTween
${PREFIX}Flightphase/ofxMSAInteractiveObject

${PREFIX}Flightphase/ofxTextInputField
cd ofxTextInputField
git checkout -b develop --track origin/develop
cd ..

${PREFIX}obviousjim/ofxRGBDepth
cd ofxRGBDepth
git checkout -b develop --track origin/develop
cd ..

${PREFIX}obviousjim/ofxCv
${PREFIX}obviousjim/ofxKinect
${PREFIX}obviousjim/ofxOpenNI
${PREFIX}obviousjim/ofxMSATimer

${PREFIX}YCAMInterlab/ofxTimecode
${PREFIX}YCAMInterlab/ofxTimeline

cd ofxTimeline
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${PREFIX}obviousjim/ofxObjLoader		
