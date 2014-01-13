#!/bin/bash

# Dependencies for RGBDToolkit

#make sure you are in the RGBDToolkit project when you run this script


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


#private, must be read
# git clone git@github.com:rezaali/ofxExtras
#git clone git@github.com:CLOUDS-Interactive-Documentary/ofxExtras

# git clone https://github.com/rezaali/ofxUI
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

# git clone https://github.com/kylemcdonald/ofxCv
${NEWPREFIX}ofxCv

# git clone https://github.com/neilmendoza/ofxGpuParticles.git
${NEWPREFIX}ofxGpuParticles

cd ofxPostProcessing
git checkout -b develop --track origin/develop
git pull origin develop
cd ..


${NEWPREFIX}ofxGenerative #temp obviousjim for windows fix
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


${NEWPREFIX}ofxKinect
${NEWPREFIX}ofxOpenNI
${NEWPREFIX}ofxMSATimer

${NEWPREFIX}ofxTimecode
${NEWPREFIX}ofxTimeline

cd ofxTimeline
git checkout -b develop --track origin/develop
git pull origin develop
cd ..

${NEWPREFIX}ofxObjLoader	

#https://github.com/bgstaal/ofxShivaVG
${NEWPREFIX}ofxShivaVG
cd ofxShivaVG
git checkout -b 0.7.4 --track origin/0.7.4
git pull origin 0.7.4 
cd ..


# git clone https://github.com/satoruhiga/ofxCocoaGLView
# git clone https://github.com/satoruhiga/ofxPCL
# git clone https://github.com/memo/ofxMSAObjCPointer
# git clone https://github.com/obviousjim/ofxMSACore #temp obviousjim for windows
# git clone https://github.com/roymacdonald/ofxCameraSaveLoad
# git clone https://github.com/memo/ofxMSAPhysics
# git clone https://github.com/memo/ofxMSAObjCPointer
# git clone https://github.com/Larsberg/ofxMarchingCubes
# git clone https://github.com/andreasmuller/ofxAutoReloadedShader
# git clone https://github.com/obviousjim/ofxDelaunay
# git clone https://github.com/prisonerjohn/ofxSQLite
# git clone https://github.com/prisonerjohn/ofxTextWriter
# git clone https://github.com/prisonerjohn/ofxBillboard
# git clone https://github.com/quinkennedy/ofxRegex
# git clone https://github.com/satoruhiga/ofxAnimationPrimitives
# git clone https://github.com/obviousjim/ofxBox2d.git
# git clone https://github.com/obviousjim/ofxOpenSteer.git
# git clone https://github.com/Larsberg/ofxSimpleSpline
# git clone https://github.com/Larsberg/ofxSimpleSurface
# git clone https://github.com/labatrockwell/ofxLibwebsockets
# git clone https://github.com/jefftimesten/ofxJSON.git
# git clone https://github.com/galsasson/ofxTonic
# git clone https://github.com/neilmendoza/ofxPtf
# git clone https://github.com/neilmendoza/ofxNearestNeighbour
# git clone https://github.com/neilmendoza/ofxPostProcessing

# ${NEWPREFIX}obviousjim/ofxGenerative #temp obviousjim for windows fix
# ${NEWPREFIX}obviousjim/ofxAVFVideoPlayer
# ${NEWPREFIX}obviousjim/ofxOculusRift
# ${NEWPREFIX}rezaali/ofxColorPalettes
# ${NEWPREFIX}obviousjim/ofxOcean
# ${NEWPREFIX}Flightphase/ofxGameCamera
# ${NEWPREFIX}Flightphase/ofxRange
# ${NEWPREFIX}Flightphase/ofxTween
# ${NEWPREFIX}Flightphase/ofxMSAInteractiveObject
# ${NEWPREFIX}Flightphase/ofxFTGL
# ${NEWPREFIX}obviousjim/ofxMPMFluid
