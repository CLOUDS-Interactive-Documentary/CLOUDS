#!/bin/bash

cd ../../addons
echo existing addons
ls

#passing no arguments means read only
if [ -z $1 ]; then
    PREFIX="git clone git@github.com:"
    NEWPREFIX="git clone git@github.com:CLOUDS-Interactive-Documentary/"
	echo "Cloning writable"
else
    PREFIX="git clone https://github.com/"
    NEWPREFIX="git clone https://github.com/CLOUDS-Interactive-Documentary/"
	echo "Cloning read-only"
fi


${NEWPREFIX}ofxUI
cd ofxUI
git checkout -b 'clouds-develop' --track 'origin/clouds-develop'
git pull origin 'clouds-develop'
cd ..

${NEWPREFIX}ofxCocoaGLView
${NEWPREFIX}ofxPCL
${NEWPREFIX}ofxMSAObjCPointer
${NEWPREFIX}ofxMSACore
${NEWPREFIX}ofxCameraSaveLoad
${NEWPREFIX}ofxMSAPhysics
${NEWPREFIX}ofxMarchingCubes
${NEWPREFIX}ofxAutoReloadedShader
${NEWPREFIX}ofxDelaunay
${NEWPREFIX}ofxSQLite
${NEWPREFIX}ofxTextWriter
${NEWPREFIX}ofxBillboard
${NEWPREFIX}ofxRegex
${NEWPREFIX}ofxAnimationPrimitives
${NEWPREFIX}ofxBox2d
${NEWPREFIX}ofxOpenSteer
${NEWPREFIX}ofxSimpleSpline
${NEWPREFIX}ofxSimpleSurface
${NEWPREFIX}ofxLibwebsockets
${NEWPREFIX}ofxJSON
${NEWPREFIX}ofxTonic
${NEWPREFIX}ofxPtf
${NEWPREFIX}ofxNearestNeighbour
${NEWPREFIX}ofxPostProcessing
${NEWPREFIX}ofxSubtitles

${NEWPREFIX}ofxCv
${NEWPREFIX}ofxGpuParticles

cd ofxPostProcessing
git checkout -b develop --track origin/develop
git pull origin develop
cd ..


${NEWPREFIX}ofxGenerative
${NEWPREFIX}ofxAVFVideoPlayer
${NEWPREFIX}ofxOculusRift
${NEWPREFIX}ofxColorPalettes
${NEWPREFIX}ofxOcean
${NEWPREFIX}ofxGameCamera
${NEWPREFIX}ofxRange
${NEWPREFIX}ofxTween
${NEWPREFIX}ofxMSAInteractiveObject
${NEWPREFIX}ofxFTGL
${NEWPREFIX}ofxMPMFluid

${NEWPREFIX}ofxAudioUnit
${NEWPREFIX}ofxAudioFeatures
${NEWPREFIX}ofxMeshUtils

${NEWPREFIX}ofxTextInputField
cd ofxTextInputField
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${NEWPREFIX}ofxDepthKit
cd ofxDepthKit
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

#${NEWPREFIX}ofxKinect
#${NEWPREFIX}ofxOpenNI
${NEWPREFIX}ofxMSATimer

${NEWPREFIX}ofxTimecode
${NEWPREFIX}ofxTimeline

cd ofxTimeline
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${NEWPREFIX}ofxObjLoader	

