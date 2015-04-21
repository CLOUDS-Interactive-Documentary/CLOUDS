#!/bin/bash

# Dependencies for RGBDToolkit

#make sure you are in the RGBDToolkit project when you run this script


cd ../../addons
echo existing addons
ls -1a

#passing some argument reverts read only
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
git checkout -b 'clouds-64' --track 'origin/clouds-64'
git pull origin 'clouds-64'
cd ..

${NEWPREFIX}ofxCocoaGLView
${NEWPREFIX}ofxPCL
${NEWPREFIX}ofxMSAObjCPointer
${NEWPREFIX}ofxMSACore
${NEWPREFIX}ofxCameraSaveLoad
${NEWPREFIX}ofxMSAPhysics
${NEWPREFIX}ofxMarchingCubes
${NEWPREFIX}ofxAutoReloadedShader
${NEWPREFIX}ofxSQLite
${NEWPREFIX}ofxTextWriter
${NEWPREFIX}ofxBillboard
${NEWPREFIX}ofxRegex
${NEWPREFIX}ofxAnimationPrimitives
${NEWPREFIX}ofxBox2d
${NEWPREFIX}ofxLocalization

${NEWPREFIX}ofxOpenSteer
cd ofxOpenSteer
git checkout -b feature-64bit --track origin/feature-64bit
git pull origin feature-64bit
cd ..

${NEWPREFIX}ofxSimpleSpline
${NEWPREFIX}ofxSimpleSurface
${NEWPREFIX}ofxTonic
${NEWPREFIX}ofxPtf
${NEWPREFIX}ofxNearestNeighbour
${NEWPREFIX}ofxSubtitles
${NEWPREFIX}ofxBinaryMesh
${NEWPREFIX}ofxCv
${NEWPREFIX}ofxGpuParticles

${NEWPREFIX}ofxPostProcessing
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
${NEWPREFIX}ofxDelaunay

${NEWPREFIX}ofxAudioUnit
${NEWPREFIX}ofxAudioFeatures
${NEWPREFIX}ofxMeshUtils

${NEWPREFIX}ofxTextInputField
cd ofxTextInputField
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${NEWPREFIX}ofxRGBDepth
cd ofxRGBDepth
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${NEWPREFIX}ofxMSATimer
${NEWPREFIX}ofxTimecode
${NEWPREFIX}ofxTimeline

cd ofxTimeline
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${NEWPREFIX}ofxObjLoader	
