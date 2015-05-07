#!/bin/bash

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

${NEWPREFIX}ofxAnimationPrimitives
${NEWPREFIX}ofxAudioDecoder
${NEWPREFIX}ofxAudioFeatures
${NEWPREFIX}ofxAudioUnit
${NEWPREFIX}ofxAutoReloadedShader
${NEWPREFIX}ofxAVFVideoPlayer
${NEWPREFIX}ofxBillboard
${NEWPREFIX}ofxBinaryMesh
${NEWPREFIX}ofxBox2d
${NEWPREFIX}ofxCameraSaveLoad
${NEWPREFIX}ofxCocoaGLView
${NEWPREFIX}ofxColorPalettes
${NEWPREFIX}ofxCrypto
${NEWPREFIX}ofxCv
${NEWPREFIX}ofxDelaunay
${NEWPREFIX}ofxFTGL
${NEWPREFIX}ofxGameCamera
${NEWPREFIX}ofxGenerative
${NEWPREFIX}ofxGpuParticles
${NEWPREFIX}ofxLocalization
${NEWPREFIX}ofxMarchingCubes
${NEWPREFIX}ofxMeshUtils
${NEWPREFIX}ofxMPMFluid
${NEWPREFIX}ofxMSACore
${NEWPREFIX}ofxMSAInteractiveObject
${NEWPREFIX}ofxMSAObjCPointer
${NEWPREFIX}ofxMSAPhysics
${NEWPREFIX}ofxMSATimer
${NEWPREFIX}ofxNearestNeighbour
${NEWPREFIX}ofxObjLoader	
${NEWPREFIX}ofxOcean
${NEWPREFIX}ofxOculusDK2

${NEWPREFIX}ofxOpenSteer
cd ofxOpenSteer
git checkout -b feature-64bit --track origin/feature-64bit
git pull origin feature-64bit
cd ..

${NEWPREFIX}ofxPCL

${NEWPREFIX}ofxPostProcessing
cd ofxPostProcessing
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${NEWPREFIX}ofxPtf
${NEWPREFIX}ofxRange
${NEWPREFIX}ofxRegex

${NEWPREFIX}ofxRGBDepth
cd ofxRGBDepth
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${NEWPREFIX}ofxSimpleSpline
${NEWPREFIX}ofxSimpleSurface
${NEWPREFIX}ofxSQLite
${NEWPREFIX}ofxSSL
${NEWPREFIX}ofxSubtitles
${NEWPREFIX}ofxJSON
${NEWPREFIX}ofxTextInputField
cd ofxTextInputField
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${NEWPREFIX}ofxTextWriter
${NEWPREFIX}ofxTimecode

${NEWPREFIX}ofxTimeline
cd ofxTimeline
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${NEWPREFIX}ofxTonic
${NEWPREFIX}ofxTween

${NEWPREFIX}ofxUI
cd ofxUI
git checkout -b 'clouds-64' --track 'origin/clouds-64'
git pull origin 'clouds-64'
cd ..
